#pragma once

#include "vector.h"

namespace uv
{
	template <class U> struct is_matrix : std::false_type { };
	template <class U> struct is_matrix<U&> : is_matrix<U> { };
	template <class U> struct is_matrix<const U&> : is_matrix<U> { };
	template <class U> static constexpr bool is_matrix_v = is_matrix<U>::value;

	template <class T, size_t R, size_t C>
	class Mat
	{
		static constexpr auto column_indices = std::make_index_sequence<C>{};
		template <class S, size_t RR, size_t CC>
		friend class Mat;
	public:
		using Row    = Vec<T, C, R>;
		using Column = Vec<T, R, 1>;
		static_assert(sizeof(Row) == sizeof(T));
	private:
		static constexpr size_t D = std::min(R, C); // diagonal length
		std::array<Column, C> _columns;

		      Column& _col(size_t i)       { return _columns[i]; }
		const Column& _col(size_t i) const { return _columns[i]; }

		      Row& _row(size_t i)       { return reinterpret_cast<      Row&>(_columns[0][i]); }
		const Row& _row(size_t i) const { return reinterpret_cast<const Row&>(_columns[0][i]); }

		template <class OP, class S>
		auto _scalar_apply(S s) const
		{
			return Mat<type::of<OP, T, S>, R, C>(OP{}, *this, s, ColumnIndices{});
		}
		template <class OP, class S, size_t RB, size_t CB>
		auto _matrix_apply(const Mat<S, RB, CB>& m) const
		{
			static_assert(RB == R && CB == C, "Matrix-matrix by-component operation requires equal matrix dimensions");
			return Mat<type::of<OP, T, S>, R, C>(OP{}, *this, m, ColumnIndices{});
		}

		template <class V, size_t... I>
		constexpr Mat(const std::array<V, C>& columns, std::integer_sequence<size_t, I...>)
			: _columns{ columns[I]... }   { }

		template <class V, size_t... I>
		constexpr Mat(const V& v, std::integer_sequence<size_t, I...>)
			: _columns{ (details::Element(I).of(v)*Axes<I>{})... }   { }
		template <class OP, class A, class B, size_t... I, class = if_scalar_t<B>>
		constexpr Mat(OP op, const Mat<A, R, C>& a, B b, std::integer_sequence<size_t, I...>)
			: _columns{ op(a._columns[I], b)... }   { }
		template <class OP, class A, class B, size_t... I>
		constexpr Mat(OP op, const Mat<A, R, C>& a, const Mat<B, R, C>& b, std::integer_sequence<size_t, I...>)
			: _columns{ op(a._columns[I], b._columns[I])... }   { }
	public:
		using scalar_type = T;
		static constexpr struct { size_t R, C; } dim = { R, C };

		Mat() { }
		template <class S> 
		Mat(const Mat<S, R, C>& b) : Mat(b._columns, column_indices) { }
		template <class V, class = std::enable_if_t<is_scalar_v<V> || is_vector_v<D, V>>> 
		constexpr Mat(const V& diagonal) : Mat(diagonal, column_indices) { }

		template <class S> Mat& operator=(const Mat<S, R, C>& b) { return *this = Mat(b); }
		template <class V, class = std::enable_if_t<is_scalar_v<V> || is_vector_v<D, V>>>
		Mat& operator=(const V& diagonal) { return *this = Mat(diagonal); }


		template <class S, class = if_scalar_t<S>> friend Mat<type::mul<T, S>, R, C> operator*(const Mat& m, S s) { return m._scalar_apply<op::mul>(s); }
		template <class S, class = if_scalar_t<S>> friend Mat<type::mul<S, T>, R, C> operator*(S s, const Mat& m) { return m._scalar_apply<op::mul>(s); }
		template <class S, class = if_scalar_t<S>> friend Mat<type::div<T, S>, R, C> operator/(const Mat& m, S s) { return m._scalar_apply<op::div>(s); }

		template <size_t I> friend Row& operator*(Axes<I>, Mat& m) { return m._row(I); }
		template <size_t I> friend Column& operator*(Mat& m, Axes<I>) { return m._col(I); }
		template <size_t I> friend const Row& operator*(Axes<I>, const Mat& m) { return m._row(I); }
		template <size_t I> friend const Column& operator*(const Mat& m, Axes<I>) { return m._col(I); }

		template <class S, size_t I> friend Vec<type::mul<T, S>, C> operator*(Component<S, I> c, const Mat& m) { return c._value * (Axes<I>{}*m); }
		template <class S, size_t I> friend Vec<type::mul<T, S>, R> operator*(const Mat& m, Component<S, I> c) { return (m*Axes<I>{})*c._value; }

		template <class S, size_t N, int K>
		friend Vec<type::mul<S, T>, C> operator*(const Vec<S, N, K>& v, const Mat& m)
		{
			static_assert(N == R, "Left-multiplied vector must have dimensionality equal to matrix row count");
			decltype(v * m) result = v[0]*m._col(0);
			for (size_t i = 1; i < C; ++i)
				result = result + v[i]*m._col(i);
			return result;
		}
		template <class S, size_t N, int K>
		friend Vec<type::mul<T, S>, R> operator*(const Mat& m, const Vec<S, N, K>& v)
		{
			static_assert(N == C, "Right-multiplied vector must have dimensionality equal to matrix column count");
			decltype(m * v) result = m._row(0)*v[0];
			for (size_t i = 1; i < R; ++i)
				result = result + m._row(i)*v[i];
			return result;
		}

		template <class S, size_t RB, size_t CB> 
		Mat<type::mul<T, S>, R, CB> operator*(const Mat<S, RB, CB>& b) const
		{
			// A * [ B0 ... BCB ] = [ A*B0 ... A*BCB ]
			static_assert(C == RB, "Maxtrix-matrix multiplication requires that left side column count equals right side row count");
			decltype(*this * b) result;
			for (size_t i = 0; i < CB; ++i)
				result._col(i) = *this * b._col(i);
			return result;
		}
		template <class S, size_t RB, size_t CB> Mat<type::add<T, S>, R, C> operator+(const Mat<S, RB, CB>& b) const { return _matrix_apply<op::add>(b); }
		template <class S, size_t RB, size_t CB> Mat<type::add<T, S>, R, C> operator-(const Mat<S, RB, CB>& b) const { return _matrix_apply<op::sub>(b); }

		template <class S, size_t RB, size_t CB> Mat<bool, R, C> operator==(const Mat<S, RB, CB>& b) const { return _matrix_apply<op::eq>(b); }
		template <class S, size_t RB, size_t CB> Mat<bool, R, C> operator!=(const Mat<S, RB, CB>& b) const { return _matrix_apply<op::ne>(b); }
		template <class S, size_t RB, size_t CB> Mat<bool, R, C> operator< (const Mat<S, RB, CB>& b) const { return _matrix_apply<op::sl>(b); }
		template <class S, size_t RB, size_t CB> Mat<bool, R, C> operator<=(const Mat<S, RB, CB>& b) const { return _matrix_apply<op::le>(b); }
		template <class S, size_t RB, size_t CB> Mat<bool, R, C> operator>=(const Mat<S, RB, CB>& b) const { return _matrix_apply<op::ge>(b); }
		template <class S, size_t RB, size_t CB> Mat<bool, R, C> operator> (const Mat<S, RB, CB>& b) const { return _matrix_apply<op::sg>(b); }

		auto data()       { return reinterpret_cast<      T*>(this); }
		auto data() const { return reinterpret_cast<const T*>(this); }

		explicit operator bool() const { for (size_t i = 0; i < R; ++i) if (!_rows[i]) return false; return true; }
	};
	template <class T, size_t R, size_t C>
	struct is_matrix<Mat<T, R, C>> : std::true_type { };

	using float22 = Mat<float, 2, 2>;
	using float33 = Mat<float, 3, 3>;
	using float44 = Mat<float, 4, 4>;

	using double22 = Mat<double, 2, 2>;
	using double33 = Mat<double, 3, 3>;
	using double44 = Mat<double, 4, 4>;

	template <class T, size_t R, size_t C> auto& diagonal(      Mat<T, R, C>& m) { return reinterpret_cast<      Vec<T, std::min(R, C), C+1>&>(m); }
	template <class T, size_t R, size_t C> auto& diagonal(const Mat<T, R, C>& m) { return reinterpret_cast<const Vec<T, std::min(R, C), C+1>&>(m); }

	template <class V, size_t M>
	class MatrixView
	{
		V _data[M];

		constexpr void _assign() const { }
		template <class T0, class... TN, int K0, int... KN>
		void _assign(const Vec<T0, dim<V>, K0>& first, const Vec<TN, dim<V>, KN>&... rest)
		{
			_data[M - (1+sizeof...(TN))] = first;
			_assign(rest...);
		}
	public:

		template <class... TN, int... KN>
		void assign(const Vec<TN, dim<V>, KN>&... data)
		{
			static_assert(sizeof...(TN) == M, "Invalid number of vectors for matrix assignment");
			_assign(data...);
		}

		V* begin() { return _data; }
		const V* begin() const { return _data; }
		V* end() { return _data + M; }
		const V* end() const { return _data + M; }

		constexpr size_t size() const { return M; }

		      V& operator[](size_t i)       { return _data[i]; }
		const V& operator[](size_t i) const { return _data[i]; }
	};

	template <class T, size_t R, size_t C> using    RowView = MatrixView<typename Mat<T, R, C>::Row, R>;
	template <class T, size_t R, size_t C> using ColumnView = MatrixView<typename Mat<T, R, C>::Column, C>;

	template <class T, size_t R, size_t C>       RowView<T, R, C>& rows(      Mat<T, R, C>& m) { return reinterpret_cast<decltype(rows(m))>(m); }
	template <class T, size_t R, size_t C> const RowView<T, R, C>& rows(const Mat<T, R, C>& m) { return reinterpret_cast<decltype(rows(m))>(m); }
	template <class T, size_t R, size_t C>       ColumnView<T, R, C>& cols(      Mat<T, R, C>& m) { return reinterpret_cast<decltype(cols(m))>(m); }
	template <class T, size_t R, size_t C> const ColumnView<T, R, C>& cols(const Mat<T, R, C>& m) { return reinterpret_cast<decltype(cols(m))>(m); }

	namespace details
	{
		template <size_t R, size_t C>
		struct square_op
		{
			static_assert(R == C, "Matrix must be square");

			template <class T>
			static auto det(const Mat<T, R, C>& m);
			template <class T>
			static auto inv(const Mat<T, R, C>& m);
		};
		template <>
		struct square_op<2, 2>
		{
			template <class T>
			static auto det(const Mat<T, 2, 2>& m)
			{
				return cross(cols(m)[0], cols(m)[1]);
			}
			template <class T>
			static auto inv(const Mat<T, 2, 2>& m)
			{
				return rows(
					vector(rows(m)[1][1], rows(m)[1][0]),
					vector(rows(m)[0][1], rows(m)[0][0])
				) / det(m);
			}
		};
		template <>
		struct square_op<3, 3>
		{
			template <class T>
			static auto det(const Mat<T, 3, 3>& m)
			{
				return dot(rows(m)[0], cross(rows(m)[1], rows(m)[2]));
			}
			template <class T>
			static auto inv(const Mat<T, 3, 3>& m)
			{
				auto& mc0 = cols(m)[0];
				auto& mc1 = cols(m)[1];
				auto& mc2 = cols(m)[2];
				return rows(
					cross(mc1, mc2),
					cross(mc2, mc0),
					cross(mc0, mc1)
				) / det(m);
			}
		};

	}

	template <class T, size_t R, int... K> auto cols(const Vec<T, R, K>&... args) { Mat<T, R, sizeof...(K)> r; cols(r).assign(args...); return r; }
	template <class T, size_t C, int... K> auto rows(const Vec<T, C, K>&... args) { Mat<T, sizeof...(K), C> r; rows(r).assign(args...); return r; }

	template <class T, size_t R, size_t C>
	auto transpose(const Mat<T, R, C>& m)
	{
		Mat<T, C, R> result;
		for (int i = 0; i < C; ++i)
			rows(result)[i] = cols(m)[i];
		return result;
	}

	template <class T, size_t R, size_t C> auto    det(const Mat<T, R, C>& m) { return details::square_op<R, C>::det(m); }
	template <class T, size_t R, size_t C> auto invert(const Mat<T, R, C>& m) { return details::square_op<R, C>::inv(m); }
}

#define UVECTOR_MATRIX_DEFINED

#ifdef UVECTOR_ROTATION_DEFINED
#include "cross/matrix_rotation.h"
#endif

#ifdef UVECTOR_POINT_DEFINED
#include "cross/matrix_point.h"
#endif

#ifdef UVECTOR_TRANSFORM_DEFINED
#include "cross/matrix_transform.h"
#endif

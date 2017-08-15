#pragma once

#include "vector.h"

namespace uv
{
	template <class T, size_t R, size_t C>
	class Matrix
	{
	public:
		using Row    = Vector<T, C, R>;
		using Column = Vector<T, R, 1>;
		static_assert(sizeof(Row) == sizeof(T));
	private:
		static constexpr size_t D = R < C ? R : C; // diagonal length
		std::array<Column, R> _columns;

		      Column& _col(size_t i)       { return _columns[i]; }
		const Column& _col(size_t i) const { return _columns[i]; }

		      Row& _row(size_t i)       { return reinterpret_cast<      Row&>(_columns[0][i]); }
		const Row& _row(size_t i) const { return reinterpret_cast<const Row&>(_columns[0][i]); }

		template <class OP, class S>
		auto _scalar_apply(S s) const
		{
			OP op;
			Matrix<type::of<OP, T, S>, R, C> result;
			for (size_t i = 0; i < C; ++i)
				for (size_t j = 0; j < R; ++i)
					result._columns[i][j] = op(_columns[i][j], s);
			return result;
		}
		template <class OP, class S, size_t RB, size_t CB>
		auto _matrix_apply(const Matrix<S, RB, CB>& m) const
		{
			static_assert(RB == R && CB == C, "Matrix-matrix by-component operation requires equal matrix dimensions");
			OP op;
			Matrix<type::of<OP, T, S>, R, C> result;
			for (size_t i = 0; i < C; ++i)
				for (size_t j = 0; j < R; ++j)
					result._columns[i][j] = op(_columns[i][j], m._columns[i][j]);
			return result;
		}
	public:
		using scalar_type = T;
		static constexpr struct { size_t R, C; } dim = { R, C };

		Matrix() { }
		Matrix(T diagonal) { *this = diagonal; }
		template <int K>
		Matrix(const Vector<T, D, K>& diagonal) { *this = diagonal; }

		Matrix& operator=(T diagonal)
		{ 
			for (size_t i = 0; i < C; ++i)
				for (size_t j = 0; j < R; ++j)
					_columns[i][j] = i == j ? diagonal : T(0);
			return *this;
		}
		template <int K>
		Matrix& operator=(const Vector<T, D, K>& diagonal)
		{
			for (size_t i = 0; i < C; ++i)
				for (size_t j = 0; j < R; ++j)
					_columns[i][j] = i == j ? diagonal[i] : T(0);
			return *this;
		}
		template <class S, class = if_scalar_t<S>> friend Matrix<type::mul<T, S>, R, C> operator*(const Matrix& m, S s) { return m._scalar_apply<op::mul>(s); }
		template <class S, class = if_scalar_t<S>> friend Matrix<type::mul<S, T>, R, C> operator*(S s, const Matrix& m) { return m._scalar_apply<op::mul>(s); }
		template <class S, class = if_scalar_t<S>> friend Matrix<type::div<T, S>, R, C> operator/(const Matrix& m, S s) { return m._scalar_apply<op::div>(s); }

		template <size_t I> friend Row& operator*(Axes<I>, Matrix& m) { return m._row(I); }
		template <size_t I> friend Column& operator*(Matrix& m, Axes<I>) { return m._col(I); }
		template <size_t I> friend const Row& operator*(Axes<I>, const Matrix& m) { return m._row(I); }
		template <size_t I> friend const Column& operator*(const Matrix& m, Axes<I>) { return m._col(I); }

		template <class S, size_t I> friend Vector<type::mul<T, S>, C> operator*(Component<S, I> c, const Matrix& m) { return c._value * (Axes<I>{}*m); }
		template <class S, size_t I> friend Vector<type::mul<T, S>, R> operator*(const Matrix& m, Component<S, I> c) { return (m*Axes<I>{})*c._value; }

		template <class S, size_t N, int K>
		friend Vector<type::mul<S, T>, C> operator*(const Vector<S, N, K>& v, const Matrix& m)
		{
			static_assert(N == R, "Left-multiplied vector must have dimensionality equal to matrix row count");
			decltype(v * m) result = v[0]*m._col(0);
			for (size_t i = 1; i < C; ++i)
				result = result + v[i]*m._col(i);
			return result;
		}
		template <class S, size_t N, int K>
		friend Vector<type::mul<T, S>, R> operator*(const Matrix& m, const Vector<S, N, K>& v)
		{
			static_assert(N == C, "Right-multiplied vector must have dimensionality equal to matrix column count");
			decltype(m * v) result = m._row(0)*v[0];
			for (size_t i = 1; i < R; ++i)
				result = result + m._row(i)*v[i];
			return result;
		}

		template <class S, size_t RB, size_t CB> 
		Matrix<type::mul<T, S>, R, CB> operator*(const Matrix<S, RB, CB>& b) const
		{
			// A * [ B0 ... BCB ] = [ A*B0 ... A*BCB ]
			static_assert(C == RB, "Maxtrix-matrix multiplication requires that left side column count equals right side row count");
			decltype(*this * b) result;
			for (size_t i = 0; i < CB; ++i)
				result._col(i) = *this * b._col(i);
			return result;
		}
		template <class S, size_t RB, size_t CB> Matrix<type::add<T, S>, R, C> operator+(const Matrix<S, RB, CB>& b) const { return _matrix_apply<op::add>(b); }
		template <class S, size_t RB, size_t CB> Matrix<type::add<T, S>, R, C> operator-(const Matrix<S, RB, CB>& b) const { return _matrix_apply<op::sub>(b); }

		template <class S, size_t RB, size_t CB> Matrix<bool, R, C> operator==(const Matrix<S, RB, CB>& b) const { return _matrix_apply<op::eq>(b); }
		template <class S, size_t RB, size_t CB> Matrix<bool, R, C> operator!=(const Matrix<S, RB, CB>& b) const { return _matrix_apply<op::ne>(b); }
		template <class S, size_t RB, size_t CB> Matrix<bool, R, C> operator< (const Matrix<S, RB, CB>& b) const { return _matrix_apply<op::sl>(b); }
		template <class S, size_t RB, size_t CB> Matrix<bool, R, C> operator<=(const Matrix<S, RB, CB>& b) const { return _matrix_apply<op::le>(b); }
		template <class S, size_t RB, size_t CB> Matrix<bool, R, C> operator>=(const Matrix<S, RB, CB>& b) const { return _matrix_apply<op::ge>(b); }
		template <class S, size_t RB, size_t CB> Matrix<bool, R, C> operator> (const Matrix<S, RB, CB>& b) const { return _matrix_apply<op::sg>(b); }

		auto data()       { return reinterpret_cast<      T*>(this); }
		auto data() const { return reinterpret_cast<const T*>(this); }

		explicit operator bool() const { for (size_t i = 0; i < R; ++i) if (!_rows[i]) return false; return true; }
	};
	using float22 = Matrix<float, 2, 2>;
	using float33 = Matrix<float, 3, 3>;
	using float44 = Matrix<float, 4, 4>;

	using double22 = Matrix<double, 2, 2>;
	using double33 = Matrix<double, 3, 3>;
	using double44 = Matrix<double, 4, 4>;

	template <class T, size_t R, size_t C> auto& diagonal(      Matrix<T, R, C>& m) { return reinterpret_cast<      Vector<T, std::min(R, C), C+1>&>(m); }
	template <class T, size_t R, size_t C> auto& diagonal(const Matrix<T, R, C>& m) { return reinterpret_cast<const Vector<T, std::min(R, C), C+1>&>(m); }

	template <class V, size_t M>
	class MatrixView
	{
		V _data[M];

		constexpr void _assign() const { }
		template <class T0, class... TN, int K0, int... KN>
		void _assign(const Vector<T0, dim<V>, K0>& first, const Vector<TN, dim<V>, KN>&... rest)
		{
			_data[M - (1+sizeof...(TN))] = first;
			_assign(rest...);
		}
	public:

		template <class... TN, int... KN>
		void assign(const Vector<TN, dim<V>, KN>&... data)
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

	template <class T, size_t R, size_t C> using    RowView = MatrixView<typename Matrix<T, R, C>::Row, R>;
	template <class T, size_t R, size_t C> using ColumnView = MatrixView<typename Matrix<T, R, C>::Column, C>;

	template <class T, size_t R, size_t C>       RowView<T, R, C>& rows(      Matrix<T, R, C>& m) { return reinterpret_cast<decltype(rows(m))>(m); }
	template <class T, size_t R, size_t C> const RowView<T, R, C>& rows(const Matrix<T, R, C>& m) { return reinterpret_cast<decltype(rows(m))>(m); }
	template <class T, size_t R, size_t C>       ColumnView<T, R, C>& cols(      Matrix<T, R, C>& m) { return reinterpret_cast<decltype(cols(m))>(m); }
	template <class T, size_t R, size_t C> const ColumnView<T, R, C>& cols(const Matrix<T, R, C>& m) { return reinterpret_cast<decltype(cols(m))>(m); }

	namespace details
	{
		template <size_t R, size_t C>
		struct square_op
		{
			static_assert(R == C, "Matrix must be square");

			template <class T>
			static auto det(const Matrix<T, R, C>& m);
			template <class T>
			static auto inv(const Matrix<T, R, C>& m);
		};
		template <>
		struct square_op<2, 2>
		{
			template <class T>
			static auto det(const Matrix<T, 2, 2>& m)
			{
				return cross(cols(m)[0], cols(m)[1]);
			}
			template <class T>
			static auto inv(const Matrix<T, 2, 2>& m)
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
			static auto det(const Matrix<T, 3, 3>& m)
			{
				return dot(rows(m)[0], cross(rows(m)[1], rows(m)[2]));
			}
			template <class T>
			static auto inv(const Matrix<T, 3, 3>& m)
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

	template <class T, size_t R, int... K> auto cols(const Vector<T, R, K>&... args) { Matrix<T, R, sizeof...(K)> r; cols(r).assign(args...); return r; }
	template <class T, size_t C, int... K> auto rows(const Vector<T, C, K>&... args) { Matrix<T, sizeof...(K), C> r; rows(r).assign(args...); return r; }

	template <class T, size_t R, size_t C>
	auto transpose(const Matrix<T, R, C>& m)
	{
		Matrix<T, C, R> result;
		for (int i = 0; i < C; ++i)
			rows(result)[i] = cols(m)[i];
		return result;
	}

	template <class T, size_t R, size_t C> auto    det(const Matrix<T, R, C>& m) { return details::square_op<R, C>::det(m); }
	template <class T, size_t R, size_t C> auto invert(const Matrix<T, R, C>& m) { return details::square_op<R, C>::inv(m); }
}

#define UVECTOR_MATRIX_DEFINED

#ifdef UVECTOR_COMPLEX_DEFINED
#include "cross/matrix_complex.h"
#endif

#ifdef UVECTOR_QUATERNION_DEFINED
#include "cross/matrix_quaternion.h"
#endif

#ifdef UVECTOR_POINT_DEFINED
#include "cross/matrix_point.h"
#endif

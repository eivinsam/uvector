#pragma once

#include "uvector.h"

namespace uv
{
	template <class T, size_t R, size_t C>
	class mat : public details::equal_test<sizeof(vec<T, R, C>), sizeof(T)>
	{
	public:
		static_assert(R > 1);
		static_assert(C > 1);

		using Row    = vec<T, C, 1>;
		using Column = vec<T, R, C>;
	private:
		static constexpr size_t D = R < C ? R : C; // diagonal length
		std::array<Row, R> _rows;
		void _assign_columns() { }
		template <int KF, int... KR>
		void _assign_columns(const vec<T, R, KF>& first, const vec<T, R, KR>&... rest)
		{
			col(C - 1 - sizeof...(KR)) = first;
			_assign_columns(rest...);
		}
	public:
		mat() { }
		mat(T c)
		{
			memset(this, 0, sizeof(*this));
			for (size_t i = 0; i < D; ++i)
				_rows[i][i] = c;
		}
		template <int K>
		mat(const vec<T, D, K>& diagonal)
		{
			memset(this, 0, sizeof(*this));
			for (size_t i = 0; i < D; ++i)
				_rows[i][i] = diagonal[i];
		}

		template <int... K>
		void assign_columns(const vec<T, R, K>&... cols)
		{ 
			static_assert(sizeof...(K) == C, "invalid number of columns");
			_assign_columns(cols...);
		}
		template <int... K>
		void assign_rows(const vec<T, C, K>&... rest)
		{
			static_assert(sizeof...(K) == R, "invalid number of rows");
			_rows = { rest... };
		}

		      Column&    col(size_t i)       { return reinterpret_cast<      Column&>(_rows[0][i]); }
		const Column&    col(size_t i) const { return reinterpret_cast<const Column&>(_rows[0][i]); }
		      Row& row(size_t i)       { return _rows[i]; }
		const Row& row(size_t i) const { return _rows[i]; }

		      T& col(size_t i, size_t j)       { return _rows[j][i]; }
		const T& col(size_t i, size_t j) const { return _rows[j][i]; }
		      T& row(size_t i, size_t j)       { return _rows[i][j]; }
		const T& row(size_t i, size_t j) const { return _rows[i][j]; }

		auto& diagonal()       { return reinterpret_cast<      vec<T, D, C + 1>&>(*this); }
		auto& diagonal() const { return reinterpret_cast<const vec<T, D, C + 1>&>(*this); }

		template <class V, size_t M>
		class View
		{
			V _data[M];
		public:
			      V* begin()       { return _data; }
			const V* begin() const { return _data; }
			      V* end()       { return _data + M; }
			const V* end() const { return _data + M; }

			constexpr size_t size() const { return M; }
		};

		auto& rows()       { return reinterpret_cast<      View<Row, R>&>(*this); }
		auto& rows() const { return reinterpret_cast<const View<Row, R>&>(*this); }
		auto& cols()       { return reinterpret_cast<      View<Column, C>&>(*this); }
		auto& cols() const { return reinterpret_cast<const View<Column, C>&>(*this); }

		explicit operator bool() const { for (size_t i = 0; i < R; ++i) if (!row(i)) return false; return true; }
	};

	template <class T, size_t R, int... K>
	auto matrix_columns(const vec<T, R, K>&... args)
	{
		mat<T, R, sizeof...(K)> result;
		result.assign_columns(args...);
		return result;
	}
	template <class T, size_t C, int... K>
	auto matrix_rows(const vec<T, C, K>&... args)
	{
		mat<T, sizeof...(K), C> result;
		result.assign_rows(args...);
		return result;
	}

	template <class S, class T, size_t R, size_t C, class = enable_if_scalar_t<S>>
	auto operator==(const mat<T, R, C>& m, S c)
	{
		mat<bool, R, C> result;
		for (size_t i = 0; i < R; ++i)
			result.row(i) = m.row(i) == c;
		return result;
	}
	template <class S, class T, size_t R, size_t C, class = enable_if_scalar_t<S>>
	auto operator!=(const mat<T, R, C>& m, S c)
	{
		mat<bool, R, C> result;
		for (size_t i = 0; i < R; ++i)
			result.row(i) = m.row(i) != c;
		return result;
	}
	template <class S, class T, size_t R, size_t C, class = enable_if_scalar_t<S>> bool operator==(S c, const mat<T, R, C>& m) { return m == c; }
	template <class S, class T, size_t R, size_t C, class = enable_if_scalar_t<S>> bool operator!=(S c, const mat<T, R, C>& m) { return m != c; }

	template <class A, class B, size_t RA, size_t RB, size_t CA, size_t CB>
	auto operator==(const mat<A, RA, CA>& a, const mat<B, RB, CB>& b)
	{
		static_assert(RA == RB, "matrices must have equal number of rows to be comparable");
		static_assert(CA == CB, "matrices must have equal number of columns to be comparable");
		mat<bool, RA, CA> result;
		for (size_t i = 0; i < RA; ++i)
			result.row(i) = a.row(i) == b.row(i);
		return result;
	}
	template <class A, class B, size_t RA, size_t RB, size_t CA, size_t CB>
	auto operator!=(const mat<A, RA, CA>& a, const mat<B, RB, CB>& b)
	{
		static_assert(RA == RB, "matrices must have equal number of rows to be comparable");
		static_assert(CA == CB, "matrices must have equal number of columns to be comparable");
		mat<bool, RA, CA> result;
		for (size_t i = 0; i < RA; ++i)
			result.row(i) = a.row(i) != b.row(i);
		return result;
	}

	template <class T, size_t R, size_t C>
	auto transpose(const mat<T, R, C>& m)
	{
		mat<T, C, R> result;
		for (int i = 0; i < C; ++i)
			result.row(i) = m.col(i);
		return result;
	}

	template <class A, class B, size_t R, size_t C, size_t N, int K>
	auto operator*(const mat<A, R, C>& m, const vec<B, N, K>& v)
	{
		static_assert(C == N, "matrix column count does not match vector length");
		vec<details::inner_product_t<A, B>, R> result;
		for (int i = 0; i < R; ++i)
			result[i] = dot(m.row(i), v);
		return result;
	}
	template <class A, class B, size_t R, size_t C, size_t N, int K>
	auto operator*(const vec<A, N, K>& v, const mat<B, R, C>& m)
	{
		static_assert(R == N, "matrix row count does not match vector length");
		vec<details::inner_product_t<A,B>, C> result;
		for (int i = 0; i < C; ++i)
			result[i] = dot(m.col(i), v);
		return result;
	}

	template <class A, class B, size_t RA, size_t CA, size_t RB, size_t CB>
	auto operator*(const mat<A, RA, CA>& a, const mat<B, RB, CB>& b)
	{
		static_assert(CA == RB, "invalid matrix dimensions for multiplication");
		mat<details::inner_product_t<A, B>, RA, CB> result;
		for (int i = 0; i < CB; ++i)
			result.col(i) = a * b.col(i);
		return result;
	}

	using float22 = mat<float, 2, 2>;
	using float33 = mat<float, 3, 3>;
	using float44 = mat<float, 4, 4>;

	using double22 = mat<double, 2, 2>;
	using double33 = mat<double, 3, 3>;
	using double44 = mat<double, 4, 4>;
}

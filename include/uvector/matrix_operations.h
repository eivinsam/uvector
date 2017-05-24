#pragma once

#include "details/matrix_details.h"

#define TEMPLATE_MATRIX_A template <class A, size_t RA, size_t CA>
#define MATRIX_A mat<A, RA, CA>

namespace uv
{
	TEMPLATE_MATRIX_A auto& diagonal(      MATRIX_A& m) { return reinterpret_cast<      vec<A, std::min(RA, CA), CA + 1>&>(m); }
	TEMPLATE_MATRIX_A auto& diagonal(const MATRIX_A& m) { return reinterpret_cast<const vec<A, std::min(RA, CA), CA + 1>&>(m); }

	template <class V, size_t M>
	class MatrixView
	{
		V _data[M];
	public:
		      V* begin()       { return _data; }
		const V* begin() const { return _data; }
		      V* end()       { return _data + M; }
		const V* end() const { return _data + M; }

		constexpr size_t size() const { return M; }

		auto& operator[](size_t i)       { return _data[i]; }
		auto& operator[](size_t i) const { return _data[i]; }
	};

	TEMPLATE_MATRIX_A auto& rows(      MATRIX_A& a) { return reinterpret_cast<      MatrixView<typename MATRIX_A::Row,    RA>&>(a); }
	TEMPLATE_MATRIX_A auto& rows(const MATRIX_A& a) { return reinterpret_cast<const MatrixView<typename MATRIX_A::Row,    RA>&>(a); }
	TEMPLATE_MATRIX_A auto& cols(      MATRIX_A& a) { return reinterpret_cast<      MatrixView<typename MATRIX_A::Column, CA>&>(a); }
	TEMPLATE_MATRIX_A auto& cols(const MATRIX_A& a) { return reinterpret_cast<const MatrixView<typename MATRIX_A::Column, CA>&>(a); }


	template <class T, size_t R, int... K>
	auto cols(const vec<T, R, K>&... args)
	{
		mat<T, R, sizeof...(K)> result;
		details::assign_cols(result, args...);
		return result;
	}
	template <class T, size_t C, int... K>
	auto rows(const vec<T, C, K>&... args)
	{
		mat<T, sizeof...(K), C> result;
		details::assign_rows(result, args...);
		return result;
	}

	template <class S, class T, size_t R, size_t C>
	auto operator==(const mat<T, R, C>& m, scalar<S> c)
	{
		mat<bool, R, C> result;
		for (size_t i = 0; i < R; ++i)
			rows(result)[i] = rows(m)[i] == c;
		return result;
	}
	template <class S, class T, size_t R, size_t C>
	auto operator!=(const mat<T, R, C>& m, scalar<S> c)
	{
		mat<bool, R, C> result;
		for (size_t i = 0; i < R; ++i)
			rows(result)[i] = rows(m)[i] != c;
		return result;
	}
	template <class S, class T, size_t R, size_t C> bool operator==(scalar<S> c, const mat<T, R, C>& m) { return m == c; }
	template <class S, class T, size_t R, size_t C> bool operator!=(scalar<S> c, const mat<T, R, C>& m) { return m != c; }

	template <class A, class B, size_t RA, size_t RB, size_t CA, size_t CB>
	auto operator==(const mat<A, RA, CA>& a, const mat<B, RB, CB>& b)
	{
		static_assert(RA == RB, "matrices must have equal number of rows to be comparable");
		static_assert(CA == CB, "matrices must have equal number of columns to be comparable");
		mat<bool, RA, CA> result;
		for (size_t i = 0; i < RA; ++i)
			rows(result)[i] = rows(a)[i] == rows(b)[i];
		return result;
	}
	template <class A, class B, size_t RA, size_t RB, size_t CA, size_t CB>
	auto operator!=(const mat<A, RA, CA>& a, const mat<B, RB, CB>& b)
	{
		static_assert(RA == RB, "matrices must have equal number of rows to be comparable");
		static_assert(CA == CB, "matrices must have equal number of columns to be comparable");
		mat<bool, RA, CA> result;
		for (size_t i = 0; i < RA; ++i)
			rows(result)[i] = rows(a)[i] != rows(b)[i];
		return result;
	}

	template <class T, size_t R, size_t C>
	auto transpose(const mat<T, R, C>& m)
	{
		mat<T, C, R> result;
		for (int i = 0; i < C; ++i)
			rows(result)[i] = cols(m)[i];
		return result;
	}

	template <class A, class B, size_t R, size_t C, size_t N, int K>
	auto operator*(const mat<A, R, C>& m, const vec<B, N, K>& v)
	{
		static_assert(C == N, "matrix column count does not match vector length");
		vec<type::inner_product<A, B>, R> result;
		for (int i = 0; i < R; ++i)
			result[i] = dot(rows(m)[i], v);
		return result;
	}
	template <class A, class B, size_t R, size_t C, size_t N, int K>
	auto operator*(const vec<A, N, K>& v, const mat<B, R, C>& m)
	{
		static_assert(R == N, "matrix row count does not match vector length");
		vec<type::inner_product<A, B>, C> result;
		for (int i = 0; i < C; ++i)
			result[i] = dot(cols(m)[i], v);
		return result;
	}

	template <class A, class B, size_t RA, size_t CA, size_t RB, size_t CB>
	auto operator*(const mat<A, RA, CA>& a, const mat<B, RB, CB>& b)
	{
		static_assert(CA == RB, "invalid matrix dimensions for multiplication");
		mat<type::inner_product<A, B>, RA, CB> result;
		for (int i = 0; i < CB; ++i)
			cols(result)[i] = a * cols(b)[i];
		return result;
	}
}

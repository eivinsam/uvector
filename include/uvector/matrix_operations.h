#pragma once

#include "details/matrix_details.h"
#include "scalar.h"

namespace uv
{
	template <class T, size_t R, int... K>
	auto cols(const Vector<T, R, K>&... args)
	{
		Matrix<T, R, sizeof...(K)> result;
		details::assign_cols(result, args...);
		return result;
	}
	template <class T, size_t C, int... K>
	auto rows(const Vector<T, C, K>&... args)
	{
		Matrix<T, sizeof...(K), C> result;
		details::assign_rows(result, args...);
		return result;
	}

	template <class S, class T, size_t R, size_t C, class = if_scalar_t<S>>
	auto operator==(const Matrix<T, R, C>& m, S c)
	{
		Matrix<bool, R, C> result;
		for (size_t i = 0; i < R; ++i)
			rows(result)[i] = rows(m)[i] == c;
		return result;
	}
	template <class S, class T, size_t R, size_t C, class = if_scalar_t<S>>
	auto operator!=(const Matrix<T, R, C>& m, S c)
	{
		Matrix<bool, R, C> result;
		for (size_t i = 0; i < R; ++i)
			rows(result)[i] = rows(m)[i] != c;
		return result;
	}
	template <class S, class T, size_t R, size_t C, class = if_scalar_t<S>> bool operator==(S c, const Matrix<T, R, C>& m) { return m == c; }
	template <class S, class T, size_t R, size_t C, class = if_scalar_t<S>> bool operator!=(S c, const Matrix<T, R, C>& m) { return m != c; }


	template <class A, class B, size_t RA, size_t RB, size_t CA, size_t CB>
	auto operator==(const Matrix<A, RA, CA>& a, const Matrix<B, RB, CB>& b)
	{
		static_assert(RA == RB, "matrices must have equal number of rows to be comparable");
		static_assert(CA == CB, "matrices must have equal number of columns to be comparable");
		Matrix<bool, RA, CA> result;
		for (size_t i = 0; i < RA; ++i)
			rows(result)[i] = rows(a)[i] == rows(b)[i];
		return result;
	}
	template <class A, class B, size_t RA, size_t RB, size_t CA, size_t CB>
	auto operator!=(const Matrix<A, RA, CA>& a, const Matrix<B, RB, CB>& b)
	{
		static_assert(RA == RB, "matrices must have equal number of rows to be comparable");
		static_assert(CA == CB, "matrices must have equal number of columns to be comparable");
		Matrix<bool, RA, CA> result;
		for (size_t i = 0; i < RA; ++i)
			rows(result)[i] = rows(a)[i] != rows(b)[i];
		return result;
	}

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

	template <class A, class B, size_t R, size_t C, size_t N, int K>
	auto operator*(const Matrix<A, R, C>& m, const Vector<B, N, K>& v)
	{
		static_assert(C == N, "matrix column count does not match vector length");
		Vector<type::inner_product<A, B>, R> result;
		for (int i = 0; i < R; ++i)
			result[i] = dot(rows(m)[i], v);
		return result;
	}
	template <class A, class B, size_t R, size_t C, size_t N, int K>
	auto operator*(const Vector<A, N, K>& v, const Matrix<B, R, C>& m)
	{
		static_assert(R == N, "matrix row count does not match vector length");
		Vector<type::inner_product<A, B>, C> result;
		for (int i = 0; i < C; ++i)
			result[i] = dot(cols(m)[i], v);
		return result;
	}

	template <class A, class B, size_t RA, size_t CA, size_t RB, size_t CB>
	auto operator*(const Matrix<A, RA, CA>& a, const Matrix<B, RB, CB>& b)
	{
		static_assert(CA == RB, "invalid matrix dimensions for multiplication");
		Matrix<type::inner_product<A, B>, RA, CB> result;
		for (int i = 0; i < CB; ++i)
			cols(result)[i] = a * cols(b)[i];
		return result;
	}

	template <class A, size_t R, size_t C, size_t I>
	auto& operator*(const Matrix<A, R, C>& m, Axes<I>)
	{
		static_assert(I < C, "Axis index must be smaller than matrix column count");
		return cols(m)[I];
	}
	template <class A, size_t R, size_t C, size_t I>
	auto& operator*(Axes<I>, const Matrix<A, R, C>& m)
	{
		static_assert(I < R, "Axis index must be smaller than matrix row count");
		return rows(m)[I];
	}

	template <class A, class B, size_t RA, size_t CA, size_t IB>
	auto operator*(const Matrix<A, RA, CA>& m, Component<B, IB> c)
	{
		static_assert(IB < CA, "Component index must be smaller than matrix column count");
		return cols(m)[IB] * (*c);
	}

	template <class A, class B, size_t RA, size_t CA, size_t IB>
	auto operator*(Component<B, IB> c, const Matrix<A, RA, CA>& m)
	{
		static_assert(IB < RA, "Component index must be smaller than matrix row count");
		return (*c) * rows(m)[IB];
	}

	template <class A, class B, size_t RA, size_t CA, class = if_scalar_t<B>>
	auto operator*(const Matrix<A, RA, CA>& m, B c)
	{
		Matrix<type::of<op::mul, A, B>, RA, CA> result;
		for (size_t i = 0; i < RA; ++i)
			rows(result)[i] = rows(m)[i] * c;
		return result;
	}
	template <class A, class B, size_t RA, size_t CA, class = if_scalar_t<B>>
	auto operator*(B c, const Matrix<A, RA, CA>& m)
	{
		Matrix<type::of<op::mul, B, A>, RA, CA> result;
		for (size_t i = 0; i < RA; ++i)
			rows(result)[i] = c * rows(m)[i];
		return result;
	}

	template <class A, class B, size_t RA, size_t CA, class = if_scalar_t<B>>
	auto operator/(const Matrix<A, RA, CA>& m, B c)
	{
		Matrix<type::of<op::div, A, B>, RA, CA> result;
		for (size_t i = 0; i < RA; ++i)
			rows(result)[i] = rows(m)[i] / c;
		return result;
	}
}

#define UVECTOR_MATRIX_OPS_DEFINED

#ifdef UVECTOR_QUATERNION_OPS_DEFINED
#include "cross/matrix_quaternion.h"
#endif

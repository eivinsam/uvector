#pragma once

#include "../matrix_operations.h"
#include "../component_operations.h"

namespace uv
{
	template <class A, size_t R, size_t C, size_t I>
	auto& operator*(const Matrix<A, R, C>& m, Axis<I>) 
	{
		static_assert(I < C, "Axis index must be smaller than matrix column count");
		return cols(m)[I];
	}
	template <class A, size_t R, size_t C, size_t I>
	auto& operator*(Axis<I>, const Matrix<A, R, C>& m)
	{
		static_assert(I < R, "Axis index must be smaller than matrix row count");
		return rows(m)[I];
	}

	template <class A, class B, size_t RA, size_t CA, size_t IB>
	auto operator*(const Matrix<A, RA, CA>& m, Component<B, IB> c)
	{
		static_assert(IB < CA, "Component index must be smaller than matrix column count");
		return cols(m)[IB] * scalar(*c);
	}

	template <class A, class B, size_t RA, size_t CA, size_t IB>
	auto operator*(Component<B, IB> c, const Matrix<A, RA, CA>& m)
	{
		static_assert(IB < RA, "Component index must be smaller than matrix row count");
		return scalar(*c) * rows(m)[IB];
	}
}

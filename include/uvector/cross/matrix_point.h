#pragma once

#include "../point.h"
#include "../matrix.h"

namespace uv
{
	template <class A, class B, size_t N> Point<type::mul<A, B>, N> operator*(const Matrix<A, N, N>& Rs, const Point<B, N>& p) { return { Rs * p.v }; }

}


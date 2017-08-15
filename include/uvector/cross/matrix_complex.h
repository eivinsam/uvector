#pragma once

#include "../matrix.h"
#include "../complex.h"

namespace uv
{
	template <class T> 
	auto matrix(const Complex<T>& rot) { return rows(vector(real(rot), -imag(rot)), vector(imag(rot), real(rot))); }

	template <class A, class B, int K>
	auto matrix(const Complex<A>& rot, const Vector<B, 3, K>& axis)
	{
		auto nrot = decompose(rot).direction;
		auto naxis = decompose(axis).direction;
		auto c = real(nrot);
		auto cc = 1 - c;
		auto s = imag(nrot);

		return rows
		(
			vector(c, -s*naxis[2], +s*naxis[1]) + naxis[0]*naxis,
			vector(+s*naxis[2], c, -s*naxis[0]) + naxis[1]*naxis,
			vector(-s*naxis[1], +s*naxis[0], c) + naxis[2]*naxis
		);
	}

	template <class T, size_t I>
	auto matrix(const Complex<T>& rot, Axes<I>)
	{
		static_assert(I < 3, "Axis of rotation must be X, Y or Z");
		static constexpr size_t J = (I + 1) % 3;
		static constexpr size_t K = (I + 2) % 3;
		Matrix<type::identity<T>, 3, 3> result = 0;
		rows(result)[I][I] = 1;
		rows(result)[J][J] = real(rot); rows(result)[J][K] = -imag(rot);
		rows(result)[K][J] = imag(rot); rows(result)[K][K] = +real(rot);
		return result;
	}


}

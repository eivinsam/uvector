#pragma once

#include <cmath>

#include "scalar.h"

namespace uv
{
	template <class A, class B, class = std::void_t<if_scalar_t<A>, if_scalar_t<B>>>
	auto max(A a, B b) { return a >= b ? a : b; }
	template <class A, class B, class = std::void_t<if_scalar_t<A>, if_scalar_t<B>>>
	auto min(A a, B b) { return a <= b ? a : b; }

	template <class T>
	auto twice(T v) { return v + v; }

	template <class T, class = if_scalar_t<T>>
	auto square(T value) { return value*value; }

	using std::sqrt;
	using std::cbrt;

	template <class T, class = if_scalar_t<T>>
	auto length(T value) { return std::abs(value); }

	template <class A, class B>
	auto distance(const A& a, const B& b) { return length(a - b); }

	template <class T> // Decomposes vector into direction vector and scalar length
	auto decompose(const T& a) 
	{
		const auto len = length(a);
		struct result_t
		{
			std::decay_t<decltype(a / len)> direction;
			decltype(len) length;
		};
		return result_t{ a / len, len };
	}

	template <class T>
	auto direction(const T& a) { return decompose(a).direction; }

	template <class T> // Check if 'a' is of approximately unit magitude
	bool nearUnit(const T& a)
	{
		using S = decltype(square(a));
		static constexpr auto p = S(0.00001);
		const auto d = square(a) - S(1);
		return -p < d && d < p;
	}
}

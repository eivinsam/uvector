#pragma once

#include <cmath>

#include "scalar.h"

namespace uv
{
	template <class T>
	auto twice(T v) { return v + v; }

	template <class T, class = if_scalar_t<T>>
	auto square(T value) { return value*value; }

	template <class T, class = if_scalar_t<T>>
	auto length(T value) { return std::abs(value); }

	template <class A, class B>
	auto distance(const A& a, const B& b) { return length(a - b); }

	template <class T>
	auto decompose(const T& a) // decomposes vector into direction vector and scalar length
	{
		const auto len = length(a);
		struct result_t
		{
			decltype(a / len) direction;
			decltype(len) length;
		};
		return result_t{ a / len, len };
	}
}

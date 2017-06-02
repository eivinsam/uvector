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
}

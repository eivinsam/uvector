#pragma once

#include "../vector.h"
#include "../bounds.h"

namespace uv
{
	using floatB2 = Vector<Bounds<float>, 2>;
	using floatB3 = Vector<Bounds<float>, 3>;
	using floatB4 = Vector<Bounds<float>, 4>;

	using doubleB2 = Vector<Bounds<double>, 2>;
	using doubleB3 = Vector<Bounds<double>, 3>;
	using doubleB4 = Vector<Bounds<double>, 4>;
}

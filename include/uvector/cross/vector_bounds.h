#pragma once

#include "../vector.h"
#include "../bounds.h"

namespace uv
{
	using Bounds2f = Vector<Bounds<float>, 2>;
	using Bounds3f = Vector<Bounds<float>, 3>;
	using Bounds4f = Vector<Bounds<float>, 4>;

	using Bounds2d = Vector<Bounds<double>, 2>;
	using Bounds3d = Vector<Bounds<double>, 3>;
	using Bounds4d = Vector<Bounds<double>, 4>;
}

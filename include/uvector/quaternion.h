#pragma once

namespace uv { }

#include "scalar.h"
#include "vector.h"

namespace uv
{
	template <class T>
	class Quaternion
	{
	public:
		Vector<T, 4> v;

		Quaternion() { }
		constexpr Quaternion(const Quaternion& q) : v(q.v) { }
	};


	using floatq  = Quaternion<float>;
	using doubleq = Quaternion<double>;
}

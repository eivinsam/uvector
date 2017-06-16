#pragma once

#include "scalar.h"

namespace uv
{
	template <class T, size_t N = 1>
	struct Bounds;

	template <class T>
	struct Bounds<T, 1>
	{
		static_assert(is_scalar_v<T>, "T must be scalar");
		T min;
		T max;

		      T& operator[](bool i)       { return reinterpret_cast<      T*>(this)[i]; }
		const T& operator[](bool i) const { return reinterpret_cast<const T*>(this)[i]; }
	};

	template <class T>
	struct is_scalar<Bounds<T, 1>> : public std::true_type { };

	using Boundsf = Bounds<float>;
	using Boundsd = Bounds<double>;
}

#define UVECTOR_BOUNDS_DEFINED

#ifdef UVECTOR_VECTOR_DEFINED
#include "cross/vector_bounds.h"
#endif

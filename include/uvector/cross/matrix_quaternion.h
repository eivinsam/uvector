#pragma once

#include "../matrix_operations.h"
#include "../quaternion.h"

namespace uv
{
	template <class T, size_t R, size_t C>
	auto quaternion(const Matrix<T, R, C>& m)
	{
		static_assert(R == 3 && C == 3, "Only 3x3 matrices can be converted to quaternions");

		// from http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
		// TODO: presicion for quaternion-matrix-quaternion roundtrip is poor for floats, force conversion to double?

		using namespace axes;

		const auto scale = cbrt(det(m));

		auto mr = rows(m);
		return quaternion(
			copysign(sqrt(max(0, scale + mr[0][0] - mr[1][1] - mr[2][2])) / 2, mr[2][1] - mr[1][2]),
			copysign(sqrt(max(0, scale - mr[0][0] + mr[1][1] - mr[2][2])) / 2, mr[0][2] - mr[2][0]),
			copysign(sqrt(max(0, scale - mr[0][0] - mr[1][1] + mr[2][2])) / 2, mr[1][0] - mr[0][1]),
			         sqrt(max(0, scale + mr[0][0] + mr[1][1] + mr[2][2])) / 2 );
	}

	template <class T>
	auto matrix(const Quaternion<T>& q)
	{
		using namespace axes;
		return cols(q*X, q*Y, q*Z);
	}

}

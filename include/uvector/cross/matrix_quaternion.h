#pragma once

#include "../matrix.h"
#include "../quaternion.h"

namespace uv
{
	template <class T, size_t R, size_t C>
	auto quaternion(const Matrix<T, R, C>& m)
	{
		static_assert(R == 3 && C == 3, "Only 3x3 matrices can be converted to quaternions");

		static constexpr T h = T(0.5);
		static constexpr T q = T(0.25);
		// from http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
		const T tr = m[0][0] + m[1][1] + m[2][2];

		if (tr > 0)
		{
			const T S = h / sqrt(tr + 1); // S=4*qw 
			return quaternion
			(
				(m[1][2] - m[2][1]) * S,
				(m[2][0] - m[0][2]) * S,
				(m[0][1] - m[1][0]) * S,
				q / S
			);
		}
		else if ((m[0][0] > m[1][1]) && (m[0][0] > m[2][2]))
		{
			const T S = h / sqrt(1 + m[0][0] - m[1][1] - m[2][2]); // S=4*qx 
			return quaternion
			(
				q / S,
				(m[1][0] + m[0][1]) * S,
				(m[2][0] + m[0][2]) * S,
				(m[1][2] - m[2][1]) * S
			);
		}
		else if (m[1][1] > m[2][2])
		{
			const T S = h / sqrt(1 + m[1][1] - m[0][0] - m[2][2]); // S=4*qy
			return quaternion
			(
				(m[1][0] + m[0][1]) * S,
				q / S,
				(m[2][1] + m[1][2]) * S,
				(m[2][0] - m[0][2]) * S
			);
		}
		else
		{
			const T S = h / sqrt(1 + m[2][2] - m[0][0] - m[1][1]); // S=4*qz
			return quaternion
			(
				xyz.x = (m[2][0] + m[0][2]) * S,
				xyz.y = (m[2][1] + m[1][2]) * S,
				xyz.z = q / S,
				w = (m[0][1] - m[1][0]) * S
			);
		}

	}

}

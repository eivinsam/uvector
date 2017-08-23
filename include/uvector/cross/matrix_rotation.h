#pragma once

#include "../matrix.h"
#include "../rotation.h"

namespace uv
{
	template <class T, size_t R, size_t C>
	Quat<T> quaternion(const Mat<T, R, C>& m)
	{
		static_assert(R == 3 && C == 3, "Only 3x3 matrices can be converted to quaternions");

		// from http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
		// TODO: presicion for quaternion-matrix-quaternion roundtrip is poor for floats, force conversion to double?

		using namespace axes;

		const auto scale = cbrt(det(m));

		auto& mr = rows(m);
		return quaternion(sqrt(std::max(T(0), scale + mr[0][0] + mr[1][1] + mr[2][2])) / 2,
			vector(
			copysign(sqrt(std::max(T(0), scale + mr[0][0] - mr[1][1] - mr[2][2])) / 2, mr[2][1] - mr[1][2]),
			copysign(sqrt(std::max(T(0), scale - mr[0][0] + mr[1][1] - mr[2][2])) / 2, mr[0][2] - mr[2][0]),
			copysign(sqrt(std::max(T(0), scale - mr[0][0] - mr[1][1] + mr[2][2])) / 2, mr[1][0] - mr[0][1])   ));
	}

	template <class T>
	Mat<T, 3, 3> matrix(const Quat<T>& q)
	{
		using namespace axes;
		// / ww + xx - yy - zz, 2yx - 2zw, 2zx + 2yw \    / xx + ww, yx - zw, zx + yw \
		// | 2xy + 2zw, ww + yy - xx - zz, 2zy - 2xw | = 2| xy + zw, yy + ww, zy - xw | - Iqq = 2(vv' + Xvw) + (ww - v'v)I
		// \ 2xz - 2yw, 2yz + 2xw, ww + zz - xx - yy /    \ xz - yw, yz + xw, zz + ww /
		const auto d = square(q.re) - square(q.im);
		const auto  d_im = q.im + q.im;
		const auto re_im = q.re * d_im;

		return rows(
			d_im*X*q.im + vector(d, -(re_im*Z), +(re_im*Y)),
			d_im*Y*q.im + vector(+(re_im*Z), d, -(re_im*X)),
			d_im*Z*q.im + vector(-(re_im*Y), +(re_im*X), d)
		);
	}

	template <class T>
	Mat<T, 3, 3> matrix(const Rot3<T>& r)
	{
		return matrix(quaternion(r));
	}

	template <class T, size_t R, size_t C>
	Rot3<T> rotation(const Mat<T, R, C>& m)
	{
		return rotation(quaternion(m));
	}

}

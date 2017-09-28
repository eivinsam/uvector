#pragma once

#include "../matrix.h"
#include "../transform.h"

namespace uv
{
	template <class T>
	Mat<type::identity<T>, 4, 4> homogeneous(const Trans3<T>& tf)
	{
		using namespace axes;
		auto R = matrix(tf.r);
		return cols(
			R*X + 0 * W,
			R*Y + 0 * W,
			R*Z + 0 * W,
			X*(tf.t*X/T(1)) + Y*(tf.t*Y/T(1)) + Z*(tf.t*Z/T(1)) + W);
	}

}

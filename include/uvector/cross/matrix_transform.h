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
			R*X + W(0),
			R*Y + W(0),
			R*Z + W(0),
			tf.t/T(1) + W);
	}

}

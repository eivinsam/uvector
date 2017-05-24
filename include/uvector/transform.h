#pragma once

#include "matrix.h"
#include "component.h"

namespace uv
{
	template <class T>
	class transform
	{
		using selectors::xyz;
		using Rotation = mat<T, 3, 3>;
		using Translation = vec<T, 3>;
	public:
		Rotation    R;
		Translation t;

		transform(const transform& T) = default;
		transform() : R(1), t(0) { }

		transform(const Rotation& R) : R(R), t(0) { }
		transform(const Translation& t) : R(1), t(t) { }

		transform(const Rotation& R, const Translation& t) : R(R), t(t) { }
		transform(const mat<T, 4, 4>& HM) : R(xyz(HM.col(0)), xyz(HM.col(1)), xyz(HM.col(2))), t(xyz(HM.col(3))) { }

		transform& operator+=(const Translation& tr) { t += R*tr; return *this; }
		transform& operator*=(const Rotation& rot) {             R *= rot; return *this; }
		transform& operator*=(const transform& T)  { t += R*T.t; R *= T.R; return *this; }
		transform   operator*(const transform& T) const { return transform(*this) *= T; }
		Translation operator*(const Translation& p) const { return R*p + t; }

		template <int K>
		Translation operator*(component<T, K> c) const { return R*c + t; }

		transform inverse() const { Rotation IR = R.inverse(); return { IR, -(IR*t) }; }

		operator mat<T, 4, 4>() const { return matrix_columns(float4(R[0], 0), float4(R[1], 0), float4(R[2], 0), float4(t, 1)); }
	};

}

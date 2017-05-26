#pragma once

#include "matrix.h"
#include "component.h"

namespace uv
{
	template <class T>
	class Transform
	{
		using selectors::xyz;
		using Rotation = Matrix<T, 3, 3>;
		using Translation = Vector<T, 3>;
	public:
		Rotation    R;
		Translation t;

		Transform(const Transform& T) = default;
		Transform() : R(1), t(0) { }

		Transform(const Rotation& R) : R(R), t(0) { }
		Transform(const Translation& t) : R(1), t(t) { }

		Transform(const Rotation& R, const Translation& t) : R(R), t(t) { }
		Transform(const Matrix<T, 4, 4>& HM) : R(xyz(HM.col(0)), xyz(HM.col(1)), xyz(HM.col(2))), t(xyz(HM.col(3))) { }

		Transform& operator+=(const Translation& tr) { t += R*tr; return *this; }
		Transform& operator*=(const Rotation& rot) {             R *= rot; return *this; }
		Transform& operator*=(const Transform& T)  { t += R*T.t; R *= T.R; return *this; }
		Transform   operator*(const Transform& T) const { return Transform(*this) *= T; }
		Translation operator*(const Translation& p) const { return R*p + t; }

		template <int K>
		Translation operator*(Component<T, K> c) const { return R*c + t; }

		Transform inverse() const { Rotation IR = R.inverse(); return { IR, -(IR*t) }; }

		operator Matrix<T, 4, 4>() const { return matrix_columns(float4(R[0], 0), float4(R[1], 0), float4(R[2], 0), float4(t, 1)); }
	};

}

#pragma once

#include "umatrix.h"

namespace uv
{
	template <class T>
	class quaternion
	{
		typedef vec<T, 3> vecT;
		quaternion(const vecT& xyz, T w) : xyz(xyz), w(w) { }
	public:
		vecT xyz;
		T w;

		quaternion() { }
		quaternion(const quaternion& q) : xyz(q.xyz), w(q.w) { }
		quaternion(T x, T y, T z, T w) : xyz(x, y, z), w(w) { }
		template <int K>
		quaternion(T angle, const gvec<T, 3, K>& axis)
		{
			auto ax = axis.normalized();
			angle *= T(0.5);
			xyz = ax*sin(angle);
			w = cos(angle);
		}
		quaternion(const mat<T, 3, 3>& m)
		{
			static const T h = T(0.5);
			static const T q = T(0.25);
			// from http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
			const T tr = m[0][0] + m[1][1] + m[2][2];

			if (tr > 0)
			{
				const T S = h / sqrt(tr + 1); // S=4*qw 
				xyz.x = (m[1][2] - m[2][1]) * S;
				xyz.y = (m[2][0] - m[0][2]) * S;
				xyz.z = (m[0][1] - m[1][0]) * S;
				w = q / S;
			}
			else if ((m[0][0] > m[1][1]) && (m[0][0] > m[2][2]))
			{
				const T S = h / sqrt(1 + m[0][0] - m[1][1] - m[2][2]); // S=4*qx 
				xyz.x = q / S;
				xyz.y = (m[1][0] + m[0][1]) * S;
				xyz.z = (m[2][0] + m[0][2]) * S;
				w = (m[1][2] - m[2][1]) * S;
			}
			else if (m[1][1] > m[2][2])
			{
				const T S = h / sqrt(1 + m[1][1] - m[0][0] - m[2][2]); // S=4*qy
				xyz.x = (m[1][0] + m[0][1]) * S;
				xyz.y = q / S;
				xyz.z = (m[2][1] + m[1][2]) * S;
				w = (m[2][0] - m[0][2]) * S;
			}
			else
			{
				const T S = h / sqrt(1 + m[2][2] - m[0][0] - m[1][1]); // S=4*qz
				xyz.x = (m[2][0] + m[0][2]) * S;
				xyz.y = (m[2][1] + m[1][2]) * S;
				xyz.z = q / S;
				w = (m[0][1] - m[1][0]) * S;
			}
		}
		static const quaternion& identity() { static const quaternion I(0, 0, 0, 1); return quaternion; }

		quaternion& operator+=(const quaternion& q) { xyz += q.xyz; w += q.w; return *this; }
		quaternion& operator-=(const quaternion& q) { xyz -= q.xyz; w -= q.w; return *this; }
		quaternion& operator*=(const quaternion& q) { *this = *this*q; return *this; }

		quaternion operator+(const quaternion& q) const { return quaternion(*this) += q; }
		quaternion operator-(const quaternion& q) const { return quaternion(*this) -= q; }
		quaternion operator*(const quaternion& q) const
		{
			return quaternion(
				q.xyz*w + xyz*q.w + cross(xyz, q.xyz),
				w*q.w - dot(xyz, q.xyz));
		}

		quaternion& operator*=(T s) { xyz *= s; w *= s; return *this; }
		quaternion& operator/=(T s) { xyz /= s; w /= s; return *this; }

		quaternion operator*(T s) const { return quaternion(*this) *= s; }
		quaternion operator/(T s) const { return quaternion(*this) /= s; }

		T square() const { return xyz.square() + w*w; }
		T length() const { return sqrt(square()); }
		quaternion normalized() const { return *this / length(); }

		quaternion operator-() const { return quaternion(-xyz, -w); }
		quaternion conjugate() const { return quaternion(-xyz, +w); }

		vecT X() const { return (vecT(w*w - T(0.5), w*(+xyz.z), w*(-xyz.y)) + xyz*xyz.x) * 2; }
		vecT Y() const { return (vecT(w*(-xyz.z), w*w - T(0.5), w*(+xyz.x)) + xyz*xyz.y) * 2; }
		vecT Z() const { return (vecT(w*(+xyz.y), w*(-xyz.x), w*w - T(0.5)) + xyz*xyz.z) * 2; }

		T angle_to(const quaternion& q) const
		{
			float p = clamp(dot(xyz, q.xyz) + w*q.w, -1, 1);
			return acos(2 * p*p - 1);
		}

		template <int K>
		vecT operator*(const gvec<T, 3, K>& v) { return (v*(w*w - T(0.5)) + cross(xyz, v)*w + xyz*dot(xyz, v)) * 2; }
	};

	template <class T>
	T dot(const quaternion<T>& a, const quaternion<T>& b) { return dot(a.xyz, b.xyz) + a.w * b.w; }

	using floatq  = quaternion<float>;
	using doubleq = quaternion<double>;
}

#pragma once

#include "point.h"
#include "rotation.h"

namespace uv
{
	template <class T>
	class Trans3
	{
		using U = type::identity<T>;
	public:
		Rot3<U> r;
		Vec3<T> t;

		Trans3() = delete;
		Trans3(Identity I) : r(I), t(T(0)) { }
		template <class W>
		Trans3(const Rot3<W>& r) : r(r), t(T(0)) { }
		template <class V, class = if_vector_t<3, V>>
		Trans3(const V& t) : r(identity), t(t) { }
		template <class W, class V, class = if_vector_t<3, V>>
		Trans3(const Rot3<W>& r, const V& t) : r(r), t(t) { }

		template <int K>
		Trans3& operator=(const Vec3<T, K>& translation) { r = 1; t = translation; return *this; }
		Trans3& operator=(const Rot3<U>& rotation) { r = rotation; t = T(0); return *this; }

		template <class B>        friend auto operator*(Trans3 tf, const Rot3<B>& r)    { return tf *= r; }

		template <class B>
		friend Trans3<type::add<T, B>> operator*(const Trans3& a,  const Trans3<B>&    b) { return { a.r*b.r, a.t + a.r*b.t }; }
		template <class V, class = if_vector_t<3, V>> friend auto operator*(const Trans3& tf, const V& v) { return tf.r * v; }
		template <class B>           friend Point3<type::add<T, B>> operator*(const Trans3& tf, const Point3<B>&    p) { return point(tf.r * p.v + tf.t); }

		friend decltype(auto) operator*(const Trans3& tf, Origo<0> o) { return o + tf.t; }
		friend decltype(auto) operator*(const Trans3& tf, Origo<3> o) { return o + tf.t; }

		Trans3& operator*=(const Trans3& b) { *this = *this * b; return *this; }
		Trans3& operator*=(const Rot3<U>& b) { r = r * b; return *this; }

		friend Trans3 invert(Trans3 tf)
		{
			tf.r = invert(tf.r);
			tf.t = -(tf.r*tf.t);
			return tf;
		}

		template <class V, class = if_vector_t<3, V>>
		Trans3 translate(const V& v) { Trans3 result = *this; result.t += v; return result; }
	};
	using Transform3f = Trans3<float>;
	using Transform3d = Trans3<double>;

}

#define UVECTOR_TRANSFORM_DEFINED

#ifdef UVECTOR_MATRIX_DEFINED
#include "cross/matrix_transform.h"
#endif

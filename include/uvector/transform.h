#pragma once

#include "point.h"
#include "matrix.h"

namespace uv
{

	template <class T, size_t N>
	class Transform
	{
	public:
		Matrix<type::identity<T>, N, N> Rs;
		Vector<T, N> t;

		template <class B, int K> friend auto operator+(Transform tf, const Vector<B, N, K>& t)  { tf.t = tf.t + tf.Rs*t;  return tf; }
		template <class B>        friend auto operator*(Transform tf, const Matrix<B, N, N>& Rs) { tf.Rs = tf.Rs*Rs; return tf; }

		template <class B>           friend auto operator*(const Transform& a,  const Transform<B, N>&    b) { return { a.t + a.Rs*b.t, a.Rs * b.Rs }; }
		template <class B, int K>    friend auto operator*(const Transform& tf, const    Vector<B, N, K>& d) { return tf.Rs * d; }
		template <class B>           friend auto operator*(const Transform& tf, const     Point<B, N>&    p) { return tf.Rs * p + tf.t; }
		template <class B, size_t I> friend auto operator*(const Transform& tf, Component<B, I> c)           { return tf.Rs * c + tf.t; }

		friend Transform invert(Transform tf)
		{
			tf.Rs = invert(tf.Rs);
			tf.t = -(tf.Rs*tf.t);
			return tf;
		}

		friend Matrix<T, N + 1, N + 1> homogeneous(const Transform& tf)
		{
			decltype(homogeneous(tf)) result;
			for (size_t i = 0; i < N; ++i)
				for (size_t j = 0; j < N; ++j)
					rows(result)[i][j] = rows(tf.Rs)[i][j];
			for (size_t i = 0; i < N; ++i)
				cols(result)[N][i] = tf.t[i];
			for (size_t i = 0; i < N; ++i)
				rows(result)[N][i] = T(0);
			rows(result)[N][N] = 1;
			return result;
		}
	};
	using Transform2f = Transform<float, 2>;
	using Transform3f = Transform<float, 3>;
	using Transform4f = Transform<float, 4>;
	using Transform2d = Transform<double, 2>;
	using Transform3d = Transform<double, 3>;
	using Transform4d = Transform<double, 4>;

	template <class T, size_t N, int K> auto transform(const Vector<T, N, K>& t)                                            { return Transform<T, N>{ 1,  t }; }
	template <class T, size_t N>        auto transform(const Matrix<type::identity<T>, N, N>& Rs)                           { return Transform<T, N>{ Rs, 0 }; }
	template <class T, size_t N, int K> auto transform(const Matrix<type::identity<T>, N, N>& Rs, const Vector<T, N, K>& t) { return Transform<T, N>{ Rs, t }; }
}

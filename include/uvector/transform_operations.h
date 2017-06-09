#pragma once

#include "transform.h"
#include "matrix_operations.h"

#pragma push_macro("TEMPLATE_TN")
#define TEMPLATE_TN template <class T, size_t N>
#pragma push_macro("TEMPLATE_ABN")
#define TEMPLATE_ABN template <class A, class B, size_t N>
#pragma push_macro("PNT_RESULT")
#define PNT_RESULT(OP) Point<type::of<op::OP, A, B>, N>
#pragma push_macro("DIR_RESULT")
#define DIR_RESULT(OP) Direction<type::of<op::OP, A, B>, N>

namespace uv
{
	TEMPLATE_ABN PNT_RESULT(add) operator+(const Point<A, N>& p, const Direction<B, N>& d) { return { p.v + d }; }
	TEMPLATE_ABN PNT_RESULT(add) operator+(const Direction<A, N>& d, const Point<B, N>& p) { return { d + p.v }; }

	TEMPLATE_ABN DIR_RESULT(sub) operator-(const Point<A, N>& a, const     Point<B, N>& b) { return { a.v - b.v }; }
	TEMPLATE_ABN PNT_RESULT(sub) operator-(const Point<A, N>& a, const Direction<B, N>& b) { return { a.v - b }; }

	TEMPLATE_ABN auto dot(const Point<A, N>& a, const Direction<B, N>& b) { return dot(a.v, b); }
	TEMPLATE_ABN auto dot(const Direction<A, N>& a, const Point<B, N>& b) { return dot(a, b.v); }

	TEMPLATE_ABN PNT_RESULT(mul) operator*(const Matrix<A, N, N>& R, const Point<B, N>& p) { return { R * p.v }; }

	TEMPLATE_TN auto transform(const Direction<T, N>& t) { return Transform<T, N>{ 1, t }; }
	TEMPLATE_TN auto transform(const  Rotation<T, N>& R) { return Transform<T, N>{ R, 0 }; }
	TEMPLATE_TN auto transform(const  Rotation<T, N>& R, const Direction<T, N>& t) { return Transform<T, N>{ R, t }; }

	TEMPLATE_ABN auto operator+(Transform<A, N> tf, const Direction<B, N>& t) { tf.t = tf.t + tf.R*t; return tf; }
	TEMPLATE_ABN auto operator*(Transform<A, N> tf, const  Rotation<B, N>& R) { tf.R =        tf.R*R; return tf; }

	TEMPLATE_ABN auto operator*(const Transform<A, N>& a,  const Transform<B, N>& b) { return { a.t + a.R*b.t, a.R * b.R }; }
	TEMPLATE_ABN DIR_RESULT(mul) operator*(const Transform<A, N>& tf, const Direction<B, N>& d) { return tf.R * d; }
	TEMPLATE_ABN PNT_RESULT(mul) operator*(const Transform<A, N>& tf, const     Point<B, N>& p) { return tf.R * p + tf.t; }


	template <class A, class B, size_t N, size_t I>
	auto operator*(const Transform<A, N>& tf, Component<B, I> c)        { return tf.R * c + tf.t; }

	// Returns the inverse transformation only for orthonormal tf.R
	TEMPLATE_TN Transform<T, N> invert_unsafe(Transform<T, N> tf)
	{
		tf.R = transpose(tf.R);
		tf.t = -(tf.R*tf.t);
		return tf;
	}
	TEMPLATE_TN Transform<T, N> invert(Transform<T, N> tf)
	{
		tf.R = invert(tf.R);
		tf.t = -(tf.R*tf.t);
		return tf;
	}

	template <class T, size_t N> auto homogeneous(const Transform<T, N>& tf)
	{
		Matrix<T, N + 1, N + 1> result;
		for (size_t i = 0; i < N; ++i)
			for (size_t j = 0; j < N; ++j)
				rows(result)[i][j] = rows(tf.R)[i][j];
		for (size_t i = 0; i < N; ++i)
			cols(result)[N][i] = tf.t[i];
		for (size_t i = 0; i < N; ++i)
			rows(result)[N][i] = T(0);
		rows(result)[N][N] = 1;
		return result;
	}

}

#pragma pop_macro("TEMPLATE_TN")
#pragma pop_macro("TEMPLATE_ABN")
#pragma pop_macro("PNT_RESULT")
#pragma pop_macro("DIR_RESULT")
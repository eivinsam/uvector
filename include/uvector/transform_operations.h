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
#define DIR_RESULT(OP) Vector<type::of<op::OP, A, B>, N>
#pragma push_macro("MATRIX_N")
#define MATRIX_N(T) Matrix<type::identity<T>, N, N>

namespace uv
{
	template <class T, size_t N, int K> auto& point(      Vector<T, N, K>& d) { return reinterpret_cast<      Point<T, N, K>&>(d); }
	template <class T, size_t N, int K> auto& point(const Vector<T, N, K>& d) { return reinterpret_cast<const Point<T, N, K>&>(d); }

	TEMPLATE_ABN PNT_RESULT(add) operator+(const Point<A, N>& p, const Vector<B, N>& d) { return { p.v + d }; }
	TEMPLATE_ABN PNT_RESULT(add) operator+(const Vector<A, N>& d, const Point<B, N>& p) { return { d + p.v }; }

	TEMPLATE_ABN DIR_RESULT(sub) operator-(const Point<A, N>& a, const  Point<B, N>& b) { return { a.v - b.v }; }
	TEMPLATE_ABN PNT_RESULT(sub) operator-(const Point<A, N>& a, const Vector<B, N>& b) { return { a.v - b }; }

	TEMPLATE_ABN auto dot(const Point<A, N>& a, const Vector<B, N>& b) { return dot(a.v, b); }
	TEMPLATE_ABN auto dot(const Vector<A, N>& a, const Point<B, N>& b) { return dot(a, b.v); }

	TEMPLATE_ABN auto operator*(const Matrix<A, N, N>& Rs, const Point<B, N>& p) { return point(Rs * p.v); }

	TEMPLATE_TN auto transform(const Vector<T, N>& t) { return Transform<T, N>{ 1, t }; }
	TEMPLATE_TN auto transform(const MATRIX_N(T)& Rs) { return Transform<T, N>{ Rs, 0 }; }
	TEMPLATE_TN auto transform(const MATRIX_N(T)& Rs, const Vector<T, N>& t) { return Transform<T, N>{ Rs, t }; }

	TEMPLATE_ABN auto operator+(Transform<A, N> tf, const Vector<B, N>& t) { tf.t = tf.t + tf.Rs*t; return tf; }
	TEMPLATE_ABN auto operator*(Transform<A, N> tf, const MATRIX_N(B)& Rs) { tf.Rs =        tf.Rs*Rs; return tf; }

	TEMPLATE_ABN auto operator*(const Transform<A, N>& a,  const Transform<B, N>& b) { return { a.t + a.Rs*b.t, a.Rs * b.Rs }; }
	TEMPLATE_ABN auto operator*(const Transform<A, N>& tf, const    Vector<B, N>& d) { return tf.Rs * d; }
	TEMPLATE_ABN auto operator*(const Transform<A, N>& tf, const     Point<B, N>& p) { return tf.Rs * p + tf.t; }


	template <class A, class B, size_t N, size_t I>
	auto operator*(const Transform<A, N>& tf, Component<B, I> c)        { return tf.Rs * c + tf.t; }

	TEMPLATE_TN Transform<T, N> invert(Transform<T, N> tf)
	{
		tf.Rs = invert(tf.Rs);
		tf.t = -(tf.Rs*tf.t);
		return tf;
	}

	template <class T, size_t N> auto homogeneous(const Transform<T, N>& tf)
	{
		Matrix<T, N + 1, N + 1> result;
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

}

#pragma pop_macro("TEMPLATE_TN")
#pragma pop_macro("TEMPLATE_ABN")
#pragma pop_macro("PNT_RESULT")
#pragma pop_macro("DIR_RESULT")
#pragma pop_macro("MATRIX_N")

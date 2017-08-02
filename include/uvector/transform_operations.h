#pragma once

#include "transform.h"
#include "matrix_operations.h"

#pragma push_macro("TEMPLATE_ABN")
#define TEMPLATE_ABN template <class A, class B, size_t N>
#pragma push_macro("TEMPLATE_ABNK")
#define TEMPLATE_ABNK template <class A, class B, size_t N, int K>
#pragma push_macro("MATRIX_N")
#define MATRIX_N(T) Matrix<type::identity<T>, N, N>

namespace uv
{
	template <class T, size_t N> auto& point(      Vector<T, N>& d) { return reinterpret_cast<      Point<T, N>&>(d); }
	template <class T, size_t N> auto& point(const Vector<T, N>& d) { return reinterpret_cast<const Point<T, N>&>(d); }

	template <class T, size_t N, int K> Point<T, N> point(const Vector<T, N, K>& d) { return { d }; }

	template <class T, size_t N, int K> Point<T, N> operator+(const Vector<T, N, K>& v, Origo) { return point(v); }
	template <class T, size_t N, int K> Point<T, N> operator+(Origo, const Vector<T, N, K>& v) { return point(v); }

	template <class T, size_t N> Vector<T, N> operator-(const Point<T, N>& p, Origo) { return p.v; }

	template <class A, class B, size_t NA, size_t NB, size_t NC, int KC>
	auto ifelse(const Vector<bool, NC, KC>& cond, const Point<A, NA>& a, const Point<B, NB>& b)
	{
		static constexpr size_t N = require::equal<NC, require::equal<NA, NB>>;
		Point<std::common_type_t<A, B>, N> result;
		for (size_t i = 0; i < N; ++i)
			result.v[i] = cond[i] ? a.v[i] : b.v[i];
		return result;
	}

	template <class A, size_t N, size_t... I> auto operator*(const Point<A, N>& p, Axes<I...> a) { return point(p.v * a); }
	template <class A, size_t N, size_t... I> auto operator*(Axes<I...> a, const Point<A, N>& p) { return point(a * p.v); }

	TEMPLATE_ABNK Point<type::add<A, B>, N> operator+(const Point<A, N>& p, const Vector<B, N, K>& d) { return point(p.v + d); }
	TEMPLATE_ABNK Point<type::add<A, B>, N> operator+(const Vector<A, N, K>& d, const Point<B, N>& p) { return point(d + p.v); }

	TEMPLATE_ABN  Vector<type::sub<A, B>, N> operator-(const Point<A, N>& a, const  Point<B, N>&    b) { return a.v - b.v; }
	TEMPLATE_ABNK  Point<type::sub<A, B>, N> operator-(const Point<A, N>& a, const Vector<B, N, K>& b) { return point(a.v - b); }

	TEMPLATE_ABN auto operator+(const Point<A, N>& p, B s) { static_assert(is_scalar_v<B>); return point(p.v + s); }
	TEMPLATE_ABN auto operator-(const Point<A, N>& p, B s) { static_assert(is_scalar_v<B>); return point(p.v - s); }
	TEMPLATE_ABN auto operator+(B s, const Point<A, N>& p) { static_assert(is_scalar_v<B>); return point(s + p.v); }

	TEMPLATE_ABN Vector<bool, N> operator==(const Point<A, N>& a, const  Point<B, N>& b) { return a.v == b.v; }
	TEMPLATE_ABN Vector<bool, N> operator!=(const Point<A, N>& a, const  Point<B, N>& b) { return a.v != b.v; }
	TEMPLATE_ABN Vector<bool, N> operator< (const Point<A, N>& a, const  Point<B, N>& b) { return a.v <  b.v; }
	TEMPLATE_ABN Vector<bool, N> operator<=(const Point<A, N>& a, const  Point<B, N>& b) { return a.v <= b.v; }
	TEMPLATE_ABN Vector<bool, N> operator>=(const Point<A, N>& a, const  Point<B, N>& b) { return a.v >= b.v; }
	TEMPLATE_ABN Vector<bool, N> operator> (const Point<A, N>& a, const  Point<B, N>& b) { return a.v >  b.v; }

	TEMPLATE_ABN auto distance(const Point<A, N>& a, const Point<B, N>& b) { return distance(a.v, b.v); }

	template <class T, size_t N> T distance(const Point<T, N>& p, Origo) { return length(p.v); }
	template <class T, size_t N> T distance(Origo, const Point<T, N>& p) { return length(p.v); }

	template <class T, size_t N> bool isfinite(const Point<T, N>& p) { return isfinite(p.v); }

	TEMPLATE_ABNK type::dot<A, B> dot(const Point<A, N>& a, const Vector<B, N, K>& b) { return dot(a.v, b); }
	TEMPLATE_ABNK type::dot<A, B> dot(const Vector<A, N, K>& a, const Point<B, N>& b) { return dot(a, b.v); }

	template <class First, class... Rest, size_t N>
	auto mean(const Point<First, N>& first, const Point<Rest, N>&... rest)
	{
		auto S = sum(first.v, rest.v...);
		return point(S/(1+sizeof...(Rest)));
	}

	TEMPLATE_ABN auto operator*(const Matrix<A, N, N>& Rs, const Point<B, N>& p) { return point(Rs * p.v); }

	template <class T, size_t N, int K> auto transform(const Vector<T, N, K>& t) { return Transform<T, N>{ 1, t }; }
	template <class T, size_t N>        auto transform(const MATRIX_N(T)& Rs) { return Transform<T, N>{ Rs, 0 }; }
	template <class T, size_t N, int K> auto transform(const MATRIX_N(T)& Rs, const Vector<T, N, K>& t) { return Transform<T, N>{ Rs, t }; }

	TEMPLATE_ABNK auto operator+(Transform<A, N> tf, const Vector<B, N, K>& t)  { tf.t  = tf.t + tf.Rs*t;  return tf; }
	TEMPLATE_ABN  auto operator*(Transform<A, N> tf, const Matrix<B, N, N>& Rs) { tf.Rs =        tf.Rs*Rs; return tf; }

	TEMPLATE_ABN  auto operator*(const Transform<A, N>& a,  const Transform<B, N>&    b) { return { a.t + a.Rs*b.t, a.Rs * b.Rs }; }
	TEMPLATE_ABNK auto operator*(const Transform<A, N>& tf, const    Vector<B, N, K>& d) { return tf.Rs * d; }
	TEMPLATE_ABN  auto operator*(const Transform<A, N>& tf, const     Point<B, N>&    p) { return tf.Rs * p + tf.t; }


	template <class A, class B, size_t N, size_t I>
	auto operator*(const Transform<A, N>& tf, Component<B, I> c)        { return tf.Rs * c + tf.t; }

	template <class T, size_t N>
	Transform<T, N> invert(Transform<T, N> tf)
	{
		tf.Rs = invert(tf.Rs);
		tf.t = -(tf.Rs*tf.t);
		return tf;
	}

	template <class T, size_t N> 
	auto homogeneous(const Transform<T, N>& tf)
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

#pragma pop_macro("TEMPLATE_ABN")
#pragma pop_macro("TEMPLATE_ABNK")
#pragma pop_macro("MATRIX_N")

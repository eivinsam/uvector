#pragma once

#include "vector.h"

namespace uv
{
	struct Origo { };
	static constexpr Origo origo = {};

	template <class T, size_t N, int K = 1>
	class Point
	{
	public:
		using scalar_type = T;
		static constexpr size_t dim = N;

		Vector<T, N, K> v;

		Point() { }
		Point(Origo) : v(T(0)) { }
		template <int KB>
		explicit Point(const Vector<T, N, KB>& a) : v(a) { }

		Point& operator=(Origo) { v = T(0); return *this; }

		Vector<T, N> operator-(Origo) const { return -p.v; }

		friend bool isfinite(const Point& p) { return isfinite(p.v); }

		friend T distance(const Point& p, Origo) { return length(p.v); }
		friend T distance(Origo, const Point& p) { return length(p.v); }

		template <class S> friend auto distance(const Point& a, const Point<S, N>& b) { return distance(a.v, b.v); }

		template <class S, int K> friend type::mul<T, S> dot(const Point& a, const Vector<S, N, K>& b) { return dot(a.v, b); }
		template <class S, int K> friend type::mul<S, T> dot(const Vector<S, N, K>& a, const Point& b) { return dot(a, b.v); }

		template <size_t I> friend decltype(auto) operator*(const Point& p, Axes<I> a) { return p.v * a; }
		template <size_t I> friend decltype(auto) operator*(Axes<I> a, const Point& p) { return a * p.v; }

		template <class S, class = if_scalar_t<S>> friend auto operator+(const Point& p, S s) { return Point<type::add<T, S>, N>(p.v + s); }
		template <class S, class = if_scalar_t<S>> friend auto operator-(const Point& p, S s) { return Point<type::add<T, S>, N>(p.v - s); }
		template <class S, class = if_scalar_t<S>> friend auto operator+(S s, const Point& p) { return Point<type::add<T, S>, N>(s + p.v); }

		template <class S, int K> friend auto operator+(const Point& p, const Vector<S, N, K>& d) { return Point<type::add<T, S>, N>(p.v + d); }
		template <class S, int K> friend auto operator+(const Vector<S, N, K>& d, const Point& p) { return Point<type::add<T, S>, N>(d + p.v); }
		template <class S, int K> friend auto operator-(const Point& p, const Vector<S, N, K>& d) { return Point<type::add<T, S>, N>(p.v - d); }
		template <class S>        friend Vector<type::add<T, S>, N> operator-(const Point& a, const Point<S, N>& b) { return a.v - b.v; }

		template <class S> Vector<bool, N> operator==(const Point<S, N>& b) const { return v == b.v; }
		template <class S> Vector<bool, N> operator!=(const Point<S, N>& b) const { return v != b.v; }
		template <class S> Vector<bool, N> operator< (const Point<S, N>& b) const { return v <  b.v; }
		template <class S> Vector<bool, N> operator<=(const Point<S, N>& b) const { return v <= b.v; }
		template <class S> Vector<bool, N> operator>=(const Point<S, N>& b) const { return v >= b.v; }
		template <class S> Vector<bool, N> operator> (const Point<S, N>& b) const { return v >  b.v; }
	};
	using Point2f = Point<float, 2>;
	using Point3f = Point<float, 3>;
	using Point4f = Point<float, 4>;
	using Point2d = Point<double, 2>;
	using Point3d = Point<double, 3>;
	using Point4d = Point<double, 4>;

	template <class T, size_t N> auto& point(Vector<T, N>& d) { return reinterpret_cast<      Point<T, N>&>(d); }
	template <class T, size_t N> auto& point(const Vector<T, N>& d) { return reinterpret_cast<const Point<T, N>&>(d); }

	template <class T, size_t N, int K> Point<T, N> point(const Vector<T, N, K>& d) { return { d }; }

	template <class T, size_t N, int K> Point<T, N> operator+(const Vector<T, N, K>& v, Origo) { return { v }; }
	template <class T, size_t N, int K> Point<T, N> operator+(Origo, const Vector<T, N, K>& v) { return { v }; }

	template <class A, class B, size_t NA, size_t NB, size_t NC, int KC>
	auto ifelse(const Vector<bool, NC, KC>& cond, const Point<A, NA>& a, const Point<B, NB>& b)
	{
		static constexpr size_t N = require::equal<NC, require::equal<NA, NB>>;
		Point<std::common_type_t<A, B>, N> result;
		for (size_t i = 0; i < N; ++i)
			result.v[i] = cond[i] ? a.v[i] : b.v[i];
		return result;
	}

	template <class... TN, size_t N>
	auto mean(const Point<TN, N>&... points)
	{
		return point(sum(points.v...) / sizeof...(TN));
	}
}

#define UVECTOR_POINT_DEFINED

#ifdef UVECTOR_MATRIX_DEFINED
#include "cross/matrix_point.h"
#endif

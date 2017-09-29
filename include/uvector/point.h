#pragma once

#include "vector.h"

namespace uv
{
	template <size_t N>
	struct Origo { };
	static constexpr Origo<0> origo = {};
	static constexpr Origo<2> origo2 = {};
	static constexpr Origo<3> origo3 = {};
	static constexpr Origo<4> origo4 = {};

	template <class T, size_t N, int K = 1>
	class Point
	{
	public:
		using scalar_type = T;
		static constexpr size_t dim = N;

		Vec<T, N, K> v;

		Point() { }
		constexpr Point(Origo<0>) : v(T(0)) { }
		constexpr Point(Origo<N>) : v(T(0)) { }
		template <class S, int KB>
		constexpr Point(const Point<S, N, KB>& a) : v(a.v) { }
		template <class... S, class = if_scalars_t<N, S...>>
		constexpr Point(S... s) : v(s...) { }
		template <class V, class = if_vector_t<N, V>>
		explicit constexpr Point(const V& a) : v(a) { }

		Point& operator=(Origo<0>) { v = T(0); return *this; }
		Point& operator=(Origo<N>) { v = T(0); return *this; }

		friend const Vec<T, N, K>& operator-(const Point& p, Origo<0>) { return  p.v; }
		friend const Vec<T, N, K>& operator-(const Point& p, Origo<N>) { return  p.v; }
		friend       Vec<T, N>     operator-(Origo<0>, const Point& p) { return -p.v; }
		friend       Vec<T, N>     operator-(Origo<N>, const Point& p) { return -p.v; }

		friend bool isfinite(const Point& p) { return isfinite(p.v); }

		friend T distance(const Point& p, Origo<0>) { return length(p.v); }
		friend T distance(const Point& p, Origo<N>) { return length(p.v); }
		friend T distance(Origo<0>, const Point& p) { return length(p.v); }
		friend T distance(Origo<N>, const Point& p) { return length(p.v); }

		template <class S> friend auto distance(const Point& a, const Point<S, N>& b) { return distance(a.v, b.v); }


		template <size_t I>       T& operator[](Axes<I> a)       { return v[a]; }
		template <size_t I> const T& operator[](Axes<I> a) const { return v[a]; }

		template <size_t... I> Point<T, sizeof...(I)> operator[](Axes<I...> a) const { return point(v[a]); }

		template <class S, class = if_scalar_t<S>> friend auto operator+(const Point& p, S s) { return Point<type::add<T, S>, N>(p.v + s); }
		template <class S, class = if_scalar_t<S>> friend auto operator-(const Point& p, S s) { return Point<type::add<T, S>, N>(p.v - s); }
		template <class S, class = if_scalar_t<S>> friend auto operator+(S s, const Point& p) { return Point<type::add<T, S>, N>(s + p.v); }

		template <class S>        friend auto operator-(const Point& a, const Point<S, N>& b) { return a.v - b.v; }

		template <class S> Vec<bool, N> operator==(const Point<S, N>& b) const { return v == b.v; }
		template <class S> Vec<bool, N> operator< (const Point<S, N>& b) const { return v <  b.v; }
	};

	template <class T> using Point2 = Point<T, 2>;
	template <class T> using Point3 = Point<T, 3>;
	template <class T> using Point4 = Point<T, 4>;

	using Point2f = Point<float, 2>;
	using Point3f = Point<float, 3>;
	using Point4f = Point<float, 4>;
	using Point2d = Point<double, 2>;
	using Point3d = Point<double, 3>;
	using Point4d = Point<double, 4>;

	template <class T, size_t N> auto& point(Vec<T, N>& d) { return reinterpret_cast<      Point<T, N>&>(d); }
	template <class T, size_t N> auto& point(const Vec<T, N>& d) { return reinterpret_cast<const Point<T, N>&>(d); }

	template <class T, size_t N, int K> const Point<T, N, K>& point(const Vec<T, N, K>& d) { return reinterpret_cast<const Point<T, N, K>&>(d); }

	template <class T, size_t N, int K, class V, class = if_vector_t<N, V>> type::mul<T, scalar<V>> dot(const Point<T, N, K>& a, const V& b) { return dot(a.v, b); }
	template <class T, size_t N, int K, class V, class = if_vector_t<N, V>> type::mul<scalar<V>, T> dot(const V& a, const Point<T, N, K>& b) { return dot(a, b.v); }

	template <class T, size_t N, int K, class V, class = if_vector_t<N, V>> auto operator+(const Point<T, N, K>& p, const V& d) { return Point<type::add<T, scalar<V>>, N>(p.v + d); }
	template <class T, size_t N, int K, class V, class = if_vector_t<N, V>> auto operator+(const V& d, const Point<T, N, K>& p) { return Point<type::add<T, scalar<V>>, N>(d + p.v); }
	template <class T, size_t N, int K, class V, class = if_vector_t<N, V>> auto operator-(const Point<T, N, K>& p, const V& d) { return Point<type::add<T, scalar<V>>, N>(p.v - d); }

	template <class T, size_t N, int K> const Point<T, N, K>& operator+(const Vec<T, N, K>& v, Origo<0>) { return reinterpret_cast<const Point<T, N, K>&>(v); }
	template <class T, size_t N, int K> const Point<T, N, K>& operator+(const Vec<T, N, K>& v, Origo<N>) { return reinterpret_cast<const Point<T, N, K>&>(v); }
	template <class T, size_t N, int K> const Point<T, N, K>& operator+(Origo<0>, const Vec<T, N, K>& v) { return reinterpret_cast<const Point<T, N, K>&>(v); }
	template <class T, size_t N, int K> const Point<T, N, K>& operator+(Origo<N>, const Vec<T, N, K>& v) { return reinterpret_cast<const Point<T, N, K>&>(v); }

	template <class T, size_t I, size_t N> constexpr Point<T, N> operator+(const Component<T, I>& c, Origo<N> o) { return Vec<T, N>(c) + o; }
	template <class T, size_t I, size_t N> constexpr Point<T, N> operator+(Origo<N> o, const Component<T, I>& c) { return o + Vec<T, N>(c); }

	template <class A, class B, size_t NA, size_t NB, size_t NC, int KC>
	auto ifelse(const Vec<bool, NC, KC>& cond, const Point<A, NA>& a, const Point<B, NB>& b)
	{
		static constexpr size_t N = require::equal<NC, require::equal<NA, NB>>;
		Point<std::common_type_t<A, B>, N> result;
		for (size_t i = 0; i < N; ++i)
			result.v[i] = cond[i] ? a.v[i] : b.v[i];
		return result;
	}

	template <class A, class B, class C, size_t NA, size_t NB>
	auto interpolate(C c, const Point<A, NA>& a, const Point<B, NB>& b) { return origo + interpolate(c, a.v, b.v); }

	template <class... TN, size_t N>
	auto mean(const Point<TN, N>&... points)
	{
		return point(sum(points.v...) / sizeof...(TN));
	}
	template <class T, size_t N, int K>
	std::ostream& operator<<(std::ostream& out, const Point<T, N, K>& v)
	{
		out << '(' << v.v[0];
		for (size_t i = 1; i < N; ++i)
			out << ", " << v.v[i];
		return out << ')';
	}
}

#define UVECTOR_POINT_DEFINED

#ifdef UVECTOR_MATRIX_DEFINED
#include "cross/matrix_point.h"
#endif

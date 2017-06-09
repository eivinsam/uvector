#pragma once

#include <algorithm>

#include "bounds.h"

#pragma push_macro("TEMPLATE_SCALAR_A_BOUNDS_B")
#define TEMPLATE_SCALAR_A_BOUNDS_B template <class A, class B, class = if_scalar<A>>

namespace uv
{
	template <class T>
	auto bounds_minmax(T min, T max)
	{
		Bounds<T> result;
		result.min = min;
		result.max = max;
		return result;
	}

	template <class T>
	constexpr Bounds<T> bounds(T value) { Bounds<T> r; r.min = r.max = value; return r; }
	template <class T, class A, class B> 
	constexpr Bounds<T> bounds(A a, B b) { return a < b ? bounds_minmax<T>(a, b) : bounds_minmax<T>(b, a); }

	template <class T, class First, class... Rest>
	constexpr Bounds<T> bounds(First first, Rest... rest)
	{
		Bounds<T> result = bounds<T>(rest...);
		if (result.min > first)
			result.min = first;
		if (result.max < first)
			result.max = first;
		return result;
	}

	template <class First, class... Rest>
	constexpr Bounds<First> bounds(First first, Rest... rest) { return bounds<First>(first, rest...); }

	TEMPLATE_SCALAR_A_BOUNDS_B constexpr auto operator+(A a, const Bounds<B>& b) { return bounds_minmax(a + b.min, a + b.max); }
	TEMPLATE_SCALAR_A_BOUNDS_B constexpr auto operator-(A a, const Bounds<B>& b) { return bounds_minmax(a - b.max, a - b.min); }
	TEMPLATE_SCALAR_A_BOUNDS_B constexpr auto operator*(A a, const Bounds<B>& b) { return bounds(a*b.min, a*b.max); }
	TEMPLATE_SCALAR_A_BOUNDS_B constexpr auto operator/(A a, const Bounds<B>& b) { return bounds(a / b.min, a / b.max); }

	TEMPLATE_SCALAR_A_BOUNDS_B constexpr auto operator+(const Bounds<B>& b, A a) { return bounds_minmax(b.min + a, b.max + a); }
	TEMPLATE_SCALAR_A_BOUNDS_B constexpr auto operator-(const Bounds<B>& b, A a) { return bounds_minmax(b.max - a, b.min - a); }
	TEMPLATE_SCALAR_A_BOUNDS_B constexpr auto operator*(const Bounds<B>& b, A a) { return bounds(b.min * a, b.max * a); }
	TEMPLATE_SCALAR_A_BOUNDS_B constexpr auto operator/(const Bounds<B>& b, A a) { return bounds(b.min / b, b.max / b); }

	template <class A> constexpr Bounds<A> operator-(const Bounds<A>& a) { return { -a.min, -a.max }; }

	template <class T> constexpr auto operator|(const Bounds<T>& a, const Bounds<T>& b) { return { std::min(a.min, b.min), std::max(a.max, b.max) }; }
	template <class T> constexpr auto operator&(const Bounds<T>& a, const Bounds<T>& b) { return { std::max(a.min, b.min), std::min(a.max, b.max) }; }

	template <class A, class B> constexpr auto operator+(const Bounds<A>& a, const Bounds<B>& b) { return bounds_minmax(a.min + b.min, a.max + b.max); }
	template <class A, class B> constexpr auto operator-(const Bounds<A>& a, const Bounds<B>& b) { return bounds_minmax(a.min - b.max, a.max - b.min); }
	template <class A, class B> constexpr auto operator*(const Bounds<A>& a, const Bounds<B>& b) { return a.min*b | a.max*b; }
	template <class A, class B> constexpr auto operator/(const Bounds<A>& a, const Bounds<B>& b) { return a.min/b | a.max/b; }
}

#pragma pop_macro("TEMPLATE_SCALAR_A_BOUNDS_B")

#pragma once

#include <algorithm>

#include "bounds.h"
#include "details/vector_details.h"

#pragma push_macro("TEMPLATE_BOUNDS_AB")
#define TEMPLATE_BOUNDS_AB template <class A, class B, size_t NA, size_t NB>
#pragma push_macro("TEMPLATE_SCALAR_A_BOUNDS_B")
#define TEMPLATE_SCALAR_A_BOUNDS_B template <class A, class B, class = if_scalar<A>>

namespace uv
{
	namespace details
	{
		template <class T>
		struct vector_and<Bounds<T, 1>>
		{
			template <class OP, class V>
			static auto apply(const V& v, Bounds<T, 1> b) { return scalar_apply<OP>(v, b); }
		};
		template <class T, size_t N>
		struct vector_and<Bounds<T, N>>
		{
			template <class OP, class V>
			static auto apply(const V& v, const Bounds<T, N>& b) { return vector_apply<OP>(v, b); }
		};

		template <class T, class S>
		void update(Bounds<T>& dst, S src)
		{
			if (dst.min > src)
			{
				dst.min = src;
				return;
			}
			if (dst.max < src)
				dst.max = src;
		}

		template <class T, size_t N>
		struct bounds_builder
		{
			template <class Last>
			static Bounds<T, N> build(Last last)
			{ 
				static_assert(is_scalar_v<Last>);
				Bounds<T, N> result;
				for (size_t i = 0; i < N; ++i)
					result[i].max = result[i].min = last;
				return result;
			}
			template <class A, size_t NA, int KA>
			static Bounds<T, N> build(const Vector<A, NA, KA>& last)
			{
				require::equal<N, NA>;
				Bounds<T, N> result;
				for (size_t i = 0; i < N; ++i)
					result[i].max = result[i].min = last[i];
				return result;
			}

			template <class First, class... Rest>
			static Bounds<T, N> build(First first, const Rest&... rest)
			{
				static_assert(is_scalar_v<First>);
				Bounds<T, N> result = build(rest...);
				for (size_t i = 0; i < N; ++i)
					update(result[i], first);
				return result;
			}
			template <class A, size_t NA, int KA, class... Rest>
			static Bounds<T, N> build(const Vector<A, NA, KA>& first, const Rest&... rest)
			{
				require::equal<N, NA>;
				Bounds<T, N> result = build(rest...);
				for (size_t i = 0; i < N; ++i)
					update(result[i], first[i]);
				return result;
			}
		};
		template <class T>
		struct bounds_builder<T, 1>
		{
			template <class Last>
			static Bounds<T> build(Last last)
			{
				static_assert(is_scalar_v<Last>);
				Bounds<T> result;
				result.max = result.min = last;
			}
			template <class First, class... Rest>
			static Bounds<T> build(First first, Rest... rest)
			{
				static_assert(is_scalar_v<First>);
				Bounds<T> result = build(rest...);
				update(result, first);
				return result;
			}
		};
	}

	template <class A, class B>
	auto bounds_minmax(A minimum, B maximum)
	{
		static_assert(is_scalar_v<A>);
		static_assert(is_scalar_v<B>);
		Bounds<std::common_type_t<scalar<A>, scalar<B>>> result;
		result.min = minimum;
		result.max = maximum;
		return result;
	}

	template <class T, size_t N, class... Args>
	Bounds<T, N> bounds(const Args&... args) { return details::bounds_builder<T, N>::build(args...); }

	template <class First, class... Rest>
	auto bounds(const First& first, const Rest&... rest) { return bounds<scalar<First>, dim<First>>(first, rest...); }


	TEMPLATE_SCALAR_A_BOUNDS_B constexpr auto operator+(A a, const Bounds<B>& b) { return bounds_minmax(a + min(b), a + max(b)); }
	TEMPLATE_SCALAR_A_BOUNDS_B constexpr auto operator-(A a, const Bounds<B>& b) { return bounds_minmax(a - max(b), a - min(b)); }
	TEMPLATE_SCALAR_A_BOUNDS_B constexpr auto operator*(A a, const Bounds<B>& b) { return bounds(a*min(b), a*max(b)); }
	TEMPLATE_SCALAR_A_BOUNDS_B constexpr auto operator/(A a, const Bounds<B>& b) { return bounds(a / b.min, a / b.max); }
	TEMPLATE_SCALAR_A_BOUNDS_B constexpr auto operator+(const Bounds<B>& b, A a) { return bounds_minmax(min(b) + a, max(b) + a); }
	TEMPLATE_SCALAR_A_BOUNDS_B constexpr auto operator-(const Bounds<B>& b, A a) { return bounds_minmax(max(b) - a, min(b) - a); }
	TEMPLATE_SCALAR_A_BOUNDS_B constexpr auto operator*(const Bounds<B>& b, A a) { return bounds(min(b) * a, max(b) * a); }
	TEMPLATE_SCALAR_A_BOUNDS_B constexpr auto operator/(const Bounds<B>& b, A a) { return bounds(min(b) / b, max(b) / b); }

	template <class A> constexpr Bounds<A> operator-(const Bounds<A>& a) { return bounds_minmax( -max(a), -min(a) ); }

	TEMPLATE_BOUNDS_AB constexpr auto operator|(const Bounds<A, NA>& a, const Bounds<B, NB>& b) { return bounds_minmax(min(min(a), min(b)), max(max(a), max(b))); }
	TEMPLATE_BOUNDS_AB constexpr auto operator&(const Bounds<A, NA>& a, const Bounds<B, NB>& b) { return bounds_minmax(max(min(a), min(b)), min(max(a), max(b))); }

	template <class A, class B> constexpr auto operator+(const Bounds<A>& a, const Bounds<B>& b) { return bounds_minmax(a.min + b.min, a.max + b.max); }
	template <class A, class B> constexpr auto operator-(const Bounds<A>& a, const Bounds<B>& b) { return bounds_minmax(a.min - b.max, a.max - b.min); }
	template <class A, class B> constexpr auto operator*(const Bounds<A>& a, const Bounds<B>& b) { return min(a)*b | max(a)*b; }
	template <class A, class B> constexpr auto operator/(const Bounds<A>& a, const Bounds<B>& b) { return min(a)/b | max(a)/b; }
}

#pragma pop_macro("TEMPLATE_BOUNDS_AB")
#pragma pop_macro("TEMPLATE_SCALAR_A_BOUNDS_B")

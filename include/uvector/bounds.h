#pragma once

#include "vector.h"

namespace uv
{
	struct Empty { };
	static constexpr Empty empty = {};

	template <class T>
	struct Bounds
	{
	private:
		constexpr Bounds(T min, T max) : min(min), max(max) { }
	public:
		T min;
		T max;

		Bounds() = default;
		constexpr Bounds(T value) { *this = value; }
		constexpr Bounds(Empty) { *this = empty; }

		constexpr Bounds& operator=(T value) { min = max = value; return *this; }
		constexpr Bounds& operator=(Empty) { min = T(1); max = T(0); return *this; }

		template <class B>
		friend Bounds<type::common<T, B>> intersect(const Bounds& a, const Bounds<B>& b)
		{
			return { max(a.min, b.min), min(a.max, b.max) };
		}

		friend type::add<T> size(const Bounds& b) { return b ? b.max - b.min: type::add<T>(0); }

		template <class T>
		T mean(const Bounds<T>& b) { return (max(b) + min(b)) / 2; }

		template <class S, class = if_scalar_t<S>> friend constexpr Bounds<type::add<S, T>> operator+(S a, const Bounds& b) { return { a + b.min, a + b.max }; }
		template <class S, class = if_scalar_t<S>> friend constexpr Bounds<type::add<S, T>> operator-(S a, const Bounds& b) { return { a - b.max, a - b.min }; }
		template <class S, class = if_scalar_t<S>> friend constexpr Bounds<type::mul<S, T>> operator*(S a, const Bounds& b) { return bounds(a*b.min, a*b.max); }
		template <class S, class = if_scalar_t<S>> friend constexpr Bounds<type::div<S, T>> operator/(S a, const Bounds& b) { return bounds(a / b.min, a / b.max); }


		template <class S, class = if_scalar_t<S>> friend constexpr Bounds<type::add<T, S>> operator+(const Bounds& b, S a) { return { b.min + a, b.max + a }; }
		template <class S, class = if_scalar_t<S>> friend constexpr Bounds<type::add<T, S>> operator-(const Bounds& b, S a) { return { b.max - a, b.min - a }; }
		template <class S, class = if_scalar_t<S>> friend constexpr Bounds<type::mul<T, S>> operator*(const Bounds& b, S a) { return bounds(b.min * a, b.max * a); }
		template <class S, class = if_scalar_t<S>> friend constexpr Bounds<type::div<T, S>> operator/(const Bounds& b, S a) { return bounds(b.min / b, b.max / b); }

		template <class A> constexpr Bounds operator-() const { return { -max, -min }; }


		template <class S> constexpr Bounds<type::add<T, S>> operator+(const Bounds<S>& b) const { return bounds_minmax(min + b.min, max + b.max); }
		template <class S> constexpr Bounds<type::add<T, S>> operator-(const Bounds<S>& b) const { return bounds_minmax(min - b.max, max - b.min); }
		template <class S> constexpr Bounds<type::mul<T, S>> operator*(const Bounds<S>& b) const { return (max*b) | (min*b); }
		template <class S> constexpr Bounds<type::div<T, S>> operator/(const Bounds<S>& b) const { return (max / b) | (min / b); }


		      T& operator[](bool i)       { return reinterpret_cast<      T*>(this)[i]; }
		const T& operator[](bool i) const { return reinterpret_cast<const T*>(this)[i]; }

		explicit operator bool() const { return max >= min; }
	};

	using foo = decltype(Bounds<float>(0) * 0.0f);
	using bar = decltype(std::declval<foo>() + std::declval<foo>());
	static_assert(std::is_same_v<foo, bar>);

	namespace details
	{
		template <class T> struct BoundsOf { using type = Bounds<T>; };
		template <class T> struct BoundsOf<T&> : BoundsOf<T> { };
		template <class T> struct BoundsOf<const T> : BoundsOf<T> { };

		template <class T>
		struct BoundsOf<Bounds<T>> { using type = Bounds<T>; };
		template <class T, size_t N, int K>
		struct BoundsOf<Vector<T, N, K>> { using type = Vector<typename BoundsOf<T>::type, N, K>; };
	}
	using Boundsf = Bounds<float>;
	using Boundsd = Bounds<double>;

	using Bounds2f = Vector<Bounds<float>, 2>;
	using Bounds3f = Vector<Bounds<float>, 3>;
	using Bounds4f = Vector<Bounds<float>, 4>;

	using Bounds2d = Vector<Bounds<double>, 2>;
	using Bounds3d = Vector<Bounds<double>, 3>;
	using Bounds4d = Vector<Bounds<double>, 4>;

	template <class T>
	struct is_scalar<Bounds<T>> : std::true_type { };

	namespace type
	{
		template <class T> 
		using bounds = details::BoundsOf<T>;
	}

	template <class T>
	type::bounds<T> bounds(const T& last) { return type::bounds<T>(last); }
	template <class T, class Last, size_t N, int K>
	Vector<Bounds<T>, N, K> bounds(const Vector<Last, N, K>& last)
	{
		decltype(bounds(last)) result;
		for (size_t i = 0; i < N; ++i)
			result[i] = last[i];
		return result;
	}
	template <class A, class B>
	auto bounds(const A& a, const B& b)
	{
		auto altb = a < b;
		type::common<A, B> result;
		min(result) = ifelse(altb, a, b);
		max(result) = ifelse(altb, b, a);
		return result;
	}

	template <class T, class A, class B>
	Bounds<T> bounds(const A& a, const B& b) { return bounds_minmax(uv::min(a, b), uv::max(a, b)); }
	template <class T, class First, class... Rest>
	Bounds<T> bounds(const First& first, const Rest&... rest) { return bounds<T>(first, bounds<T>(rest...)); }

	template <class First, class... Rest>
	Bounds<First> bounds(const First& first, const Rest&... rest) { return bounds<First, First, Rest...>(first, rest...); }



	template <class A, class B>
	auto bounds_minmax(const A& minimum, const B& maximum)
	{
		static_assert(is_scalar_v<A>);
		static_assert(is_scalar_v<B>);
		Bounds<type::common<A, B>> result;
		result.min = minimum;
		result.max = maximum;
		return result;
	}
}

namespace std
{
	template <class A, class B>
	struct common_type<A, uv::Bounds<B>> { using type = uv::Bounds<common_type_t<A, B>>; };
	template <class A, class B>
	struct common_type<uv::Bounds<A>, B> { using type = uv::Bounds<common_type_t<A, B>>; };
	template <class A, class B>
	struct common_type<uv::Bounds<A>, uv::Bounds<B>> { using type = uv::Bounds<common_type_t<A, B>>; };
}


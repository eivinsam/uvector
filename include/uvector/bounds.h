#pragma once

#include "vector.h"

namespace uv
{
	struct Empty { };
	static constexpr Empty empty = {};

	template <>
	struct is_scalar<Empty> : std::true_type { };

	template <class T>
	struct Bounds;

	namespace details
	{
		template <class T> struct BoundsOf { using type = Bounds<T>; };
		template <class T> struct BoundsOf<T&> : BoundsOf<T> { };
		template <class T> struct BoundsOf<const T> : BoundsOf<T> { };

		template <class T> 
		struct BoundsOf<Bounds<T>> : BoundsOf<T> { };
		template <class T, size_t N, int K>
		struct BoundsOf<Vec<T, N, K>> 
		{
			using type = Vec<typename BoundsOf<T>::type, N, K>;
		};
	}
	namespace type
	{
		template <class T>
		using bounds = typename details::BoundsOf<T>::type;
	}

	template <class T> struct is_simple_scalar : is_scalar<T> { };
	template <class T> struct is_simple_scalar<Bounds<T>> : std::false_type { };
	template <class T> 
	static constexpr bool is_simple_scalar_v = is_simple_scalar<T>::value;
	template <class T>
	using if_simple_scalar_t = std::enable_if_t<is_simple_scalar<T>::value>;

	template <class T, class = if_simple_scalar_t<T>> constexpr T min(T value) { return value; }
	template <class T, class = if_simple_scalar_t<T>> constexpr T max(T value) { return value; }

	template <class T>
	struct Bounds
	{
		static_assert(std::numeric_limits<T>::is_specialized, "uv::Bounds<T> requires specialization of std::numeric_limits<T>");
		static constexpr T no_min = std::numeric_limits<T>::has_infinity ?  std::numeric_limits<T>::infinity() : std::numeric_limits<T>::max();
		static constexpr T no_max = std::numeric_limits<T>::has_infinity ? -std::numeric_limits<T>::infinity() : std::numeric_limits<T>::lowest();
	private:
		constexpr Bounds(T min, T max) : min(min), max(max) { if (max < min) *this = empty; }
	public:
		T min;
		T max;

		Bounds() = default;
		constexpr Bounds(T value) : min(value), max(value) { }
		constexpr Bounds(Empty) : min(no_min), max(no_max) {}

		constexpr Bounds& operator=(T value) { min = max = value; return *this; }
		constexpr Bounds& operator=(Empty) { min = no_min; max = no_max; return *this; }

		friend T& min(Bounds& b) { return b.min; }
		friend T& max(Bounds& b) { return b.max; }
		friend const T& min(const Bounds& b) { return b.min; }
		friend const T& max(const Bounds& b) { return b.max; }

		template <size_t N, int K> friend auto& min(Vec<Bounds<T>, N, K>& v) { return reinterpret_cast<Vec<T, N, K * 2>&>(v[0].min); }
		template <size_t N, int K> friend auto& max(Vec<Bounds<T>, N, K>& v) { return reinterpret_cast<Vec<T, N, K * 2>&>(v[0].max); }
		template <size_t N, int K> friend auto& min(const Vec<Bounds<T>, N, K>& v) { return reinterpret_cast<const Vec<T, N, K * 2>&>(v[0].min); }
		template <size_t N, int K> friend auto& max(const Vec<Bounds<T>, N, K>& v) { return reinterpret_cast<const Vec<T, N, K * 2>&>(v[0].max); }

		template <class B>
		friend Bounds<type::common<T, B>> intersect(const Bounds& a, const Bounds<B>& b)
		{
			return { max(a.min, b.min), min(a.max, b.max) };
		}

		template <class S, class = if_simple_scalar_t<S>> friend constexpr auto operator+(const S& a, const Bounds& b) { return Bounds<type::add<T, S>>{ a + b.min, a + b.max }; }
		template <class S, class = if_simple_scalar_t<S>> friend constexpr auto operator-(const S& a, const Bounds& b) { return Bounds<type::add<T, S>>{ a - b.max, a - b.min }; }
		template <class S, class = if_simple_scalar_t<S>> friend constexpr auto operator+(const Bounds& b, const S& a) { return Bounds<type::add<T, S>>{ b.min + a, b.max + a }; }
		template <class S, class = if_simple_scalar_t<S>> friend constexpr auto operator-(const Bounds& b, const S& a) { return Bounds<type::add<T, S>>{ b.max - a, b.min - a }; }

		template <class A> constexpr Bounds operator-() const { return { -max, -min }; }

		template <class S> constexpr Bounds<type::add<T, S>> operator+(const Bounds<S>& b) const { return { min + b.min, max + b.max }; }
		template <class S> constexpr Bounds<type::add<T, S>> operator-(const Bounds<S>& b) const { return { min - b.max, max - b.min }; }
		template <class S> constexpr Bounds<type::mul<T, S>> operator*(const Bounds<S>& b) const { return bounds(max * b, min * b); }
		template <class S> constexpr Bounds<type::div<T, S>> operator/(const Bounds<S>& b) const { return bounds(max / b, min / b); }


		      T& operator[](bool i)       { return reinterpret_cast<      T*>(this)[i]; }
		const T& operator[](bool i) const { return reinterpret_cast<const T*>(this)[i]; }

		explicit operator bool() const { return max >= min; }
	};
	using Boundsf = Bounds<float>;
	using Boundsd = Bounds<double>;

	template <class T> using Bounds2 = Vec<Bounds<T>, 2>;
	template <class T> using Bounds3 = Vec<Bounds<T>, 3>;
	template <class T> using Bounds4 = Vec<Bounds<T>, 4>;

	using Bounds2f = Vec<Bounds<float>, 2>;
	using Bounds3f = Vec<Bounds<float>, 3>;
	using Bounds4f = Vec<Bounds<float>, 4>;

	using Bounds2d = Vec<Bounds<double>, 2>;
	using Bounds3d = Vec<Bounds<double>, 3>;
	using Bounds4d = Vec<Bounds<double>, 4>;

	template <class T>
	struct is_scalar<Bounds<T>> : std::true_type { };

	template <class T>
	type::add<T> span(const Bounds<T>& b) { return b ? b.max - b.min : type::add<T>(0); }
	template <class T, size_t N, int K>
	Vec<type::add<T>, N> span(const Vec<Bounds<T>, N, K>& b)
	{
		decltype(span(b)) result;
		for (size_t i = 0; i < N; ++i)
			result[i] = span(b[i]);
		return result;
	}

	template <class T>
	T mean(const Bounds<T>& b) { return (max(b) + min(b)) / 2; }
	template <class T, size_t N, int K>
	Vec<T, N> mean(const Vec<Bounds<T>, N, K>& v) { Vec<T, N> r; for (size_t i = 0; i < N; ++i) r[i] = mean(v[i]); return r; }

	template <class T>
	type::bounds<T> bounds(const T& last) { return type::bounds<T>(last); }
	template <class A, class B>
	type::bounds<type::common<A, B>> bounds(const A& a, const B& b)
	{
		type::bounds<type::common<A, B>> result;
		min(result) = min(min(a), min(b));
		max(result) = max(max(a), max(b));
		return result;
	}

	template <class First, class... Rest>
	auto bounds(const First& first, const Rest&... rest) { return bounds(first, bounds(rest...)); }

	template <class T, class S, class = if_simple_scalar_t<S>> constexpr auto operator*(const S& a, const Bounds<T>& b) { return bounds(a*b.min, a*b.max); }
	template <class T, class S, class = if_simple_scalar_t<S>> constexpr auto operator/(const S& a, const Bounds<T>& b) { return bounds(a / b.min, a / b.max); }
	template <class T, class S, class = if_simple_scalar_t<S>> constexpr auto operator*(const Bounds<T>& b, const S& a) { return bounds(b.min * a, b.max * a); }
	template <class T, class S, class = if_simple_scalar_t<S>> constexpr auto operator/(const Bounds<T>& b, const S& a) { return bounds(b.min / b, b.max / b); }


	//template <class A, class B>
	//auto bounds_minmax(const A& minimum, const B& maximum)
	//{
	//	static_assert(is_scalar_v<A>);
	//	static_assert(is_scalar_v<B>);
	//	Bounds<type::common<A, B>> result;
	//	result.min = minimum;
	//	result.max = maximum;
	//	return result;
	//}
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


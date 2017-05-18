#pragma once

#include "uvector.h"

namespace uv
{
	template <class T, size_t I>
	class component
	{
		T _value;
	public:
		component() { }
		explicit component(T value) : _value(value) { }

		      T& operator*()       { return _value; }
		const T& operator*() const { return _value; }

		component operator-() const { return component{ -_value }; }
	};

	namespace details
	{
		template <class A, class B, size_t IA, size_t IB>
		struct component_op
		{
			static auto add(component<A, IA> a, component<B, IB> b)
			{
				using T = decltype(*a + *b);
				vec<T, std::max(IA, IB) + 1> result;
				for (size_t i = 0; i < result.size(); ++i)
					switch (i)
					{
					case IA: result[i] = *a;
					case IB: result[i] = *b;
					default: result[i] = T(0);
					}
				return result;
			}
			static auto sub(component<A, IA> a, component<B, IB> b)
			{
				using T = decltype(*a - *b);
				vec<T, std::max(IA, IB) + 1> result;
				for (size_t i = 0; i < result.size(); ++i)
					switch (i)
					{
					case IA: result[i] = *a;
					case IB: result[i] = -*b;
					default: result[i] = T(0);
					}
				return result;
			}
		};
		template <class A, class B, size_t I>
		struct component_op<A, B, I, I>
		{
			static auto add(component<A, I> a, component<B, I> b)
			{
				return component<decltype(*a + *b), I>{ *a + *b };
			}
			static auto sub(component<A, I> a, component<B, I> b)
			{
				return component<decltype(*a - *b), I>{ *a - *b };
			}
		};
	}

	template <class A, class B, size_t N, int K, size_t I>
	auto operator+(const vec<A, N, K>& v, component<B, I> c)
	{
		vec<decltype(v[0] + *c), std::max(N, I + 1)> result;
		for (int i = 0; i < N; ++i)
			result[i] = v[i];
		for (int i = N; i <= I; ++i)
			result[i] = A(0);
		result[I] += *c;
		return result;
	}
	template <class A, class B, size_t N, int K, size_t I>
	auto operator+(component<B, I> c, const vec<A, N, K>& v) { return v + c; }
	template <class A, class B, size_t N, int K, size_t I>
	auto operator-(const vec<A, N, K>& v, component<B, I> c)
	{
		vec<decltype(v[0] - *c), std::max(N, I + 1)> result;
		for (int i = 0; i < N; ++i)
			result[i] = v[i];
		for (int i = N; i <= I; ++i)
			result[i] = A(0);
		result[I] -= *c;
		return result;
	}
	template <class A, class B, size_t N, int K, size_t I>
	auto operator-(component<B, I> c, const vec<A, N, K>& v)
	{
		vec<decltype(v[0] - *c), std::max(N, I + 1)> result;
		for (int i = 0; i < N; ++i)
			result[i] = -v[i];
		for (int i = N; i <= I; ++i)
			result[i] = 0;
		result[I] += *c;
		return result;
	}

	template <class A, class B, size_t IA, size_t IB>
	inline auto operator+(component<A, IA> a, component<B, IB> b) { return details::component_op<A, B, IA, IB>::add(a, b); }
	template <class A, class B, size_t IA, size_t IB>
	inline auto operator-(component<A, IA> a, component<B, IB> b) { return details::component_op<A, B, IA, IB>::sub(a, b); }

	template <class A, class B, size_t N, int K, size_t I>
	auto operator*(const vec<A, N, K>& v, component<B, I> c)
	{
		static_assert(I < N, "invalid vector-component combination");
		return component<decltype(v[I] * *c), I>{ v[I] * *c };
	}
	template <class A, class B, size_t N, int K, size_t I>
	auto operator*(component<B, I> c, const vec<A, N, K>& v) { return v*c; }
	template <class A, class B, size_t IA, size_t IB>
	auto operator*(component<A, IA> a, component<B, IB> b)
	{
		static_assert(IA == IB, "multiplication between different components");
		return component<decltype(*a * *b), IA>{ *a * *b };
	}

	namespace components
	{
		template <class T> auto X(T value) { return component<T, 0>{ value }; }
		template <class T> auto Y(T value) { return component<T, 1>{ value }; }
		template <class T> auto Z(T value) { return component<T, 2>{ value }; }
		template <class T> auto W(T value) { return component<T, 3>{ value }; }
	}
}

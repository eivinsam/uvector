#pragma once

#include "details/component_details.h"

namespace uv
{
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
}

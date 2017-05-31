#pragma once

#include "details/component_details.h"

namespace uv
{
	template <class T, size_t I> auto operator*(Axis<I>, T value) { return Component<T, I>(value); }
	template <class T, size_t I> auto operator*(T value, Axis<I>) { return Component<T, I>(value); }

	template <class T, size_t N, int K, size_t I>
	auto operator*(const Vector<T, N, K>& v, Axis<I>) { return Component<T, I>(v[I]); }
	template <class T, size_t N, int K, size_t I>
	auto operator*(Axis<I>, const Vector<T, N, K>& v) { return Component<T, I>(v[I]); }

	template <class A, class B, size_t N, int K, size_t I>
	auto operator+(const Vector<A, N, K>& v, Component<B, I> c)
	{
		Vector<decltype(v[0] + *c), std::max(N, I + 1)> result;
		for (int i = 0; i < N; ++i)
			result[i] = v[i];
		for (int i = N; i <= I; ++i)
			result[i] = A(0);
		result[I] += *c;
		return result;
	}
	template <class A, class B, size_t N, int K, size_t I>
	auto operator+(Component<B, I> c, const Vector<A, N, K>& v) { return v + c; }
	template <class A, class B, size_t N, int K, size_t I>
	auto operator-(const Vector<A, N, K>& v, Component<B, I> c)
	{
		Vector<decltype(v[0] - *c), std::max(N, I + 1)> result;
		for (int i = 0; i < N; ++i)
			result[i] = v[i];
		for (int i = N; i <= I; ++i)
			result[i] = A(0);
		result[I] -= *c;
		return result;
	}
	template <class A, class B, size_t N, int K, size_t I>
	auto operator-(Component<B, I> c, const Vector<A, N, K>& v)
	{
		Vector<decltype(v[0] - *c), std::max(N, I + 1)> result;
		for (int i = 0; i < N; ++i)
			result[i] = -v[i];
		for (int i = N; i <= I; ++i)
			result[i] = 0;
		result[I] += *c;
		return result;
	}

	template <class A, class B, size_t IA, size_t IB>
	inline auto operator+(Component<A, IA> a, Component<B, IB> b) { return details::component_op<A, B, IA, IB>::add(a, b); }
	template <class A, class B, size_t IA, size_t IB>
	inline auto operator-(Component<A, IA> a, Component<B, IB> b) { return details::component_op<A, B, IA, IB>::sub(a, b); }

	template <class A, class B, size_t N, int K, size_t I>
	auto operator*(const Vector<A, N, K>& v, Component<B, I> c)
	{
		static_assert(I < N, "invalid vector-component combination");
		return Component<decltype(v[I] * *c), I>{ v[I] * *c };
	}
	template <class A, class B, size_t N, int K, size_t I>
	auto operator*(Component<B, I> c, const Vector<A, N, K>& v) { return v*c; }
	template <class A, class B, size_t IA, size_t IB>
	auto operator*(Component<A, IA> a, Component<B, IB> b)
	{
		static_assert(IA == IB, "multiplication between different components");
		return Component<decltype(*a * *b), IA>{ *a * *b };
	}
}

#define UVECTOR_COMPONENT_OPS_DEFINED

#ifdef UVECTOR_MATRIX_OPS_DEFINED
#include "cross/matrix_component.h"
#endif

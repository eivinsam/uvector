#pragma once

#include "../component.h"

namespace uv
{
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
}

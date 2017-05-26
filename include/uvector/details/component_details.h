#pragma once

#include "../component.h"

namespace uv
{
	namespace details
	{
		template <class A, class B, size_t IA, size_t IB>
		struct component_op
		{
			static auto add(Component<A, IA> a, Component<B, IB> b)
			{
				using T = decltype(*a + *b);
				Vector<T, std::max(IA, IB) + 1> result;
				for (size_t i = 0; i < result.size(); ++i)
					switch (i)
					{
					case IA: result[i] = *a;
					case IB: result[i] = *b;
					default: result[i] = T(0);
					}
				return result;
			}
			static auto sub(Component<A, IA> a, Component<B, IB> b)
			{
				using T = decltype(*a - *b);
				Vector<T, std::max(IA, IB) + 1> result;
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
			static auto add(Component<A, I> a, Component<B, I> b)
			{
				return Component<decltype(*a + *b), I>{ *a + *b };
			}
			static auto sub(Component<A, I> a, Component<B, I> b)
			{
				return Component<decltype(*a - *b), I>{ *a - *b };
			}
		};
	}
}

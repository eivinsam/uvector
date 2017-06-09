#pragma once

#include <type_traits>

namespace uv
{
	template <class T>
	static constexpr T pi = T(3.1415926535897932384626433832795);
	static constexpr auto pif = pi<float>;
	static constexpr auto pid = pi<double>;

	template <class T>
	struct is_scalar : public std::is_arithmetic<T> { };
	template <class T>
	static constexpr bool is_scalar_v = is_scalar<T>::value;

	template <class T, class R = void>
	struct if_scalar : public std::enable_if<is_scalar_v<T>, R> { };
	template <class T, class R = void>
	using if_scalar_t = typename if_scalar<T, R>::type;

	namespace op
	{
		struct add { template <class A, class B> auto operator()(const A& a, const B& b) { return a + b; } };
		struct sub { template <class A, class B> auto operator()(const A& a, const B& b) { return a - b; } };
		struct mul { template <class A, class B> auto operator()(const A& a, const B& b) { return a * b; } };
		struct div { template <class A, class B> auto operator()(const A& a, const B& b) { return a / b; } };

		struct eq { template <class A, class B> auto operator()(const A& a, const B& b) { return a == b; } }; // Equal
		struct ne { template <class A, class B> auto operator()(const A& a, const B& b) { return a != b; } }; // Not Equal
		struct sl { template <class A, class B> auto operator()(const A& a, const B& b) { return a <  b; } }; // Strictly Less
		struct le { template <class A, class B> auto operator()(const A& a, const B& b) { return a <= b; } }; // Less or Equal
		struct ge { template <class A, class B> auto operator()(const A& a, const B& b) { return a >= b; } }; // Greater or Equal
		struct sg { template <class A, class B> auto operator()(const A& a, const B& b) { return a >  b; } }; // Strictly Greater

		struct min { template <class A, class B> auto operator()(const A& a, const B& b) { return a <= b ? a : b; } };
		struct max { template <class A, class B> auto operator()(const A& a, const B& b) { return a >= b ? a : b; } };
	}

	namespace type
	{
		template <class OP, class A, class B = A> using of = decltype(OP{}(std::declval<A>(), std::declval<B>()));

		template <class A, class B = A>
		using inner_product = of<op::add, of<op::mul, A, B>>;

		template <class T>
		using identity = of<op::div, T, T>;
	}
}

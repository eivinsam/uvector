#pragma once

#include <type_traits>
#include <cmath>

namespace uv
{
	template <class T>
	static constexpr T pi = T(3.1415926535897932384626433832795);
	static constexpr auto pif = pi<float>;
	static constexpr auto pid = pi<double>;

	static constexpr auto inff = std::numeric_limits<float> ::infinity();
	static constexpr auto infd = std::numeric_limits<double>::infinity();

	template <class T> struct is_scalar : std::is_arithmetic<T> { };
	template <class T> struct is_scalar<T&> : is_scalar<T> { };
	template <class T> struct is_scalar<const T> : is_scalar<T> { };
	template <class T> static constexpr bool is_scalar_v = is_scalar<T>::value;

	template <class T, class R = void>
	struct if_scalar : public std::enable_if<is_scalar_v<T>, R> { };
	template <class T, class R = void>
	using if_scalar_t = typename if_scalar<T, R>::type;

	namespace details
	{
		template <class T>
		struct Scalar
		{
			static_assert(is_scalar_v<T>, "Type has no scalar type");
			using type = T;
		};
		template <class T> struct Scalar<T&> : Scalar<T> { };
		template <class T> struct Scalar<const T> : Scalar<T> { };

		template <class T, class = void>
		struct Dim : std::integral_constant<size_t, 1>
		{
			static_assert(is_scalar_v<T>, "Type is not a scalar and has no static member dim");
		};
		template <class T>
		struct Dim<T, std::void_t<decltype(std::decay_t<T>::dim)>>
		{
			static constexpr auto value = std::decay_t<T>::dim;
		};
	}
	template <class T>
	using scalar = typename details::Scalar<T>::type;
	template <class T>
	static constexpr auto dim = details::Dim<T>::value;

	namespace op
	{
		struct add { template <class A, class B> auto operator()(A a, B b) { return a + b; } };
		struct sub { template <class A, class B> auto operator()(A a, B b) { return a - b; } };
		struct mul { template <class A, class B> auto operator()(A a, B b) { return a * b; } };
		struct div { template <class A, class B> auto operator()(A a, B b) { return a / b; } };

		struct bor  { template <class A, class B> auto operator()(A a, B b) { return a | b; } };
		struct band { template <class A, class B> auto operator()(A a, B b) { return a & b; } };

		struct eq { template <class A, class B> auto operator()(A a, B b) { return a == b; } }; // Equal
		struct ne { template <class A, class B> auto operator()(A a, B b) { return a != b; } }; // Not Equal
		struct sl { template <class A, class B> auto operator()(A a, B b) { return a <  b; } }; // Strictly Less
		struct le { template <class A, class B> auto operator()(A a, B b) { return a <= b; } }; // Equal or Less
		struct ge { template <class A, class B> auto operator()(A a, B b) { return a >= b; } }; // Equal or Greater
		struct sg { template <class A, class B> auto operator()(A a, B b) { return a >  b; } }; // Strictly Greater

		struct min { template <class A, class B> auto operator()(A a, B b) { return a <= b ? a : b; } };
		struct max { template <class A, class B> auto operator()(A a, B b) { return a >= b ? a : b; } };

		template <class OP>
		struct reverse
		{
			struct type { template <class A, class B> auto operator()(A a, B b) { return OP{}(b, a); } };
		};
		template <> struct reverse<add> { using type = add; };
		template <> struct reverse<eq>  { using type = eq; };
		template <> struct reverse<ne>  { using type = ne; };
		template <> struct reverse<sl>  { using type = sg; };
		template <> struct reverse<le>  { using type = ge; };
		template <> struct reverse<ge>  { using type = le; };
		template <> struct reverse<sg>  { using type = sl; };
		template <> struct reverse<min> { using type = min; };
		template <> struct reverse<max> { using type = max; };

		template <class OP>
		using rev = typename reverse<OP>::type;
	}


	namespace type
	{
		template <class OP, class A, class B = A> using of = decltype(OP{}(std::declval<A>(), std::declval<B>()));

		template <class A, class B>
		struct addition
		{
			static_assert(is_scalar_v<A>, "A must be a scalar type");
			static_assert(is_scalar_v<B>, "B must be a scalar type");
			using type = decltype(std::declval<A>() + std::declval<B>());
			static_assert(std::is_same_v<type, decltype(std::declval<B>() + std::declval<A>())>, "Scalar type addition result type must be symmetric");
			static_assert(std::is_same_v<type, decltype(std::declval<A>() - std::declval<B>())>, "Scalar type substraction result type must be the same as for addition");
			static_assert(std::is_same_v<type, decltype(std::declval<B>() - std::declval<A>())>, "Scalar type substraction result type must be the same as for addition");
		};
		template <class A, class B = A> using add = typename addition<A, B>::type;

		template <class A, class B>
		struct multiplication
		{
			using type = decltype(std::declval<A>() * std::declval<B>());
			static_assert(std::is_same_v<type, decltype(std::declval<B>() * std::declval<A>())>, "Scalar type product result type must be symmetric");
			static_assert(std::is_same_v<type, add<type>>, "Sum of product of scalar types should be same as plain product type");
		};

		template <class A, class B = A> using mul = typename multiplication<A, B>::type;
		template <class A, class B = A> using div = decltype(std::declval<A>() / std::declval<B>());

		template <class A, class B = A> using common = std::common_type_t<A, B>;

		template <class T>
		using identity = div<T, T>;

	}

	template <class T>
	auto twice(T v) { return v + v; }

	template <class T, class = if_scalar_t<T>>
	auto square(T value) { return value*value; }

	using std::sqrt;
	using std::cbrt;

	template <class T, class = std::enable_if_t<is_scalar_v<T>>> 
	T length(T value) { using namespace std; return abs(value); }

	template <class T, class = std::enable_if_t<!is_scalar_v<T>>>
	scalar<T> length(T value) { return sqrt(square(value)); }

	template <class A, class B>
	auto distance(const A& a, const B& b) { return length(a - b); }

	template <class T>
	struct decompose_result
	{
		decltype(std::declval<T>() / std::declval<scalar<T>>()) direction;
		scalar<T> length;
	};
	template <class T> // Decomposes vector into direction vector and scalar length
	decompose_result<T> decompose(const T& a)
	{
		const auto len = length(a);
		return { a / len, len };
	}

	template <class T>
	auto direction(const T& a) { return decompose(a).direction; }

	template <class T> // Check if 'a' is of approximately unit magitude
	bool nearUnit(const T& a)
	{
		using S = decltype(square(a));
		static constexpr auto p = S(0.00001);
		const auto d = square(a) - S(1);
		return -p < d && d < p;
	}
}

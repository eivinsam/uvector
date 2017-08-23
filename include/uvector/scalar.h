#pragma once

#include <type_traits>
#include <cmath>

namespace uv
{
	static constexpr auto inf = std::numeric_limits<float> ::infinity();

	struct Identity { };
	static constexpr Identity identity = {};

	template <class T> struct is_scalar : std::is_arithmetic<T> { };
	template <class T> struct is_scalar<T&> : is_scalar<T> { };
	template <class T> struct is_scalar<const T> : is_scalar<T> { };
	template <class T> static constexpr bool is_scalar_v = is_scalar<T>::value;

	template <class T, class R = void>
	using if_scalar_t = std::enable_if_t<is_scalar<T>::value, R>;

	struct Unit
	{
		constexpr Unit() { }

		template <class T, class = if_scalar_t<T>> friend constexpr T operator+(Unit, T value) { return value + T(1); }
		template <class T, class = if_scalar_t<T>> friend constexpr T operator-(Unit, T value) { return value - T(1); }
		template <class T, class = if_scalar_t<T>> friend constexpr T operator*(Unit, T value) { return value; }
		template <class T, class = if_scalar_t<T>> friend constexpr T operator/(Unit, T value) { return value; }
		template <class T, class = if_scalar_t<T>> friend constexpr T operator+(T value, Unit) { return T(1) + value; }
		template <class T, class = if_scalar_t<T>> friend constexpr T operator-(T value, Unit) { return T(1) - value; }
		template <class T, class = if_scalar_t<T>> friend constexpr T operator*(T value, Unit) { return value; }
		template <class T, class = if_scalar_t<T>> friend constexpr auto operator/(T value, Unit) { return 1 / value; }
	};
	template <>
	struct is_scalar<Unit> : std::true_type { };

	template <size_t N, class T> struct is_unit : std::false_type { };
	template <size_t N, class T> struct is_unit<N, T&> : is_unit<N, T> { };
	template <size_t N, class T> struct is_unit<N, const T> : is_unit<N, T> { };
	template <>
	struct is_unit<1, Unit> : std::true_type { };

	template <size_t N, class T>
	static constexpr bool is_unit_v = is_unit<N, T>::value;
	template <size_t N, class T, class R = void>
	using if_unit_t = std::enable_if_t<is_unit<N, T>::value, R>;

	class Pi
	{
		double _p;
		static constexpr int _q = 6;

		static constexpr double pi = 3.1415926535897932384626433832795;
		static constexpr double sqrth = 0.70710678118654752440084436210485;
		static constexpr double sqrtt = 0.86602540378443864676372317075294;

		explicit constexpr Pi(double p) : _p(p) { }

		template <class T>
		using if_arith_t = std::enable_if_t<std::is_arithmetic<T>::value>;
	public:
		constexpr Pi() : _p(_q) { }

		constexpr Pi operator-() const { return Pi{ -_p }; }

		template <class T> constexpr friend Pi operator*(Pi p, T c) { return Pi{ p._p*c }; }
		template <class T> constexpr friend Pi operator*(T c, Pi p) { return Pi{ p._p*c }; }
		template <class T> constexpr friend Pi operator/(Pi p, T c) { return Pi{ p._p / c }; }
		
		friend constexpr double operator+(Pi p, Unit) { return double(p) + 1; }
		friend constexpr double operator-(Pi p, Unit) { return double(p) - 1; }
		friend constexpr double operator+(Unit, Pi p) { return 1 + double(p); }
		friend constexpr double operator-(Unit, Pi p) { return 1 - double(p); }

		friend constexpr Pi operator*(Pi p, Unit) { return p; }
		friend constexpr Pi operator/(Pi p, Unit) { return p; }
		friend constexpr Pi operator*(Unit, Pi p) { return p; }

		constexpr friend Pi operator+(Pi a, Pi b) { return Pi{ a._p + b._p }; }
		constexpr friend Pi operator-(Pi a, Pi b) { return Pi{ a._p + b._p }; }
		constexpr friend double operator/(Pi a, Pi b) { return a._p / b._p; }

		constexpr double sin() const
		{
			if (_p == 0) return 0.0; // 0pi
			if (_p < 0) return -Pi(-_p).sin();
			if (_p >= _q * 2) return Pi(_p - _q*2).sin();
			if (_p >= _q) return -Pi(_p + _q).sin(); // > 1pi
			if (_p * 2 == _q) return 1.0;// pi/2
			if (_p * 2 > _q) return Pi(_q - _p).sin(); // > pi/2
			if (_p * 3 == _q) return sqrtt;// pi/3
			if (_p * 4 == _q) return sqrth;
			if (_p * 6 == _q) return 0.5;

			return std::cos(double(*this));
		}
		constexpr double cos() const { return Pi(_p + _q/2).sin(); }

		constexpr operator double() const { return _p*pi / _q; }
		constexpr operator float() const { return float(double(*this)); }
	};
	template <>
	struct is_scalar<Pi> : std::true_type { };
}
namespace std
{
	constexpr double sin(uv::Pi a) { return a.sin(); }
	constexpr double cos(uv::Pi a) { return a.cos(); }
}
namespace uv
{
	static constexpr Pi pi = {};

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
	scalar<T> length(const T& value) { return sqrt(square(value)); }

	template <class A, class B>
	auto distance(const A& a, const B& b) { return length(a - b); }


	template <class T>
	struct Decomposed
	{
		scalar<T> length;
		decltype(std::declval<T>() / std::declval<scalar<T>>()) direction;

		Decomposed(const T& value) : length(uv::length(value)), direction(value/length) { }
	};

	template <class T> // Decomposes vector into direction vector and scalar length
	Decomposed<T> decompose(const T& a) { return { a }; }

	template <class T>
	auto direction(const T& a) { return decompose(a).direction; }

	template <class T> // Check if 'a' is of approximately unit magitude
	bool nearUnit(const T& a)
	{
		using S = decltype(square(a));
		static constexpr S p = S(0.00001);
		const auto d = square(a) - S(1);
		return -p < d && d < p;
	}
}

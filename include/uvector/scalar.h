#pragma once

#include <type_traits>
#include <limits>
#include <ostream>
#include <cmath>
#include <cassert>

namespace uv
{
	using std::sin;
	using std::cos;
	using std::abs;
	using std::sqrt;
	using std::cbrt;

	template <class A, class B> auto operator!=(const A& a, const B& b) { return !(a == b); }
	template <class A, class B> auto operator> (const A& a, const B& b) { return   b < a;  }
	template <class A, class B> auto operator<=(const A& a, const B& b) { return !(b < a); }
	template <class A, class B> auto operator>=(const A& a, const B& b) { return !(a < b); }

	static constexpr auto inf = std::numeric_limits<float> ::infinity();

	struct Identity { };
	static constexpr Identity identity = {};

	template <class T> struct is_scalar : std::is_arithmetic<T> { };
	template <class T> struct is_scalar<T&> : is_scalar<T> { };
	template <class T> struct is_scalar<const T> : is_scalar<T> { };
	template <class T> static constexpr bool is_scalar_v = is_scalar<T>::value;

	template <class T, class R = void>
	using if_scalar_t = std::enable_if_t<is_scalar<T>::value, R>;

	template <size_t N, class T> struct is_unit : std::false_type { };
	template <size_t N, class T> struct is_unit<N, T&> : is_unit<N, T> { };
	template <size_t N, class T> struct is_unit<N, const T> : is_unit<N, T> { };

	template <size_t N, class T>
	static constexpr bool is_unit_v = is_unit<N, T>::value;
	template <size_t N, class T, class R = void>
	using if_unit_t = std::enable_if_t<is_unit<N, T>::value, R>;

	template <class T> struct is_unit_any : std::false_type { };
	template <class T> struct is_unit_any<T&> : is_unit_any<T> { };
	template <class T> struct is_unit_any<const T> : is_unit_any<T> { };

	template <class T>
	static constexpr bool is_unit_any_v = is_unit_any<T>::value;
	template <class T, class R = void>
	using if_unit_any_t = std::enable_if_t<is_unit_any<T>::value, R>;

	template <class S>
	using if_arithmetic_t = std::enable_if_t<std::is_arithmetic<S>::value>;

	template <class T, class = void>
	struct convert_to
	{
		template <class S>
		static constexpr T from(S v) { return { v }; }
	};
	template <>
	struct convert_to<float>
	{
		template <class S>
		static constexpr float from(S v) { return { v }; }

		static constexpr float from(char v)  { return float(v); }
		static constexpr float from(short v) { return float(v); }
		static constexpr float from(int v) { assert(int(float(v)) == v); return float(v); }
	};
	template <>
	struct convert_to<double>
	{
		template <class S>
		static constexpr double from(S v) { return { v }; }

		static constexpr double from(char v)  { return double(v); }
		static constexpr double from(short v) { return double(v); }
		static constexpr double from(int v)   { return double(v); }
	};


	struct weak_double
	{
		double value;

		weak_double() = default;
		template <class S, class = if_arithmetic_t<S>>
		constexpr weak_double(S v) : value(v) { }

		constexpr weak_double operator+() const { return +value; }
		constexpr weak_double operator-() const { return -value; }

		template <class S, class = if_arithmetic_t<S>> weak_double& operator+=(S b) { value += b; return *this; }
		template <class S, class = if_arithmetic_t<S>> weak_double& operator-=(S b) { value -= b; return *this; }
		template <class S, class = if_arithmetic_t<S>> weak_double& operator*=(S b) { value *= b; return *this; }
		template <class S, class = if_arithmetic_t<S>> weak_double& operator/=(S b) { value /= b; return *this; }

		constexpr weak_double& operator+=(weak_double b) { value += b.value; return *this; }
		constexpr weak_double& operator-=(weak_double b) { value -= b.value; return *this; }
		constexpr weak_double& operator*=(weak_double b) { value *= b.value; return *this; }
		constexpr weak_double& operator/=(weak_double b) { value /= b.value; return *this; }

		template <class S, class = if_arithmetic_t<S>> friend constexpr weak_double operator+(weak_double a, S b) { return a.value + b; }
		template <class S, class = if_arithmetic_t<S>> friend constexpr weak_double operator-(weak_double a, S b) { return a.value - b; }
		template <class S, class = if_arithmetic_t<S>> friend constexpr weak_double operator*(weak_double a, S b) { return a.value * b; }
		template <class S, class = if_arithmetic_t<S>> friend constexpr weak_double operator/(weak_double a, S b) { return a.value / b; }

		template <class S, class = if_arithmetic_t<S>> friend constexpr weak_double operator+(S a, weak_double b) { return a + b.value; }
		template <class S, class = if_arithmetic_t<S>> friend constexpr weak_double operator-(S a, weak_double b) { return a - b.value; }
		template <class S, class = if_arithmetic_t<S>> friend constexpr weak_double operator*(S a, weak_double b) { return a * b.value; }
		template <class S, class = if_arithmetic_t<S>> friend constexpr weak_double operator/(S a, weak_double b) { return a / b.value; }

		friend constexpr weak_double operator+(weak_double a, weak_double b) { return a.value + b.value; }
		friend constexpr weak_double operator-(weak_double a, weak_double b) { return a.value - b.value; }
		friend constexpr weak_double operator*(weak_double a, weak_double b) { return a.value * b.value; }
		friend constexpr weak_double operator/(weak_double a, weak_double b) { return a.value / b.value; }

		template <class S, class = if_arithmetic_t<S>> friend constexpr bool operator==(weak_double a, S b) { return a.value == b; }
		template <class S, class = if_arithmetic_t<S>> friend constexpr bool operator< (weak_double a, S b) { return a.value <  b; }
		template <class S, class = if_arithmetic_t<S>> friend constexpr bool operator==(S a, weak_double b) { return a == b.value; }
		template <class S, class = if_arithmetic_t<S>> friend constexpr bool operator< (S a, weak_double b) { return a <  b.value; }

		friend constexpr bool operator==(weak_double a, weak_double b) { return a.value == b.value; }
		friend constexpr bool operator< (weak_double a, weak_double b) { return a.value <  b.value; }

		constexpr operator double() const { return value; }
		constexpr operator float() const { return float(value); }
	};
	template <>
	struct is_scalar<weak_double> : std::true_type { };

	inline weak_double  abs(weak_double v) { return std::abs(v.value); }
	inline weak_double sqrt(weak_double v) { return std::sqrt(v.value); }
	inline weak_double copysign(weak_double value, weak_double sign) { return std::copysign(value.value, sign.value); }

	inline std::ostream& operator<<(std::ostream& out, weak_double v) { return out << v.value; }

	class Angle
	{
		double _p;
		static constexpr int _q = 180;

		static constexpr double pi = 3.1415926535897932384626433832795;
		static constexpr double sqrth = 0.70710678118654752440084436210485;
		static constexpr double sqrtt = 0.86602540378443864676372317075294;


		explicit constexpr Angle(double p) : _p(p) { }

		template <class T>
		using if_arith_t = std::enable_if_t<std::is_arithmetic<T>::value>;
	public:

		constexpr double _taylor_sin() const
		{
			const double x = _p*pi/_q;
			const double mxx = -x*x;
			double px = x;
			double s = px;
			for (int k = 2; k <= 14; k += 2)
			{
				px *= mxx/(k*(k+1));
				s += px;
			}
			return s;
		}
		constexpr double _taylor_cos() const
		{
			const double x = _p*pi/_q;
			const double mxx = -x*x;
			double px = 1;
			double s = px;
			for (int k = 2; k <= 14; k += 2)
			{
				px *= mxx/(k*(k-1));
				s += px;
			}
			return s;

		}
		constexpr Angle() : _p(_q) { }

		constexpr Angle operator+() const { return Angle(+_p); }
		constexpr Angle operator-() const { return Angle(-_p); }

		template <class T> constexpr friend Angle operator*(Angle p, T c) { return Angle{ p._p*c }; }
		template <class T> constexpr friend Angle operator*(T c, Angle p) { return Angle{ p._p*c }; }
		template <class T> constexpr friend Angle operator/(Angle p, T c) { return Angle{ p._p / c }; }
		
		constexpr friend Angle operator+(Angle a, Angle b) { return Angle{ a._p + b._p }; }
		constexpr friend Angle operator-(Angle a, Angle b) { return Angle{ a._p + b._p }; }
		constexpr friend weak_double operator/(Angle a, Angle b) { return { a._p / b._p }; }

		constexpr weak_double sin() const
		{
			if (_p * 1< 0) return -Angle(-_p).sin();
			if (_p * 1 >= _q * 2) return +Angle(_p - _q*2).sin();
			if (_p * 1 >= _q * 1) return -Angle(_p + _q).sin(); // > 1pi
			if (_p * 2 > _q * 1) return Angle(_q - _p).sin(); // > pi/2
			if (_p * 4 > _q * 1) return Angle(_q/2 - _p).cos(); // > pi/4
			if (_p * 6 == _q) return 0.5;
			return _taylor_sin();
		}
		constexpr weak_double cos() const
		{
			if (_p * 1 < 0) return Angle(-_p).cos();
			if (_p * 1 >= _q * 2) return +Angle(_p - _q*2).cos();
			if (_p * 1 >= _q * 1) return -Angle(_p + _q).cos();
			if (_p * 2 > _q * 1) return -Angle(_q - _p).cos();
			if (_p * 4 > _q * 1) return Angle(_q/2 - _p).sin();
			return _taylor_cos();
		}
		constexpr weak_double tan() const
		{
			if (_p == 0) return 0.0; // 0pi
			if (_p < 0) return -Angle(-_p).tan();
			if (_p >= _q) return Angle(_p - _q).tan(); // > 1pi
			if (_p * 2 > _q) return -Angle(_q - _p).tan(); // > pi/2
			if (_p * 2 == _q) return inf;// pi/2
			if (_p * 3 == _q) return sqrtt/0.5;// pi/3
			if (_p * 4 == _q) return 1.0;
			if (_p * 6 == _q) return 0.5/sqrtt;

			return std::tan(double(*this));
		}

		template <class V>
		constexpr auto about(const V&) const;

		constexpr operator double() const { return _p*pi / _q; }
		constexpr operator float() const { return float(double(*this)); }
	};
	template <>
	struct is_scalar<Angle> : std::true_type { };

	constexpr weak_double sin(uv::Angle a) { return a.sin(); }
	constexpr weak_double cos(uv::Angle a) { return a.cos(); }
	constexpr weak_double tan(uv::Angle a) { return a.tan(); }

	static constexpr Angle pi = {};

	static constexpr auto degrees = pi/180;

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
		struct add { template <class A, class B> constexpr auto operator()(A a, B b) const { return a + b; } };
		struct sub { template <class A, class B> constexpr auto operator()(A a, B b) const { return a - b; } };
		struct mul { template <class A, class B> constexpr auto operator()(A a, B b) const { return a * b; } };
		struct div { template <class A, class B> constexpr auto operator()(A a, B b) const { return a / b; } };

		struct eq { template <class A, class B> constexpr auto operator()(A a, B b) const { return a == b; } }; // Equal
		struct ne { template <class A, class B> constexpr auto operator()(A a, B b) const { return a != b; } }; // Not Equal
		struct sl { template <class A, class B> constexpr auto operator()(A a, B b) const { return a <  b; } }; // Strictly Less
		struct le { template <class A, class B> constexpr auto operator()(A a, B b) const { return a <= b; } }; // Equal or Less
		struct ge { template <class A, class B> constexpr auto operator()(A a, B b) const { return a >= b; } }; // Equal or Greater
		struct sg { template <class A, class B> constexpr auto operator()(A a, B b) const { return a >  b; } }; // Strictly Greater

		struct min { template <class A, class B> constexpr auto operator()(A a, B b) const { return a <= b ? a : b; } };
		struct max { template <class A, class B> constexpr auto operator()(A a, B b) const { return a >= b ? a : b; } };

		template <class OP>
		struct reverse
		{
			struct type { template <class A, class B> constexpr auto operator()(A a, B b) const { return OP{}(b, a); } };
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

	template <class T>
	auto length(const T& value)
	{
		if constexpr (is_scalar_v<T>)
			return abs(value);
		else
			return sqrt(square(value));
	}

	template <class A, class B>
	auto distance(const A& a, const B& b) { return length(a - b); }

	template <class A, class B, class C, class = decltype(std::declval<A>()*(1 - std::declval<C>()) + std::declval<B>()*std::declval<C>())>
	auto interpolate(C c, A a, B b)
	{
		return a*(1 - c) + b*c;
	}

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

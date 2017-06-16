#pragma once

#include <cmath>

#include "scalar.h"

namespace uv
{
	template <class A, class B, class = std::void_t<if_scalar_t<A>, if_scalar_t<B>>>
	auto max(A a, B b) { return a >= b ? a : b; }
	template <class A, class B, class = std::void_t<if_scalar_t<A>, if_scalar_t<B>>>
	auto min(A a, B b) { return a <= b ? a : b; }

	template <class T>
	auto twice(T v) { return v + v; }

	template <class T, class = if_scalar_t<T>>
	auto square(T value) { return value*value; }

	using std::sqrt;
	using std::cbrt;

	template <class T, class = if_scalar_t<T>>
	auto length(T value) { return std::abs(value); }

	template <class A, class B>
	auto distance(const A& a, const B& b) { return length(a - b); }

	template <class T>
	bool UnitLength<T>::_is_approx_unit_length()
	{
		using SQ = decltype(square(_value));
		static constexpr auto above = SQ(1 + 1e-4f);
		static constexpr auto below = SQ(1 / above);
		const auto sq = square(_value);
		return below < sq && sq < above;
	}

	template <class T>
	std::ostream& operator<<(std::ostream& out, const UnitLength<T>& u)
	{
		return out << *u;
	}

	template <class T> UnitLength<T> operator-(const UnitLength<T>& a) { return UnitLength<T>::no_check(-*a); }
	template <class T> auto square(const UnitLength<T>& a) { return type::of<op::mul<T>>(1); }
	template <class T> auto length(const UnitLength<T>& a) { return T(1); }

#pragma push_macro("DEFINE_OP")
#define DEFINE_OP(op) template <class A, class B> auto operator##op(const UnitLength<A>& a, const UnitLength<B>& b) { return *a op *b; }

	DEFINE_OP(+); DEFINE_OP(-); DEFINE_OP(*); DEFINE_OP(/);

	DEFINE_OP(==); DEFINE_OP(!=); 
	DEFINE_OP(< ); DEFINE_OP(<=);
	DEFINE_OP(> ); DEFINE_OP(>=);

#pragma pop_macro("DEFINE_OP")
#pragma push_macro("DEFINE_OP")
#define DEFINE_OP(op) template <class A, class B> auto operator##op(const UnitLength<A>& a, const B& b) { return *a op b; }

	DEFINE_OP(+); DEFINE_OP(-); DEFINE_OP(*); DEFINE_OP(/);

	DEFINE_OP(==); DEFINE_OP(!=);
	DEFINE_OP(< ); DEFINE_OP(<=);
	DEFINE_OP(> ); DEFINE_OP(>=);

#pragma pop_macro("DEFINE_OP")
#pragma push_macro("DEFINE_OP")
#define DEFINE_OP(op) template <class A, class B> auto operator##op(const A& a, const UnitLength<B>& b) { return a op *b; }

	DEFINE_OP(+); DEFINE_OP(-); DEFINE_OP(*); DEFINE_OP(/);

	DEFINE_OP(==); DEFINE_OP(!=);
	DEFINE_OP(< ); DEFINE_OP(<=);
	DEFINE_OP(> ); DEFINE_OP(>=);

#pragma pop_macro("DEFINE_OP")

	template <class T>
	auto decompose(const T& a) // decomposes vector into direction vector and scalar length
	{
		const auto len = length(a);
		using U = std::remove_const_t<std::remove_reference_t<decltype(a / len)>>;
		struct result_t
		{
			UnitLength<U> direction;
			decltype(len) length;
		};
		return result_t{ UnitLength<U>::no_check(a / len), len };
	}
}

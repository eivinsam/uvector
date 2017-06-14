#pragma once

#include <cmath>

#include "scalar.h"

namespace uv
{
	template <class T>
	auto twice(T v) { return v + v; }

	template <class T, class = if_scalar_t<T>>
	auto square(T value) { return value*value; }

	template <class T, class = if_scalar_t<T>>
	auto length(T value) { return std::abs(value); }

	template <class A, class B>
	auto distance(const A& a, const B& b) { return length(a - b); }

	template <class T>
	class UnitLength
	{
		T _value;

		bool _is_approx_unit_length()
		{
			using U = decltype(length(_value));
			static constexpr auto above = U(1 + 1e-4f);
			static constexpr auto below = U(1 / above);
			const auto l = length(_value);
			return below < l && l < above;
		}
	public:
		UnitLength(const T& value) : _value(value) { Expects(_is_approx_unit_length()); }

		static UnitLength no_check(const T& value) { return reinterpret_cast<const UnitLength&>(value); }

		const T* operator->() const { return &_value; }
		const T& operator* () const { return _value; }

		template <class = std::void_t<decltype(_value[0])>>
		const auto& operator[](size_t i) const { return _value[i]; }

		template <class = std::void_t<decltype(std::begin(_value))>> auto begin() const { return std::begin(_value); }
		template <class = std::void_t<decltype(std::end  (_value))>> auto end()   const { return std::end  (_value);   }
	};

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

#pragma once

#include "vector.h"

namespace uv
{
	template <class T>
	struct Imaginary
	{
		static_assert(is_scalar_v<T>, "Imaginary component must be scalar");
		T value;

		constexpr friend T square(Imaginary i) { return -i.value*i.value; }
		constexpr friend Imaginary conjugate(Imaginary i) { return -i; }

		constexpr Imaginary operator-() const { return { -value }; }


		template <class S> constexpr Imaginary<type::add<S, T>> operator+(Imaginary<S> b) const { return { value + b.value }; }
		template <class S> constexpr Imaginary<type::add<S, T>> operator-(Imaginary<S> b) const { return { value - b.value }; }
		template <class S> constexpr type::mul<T, S> operator*(Imaginary<S> b) const { return -value*b.value; }
		template <class S> constexpr type::div<T, S> operator/(Imaginary<S> b) const { return +value/b.value; }

		template <class S> constexpr bool operator==(Imaginary<S> b) const { return value == b.value; }
		template <class S> constexpr bool operator!=(Imaginary<S> b) const { return value != b.value; }
		template <class S> constexpr bool operator< (Imaginary<S> b) const { return value <  b.value; }
		template <class S> constexpr bool operator<=(Imaginary<S> b) const { return value <= b.value; }
		template <class S> constexpr bool operator>=(Imaginary<S> b) const { return value >= b.value; }
		template <class S> constexpr bool operator> (Imaginary<S> b) const { return value >  b.value; }

		template <class S, class = if_scalar_t<S>> constexpr friend Imaginary<type::mul<T, S>> operator*(Imaginary i, S r) { return { i.value * r }; }
		template <class S, class = if_scalar_t<S>> constexpr friend Imaginary<type::div<T, S>> operator/(Imaginary i, S r) { return { i.value / r }; }
		template <class S, class = if_scalar_t<S>> constexpr friend Imaginary<type::mul<S, T>> operator*(S r, Imaginary i) { return { +r * i.value }; }
		template <class S, class = if_scalar_t<S>> constexpr friend Imaginary<type::div<S, T>> operator/(S r, Imaginary i) { return { -r / i.value }; }
	};
	template <>
	struct Imaginary<void>
	{
		constexpr int operator+(Imaginary) const { return 2; }
		constexpr int operator-(Imaginary) const { return 0; }
		constexpr int operator*(Imaginary) const { return -1; }
		constexpr int operator/(Imaginary) const { return 1; }

		template <class S> friend constexpr Imaginary<S>      operator+(Imaginary, Imaginary<S> b) { return { S(1) + b.value }; }
		template <class S> friend constexpr Imaginary<S>      operator-(Imaginary, Imaginary<S> b) { return { S(1) - b.value }; }
		template <class S> friend constexpr           S       operator*(Imaginary, Imaginary<S> b) { return    -b.value; }
		template <class S> friend constexpr type::div<int, S> operator/(Imaginary, Imaginary<S> b) { return 1 / b.value; }

		template <class S> friend constexpr Imaginary<S>      operator+(Imaginary<S> a, Imaginary) { return { a.value + S(1) }; }
		template <class S> friend constexpr Imaginary<S>      operator-(Imaginary<S> a, Imaginary) { return { a.value - S(1) }; }
		template <class S> friend constexpr           S       operator*(Imaginary<S> a, Imaginary) { return -a.value; }
		template <class S> friend constexpr type::div<int, S> operator/(Imaginary<S> a, Imaginary) { return +a.value; }

		template <class S, class = if_scalar_t<S>> friend constexpr Imaginary<S>                 operator*(S r, Imaginary) { return { +r }; }
		template <class S, class = if_scalar_t<S>> friend constexpr Imaginary<S>                 operator/(S r, Imaginary) { return { -r }; }
		template <class S, class = if_scalar_t<S>> friend constexpr Imaginary<S>                 operator*(Imaginary, S r) { return { +r }; }
		template <class S, class = if_scalar_t<S>> friend constexpr Imaginary<type::div<int, S>> operator/(Imaginary, S r) { return { 1 / r }; }
	};
	static constexpr Imaginary<void> imaginary = {};

	template <class T>
	class Complex
	{
	public:
		T re; Imaginary<T> im;

		friend constexpr type::mul<T> square(const Complex& c) { return c.re*c.re + c.im.value*c.im.value; }
		friend constexpr Complex conjugate(const Complex& c) { return { c.re, -c.im }; }

		constexpr Complex operator-() const { return { -re, -im }; }

		template <class S, class = if_scalar_t<S>> friend constexpr Complex<type::add<T, S>> operator+(const Complex& c, S r) { return { c.re + r, c.im }; }
		template <class S, class = if_scalar_t<S>> friend constexpr Complex<type::add<T, S>> operator-(const Complex& c, S r) { return { c.re - r, c.im }; }
		template <class S, class = if_scalar_t<S>> friend constexpr Complex<type::mul<T, S>> operator*(const Complex& c, S r) { return { c.re * r, c.im * r }; }
		template <class S, class = if_scalar_t<S>> friend constexpr Complex<type::div<T, S>> operator/(const Complex& c, S r) { return { c.re / r, c.im / r }; }
		template <class S, class = if_scalar_t<S>> friend constexpr Complex<type::add<T, S>> operator-(S r, const Complex& c) { return { r - c.re, -c.im }; }
		template <class S, class = if_scalar_t<S>> friend constexpr Complex<type::div<T, S>> operator/(S r, const Complex& c) { return (r*conjugate(c)) / square(c); }

		template <class S> constexpr Complex<type::add<T, S>> operator+(const Complex<S>& c) const { return { re + c.re, im + c.im }; }
		template <class S> constexpr Complex<type::add<T, S>> operator-(const Complex<S>& c) const { return { re - c.re, im - c.im}; }
		template <class S> constexpr Complex<type::mul<T, S>> operator*(const Complex<S>& c) const { return (re*c.re + im*c.im) + (re*c.im + im*c.re); }
		template <class S> constexpr Complex<type::div<T, S>> operator/(const Complex<S>& c) const { return (*this*conjugate(c))/square(c); }
	};

	template <class A, class B, class = if_scalar_t<A>> constexpr Complex<type::common<A, B>> operator+(A re, Imaginary<B> im) { return { re,  im }; }
	template <class A, class B, class = if_scalar_t<A>> constexpr Complex<type::common<A, B>> operator-(A re, Imaginary<B> im) { return { re, -im }; }
	template <class A, class B, class = if_scalar_t<B>> constexpr Complex<type::common<A, B>> operator+(Imaginary<A> im, B re) { return { +re, im }; }
	template <class A, class B, class = if_scalar_t<B>> constexpr Complex<type::common<A, B>> operator-(Imaginary<A> im, B re) { return { -re, im }; }

	template <class S, class = if_scalar_t<S>> constexpr Complex<S> operator+(S s, Imaginary<void>) { return { s, {S(+1)} }; }
	template <class S, class = if_scalar_t<S>> constexpr Complex<S> operator-(S s, Imaginary<void>) { return { s, {S(-1)} }; }
	template <class S, class = if_scalar_t<S>> constexpr Complex<S> operator+(Imaginary<void>, S s) { return { +s, {S(1)} }; }
	template <class S, class = if_scalar_t<S>> constexpr Complex<S> operator-(Imaginary<void>, S s) { return { -s, {S(1)} }; }

	template <class A, class = if_scalar_t<A>>
	auto rotation(A angle) { using namespace std; return cos(angle) + imaginary*sin(angle); }

	using floatc  = Complex<float>;
	using doublec = Complex<double>;
}

#define UVECTOR_COMPLEX_DEFINED

#ifdef UVECTOR_MATRIX_DEFINED
#include "cross/matrix_complex.h"
#endif
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

		template <class S, class = if_scalar_t<S>> constexpr friend auto operator*(Imaginary i, S r) { return Imaginary<type::mul<T, S>>{ i.value * r }; }
		template <class S, class = if_scalar_t<S>> constexpr friend auto operator/(Imaginary i, S r) { return Imaginary<type::div<T, S>>{ i.value / r }; }
		template <class S, class = if_scalar_t<S>> constexpr friend auto operator*(S r, Imaginary i) { return Imaginary<type::mul<S, T>>{ +r * i.value }; }
		template <class S, class = if_scalar_t<S>> constexpr friend auto operator/(S r, Imaginary i) { return Imaginary<type::div<S, T>>{ -r / i.value }; }
	};
	static constexpr Imaginary<char> imaginary = { 1 };

	template <class T>
	class Complex
	{
	public:
		T re; Imaginary<T> im;

		friend constexpr type::mul<T> square(const Complex& c) { return c.re*c.re + c.im.value*c.im.value; }
		friend constexpr Complex conjugate(const Complex& c) { return { c.re, -c.im }; }

		constexpr Complex operator-() const { return { -re, -im }; }

		template <class S, class = if_scalar_t<S>> friend constexpr auto operator+(const Complex& c, S r) { return Complex<type::add<T, S>>{ c.re + r, c.im }; }
		template <class S, class = if_scalar_t<S>> friend constexpr auto operator-(const Complex& c, S r) { return Complex<type::add<T, S>>{ c.re - r, c.im }; }
		template <class S, class = if_scalar_t<S>> friend constexpr auto operator*(const Complex& c, S r) { return Complex<type::mul<T, S>>{ c.re * r, c.im * r }; }
		template <class S, class = if_scalar_t<S>> friend constexpr auto operator/(const Complex& c, S r) { return Complex<type::div<T, S>>{ c.re / r, c.im / r }; }
		template <class S, class = if_scalar_t<S>> friend constexpr auto operator-(S r, const Complex& c) { return Complex<type::add<T, S>>{ r - c.re, -c.im }; }
		template <class S, class = if_scalar_t<S>> friend constexpr auto operator/(S r, const Complex& c) { return Complex<type::div<T, S>>{ (r*conjugate(c)) / square(c) }; }

		template <class S> constexpr Complex<type::add<T, S>> operator+(const Complex<S>& c) const { return { re + c.re, im + c.im }; }
		template <class S> constexpr Complex<type::add<T, S>> operator-(const Complex<S>& c) const { return { re - c.re, im - c.im}; }
		template <class S> constexpr Complex<type::mul<T, S>> operator*(const Complex<S>& c) const { return (re*c.re + im*c.im) + (re*c.im + im*c.re); }
		template <class S> constexpr Complex<type::div<T, S>> operator/(const Complex<S>& c) const { return (*this*conjugate(c))/square(c); }
	};

	template <class A, class B, class = if_scalar_t<A>> constexpr Complex<type::common<A, B>> operator+(A re, Imaginary<B> im) { return { re,  im }; }
	template <class A, class B, class = if_scalar_t<A>> constexpr Complex<type::common<A, B>> operator-(A re, Imaginary<B> im) { return { re, -im }; }
	template <class A, class B, class = if_scalar_t<B>> constexpr Complex<type::common<A, B>> operator+(Imaginary<A> im, B re) { return { +re, im }; }
	template <class A, class B, class = if_scalar_t<B>> constexpr Complex<type::common<A, B>> operator-(Imaginary<A> im, B re) { return { -re, im }; }

	using floatc  = Complex<float>;
	using doublec = Complex<double>;
}

#define UVECTOR_COMPLEX_DEFINED

#ifdef UVECTOR_MATRIX_DEFINED
#include "cross/matrix_complex.h"
#endif
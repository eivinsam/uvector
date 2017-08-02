#pragma once

#include "complex.h"
#include "vector_operations.h"

namespace uv
{
	template <class T>
	T       real(T value) { return value; }
	template <class T>
	Imag<T> imag(T value) { return { value }; }

	template <class T> Imag<T> operator*(T value, Imaginary) { return {   value }; }
	template <class T> Imag<T> operator/(T value, Imaginary) { return {  -value }; }
	template <class T> Imag<T> operator*(Imaginary, T value) { return {   value }; }
	template <class T> Imag<T> operator/(Imaginary, T value) { return { 1/value }; }
	
	template <class T> Imag<T> operator-(Imag<T> a) { return { -a.value }; }

	template <class T> auto square(Imag<T> a) { return -square(a.value); }

	template <class T> T operator*(Imag<T> value, Imaginary) { return     -value.value; }
	template <class T> T operator/(Imag<T> value, Imaginary) { return      value.value; }
	template <class T> T operator*(Imaginary, Imag<T> value) { return     -value.value; }
	template <class T> T operator/(Imaginary, Imag<T> value) { return -1 / value.value; }

	template <class A, class B> auto operator+(Imag<A> a, Imag<B> b) { return imag(a.value + b.value); }
	template <class A, class B> auto operator-(Imag<A> a, Imag<B> b) { return imag(a.value - b.value); }
	template <class A, class B> auto operator*(Imag<A> a, Imag<B> b) { return real(-a.value * b.value); }
	template <class A, class B> auto operator/(Imag<A> a, Imag<B> b) { return real(a.value / b.value); }

	template <class T, class S, class = if_scalar_t<S>> auto operator+(S a, Imag<T> b) { return Complex<std::common_type_t<T, S>>{  a,  b.value }; }
	template <class T, class S, class = if_scalar_t<S>> auto operator-(S a, Imag<T> b) { return Complex<std::common_type_t<T, S>>{  a, -b.value }; }
	template <class T, class S, class = if_scalar_t<S>> auto operator+(Imag<T> a, S b) { return Complex<std::common_type_t<T, S>>{  b, a.value }; }
	template <class T, class S, class = if_scalar_t<S>> auto operator-(Imag<T> a, S b) { return Complex<std::common_type_t<T, S>>{ -b,  a.value }; }

	template <class T, class S, class = if_scalar_t<S>> auto operator*(S a, Imag<T> b) { return imag(a *  b.value); }
	template <class T, class S, class = if_scalar_t<S>> auto operator/(S a, Imag<T> b) { return imag(a / -b.value); }
	template <class T, class S, class = if_scalar_t<S>> auto operator*(Imag<T> a, S b) { return imag(a.value * b); }
	template <class T, class S, class = if_scalar_t<S>> auto operator/(Imag<T> a, S b) { return imag(a.value / b); }


	template <class A>
	auto rotate(A angle) { using namespace std; return cos(angle) + imag(sin(angle)); }

	template <class T> auto& real(      Complex<T>& c) { return c.real; }
	template <class T> auto& real(const Complex<T>& c) { return c.real; }
	template <class T> auto& imag(      Complex<T>& c) { return c.imag.value; }
	template <class T> auto& imag(const Complex<T>& c) { return c.imag.value; }

	template <class T> auto operator-(const Complex<T>& a) { return -a.real - a.imag; }
	template <class T> auto conjugate(const Complex<T>& a) { return  a.real - a.imag; }
	template <class T> auto square(const Complex<T>& a) { return square(real(a)) + square(imag(a)); }
	template <class T> auto length(const Complex<T>& a) { using namespace std; return sqrt(square(a)); }

	template <class T> auto operator*(const Complex<T> a, Imaginary) { return a.real * imaginary + a.imag * imaginary; }
	template <class T> auto operator/(const Complex<T> a, Imaginary) { return a.real / imaginary + a.imag / imaginary; }
	template <class T> auto operator*(Imaginary, const Complex<T> a) { return imaginary * a.real + imaginary * a.imag; }
	template <class T> auto operator/(Imaginary, const Complex<T> a) { return imaginary / a.real + imaginary / a.imag; }

	template <class T, class S, if_scalar_t<S>> auto operator+(const Complex<T>& a, S b) { return (a.real + b) + a.imag; }
	template <class T, class S, if_scalar_t<S>> auto operator-(const Complex<T>& a, S b) { return (a.real + b) + a.imag; }
	template <class T, class S, if_scalar_t<S>> auto operator*(const Complex<T>& a, S b) { return a.real*b + a.imag*b; }
	template <class T, class S, if_scalar_t<S>> auto operator/(const Complex<T>& a, S b) { return a.real/b + a.imag/b; }

	template <class T, class S, if_scalar_t<S>> auto operator+(S a, const Complex<T>& b) { return (a + b.real) + b.imag; }
	template <class T, class S, if_scalar_t<S>> auto operator-(S a, const Complex<T>& b) { return (a + b.real) + b.imag; }
	template <class T, class S, if_scalar_t<S>> auto operator*(S a, const Complex<T>& b) { return a*b.real + a*b.imag; }
	template <class T, class S, if_scalar_t<S>> auto operator/(S a, const Complex<T>& b) { return a/b.real + a/b.imag; }

	template <class T, class S> auto operator+(const Complex<T>& a, Imag<S> b) { return a.real + (a.imag + b); }
	template <class T, class S> auto operator-(const Complex<T>& a, Imag<S> b) { return a.real + (a.imag + b); }
	template <class T, class S> auto operator*(const Complex<T>& a, Imag<S> b) { return a.real*b + a.imag*b; }
	template <class T, class S> auto operator/(const Complex<T>& a, Imag<S> b) { return a.real/b + a.imag/b; }

	template <class T, class S> auto operator+(Imag<S> a, const Complex<T>& b) { return b.real + (a + b.imag); }
	template <class T, class S> auto operator-(Imag<S> a, const Complex<T>& b) { return b.real + (a + b.imag); }
	template <class T, class S> auto operator*(Imag<S> a, const Complex<T>& b) { return a*b.real + a*b.imag; }
	template <class T, class S> auto operator/(Imag<S> a, const Complex<T>& b) { return a/b.real + a/b.imag; }
	
	template <class A, class B> auto operator+(const Complex<A>& a, const Complex<B>& b) { return (a.real+b.real) + (a.imag + b.imag); }
	template <class A, class B> auto operator-(const Complex<A>& a, const Complex<B>& b) { return (a.real-b.real) + (a.imag - b.imag); }

	template <class A, class B> auto operator*(const Complex<A>& a, const Complex<B>& b) { return ((a.real*b.real + a.imag*b.imag) + (a.imag*b.real + a.real*b.imag)); }
	template <class A, class B> auto operator/(const Complex<A>& a, const Complex<B>& b) { return (a*conjugate(b)) / square(b); }
}

#define UVECTOR_COMPLEX_OPS_DEFINED

#ifdef UVECTOR_MATRIX_OPS_DEFINED
#include "cross/matrix_complex.h"
#endif
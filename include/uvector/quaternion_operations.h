#pragma once

#include "quaternion.h"
#include "vector_operations.h"

#include <cmath>

namespace uv
{
	namespace details
	{
		template <class T>
		struct is_quat_s : public std::false_type { };
		template <class T>
		struct is_quat_s<Quaternion<T>> : public std::true_type { };

		template <class T>
		static constexpr bool is_quat = is_quat_s<std::remove_cv_t<std::remove_reference_t<T>>>::value;

		template <class T, class R = void>
		struct if_quat_s : public std::enable_if<is_quat<T>, R> { };
		template <class T, class R = void>
		using if_quat = typename if_quat_s<T, R>::type;
	}

	template <class T> auto& real(      Quaternion<T>& q) { return q.v * axes::W; }
	template <class T> auto& real(const Quaternion<T>& q) { return q.v * axes::W; }
	template <class T> auto& imag(      Quaternion<T>& q) { return q.v * axes::XYZ; }
	template <class T> auto& imag(const Quaternion<T>& q) { return q.v * axes::XYZ; }

	template <class T, int K>
	constexpr auto quaternion(T re, const Vector<T, 3, K>& im)
	{
		Quaternion<T> result;
		real(result) = re;
		imag(result) = im;
		return result;
	}
	template <class T>
	constexpr auto quaternion(T x, T y, T z, T w)
	{
		using namespace axes;
		Quaternion<T> result;
		result.v*X = x;
		result.v*Y = y;
		result.v*Z = z;
		result.v*W = w;
		return result;
	}

	template <class A, class T, int K, class = if_scalar_t<A>>
	auto rotate(A angle, const Vector<T, 3, K>& axis)   { auto a = angle * 0.5f; return quaternion(cos(a), decompose(axis).direction*sin(a)); }
	template <class T, class A, class = if_scalar_t<A>>
	auto rotate(A angle, Axes<0>) { auto a = angle * 0.5f; return quaternion(T(sin(a)), T(0), T(0), T(cos(a))); }
	template <class T, class A, class = if_scalar_t<A>>
	auto rotate(A angle, Axes<1>) { auto a = angle * 0.5f; return quaternion(T(0), T(sin(a)), T(0), T(cos(a))); }
	template <class T, class A, class = if_scalar_t<A>>
	auto rotate(A angle, Axes<2>) { auto a = angle * 0.5f; return quaternion(T(0), T(0), T(sin(a)), T(cos(a))); }
	template <class T, class A, size_t I, class = if_scalar_t<A>>
	auto rotate(A angle, Component<T, I>) { return quaternion<T>(angle, Axis<I>{}); }



	template <class A>
	auto operator-(const Quaternion<A>& a) { Quaternion<A> result; result.v = -a.v; return result; }
	template <class A>
	auto conjugate(const Quaternion<A>& a) { return quaternion(real(a), -imag(a)); }

	template <class T, class F>
	auto with(F&& f) { return f(T{}); }

	template <class A, class B>
	auto operator+(const Quaternion<A>& a, const Quaternion<B>& b) { return quaternion(real(a) + real(b), imag(a) + imag(b)); }
	template <class A, class B>
	auto operator-(const Quaternion<A>& a, const Quaternion<B>& b) { return quaternion(real(a) - real(b), imag(a) - imag(b)); }

	template <class A, class B>
	auto operator*(const Quaternion<A>& a, const Quaternion<B>& b)
	{
		return quaternion(
			real(a)*real(b) - dot(imag(a), imag(b)),
			imag(b)*real(a) + imag(a)*real(b) + cross(imag(a), imag(b)));
	}

	template <class A, class B, class = if_scalar_t<B>>
	auto operator*(const Quaternion<A>& q, B c) { return quaternion(real(q)*c, imag(q)*c); }
	template <class A, class B, class = if_scalar_t<B>>
	auto operator/(const Quaternion<A>& q, B c) { return quaternion(real(q)/c, imag(q)/c); }


	template <class T>
	T dot(const Quaternion<T>& a, const Quaternion<T>& b) { return dot(a.v, b.v); }

	template <class T>
	auto square(const Quaternion<T>& q) { return square(q.v); }
	template <class T>
	auto length(const Quaternion<T>& q) { return sqrt(square(q)); }

	template <class A, class B, int K>
	auto operator*(const Quaternion<A>& q, const Vector<B, 3, K>& v) { return v + twice(cross(imag(q), cross(imag(q), v) + real(q)*v)); }

	template <class T, size_t I>
	auto operator*(const Quaternion<T>& q, Axes<I>)
	{
		static_assert(I < 3, "Invalid axis");
		static constexpr size_t II = (I + 1) % 3;
		static constexpr size_t III = (I + 2) % 3;
		Vector<type::dot<T>, 3> result;
		auto& re = real(q);
		auto& im = imag(q);
		result[I] = square(re) + square(im[I]) - square(im[II]) - square(im[III]);
		result[II]  = twice(im[I] * im[II] + im[III] * re);
		result[III] = twice(im[I] * im[III] - im[II] * re);
		return result;
	}
	template <class A, class B, size_t I>
	auto operator*(const Quaternion<A>& q, Component<B, I> c) { return (q*Axes<I>{}) * *c; }

	template <class A, class B>
	auto angle(const Quaternion<A>& a, const Quaternion<B>& b)
	{
		auto p = dot(a, b);
		using P = decltype(p);

		if (p < P(-1) || p > P(+1))
			return acos(P(1));

		return acos(2*p*p - 1);
	}
}

#define UVECTOR_QUATERNION_OPS_DEFINED

#ifdef UVECTOR_MATRIX_OPS_DEFINED
#include "cross/matrix_quaternion.h"
#endif

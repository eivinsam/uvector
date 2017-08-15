#pragma once

namespace uv { }

#include "scalar.h"
#include "vector.h"

namespace uv
{
	template <class T>
	class Quaternion
	{
	public:
		T re;
		Vector<T, 3> im;

		Quaternion() { }
		constexpr Quaternion(const Quaternion& q) = default;
		Quaternion(T re, const Vector<T, 3>& im) : re(re), im(im) { }

		friend type::mul<T> square(const Quaternion& q) { return square(q.re) + square(q.im); }
		friend Quaternion conjugate(const Quaternion& q) { return { re, -im }; }

		Quaternion operator-() const { return { -re, -im }; }

		template <class S, class = if_scalar_t<S>> friend Quaternion<type::add<T, S>> operator+(const Quaternion& q, S s) { return { q.re + s, q.im }; }
		template <class S, class = if_scalar_t<S>> friend Quaternion<type::add<T, S>> operator-(const Quaternion& q, S s) { return { q.re - s, q.im }; }
		template <class S, class = if_scalar_t<S>> friend Quaternion<type::mul<T, S>> operator*(const Quaternion& q, S s) { return { q.re * s, q.im * s }; }
		template <class S, class = if_scalar_t<S>> friend Quaternion<type::div<T, S>> operator/(const Quaternion& q, S s) { return { q.re / s, q.im / s }; }
		template <class S, class = if_scalar_t<S>> friend Quaternion<type::add<S, T>> operator+(S s, const Quaternion& q) { return { s + q.re, q.im }; }
		template <class S, class = if_scalar_t<S>> friend Quaternion<type::add<S, T>> operator-(S s, const Quaternion& q) { return { s - q.re, -q.im }; }
		template <class S, class = if_scalar_t<S>> friend Quaternion<type::mul<S, T>> operator*(S s, const Quaternion& q) { return { s * q.re, s * q.im }; }

		template <class S, int K> 
		friend Vector<type::mul<T, S>, 3> operator*(const Quaternion& q, const Vector<S, 3, K>& v)
		{
			return v + twice(cross(q.im, cross(q.im, v) + q.re*v));
		}

		template <size_t I>
		friend Vector<type::mul<T>, 3> operator*(const Quaternion& q, Axes<I> a)
		{
			static_assert(I < 3, "Invalid axis for quaternion-axis multiplication");
			static constexpr size_t II = (I + 1) % 3;
			static constexpr size_t III = (I + 2) % 3;
			decltype(q * a) result;
			result[I] = square(q.re) + square(q.im[I]) - square(q.im[II]) - square(q.im[III]);
			result[II] = twice(q.im[I] * q.im[II] + q.im[III] * q.re);
			result[III] = twice(q.im[I] * q.im[III] - q.im[II] * q.re);
			return result;
		}
		template <class S, size_t I>
		friend auto operator*(const Quaternion& q, Component<S, I> c) { return (q*Axes<I>{}) * *c; }

		template <class S> 
		friend type::mul<T, S> dot(const Quaternion& a, const Quaternion<S>& b) { return a.re*b.re + dot(a.im, b.im); }

		template <class S> Quaternion<type::add<T, S>> operator+(const Quaternion<S>& b) const { return { re + b.re, im + b.im }; }
		template <class S> Quaternion<type::add<T, S>> operator-(const Quaternion<S>& b) const { return { re - b.re, im - b.im }; }
		template <class S>
		Quaternion<type::mul<T, S>> operator*(const Quaternion<S>& b) const
		{
			return { re * b.re - dot(im, b.im),  re * b.im + b.re * im + cross(im, b.im) };
		}
	};
	using floatq = Quaternion<float>;
	using doubleq = Quaternion<double>;

	template <class T, int K>
	constexpr Quaternion<T> quaternion(T re, const Vector<T, 3, K>& im) { return { re, im }; }
	template <class T>
	constexpr Quaternion<T> quaternion(T x, T y, T z, T w) { return { w, vector(x, y, z) }; }

	namespace unchecked
	{
		template <class A, class B>
		auto rotation(const A& from, const B& to)
		{
			if (square(from - to) < 0.00001f)
				return quaternion(1, 0);

			const auto asa = cross(from, to); // axis * sin(angle)
			const auto ca = dot(from, to);   // cos(angle)
			const auto sa = sqrt(1 - ca*ca); // sin(angle);
			const auto hca = ca*0.5f;          // cos(angle) / 2
			const auto cha = sqrt(0.5f + hca); // cos(angle / 2)
			const auto sha = sqrt(0.5f - hca); // sin(angle / 2)

			return quaterion(cha, asa*(sha / sa));
		}
	}
	template <class A, class B, int KA, int KB>
	auto rotation(const Vector<A, 3, KA>& from, const Vector<B, 3, KB>& to)
	{
		Expects(nearUnit(from));
		Expects(nearUnit(to));
		return unchecked::rotation(from, to);
	}
	template <class T, int K, size_t I>
	auto rotation(const Vector<T, 3, K>& from, Axes<I> to)
	{
		static_assert(I < 3, "Invalid axis for rotation");
		Expects(nearUnit(from));
		return unchecked::rotation(from, to);
	}
	template <class T, int K, size_t I>
	auto rotation(Axes<I> from, const Vector<T, 3, K>& to)
	{
		static_assert(I < 3, "Invalid axis for rotation");
		Expects(nearUnit(to));
		return unchecked::rotation(from, to);
	}


	template <class A, class T, int K, class = if_scalar_t<A>>
	auto rotation(A angle, const Vector<T, 3, K>& axis) { Expects(nearUnit(axis)); auto a = angle * 0.5f; return quaternion(cos(a), axis*sin(a)); }

	template <class T, class = if_scalar_t<T>> auto rotation(T angle, Axes<0>) { auto a = angle * 0.5f; return quaternion(T(sin(a)), T(0), T(0), T(cos(a))); }
	template <class T, class = if_scalar_t<T>> auto rotation(T angle, Axes<1>) { auto a = angle * 0.5f; return quaternion(T(0), T(sin(a)), T(0), T(cos(a))); }
	template <class T, class = if_scalar_t<T>> auto rotation(T angle, Axes<2>) { auto a = angle * 0.5f; return quaternion(T(0), T(0), T(sin(a)), T(cos(a))); }

	template <class T, class A, size_t I, class = if_scalar_t<A>>
	auto rotation(A angle, Component<T, I>) { return quaternion<T>(angle, Axes<I>{}); }

	template <class A, class B>
	auto angle(const Quaternion<A>& a, const Quaternion<B>& b)
	{
		auto p = dot(a, b);
		using P = decltype(p);

		if (p < P(-1) || p > P(+1))
			return acos(P(1));

		return acos(2 * p*p - 1);
	}


}

#define UVECTOR_QUATERNION_DEFINED

#ifdef UVECTOR_MATRIX_DEFINED
#include "cross/matrix_quaternion.h"
#endif

#pragma once

namespace uv
{
	template <class T>
	class Quat
	{
	public:
		Vec<T, 3> im;
		T re;

		Quat() { }
		constexpr Quat(const Quat& q) = default;
		constexpr Quat(Identity) : im(T(1)), re(T(0)) { }
		template <class S>
		constexpr Quat(const Quat<S>& q) : im(q.im), re(q.re) { }

		template <class V, class = if_vector_t<3, V>>
		constexpr Quat(T re, const V& im) : im(im), re(re) { }

		friend type::mul<T> square(const Quat& q) { return square(q.re) + square(q.im); }
		friend Quat conjugate(const Quat& q) { return { q.re, -q.im }; }

		Quat operator-() const { return { -re, -im }; }

		template <class S, class = if_scalar_t<S>> friend auto operator+(const Quat& q, S s) { return Quat<type::add<T, S>>{ q.re + s, q.im }; }
		template <class S, class = if_scalar_t<S>> friend auto operator-(const Quat& q, S s) { return Quat<type::add<T, S>>{ q.re - s, q.im }; }
		template <class S, class = if_scalar_t<S>> friend auto operator*(const Quat& q, S s) { return Quat<type::mul<T, S>>{ q.re * s, q.im * s }; }
		template <class S, class = if_scalar_t<S>> friend auto operator/(const Quat& q, S s) { return Quat<type::div<T, S>>{ q.re / s, q.im / s }; }
		template <class S, class = if_scalar_t<S>> friend auto operator+(S s, const Quat& q) { return Quat<type::add<S, T>>{ s + q.re, q.im }; }
		template <class S, class = if_scalar_t<S>> friend auto operator-(S s, const Quat& q) { return Quat<type::add<S, T>>{ s - q.re, -q.im }; }
		template <class S, class = if_scalar_t<S>> friend auto operator*(S s, const Quat& q) { return Quat<type::mul<S, T>>{ s * q.re, s * q.im }; }

		template <class S, int K>
		friend Vec<type::mul<T, S>, 3> operator*(const Quat& q, const Vec<S, 3, K>& v)
		{
			return v + twice(cross(q.im, cross(q.im, v) + q.re*v));
		}

		template <size_t I>
		friend Vec<type::mul<T>, 3> operator*(const Quat& q, Axes<I> a)
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
		friend auto operator*(const Quat& q, Component<S, I> c) { return (q*Axes<I>{}) * *c; }

		template <class S>
		friend type::mul<T, S> dot(const Quat& a, const Quat<S>& b) { return a.re*b.re + dot(a.im, b.im); }

		template <class S> Quat<type::add<T, S>> operator+(const Quat<S>& b) const { return { re + b.re, im + b.im }; }
		template <class S> Quat<type::add<T, S>> operator-(const Quat<S>& b) const { return { re - b.re, im - b.im }; }
		template <class S>
		constexpr Quat<type::mul<T, S>> operator*(const Quat<S>& b) const
		{
			return { re * b.re - dot(im, b.im),  re * b.im + b.re * im + cross(im, b.im) };
		}
	};
	namespace details
	{
		template <class T>
		struct Scalar<Quat<T>> { using type = T; };
	}
	using floatq = Quat<float>;
	using doubleq = Quat<double>;

	template <class T, class V, class = if_vector_t<3, V>>
	constexpr Quat<T> quaternion(T re, const V& im) { return { re, im }; }
	template <class T, class = if_scalar_t<T>>
	constexpr Quat<T> quaternion(T re) { return { re, T(0) }; }


	template <class A, class B>
	auto angle(const Quat<A>& a, const Quat<B>& b)
	{
		auto p = dot(a, b);
		using P = decltype(p);

		if (p < P(-1) || p > P(+1))
			return acos(P(1));

		return acos(2 * p*p - 1);
	}


	template <class T>
	class Rot3;

	
	// A rotation in two dimensions represented by the sine and cosine of the angle
	template <class T>
	class Rot2
	{
		template <class S>
		friend class Rot2;
		Dir<T, 2> _x;
	public:
		Rot2() { }
		constexpr Rot2(Identity) : _x(axes::X) { }
		constexpr Rot2(const Dir<T, 2>& x) : _x(x) { }

		friend Rot2 invert(const Rot2& r) { return { uncheckedDir(vector(_x[0], -_x[1])) }; }

		template <class V, class = if_vector_t<2, V>> friend auto operator*(const Rot2& r, const V& v) { return vector(dot(r._x, v), cross(r._x, v)); }
		template <class V, class = if_vector_t<2, V>> friend auto operator*(const V& v, const Rot2& r) { return invert(r)*v; }

		friend Dir<T, 2> operator*(const Rot2& r, Axes<0>) { return r._x; }
		friend Dir<T, 2> operator*(const Rot2& r, Axes<1>) { return uncheckedDir(vector(-r._x[1], r._x[0])); }
		friend Dir<T, 2> operator*(Axes<0> a, const Rot2& r) { return invert(r) * a; }
		friend Dir<T, 2> operator*(Axes<1> a, const Rot2& r) { return invert(r) * a; }

		template <class S> Rot2& operator*=(const S& b) { *this = *this * b; return *this; }

		template <class S> friend Dir<type::mul<T, S>, 2> operator*(const Rot2& r, const Dir<S, 2>& d) { return uncheckedDir(r * static_cast<const Vec<S, 2>&>(d)); }
		template <class S> friend Dir<type::mul<S, T>, 2> operator*(const Dir<S, 2>& d, const Rot2& r) { return uncheckedDir(static_cast<const Vec<S, 2>&>(d) * r); }

		template <class S> Rot2<type::mul<T, S>> operator*(const Rot2<S>& b) const { return { *this * b._x }; }

		template <class V, class S = type::mul<T, scalar<V>>>
		Rot3<S> about(const V&) const;
	};

	template <class T, class U = decltype(cos(std::declval<T>()))>
	constexpr Rot2<U> rotation(T angle) { return { Dir2<U>::fromUnchecked(Vec2<U>(cos(angle), sin(angle))) }; }
	template <class U, class T>
	constexpr Rot2<U> rotation(T angle) { return { Dir2<U>::fromUnchecked(Vec2<U>(U(cos(angle)), U(sin(angle)))) }; }

	template <class T> constexpr auto operator*(Rot2<T> r, Angle a) { return r * rotation(a); }
	template <class T> constexpr auto operator*(Angle a, Rot2<T> r) { return rotation(a) * r; }


	// A rotation in three dimensions represented as a unit quaternion
	template <class T>
	class Rot3
	{
		template <class S>
		friend class Rot3;
		Quat<T> _q;

		constexpr Rot3(const Quat<T>& q) : _q(q) { }

	public:
		Rot3() = delete;
		constexpr Rot3(Identity) : _q(identity) { }
		constexpr Rot3(const Rot3& b) : _q(b._q) { }
		template <class S>
		constexpr Rot3(const Rot3<S>& b) : _q(b._q) { }

		friend const Quat<T>& quaternion(const Rot3<T>& r) { return r._q; }

		friend Rot3 invert(const Rot3& r) { return { conjugate(r._q) }; }

		static constexpr Rot3 fromUnchecked(const Quat<T>& q) { /*Expects(nearUnit(q));*/ return { q }; }

		template <class V, class = if_vector_t<3, V>> friend auto operator*(const Rot3& r, const V& v)
		{
			if constexpr (is_unit_v<3, V>)
				return uncheckedDir(r._q * v);
			else
				return r._q * v;
		}
		template <class V, class = if_vector_t<3, V>> friend auto operator*(const V& v, const Rot3& r)
		{
			if constexpr (is_unit_v<3, V>)
				return uncheckedDir(v * r._q);
			else
				return v * r._q;
		}

		template <class S> constexpr Rot3<type::mul<T, S>> operator*(const Rot3<S>& rb) const { return { _q * rb._q }; }

		Rot3& operator*=(const Rot3& rb) { *this = *this * rb; return *this; }
	};

	template <class T>
	struct Decomposed<Quat<T>>
	{
		T length;
		Rot3<T> direction;

		Decomposed(const Quat<T>& q) : length(uv::length(q)), direction(Rot3<T>::fromUnchecked(q/length)) { }
	};

	template <class T>
	Rot3<T> rotation(const Quat<T>& q) { return decompose(q).direction; }

	template <class A, class B>
	auto rotation(const A& from, const B& to)
	{
		static_assert(is_unit_v<3, A>,  "First argument must be a unit vector");
		static_assert(is_unit_v<3, B>, "Second argument must be a unit vector");
		using U = type::identity<type::add<scalar<A>, scalar<B>>>;
		if (square(from - to) < 0.00001f)
			return Rot3<U>(identity);

		const auto asa = cross(from, to); // axis * sin(angle)
		const auto ca = dot(from, to);   // cos(angle)
		const auto sa = sqrt(1 - ca*ca); // sin(angle);
		return Rot2<U>(Dir<U, 2>::fromUnchecked(vector(ca, sa))).about(direction(asa));
	}

	template <class T>
	template <class V, class S>
	Rot3<S> Rot2<T>::about(const V& axis) const
	{
		static_assert(is_unit_v<3, V>, "Argument must be a unit vector");
		const T hca = _x[0] * 0.5f; // cos(angle) / 2
		const T cha = sqrt(0.5f + hca); // cos(angle / 2)
		const T sha = cha < 0.1f ?
			T(copysign(sqrt(0.5f - hca), _x[1])) : // accurate around +/-90 degrees:  sin(angle/2) = |sin(angle/2)|*sign(sin(angle))
			T(_x[1] / (2 * cha)); // sin(angle / 2) = sin(angle) / 2cos(angle/2), from sin(angle) = 2sin(angle/2)cos(angle/2)

		return Rot3<S>::fromUnchecked(quaternion(cha, sha*axis));
	}

	template <class V, class = if_vector_t<3, V>>
	Rot3<scalar<V>> rotation(const V& v) { const auto c = decompose(v); return rotation(c.length).about(c.direction); }

	template <class V>
	constexpr auto Angle::about(const V& axis) const
	{
		return Rot3<weak_double>::fromUnchecked(quaternion((*this/2).cos(), (*this/2).sin()*axis));
	}

}

#define UVECTOR_ROTATION_DEFINED

#ifdef UVECTOR_MATRIX_DEFINED
#include "cross/matrix_rotation.h"
#endif

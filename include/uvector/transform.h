#pragma once

#include "matrix.h"

namespace uv
{
	struct Origo { };
	static constexpr Origo origo;

	template <class T, size_t N>
	class Point
	{
	public:
		Vector<T, N> v;

		Point() { }
		Point(Origo) : v(T(0)) { }
		Point(const Vector<T, N>& a) : v(a) { }

		Point& operator=(Origo)                 { v = T(0); return *this; }
		Point& operator=(const Vector<T, N>& a) { v = a;    return *this; }
	};


	template <class T, size_t N>
	using Direction = Vector<T, N>;
	template <class T, size_t N>
	using Rotation = Matrix<type::identity<T>, N, N>;

	template <class T, size_t N>
	class Transform
	{
	public:
		Rotation <T, N> R;
		Direction<T, N> t;
	};

}

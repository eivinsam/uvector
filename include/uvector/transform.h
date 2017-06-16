#pragma once

#include "matrix.h"

namespace uv
{
	struct Origo { };
	static constexpr Origo origo;

	template <class T, size_t N, int K = 1>
	class Point
	{
	public:
		Vector<T, N, K> v;

		Point() { }
		Point(Origo) : v(T(0)) { }
		template <int KB>
		Point(const Vector<T, N, KB>& a) : v(a) { }

		Point& operator=(Origo)                 { v = T(0); return *this; }
		Point& operator=(const Vector<T, N>& a) { v = a;    return *this; }
	};
	using Point2f = Point<float, 2>;
	using Point3f = Point<float, 3>;
	using Point4f = Point<float, 4>;
	using Point2d = Point<double, 2>;
	using Point3d = Point<double, 3>;
	using Point4d = Point<double, 4>;

	template <class T, size_t N>
	class Transform
	{
	public:
		Matrix<type::identity<T>, N, N> Rs;
		Vector<T, N> t;
	};
	using Transform2f = Transform<float, 2>;
	using Transform3f = Transform<float, 3>;
	using Transform4f = Transform<float, 4>;
	using Transform2d = Transform<double, 2>;
	using Transform3d = Transform<double, 3>;
	using Transform4d = Transform<double, 4>;
}

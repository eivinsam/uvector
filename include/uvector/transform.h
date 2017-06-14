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
	using Point2f = Point<float, 2>;
	using Point3f = Point<float, 3>;
	using Point4f = Point<float, 4>;
	using Point2d = Point<double, 2>;
	using Point3d = Point<double, 3>;
	using Point4d = Point<double, 4>;


	template <class T, size_t N>
	using Direction = Vector<T, N>;
	using Direction2f = Direction<float, 2>;
	using Direction3f = Direction<float, 3>;
	using Direction4f = Direction<float, 4>;
	using Direction2d = Direction<double, 2>;
	using Direction3d = Direction<double, 3>;
	using Direction4d = Direction<double, 4>;

	template <class T, size_t N>
	using Rotation = Matrix<type::identity<T>, N, N>;
	using Rotation2f = Rotation<float, 2>;
	using Rotation3f = Rotation<float, 3>;
	using Rotation4f = Rotation<float, 4>;
	using Rotation2d = Rotation<double, 2>;
	using Rotation3d = Rotation<double, 3>;
	using Rotation4d = Rotation<double, 4>;

	template <class T, size_t N>
	class Transform
	{
	public:
		Rotation <T, N> R;
		Direction<T, N> t;
	};
	using Transform2f = Transform<float, 2>;
	using Transform3f = Transform<float, 3>;
	using Transform4f = Transform<float, 4>;
	using Transform2d = Transform<double, 2>;
	using Transform3d = Transform<double, 3>;
	using Transform4d = Transform<double, 4>;
}

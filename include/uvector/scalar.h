#pragma once

namespace uv
{
	template <class T>
	static constexpr T pi = T(3.1415926535897932384626433832795);
	static constexpr auto pif = pi<float>;
	static constexpr auto pid = pi<double>;

	template <class T>
	struct Scalar
	{
		T value;

		Scalar() { }
		constexpr Scalar(T v) : value(v) { }

		constexpr Scalar& operator=(T v) { value = v; }

		constexpr T operator*() const { return value; }
		constexpr T operator[](size_t) const { return value; }
	};

	template <class T>
	Scalar<T> scalar(T value) { return { value }; }
}

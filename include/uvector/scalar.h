#pragma once

namespace uv
{
	template <class T>
	static constexpr T pi = T(3.1415926535897932384626433832795);
	static constexpr auto pif = pi<float>;
	static constexpr auto pid = pi<double>;

	template <class T>
	struct scalar
	{
		T value;

		scalar() { }
		constexpr scalar(T v) : value(v) { }

		constexpr scalar& operator=(T v) { value = v; }

		constexpr T operator*() const { return value; }
		constexpr T operator[](size_t) const { return value; }
	};

	template <class T>
	scalar<T> s(T value) { return { value }; }
}

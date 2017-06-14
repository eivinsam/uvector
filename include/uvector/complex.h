#pragma once

#include "vector.h"

namespace uv
{
	struct Imaginary { };
	static constexpr Imaginary imaginary;

	template <class T>
	struct Imag
	{
		static_assert(is_scalar_v<T>, "Imaginary component must be scalar");
		T value;
	};


	template <class T>
	class Complex
	{
	public:
		T       real;
		Imag<T> imag;
	};

	using floatc  = Complex<float>;
	using doublec = Complex<double>;
}

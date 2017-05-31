#pragma once

#include <type_traits>

namespace uv
{
	template <class T>
	static constexpr T pi = T(3.1415926535897932384626433832795);
	static constexpr auto pif = pi<float>;
	static constexpr auto pid = pi<double>;

	template <class T>
	struct is_scalar : public std::is_arithmetic<T> { };
	template <class T>
	static constexpr bool is_scalar_v = is_scalar<T>::value;

	template <class T, class R = void>
	struct if_scalar : public std::enable_if<is_scalar_v<T>, R> { };
	template <class T, class R = void>
	using if_scalar_t = typename if_scalar<T, R>::type;
}

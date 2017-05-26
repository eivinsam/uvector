#pragma once

namespace uv
{
	template <class T, size_t I>
	class Component
	{
		T _value;
	public:
		Component() { }
		explicit Component(T value) : _value(value) { }

		      T& operator*()       { return _value; }
		const T& operator*() const { return _value; }

		Component operator-() const { return Component{ -_value }; }
	};

	namespace components
	{
		template <class T> auto X(T value) { return Component<T, 0>{ value }; }
		template <class T> auto Y(T value) { return Component<T, 1>{ value }; }
		template <class T> auto Z(T value) { return Component<T, 2>{ value }; }
		template <class T> auto W(T value) { return Component<T, 3>{ value }; }
	}
}

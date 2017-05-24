#pragma once

#include "vector.h"

namespace uv
{
	template <class T, size_t I>
	class component
	{
		T _value;
	public:
		component() { }
		explicit component(T value) : _value(value) { }

		      T& operator*()       { return _value; }
		const T& operator*() const { return _value; }

		component operator-() const { return component{ -_value }; }
	};

	namespace components
	{
		template <class T> auto X(T value) { return component<T, 0>{ value }; }
		template <class T> auto Y(T value) { return component<T, 1>{ value }; }
		template <class T> auto Z(T value) { return component<T, 2>{ value }; }
		template <class T> auto W(T value) { return component<T, 3>{ value }; }
	}
}

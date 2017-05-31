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

	template <size_t I>
	struct Axis { };

	namespace components
	{
		static constexpr Axis<0> X;
		static constexpr Axis<1> Y;
		static constexpr Axis<2> Z;
		static constexpr Axis<3> W;
	}
}

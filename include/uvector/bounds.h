#pragma once

#include "vector.h"

namespace uv
{
	struct empty_t { };
	static constexpr empty_t empty;

	template <class T, size_t N = 1>
	struct Bounds;

	template <class T>
	struct Bounds<T, 1>
	{
		static_assert(is_scalar_v<T>, "T must be scalar");
		T min;
		T max;

		Bounds() = default;
		constexpr Bounds(T value) { *this = value; }
		constexpr Bounds(empty_t) { *this = empty; }

		constexpr Bounds& operator=(T value) { min = max = value; return *this; }
		constexpr Bounds& operator=(empty_t) { min = T(1); max = T(0); return *this; }

		      T& operator[](bool i)       { return reinterpret_cast<      T*>(this)[i]; }
		const T& operator[](bool i) const { return reinterpret_cast<const T*>(this)[i]; }

		explicit operator bool() const { return max >= min; }
	};
	template <class T>
	struct is_scalar<Bounds<T, 1>> : public std::true_type { };

	template <class T>       T& min(      Bounds<T>& b) { return b.min; }
	template <class T> const T& min(const Bounds<T>& b) { return b.min; }
	template <class T>       T& max(      Bounds<T>& b) { return b.max; }
	template <class T> const T& max(const Bounds<T>& b) { return b.max; }

	using Boundsf = Bounds<float>;
	using Boundsd = Bounds<double>;


	template <class T, size_t N>
	struct Bounds : public Vector<Bounds<T>, N>
	{
		using Vector<Bounds<T>, N>::Vector;
		using Vector<Bounds<T>, N>::operator=;
	};
	template <class T, size_t N>
	struct is_vector<Bounds<T, N>> { static constexpr bool value = N > 1; };

	template <class T, size_t N, int K> auto& min(      Vector<Bounds<T>, N, K>& b) { return reinterpret_cast<      Vector<T, N, K * 2>&>(b[0].min); }
	template <class T, size_t N, int K> auto& min(const Vector<Bounds<T>, N, K>& b) { return reinterpret_cast<const Vector<T, N, K * 2>&>(b[0].min); }
	template <class T, size_t N, int K> auto& max(      Vector<Bounds<T>, N, K>& b) { return reinterpret_cast<      Vector<T, N, K * 2>&>(b[0].max); }
	template <class T, size_t N, int K> auto& max(const Vector<Bounds<T>, N, K>& b) { return reinterpret_cast<const Vector<T, N, K * 2>&>(b[0].max); }

	using Bounds2f = Bounds<float, 2>;
	using Bounds3f = Bounds<float, 3>;
	using Bounds4f = Bounds<float, 4>;

	using Bounds2d = Bounds<double, 2>;
	using Bounds3d = Bounds<double, 3>;
	using Bounds4d = Bounds<double, 4>;

}


#pragma once

#include "../vector.h"
#include "../bounds.h"

namespace uv
{
	template <class T, size_t N>
	struct Bounds : public Vector<Bounds<T>, N>
	{ 
		using Vector<Bounds<T>, N>::Vector;
		using Vector<Bounds<T>, N>::operator=;
	};

	using Bounds2f = Bounds<float, 2>;
	using Bounds3f = Bounds<float, 3>;
	using Bounds4f = Bounds<float, 4>;

	using Bounds2d = Bounds<double, 2>;
	using Bounds3d = Bounds<double, 3>;
	using Bounds4d = Bounds<double, 4>;

	template <class T, size_t N, int K> auto& min(      Vector<Bounds<T>, N, K>& b) { return reinterpret_cast<      Vector<T, N, K * 2>&>(b[0].min); }
	template <class T, size_t N, int K> auto& min(const Vector<Bounds<T>, N, K>& b) { return reinterpret_cast<const Vector<T, N, K * 2>&>(b[0].min); }
	template <class T, size_t N, int K> auto& max(      Vector<Bounds<T>, N, K>& b) { return reinterpret_cast<      Vector<T, N, K * 2>&>(b[0].max); }
	template <class T, size_t N, int K> auto& max(const Vector<Bounds<T>, N, K>& b) { return reinterpret_cast<const Vector<T, N, K * 2>&>(b[0].max); }
}

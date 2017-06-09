#pragma once

#include "details/vector_details.h"
#include "scalar_operations.h"

#define TEMPLATE_VECTOR_A template <class A, size_t NA, int KA>
#define TEMPLATE_VECTORS_AB template <class A, class B, size_t NA, size_t NB, int KA, int KB>
#define TEMPLATE_ANY_VECTOR template <class V, class S = typename std::remove_reference_t<V>::scalar_type>
#define TEMPLATE_VECTOR_A_SCALAR_B template <class A, class B, size_t NA, int KA, class = if_scalar_t<B>>
#define VECTOR_A Vector<A, NA, KA> 
#define VECTOR_B Vector<B, NB, KB>


namespace uv
{
	namespace require
	{
		template <size_t A, size_t B>
		constexpr details::equal_test<A, B> equal = {};
	}

	template <size_t... Indices, class V>
	auto&& select(V&& source) { return details::selector<Indices...>::on(source); }

	template <class T, size_t I, class = if_scalar_t<T>> auto operator*(Axes<I>, T value) { return Component<T, I>(value); }
	template <class T, size_t I, class = if_scalar_t<T>> auto operator*(T value, Axes<I>) { return Component<T, I>(value); }

	template <class T, size_t N, int K, size_t I> auto& operator*(      Vector<T, N, K>& v, Axes<I>) { return v[I]; }
	template <class T, size_t N, int K, size_t I> auto& operator*(const Vector<T, N, K>& v, Axes<I>) { return v[I]; }
	template <class T, size_t N, int K, size_t I> auto& operator*(Axes<I>,       Vector<T, N, K>& v) { return v[I]; }
	template <class T, size_t N, int K, size_t I> auto& operator*(Axes<I>, const Vector<T, N, K>& v) { return v[I]; }

	template <class T, size_t N, int K, size_t... I> auto& operator*(      Vector<T, N, K>& v, Axes<I...>) { return select<I...>(v); }
	template <class T, size_t N, int K, size_t... I> auto& operator*(const Vector<T, N, K>& v, Axes<I...>) { return select<I...>(v); }
	template <class T, size_t N, int K, size_t... I> auto& operator*(Axes<I...>,       Vector<T, N, K>& v) { return select<I...>(v); }
	template <class T, size_t N, int K, size_t... I> auto& operator*(Axes<I...>, const Vector<T, N, K>& v) { return select<I...>(v); }

	template <class A, class B, size_t N, int K, size_t I>
	auto operator+(const Vector<A, N, K>& v, Component<B, I> c)
	{
		Vector<type::of<op::add, A, B>, std::max(N, I + 1)> result;
		for (int i = 0; i < N; ++i)
			result[i] = v[i];
		for (int i = N; i <= I; ++i)
			result[i] = A(0);
		result[I] += *c;
		return result;
	}
	template <class A, class B, size_t N, int K, size_t I>
	auto operator+(Component<B, I> c, const Vector<A, N, K>& v) { return v + c; }
	template <class A, class B, size_t N, int K, size_t I>
	auto operator-(const Vector<A, N, K>& v, Component<B, I> c)
	{
		Vector<type::of<op::sub, A, B>, std::max(N, I + 1)> result;
		for (int i = 0; i < N; ++i)
			result[i] = v[i];
		for (int i = N; i <= I; ++i)
			result[i] = A(0);
		result[I] -= *c;
		return result;
	}
	template <class A, class B, size_t N, int K, size_t I>
	auto operator-(Component<B, I> c, const Vector<A, N, K>& v)
	{
		Vector<type::of<op::sub, B, A>, std::max(N, I + 1)> result;
		for (int i = 0; i < N; ++i)
			result[i] = -v[i];
		for (int i = N; i <= I; ++i)
			result[i] = 0;
		result[I] += *c;
		return result;
	}

	template <class A, class B, size_t IA, size_t IB>
	inline auto operator+(Component<A, IA> a, Component<B, IB> b) { return details::component_op<A, B, IA, IB>::add(a, b); }
	template <class A, class B, size_t IA, size_t IB>
	inline auto operator-(Component<A, IA> a, Component<B, IB> b) { return details::component_op<A, B, IA, IB>::sub(a, b); }

	template <class A, class B, size_t N, int K, size_t I>
	auto operator*(const Vector<A, N, K>& v, Component<B, I> c)
	{
		static_assert(I < N, "invalid vector-component combination");
		return Component<type::of<op::mul, A, B>, I>{ v[I] * *c };
	}
	template <class A, class B, size_t N, int K, size_t I>
	auto operator*(Component<B, I> c, const Vector<A, N, K>& v) { return v*c; }
	template <class A, class B, size_t IA, size_t IB>
	auto operator*(Component<A, IA> a, Component<B, IB> b)
	{
		static_assert(IA == IB, "multiplication between different components");
		return Component<type::of<op::mul, A, B>, IA>{ *a * *b };
	}

	template <class A, class B, size_t I>
	bool operator==(Component<A, I> a, B b) { return a.value == b; }


	template <class T, size_t N> auto data(      Vector<T, N>& v) { return v.begin(); }
	template <class T, size_t N> auto data(const Vector<T, N>& v) { return v.begin(); }

	TEMPLATE_VECTOR_A auto& rest(      VECTOR_A& v) { return reinterpret_cast<      Vector<A, NA-1, KA>&>(v[1]); }
	TEMPLATE_VECTOR_A auto& rest(const VECTOR_A& v) { return reinterpret_cast<const Vector<A, NA-1, KA>&>(v[1]); }

	template <int K>
	bool any(const Vector<bool, 2, K>& v) { return v[0] | v[1]; }
	template <size_t N, int K>
	bool any(const Vector<bool, N, K>& v) { return v[0] | any(rest(v)); }
	template <int K>
	bool all(const Vector<bool, 2, K>& v) { return v[0] & v[1]; }
	template <size_t N, int K>
	bool all(const Vector<bool, N, K>& v) { return v[0] & all(rest(v)); }
	template <class T, int K>
	auto sum(const Vector<T, 2, K>& v) { return v[0] + v[1]; }
	template <class T, size_t N, int K>
	auto sum(const Vector<T, N, K>& v) { return v[0] + sum(rest(v)); }
	template <class T, int K>
	auto product(const Vector<T, 2, K>& v) { return v[0] * v[1]; }
	template <class T, size_t N, int K>
	auto product(const Vector<T, N, K>& v) { return v[0] * product(rest(v)); }
	template <class T, int K>
	auto min(const Vector<T, 2, K>& v) { return op::min{}(v[0], v[1]); }
	template <class T, size_t N, int K>
	auto min(const Vector<T, N, K>& v) { return op::min{}(v[0], min(rest(v))); }
	template <class T, int K>
	auto max(const Vector<T, 2, K>& v) { return op::max{}(v[0], v[1]); }
	template <class T, size_t N, int K>
	auto max(const Vector<T, N, K>& v) { return op::max{}(v[0], max(rest(v))); }

	TEMPLATE_VECTOR_A
	auto differences(const VECTOR_A& a)
	{
		Vector<type::sub<A>, NA - 1> result;
		for (size_t i = 0; i < NA - 1; ++i)
			result[i] = a[i + 1] - a[i];
		return result;
	}

	template <size_t N, int K> inline auto operator!(const Vector<bool, N, K>& a) { Vector<bool, N> r; for (size_t i = 0; i < N; ++i) r[i] = !a[i]; return r; }


	TEMPLATE_VECTORS_AB	inline auto operator==(const VECTOR_A& a, const VECTOR_B& b) { return details::apply<op::eq>(a, b); }
	TEMPLATE_VECTORS_AB	inline auto operator!=(const VECTOR_A& a, const VECTOR_B& b) { return details::apply<op::ne>(a, b); }
	TEMPLATE_VECTORS_AB	inline auto operator< (const VECTOR_A& a, const VECTOR_B& b) { return details::apply<op::sl>(a, b); }
	TEMPLATE_VECTORS_AB	inline auto operator<=(const VECTOR_A& a, const VECTOR_B& b) { return details::apply<op::le>(a, b); }
	TEMPLATE_VECTORS_AB	inline auto operator>=(const VECTOR_A& a, const VECTOR_B& b) { return details::apply<op::ge>(a, b); }
	TEMPLATE_VECTORS_AB	inline auto operator> (const VECTOR_A& a, const VECTOR_B& b) { return details::apply<op::sg>(a, b); }

	TEMPLATE_VECTORS_AB	inline auto operator+(const VECTOR_A& a, const VECTOR_B& b) { return details::apply<op::add>(a, b); }
	TEMPLATE_VECTORS_AB inline auto operator-(const VECTOR_A& a, const VECTOR_B& b) { return details::apply<op::sub>(a, b); }
	TEMPLATE_VECTORS_AB inline auto operator*(const VECTOR_A& a, const VECTOR_B& b) { return details::apply<op::mul>(a, b); }

	TEMPLATE_VECTORS_AB inline auto min(const VECTOR_A& a, const VECTOR_B& b) { return details::apply<op::min>(a, b); }
	TEMPLATE_VECTORS_AB inline auto max(const VECTOR_A& a, const VECTOR_B& b) { return details::apply<op::max>(a, b); }

	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator==(const VECTOR_A& a, B b) { return details::apply<op::eq>(a, b); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator==(B b, const VECTOR_A& a) { return details::apply<op::eq>(b, a); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator!=(const VECTOR_A& a, B b) { return details::apply<op::ne>(a, b); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator!=(B b, const VECTOR_A& a) { return details::apply<op::ne>(b, a); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator< (const VECTOR_A& a, B b) { return details::apply<op::sl>(a, b); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator< (B b, const VECTOR_A& a) { return details::apply<op::sl>(b, a); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator<=(const VECTOR_A& a, B b) { return details::apply<op::le>(a, b); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator<=(B b, const VECTOR_A& a) { return details::apply<op::le>(b, a); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator>=(const VECTOR_A& a, B b) { return details::apply<op::ge>(a, b); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator>=(B b, const VECTOR_A& a) { return details::apply<op::ge>(b, a); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator> (const VECTOR_A& a, B b) { return details::apply<op::sg>(a, b); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator> (B b, const VECTOR_A& a) { return details::apply<op::sg>(b, a); }

	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator+(const VECTOR_A& a, B b) { return details::apply<op::add>(a, b); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator+(B b, const VECTOR_A& a) { return details::apply<op::add>(b, a); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator-(const VECTOR_A& a, B b) { return details::apply<op::sub>(a, b); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator-(B b, const VECTOR_A& a) { return details::apply<op::sub>(b, a); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator*(const VECTOR_A& a, B b) { return details::apply<op::mul>(a, b); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator*(B b, const VECTOR_A& a) { return details::apply<op::mul>(b, a); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator/(const VECTOR_A& a, B b) { return details::apply<op::div>(a, b); }

	namespace details
	{
		template <class A, class B, size_t N, int KA, int KB>
		struct binary_op
		{
			static auto dot(const Vector<A, N, KA>& a, const Vector<B, N, KB>& b)
			{
				return sum(a*b);
			}
		};
		template <class A, class B, int KA, int KB>
		struct binary_op<A, B, 2, KA, KB>
		{
			static auto dot(const Vector<A, 2, KA>& a, const Vector<B, 2, KB>& b)
			{
				return a[0] * b[0] + a[1] * b[1];
			}
			static auto cross(const Vector<A, 2, KA>& a, const Vector<B, 2, KB>& b)
			{
				return a[0] * b[1] - a[1] * b[0];
			}
		};
		template <class A, class B, int KA, int KB>
		struct binary_op<A, B, 3, KA, KB>
		{
			static auto dot(const Vector<A, 3, KA>& a, const Vector<B, 3, KB>& b)
			{
				return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
			}
			static auto cross(const Vector<A, 3, KA>& a, const Vector<B, 3, KB>& b)
			{
				return Vector<type::of<op::sub, type::of<op::mul, A, B>>, 3>(
						a[1] * b[2] - a[2] * b[1],
						a[2] * b[0] - a[0] * b[2],
						a[0] * b[1] - a[1] * b[0]);
			}
		};
	}

	TEMPLATE_VECTORS_AB
		type::inner_product<A, B> dot(const VECTOR_A& a, const VECTOR_B& b)
	{
		require::equal<NA, NB>;
		return details::binary_op<A, B, NA, KA, KB>::dot(a, b);
	}
	TEMPLATE_VECTORS_AB
		auto cross(const VECTOR_A& a, const VECTOR_B& b)
	{
		require::equal<NA, NB>;
		static_assert(NA == 2 || NA == 3, "cross product only defined in 3 or 2 dimensions");
		return details::binary_op<A, B, NA, KA, KB>::cross(a, b);
	}

	TEMPLATE_VECTOR_A auto square(const VECTOR_A& a) { return sum(a*a); }
	TEMPLATE_VECTOR_A auto length(const VECTOR_A& a) { return sqrt(square(a)); }

	TEMPLATE_VECTOR_A auto abs(const VECTOR_A& a)
	{
		using namespace std;
		VECTOR_A result;
		for (size_t i = 0; i < NA; ++i)
			result[i] = abs(a[i]);
		return result;
	}

	template <size_t N, int K>
	constexpr size_t index(const Vector<bool, N, K>& v)
	{
		size_t result = 0;
		for (size_t i = 0; i < N; ++i)
			result |= (v[i] << N);
		return result;
	}
	static constexpr size_t index() { return 0; };
	template <size_t... I>
	constexpr size_t index(Axes<I...>) { return details::index_s<I...>::value; }

	template <class First, class... Rest>
	inline auto vector(const First& first, const Rest&... rest)
	{
		using namespace details;
		Vector<scalar_of<First>, element_count<First, Rest...>::value> result;
		write_vector(result.data(), first, rest...);
		return result;
	}
	template <class T, class... Args>
	inline auto vector(Args... args)
	{
		using namespace details;
		Vector<T, element_count<Args...>::value> result;
		write_vector(result.data(), args...);
		return result;
	}


	template <class T, size_t N, int K>
	std::ostream& operator<<(std::ostream& out, const Vector<T, N, K>& v)
	{
		for (size_t i = 0; i < N; ++i)
			out << (i == 0 ? "[" : ", ") << v[i];
		return out << ']';
	}
}

#undef TEMPLATE_VECTOR_A
#undef TEMPLATE_VECTORS_AB
#undef TEMPLATE_ANY_VECTOR
#undef TEMPLATE_VECTOR_A_SCALAR_B
#undef VECTOR_A
#undef VECTOR_B

#define UVECTOR_VECTOR_OPS_DEFINED

#pragma once

#include "details/vector_details.h"
#include "scalar_operations.h"

#pragma push_macro("TEMPLATE_VECTOR_A")
#define TEMPLATE_VECTOR_A template <class A, size_t NA, int KA>
#pragma push_macro("TEMPLATE_VECTORS_AB")
#define TEMPLATE_VECTORS_AB template <class A, class B, size_t NA, size_t NB, int KA, int KB>
#pragma push_macro("TEMPLATE_VECTOR_A_OTHER_B")
#define TEMPLATE_VECTOR_A_OTHER_B template <class A, class B, size_t NA, int KA>
#pragma push_macro("VECTOR_A")
#define VECTOR_A Vector<A, NA, KA> 
#pragma push_macro("VECTOR_B")
#define VECTOR_B Vector<B, NB, KB>

namespace uv
{
	namespace require
	{
		template <size_t A, size_t B>
		static constexpr size_t equal = details::is_equal<A, B>::value;
	}

	template <class A, class B, size_t NA, size_t NB, size_t NC, int KA, int KB, int KC>
	auto ifelse(const Vector<bool, NC, KC>& cond, const VECTOR_A& a, const VECTOR_B& b)
	{
		static constexpr size_t N = require::equal<NC, require::equal<NA, NB>>;
		Vector<type::common<A, B>, N> result;
		for (size_t i = 0; i < N; ++i)
			result[i] = cond[i] ? a[i] : b[i];
		return result;
	}
	template <class A, class B, size_t NA, size_t NC, int KA, int KC>
	auto ifelse(const Vector<bool, NC, KC>& cond, const VECTOR_A& a, B b)
	{
		static_assert(is_scalar_v<B>);
		static constexpr size_t N = require::equal<NA, NC>;
		Vector<type::common<A, B>, N> result;
		for (size_t i = 0; i < N; ++i)
			result[i] = cond[i] ? a[i] : b;
		return result;
	}
	template <class A, class B, size_t NB, size_t NC, int KB, int KC>
	auto ifelse(const Vector<bool, NC, KC>& cond, A a, const VECTOR_B& b)
	{
		static_assert(is_scalar_v<A>);
		static constexpr size_t N = require::equal<NB, NC>;
		Vector<type::common<A, B>, N> result;
		for (size_t i = 0; i < N; ++i)
			result[i] = cond[i] ? a : b[i];
		return result;
	}
	template <class A, class B, size_t NC, int KC>
	auto ifelse(const Vector<bool, NC, KC>& cond, A a, B b)
	{
		static_assert(is_scalar_v<A>);
		static_assert(is_scalar_v<B>);
		static constexpr size_t N = NC;
		Vector<type::common<A, B>, N> result;
		for (size_t i = 0; i < N; ++i)
			result[i] = cond[i] ? a : b;
		return result;
	}

	template <size_t... Indices, class V>
	auto&& select(V&& source) { return details::selector<Indices...>::on(source); }

	template <class T, size_t I, class = if_scalar_t<T>> Component<T, I> operator*(Axes<I>, T value) { return Component<T, I>(value); }
	template <class T, size_t I, class = if_scalar_t<T>> Component<T, I> operator*(T value, Axes<I>) { return Component<T, I>(value); }

	template <class T, size_t N, int K, size_t I>       T& operator*(      Vector<T, N, K>& v, Axes<I>) { return v[I]; }
	template <class T, size_t N, int K, size_t I> const T& operator*(const Vector<T, N, K>& v, Axes<I>) { return v[I]; }
	template <class T, size_t N, int K, size_t I>       T& operator*(Axes<I>,       Vector<T, N, K>& v) { return v[I]; }
	template <class T, size_t N, int K, size_t I> const T& operator*(Axes<I>, const Vector<T, N, K>& v) { return v[I]; }

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


	template <class A, size_t NA, int KA> auto& rest(      VECTOR_A& v) { return reinterpret_cast<      Vector<A, NA-1, KA>&>(v[1]); }
	template <class A, size_t NA, int KA> auto& rest(const VECTOR_A& v) { return reinterpret_cast<const Vector<A, NA-1, KA>&>(v[1]); }

	template <int K>
	bool any(const Vector<bool, 2, K>& v) { return v[0] | v[1]; }
	template <size_t N, int K>
	bool any(const Vector<bool, N, K>& v) { return v[0] | any(rest(v)); }

	template <int K>
	bool all(const Vector<bool, 2, K>& v) { return v[0] & v[1]; }
	template <size_t N, int K>
	bool all(const Vector<bool, N, K>& v) { return v[0] & all(rest(v)); }

	template <class T, int K>
	type::add<T> sum(const Vector<T, 2, K>& v) { return v[0] + v[1]; }
	template <class T, size_t N, int K>
	auto sum(const Vector<T, N, K>& v) { return v[0] + sum(rest(v)); }

	template <class T, int K>
	type::mul<T> product(const Vector<T, 2, K>& v) { return v[0] * v[1]; }
	template <class T, size_t N, int K>
	auto product(const Vector<T, N, K>& v) { return v[0] * product(rest(v)); }

	template <class T, int K>
	T min(const Vector<T, 2, K>& v) { return op::min{}(v[0], v[1]); }
	template <class T, size_t N, int K>
	T min(const Vector<T, N, K>& v) { return op::min{}(v[0], min(rest(v))); }

	template <class T, int K>
	T max(const Vector<T, 2, K>& v) { return op::max{}(v[0], v[1]); }
	template <class T, size_t N, int K>
	T max(const Vector<T, N, K>& v) { return op::max{}(v[0], max(rest(v))); }

	template <class A, size_t NA, int KA>
	Vector<type::sub<A>, NA - 1> differences(const VECTOR_A& a)
	{
		Vector<type::sub<A>, NA - 1> result;
		for (size_t i = 0; i < NA - 1; ++i)
			result[i] = a[i + 1] - a[i];
		return result;
	}

	template <size_t N, int K> inline Vector<bool, N> operator!(const Vector<bool, N, K>& a) { Vector<bool, N> r; for (size_t i = 0; i < N; ++i) r[i] = !a[i]; return r; }

	TEMPLATE_VECTOR_A inline Vector<A, NA> operator-(const VECTOR_A& a) { Vector<A, NA> r; for (size_t i = 0; i < NA; ++i) r[i] = -a[i]; return r; }

	TEMPLATE_VECTORS_AB	inline Vector<bool, NA> operator==(const VECTOR_A& a, const VECTOR_B& b) { return details::vector_apply<op::eq>(a, b); }
	TEMPLATE_VECTORS_AB	inline Vector<bool, NA> operator!=(const VECTOR_A& a, const VECTOR_B& b) { return details::vector_apply<op::ne>(a, b); }
	TEMPLATE_VECTORS_AB	inline Vector<bool, NA> operator< (const VECTOR_A& a, const VECTOR_B& b) { return details::vector_apply<op::sl>(a, b); }
	TEMPLATE_VECTORS_AB	inline Vector<bool, NA> operator<=(const VECTOR_A& a, const VECTOR_B& b) { return details::vector_apply<op::le>(a, b); }
	TEMPLATE_VECTORS_AB	inline Vector<bool, NA> operator>=(const VECTOR_A& a, const VECTOR_B& b) { return details::vector_apply<op::ge>(a, b); }
	TEMPLATE_VECTORS_AB	inline Vector<bool, NA> operator> (const VECTOR_A& a, const VECTOR_B& b) { return details::vector_apply<op::sg>(a, b); }

	TEMPLATE_VECTORS_AB	inline Vector<type::add<A, B>, NA> operator+(const VECTOR_A& a, const VECTOR_B& b) { return details::vector_apply<op::add>(a, b); }
	TEMPLATE_VECTORS_AB inline Vector<type::sub<A, B>, NA> operator-(const VECTOR_A& a, const VECTOR_B& b) { return details::vector_apply<op::sub>(a, b); }
	TEMPLATE_VECTORS_AB inline Vector<type::mul<A, B>, NA> operator*(const VECTOR_A& a, const VECTOR_B& b) { return details::vector_apply<op::mul>(a, b); }

	TEMPLATE_VECTORS_AB inline Vector<type::common<A, B>, NA> min(const VECTOR_A& a, const VECTOR_B& b) { return details::vector_apply<op::min>(a, b); }
	TEMPLATE_VECTORS_AB inline Vector<type::common<A, B>, NA> max(const VECTOR_A& a, const VECTOR_B& b) { return details::vector_apply<op::max>(a, b); }

	TEMPLATE_VECTOR_A_OTHER_B Vector<bool, NA> operator==(const VECTOR_A& a, const B& b) { return details::vector_and<B>::apply<op::eq>(a, b); }
	TEMPLATE_VECTOR_A_OTHER_B Vector<bool, NA> operator!=(const VECTOR_A& a, const B& b) { return details::vector_and<B>::apply<op::ne>(a, b); }
	TEMPLATE_VECTOR_A_OTHER_B Vector<bool, NA> operator< (const VECTOR_A& a, const B& b) { return details::vector_and<B>::apply<op::sl>(a, b); }
	TEMPLATE_VECTOR_A_OTHER_B Vector<bool, NA> operator<=(const VECTOR_A& a, const B& b) { return details::vector_and<B>::apply<op::le>(a, b); }
	TEMPLATE_VECTOR_A_OTHER_B Vector<bool, NA> operator>=(const VECTOR_A& a, const B& b) { return details::vector_and<B>::apply<op::ge>(a, b); }
	TEMPLATE_VECTOR_A_OTHER_B Vector<bool, NA> operator> (const VECTOR_A& a, const B& b) { return details::vector_and<B>::apply<op::sg>(a, b); }
	TEMPLATE_VECTOR_A_OTHER_B Vector<bool, NA> operator==(const B& b, const VECTOR_A& a) { return details::vector_and<B>::apply<op::rev<op::eq>>(a, b); }
	TEMPLATE_VECTOR_A_OTHER_B Vector<bool, NA> operator!=(const B& b, const VECTOR_A& a) { return details::vector_and<B>::apply<op::rev<op::ne>>(a, b); }
	TEMPLATE_VECTOR_A_OTHER_B Vector<bool, NA> operator< (const B& b, const VECTOR_A& a) { return details::vector_and<B>::apply<op::rev<op::sl>>(a, b); }
	TEMPLATE_VECTOR_A_OTHER_B Vector<bool, NA> operator<=(const B& b, const VECTOR_A& a) { return details::vector_and<B>::apply<op::rev<op::le>>(a, b); }
	TEMPLATE_VECTOR_A_OTHER_B Vector<bool, NA> operator>=(const B& b, const VECTOR_A& a) { return details::vector_and<B>::apply<op::rev<op::ge>>(a, b); }
	TEMPLATE_VECTOR_A_OTHER_B Vector<bool, NA> operator> (const B& b, const VECTOR_A& a) { return details::vector_and<B>::apply<op::rev<op::sg>>(a, b); }

	TEMPLATE_VECTOR_A_OTHER_B inline auto operator+(const VECTOR_A& a, const B& b) { return details::vector_and<B>::apply<op::add>(a, b); }
	TEMPLATE_VECTOR_A_OTHER_B inline auto operator-(const VECTOR_A& a, const B& b) { return details::vector_and<B>::apply<op::sub>(a, b); }
	TEMPLATE_VECTOR_A_OTHER_B inline auto operator*(const VECTOR_A& a, const B& b) { return details::vector_and<B>::apply<op::mul>(a, b); }
	TEMPLATE_VECTOR_A_OTHER_B inline auto operator/(const VECTOR_A& a, const B& b) { return details::vector_and<B>::apply<op::div>(a, b); }
	TEMPLATE_VECTOR_A_OTHER_B inline auto operator+(const B& b, const VECTOR_A& a) { return details::vector_and<B>::apply<op::rev<op::add>>(a, b); }
	TEMPLATE_VECTOR_A_OTHER_B inline auto operator-(const B& b, const VECTOR_A& a) { return details::vector_and<B>::apply<op::rev<op::sub>>(a, b); }
	TEMPLATE_VECTOR_A_OTHER_B inline auto operator*(const B& b, const VECTOR_A& a) { return details::vector_and<B>::apply<op::rev<op::mul>>(a, b); }

	TEMPLATE_VECTOR_A_OTHER_B inline auto min(const VECTOR_A& a, const B& b) { return details::vector_and<B>::apply<op::min>(a, b); }
	TEMPLATE_VECTOR_A_OTHER_B inline auto max(const VECTOR_A& a, const B& b) { return details::vector_and<B>::apply<op::max>(a, b); }
	TEMPLATE_VECTOR_A_OTHER_B inline auto min(const B& b, const VECTOR_A& a) { return details::vector_and<B>::apply<op::rev<op::min>>(a, b); }
	TEMPLATE_VECTOR_A_OTHER_B inline auto max(const B& b, const VECTOR_A& a) { return details::vector_and<B>::apply<op::rev<op::max>>(a, b); }

	template <class A, size_t NA, int KA, size_t I> inline auto operator+(const VECTOR_A& a, Axes<I>) { return a + Component<A, I>(1); }
	template <class A, size_t NA, int KA, size_t I> inline auto operator-(const VECTOR_A& a, Axes<I>) { return a - Component<A, I>(1); }
	template <class A, size_t NA, int KA, size_t I> inline auto operator+(Axes<I>, const VECTOR_A& a) { return Component<A, I>(1) + a; }
	template <class A, size_t NA, int KA, size_t I> inline auto operator-(Axes<I>, const VECTOR_A& a) { return Component<A, I>(1) - a; }

	TEMPLATE_VECTORS_AB type::dot<A, B> dot(const VECTOR_A& a, const VECTOR_B& b) { return sum(a*b); }

	template <class A, size_t NA, int KA, int I>
	A dot(const VECTOR_A& a, Axes<I>)
	{
		static_assert(I < NA, "Cannot dot vector with higher-dimensional axis");
		return a[I];
	}
	template <class A, size_t NA, int KA, int I>
	A dot(Axes<I> ax, const VECTOR_A& a) { return dot(a, ax); }

	template <class A, class B, size_t NA, int KA, size_t IB>
	type::dot<A, B> dot(const VECTOR_A& a, Component<B, IB> b) { return dot(a, Axes<IB>{}) * *b; }
	template <class A, class B, size_t NA, int KA, size_t IB>
	type::dot<A, B> dot(Component<B, IB> b, const VECTOR_A& a) { return dot(a, Axes<IB>{}) * *b; }

	TEMPLATE_VECTORS_AB auto angle(const VECTOR_A& a, const VECTOR_B& b)
	{
		using R = type::identity<type::dot<A, B>>;
		return acos(std::clamp(dot(a, b) / sqrt(square(a)*square(b)), R(-1), R(1)));
	}

	template <class A, class B, size_t N, int KA, int KB>
	Vector<type::add<A, B>, N> sum(const Vector<A, N, KA>& a, const Vector<B, N, KB>& b) { return a + b; }
	template <class First, class... Rest, size_t N, int KF>
	auto sum(const Vector<First, N, KF>& first, const Rest&... rest) { return first + sum(rest...); }

	TEMPLATE_VECTORS_AB auto cross(const VECTOR_A& u, const VECTOR_B& v)
	{ 
		static_assert(NA == NB);
		return details::cross_product<NA>::of(u, v);
	}

	template <class T, int K> T cross(Axes<0>, const Vector<T, 2, K>& v) { return +v[1]; }
	template <class T, int K> T cross(Axes<1>, const Vector<T, 2, K>& v) { return -v[0]; }
	template <class T, int K> T cross(const Vector<T, 2, K>& v, Axes<0>) { return -v[1]; }
	template <class T, int K> T cross(const Vector<T, 2, K>& v, Axes<1>) { return +v[0]; }

	template <class T, int K> Vector<T, 3> cross(Axes<0>, const Vector<T, 3, K>& a) { return vector<T>(0, -a[2], +a[1]); }
	template <class T, int K> Vector<T, 3> cross(const Vector<T, 3, K>& a, Axes<0>) { return vector<T>(0, +a[2], -a[1]); }
	template <class T, int K> Vector<T, 3> cross(Axes<1>, const Vector<T, 3, K>& a) { return vector<T>(+a[2], 0, -a[0]); }
	template <class T, int K> Vector<T, 3> cross(const Vector<T, 3, K>& a, Axes<1>) { return vector<T>(-a[2], 0, +a[0]); }
	template <class T, int K> Vector<T, 3> cross(Axes<2>, const Vector<T, 3, K>& a) { return vector<T>(-a[1], +a[0], 0); }
	template <class T, int K> Vector<T, 3> cross(const Vector<T, 3, K>& a, Axes<2>) { return vector<T>(+a[1], -a[0], 0); }

	template <class V, class B, size_t IB> auto cross(const V& a, Component<B, IB> b) { return cross(a, Axes<IB>{}) * *b; }
	template <class V, class B, size_t IB> auto cross(Component<B, IB> b, const V& a) { return *b * cross(Axes<IB>{}, a); }

	TEMPLATE_VECTOR_A bool isfinite(const VECTOR_A& a) { for (size_t i = 0; i < NA; ++i) if (!isfinite(a[i])) return false; return true; }

	TEMPLATE_VECTOR_A type::dot<A> square(const VECTOR_A& a) { return sum(a*a); }
	TEMPLATE_VECTOR_A           A  length(const VECTOR_A& a) { auto sq = square(a); return sqrt(sq); }

	TEMPLATE_VECTOR_A Vector<A, NA> abs(const VECTOR_A& a)
	{
		using namespace std;
		Vector<A, NA> result;
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

	template <size_t N> 
	Vector<bool, N> from_index(size_t idx)
	{
		Vector<bool, N> result;
		for (size_t i = 0; i < N; ++i)
			result[i] = (idx & (1 << i)) != 0;
		return result;
	}

	template <class First, class... Rest>
	inline Vector<scalar<First>, details::element_count<First, Rest...>::value> vector(const First& first, const Rest&... rest)
	{
		decltype(vector(first, rest...)) result;
		details::write_vector(result.data(), first, rest...);
		return result;
	}
	template <class T, class... Args>
	inline Vector<T, details::element_count<Args...>::value> vector(Args... args)
	{
		decltype(vector<T>(args...)) result;
		details::write_vector(result.data(), args...);
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

#pragma pop_macro("TEMPLATE_VECTOR_A")
#pragma pop_macro("TEMPLATE_VECTORS_AB")
#pragma pop_macro("TEMPLATE_VECTOR_A_OTHER_B")
#pragma pop_macro("VECTOR_A")
#pragma pop_macro("VECTOR_B")

#define UVECTOR_VECTOR_OPS_DEFINED

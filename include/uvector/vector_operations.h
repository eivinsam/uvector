#pragma once

#include "details/vector_details.h"

#define TEMPLATE_VECTOR_A template <class A, size_t NA, int KA>
#define TEMPLATE_VECTORS_AB template <class A, class B, size_t NA, size_t NB, int KA, int KB>
#define TEMPLATE_ANY_VECTOR template <class V, class S = typename std::remove_reference_t<V>::scalar_type>
#define TEMPLATE_VECTOR_A_SCALAR_B template <class A, class B, size_t NA, int KA>
#define VECTOR_A Vector<A, NA, KA> 
#define VECTOR_B Vector<B, NB, KB>


namespace uv
{
	namespace require
	{
		template <size_t A, size_t B>
		constexpr details::equal_test<A, B> equal = {};
	}

	template <class T, size_t N> auto data(Vector<T, N>& v) { return v.begin(); }
	template <class T, size_t N> auto data(const Vector<T, N>& v) { return v.begin(); }

	template <size_t Size, int Stride, size_t Offset, class T, size_t N, int K>
	auto& slice(Vector<T, N, K>& v) { return reinterpret_cast<Vector<T, Size, K*Stride>&>(v[Offset]); }
	template <size_t Size, int Stride, size_t Offset, class T, size_t N, int K>
	auto& slice(const Vector<T, N, K>& v) { return reinterpret_cast<const Vector<T, Size, K*Stride>&>(v[Offset]); }

	template <class T, size_t N, int K>
	Vector<T, (N - 1), K>& rest(Vector<T, N, K>& v) { return slice<(N - 1), K, 1>(v); }
	template <class T, size_t N, int K>
	const Vector<T, (N - 1), K>& rest(const Vector<T, N, K>& v) { return slice<(N - 1), K, 1>(v); }

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

	TEMPLATE_VECTOR_A
	auto differences(const VECTOR_A& a)
	{
		Vector<type::sub<A>, NA - 1> result;
		for (size_t i = 0; i < NA - 1; ++i)
			result[i] = a[i + 1] - a[i];
		return result;
	}

	template <size_t N, int K> inline auto operator!(const Vector<bool, N, K>& a) { Vector<bool, N> r; for (size_t i = 0; i < N; ++i) r[i] = !a[i]; return r; }


	TEMPLATE_VECTORS_AB	inline auto operator==(const VECTOR_A& a, const VECTOR_B& b) { require::equal<NA, NB>; return details::apply<NA, std::equal_to<>>(a, b); }
	TEMPLATE_VECTORS_AB	inline auto operator!=(const VECTOR_A& a, const VECTOR_B& b) { require::equal<NA, NB>; return details::apply<NA, std::not_equal_to<>>(a, b); }

	TEMPLATE_VECTORS_AB	inline auto operator+(const VECTOR_A& a, const VECTOR_B& b) { require::equal<NA, NB>; return details::apply<NA, std::plus<>>(a, b); }
	TEMPLATE_VECTORS_AB inline auto operator-(const VECTOR_A& a, const VECTOR_B& b) { require::equal<NA, NB>; return details::apply<NA, std::minus<>>(a, b); }
	TEMPLATE_VECTORS_AB inline auto operator*(const VECTOR_A& a, const VECTOR_B& b) { require::equal<NA, NB>; return details::apply<NA, std::multiplies<>>(a, b); }

	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator==(const VECTOR_A& a, Scalar<B> b) { return details::apply<NA, std::equal_to<>>(a, b); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator==(Scalar<B> b, const VECTOR_A& a) { return details::apply<NA, std::equal_to<>>(b, a); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator!=(const VECTOR_A& a, Scalar<B> b) { return details::apply<NA, std::not_equal_to<>>(a, b); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator!=(Scalar<B> b, const VECTOR_A& a) { return details::apply<NA, std::not_equal_to<>>(b, a); }

	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator+(const VECTOR_A& a, Scalar<B> b) { return details::apply<NA, std::plus<>>(a, b); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator+(Scalar<B> b, const VECTOR_A& a) { return details::apply<NA, std::plus<>>(b, a); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator-(const VECTOR_A& a, Scalar<B> b) { return details::apply<NA, std::minus<>>(a, b); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator-(Scalar<B> b, const VECTOR_A& a) { return details::apply<NA, std::minus<>>(b, a); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator*(const VECTOR_A& a, Scalar<B> b) { return details::apply<NA, std::multiplies<>>(a, b); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator*(Scalar<B> b, const VECTOR_A& a) { return details::apply<NA, std::multiplies<>>(b, a); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator/(const VECTOR_A& a, Scalar<B> b) { return details::apply<NA, std::divides<>>(a, b); }

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
				return Vector<decltype(a[0] * b[0] - a[0] * b[0]), 3>
				{
					a[1] * b[2] - a[2] * b[1],
						a[2] * b[0] - a[0] * b[2],
						a[0] * b[1] - a[1] * b[0]
				};
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
	TEMPLATE_VECTOR_A
		auto square(const VECTOR_A& a)
	{
		type::inner_product<A> result(0);
		for (int i = 0; i < NA; ++i)
			result += a[i] * a[i];
		return result;
	}
	TEMPLATE_VECTOR_A
		auto length(const VECTOR_A& a)
	{
		return sqrt(square(a));
	}
	TEMPLATE_VECTOR_A
		auto decompose(const VECTOR_A& a) // decomposes vector into direction vector and scalar length
	{
		auto len = length(a);
		struct result_t
		{
			decltype(a / scalar(len)) direction;
			decltype(len) length;
		};
		return result_t{ a / scalar(len), len };
	}

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

	template <size_t... Indices, class V>
	auto&& select(V&& source) { return details::selector<Indices...>::on(source); }

	namespace selectors
	{
		template <size_t N>  details::greater_test<N, 0> requires_x;
		template <size_t N>  details::greater_test<N, 1> requires_y;
		template <size_t N>  details::greater_test<N, 2> requires_z;
		template <size_t N>  details::greater_test<N, 3> requires_w;

		TEMPLATE_ANY_VECTOR auto& x(V&& v) { return v[0]; }
		TEMPLATE_ANY_VECTOR auto& y(V&& v) { return v[1]; }
		TEMPLATE_ANY_VECTOR auto& z(V&& v) { return v[2]; }
		TEMPLATE_ANY_VECTOR auto& w(V&& v) { return v[3]; }

		TEMPLATE_ANY_VECTOR auto& xy(V&& v) { return select<0,1>(v); }
		TEMPLATE_ANY_VECTOR auto& yz(V&& v) { return select<1,2>(v); }
		TEMPLATE_ANY_VECTOR auto& zw(V&& v) { return select<2,3>(v); }
		TEMPLATE_ANY_VECTOR auto& wx(V&& v) { return select<3,0>(v); }

		TEMPLATE_ANY_VECTOR auto& yx(V&& v) { return select<1,0>(v); }
		TEMPLATE_ANY_VECTOR auto& zy(V&& v) { return select<2,1>(v); }
		TEMPLATE_ANY_VECTOR auto& wz(V&& v) { return select<3,2>(v); }
		TEMPLATE_ANY_VECTOR auto& xw(V&& v) { return select<0,3>(v); }

		TEMPLATE_ANY_VECTOR auto& xz(V&& v) { return select<0,2>(v); }
		TEMPLATE_ANY_VECTOR auto& yw(V&& v) { return select<1,3>(v); }
		TEMPLATE_ANY_VECTOR auto& zx(V&& v) { return select<2,0>(v); }
		TEMPLATE_ANY_VECTOR auto& wy(V&& v) { return select<3,1>(v); }

		TEMPLATE_ANY_VECTOR auto& xyz(V&& v) { return select<0,1,2>(v); }
		TEMPLATE_ANY_VECTOR auto& yzw(V&& v) { return select<1,2,3>(v); }
		TEMPLATE_ANY_VECTOR auto& zyx(V&& v) { return select<2,1,0>(v); }
		TEMPLATE_ANY_VECTOR auto& wzy(V&& v) { return select<3,2,1>(v); }
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

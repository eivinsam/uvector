#pragma once

#include "../vector.h"

#define VECTOR_A Vector<A, NA, KA>
#define VECTOR_B Vector<B, NB, KB>

namespace uv
{
	namespace details
	{
		template <class T>
		static constexpr bool always_false = false;

		template <class From, class To> struct CopyConstRef                 { using type =       To; };
		template <class From, class To> struct CopyConstRef<const From, To> { using type = const typename CopyConstRef<From, To>::type; };
		template <class From, class To> struct CopyConstRef<From&, To>      { using type = typename CopyConstRef<From, To>::type&; };
		
		template <class From, class To>
		using copy_cr = typename CopyConstRef<From, To>::type;
		
		template <size_t A, size_t B>
		struct is_equal
		{
			static_assert(A == B, "Vectors must be of equal length"); 
			static constexpr size_t value = A;
		};

		template <size_t A, size_t B>
		struct greater_test { static_assert(A > B, "vector too short"); };

		template <int...>
		struct is_linear_s : public std::false_type { };
		template <int First, int Second>
		struct is_linear_s<First, Second> : public std::true_type { };
		template <int First, int Second, int Third, int... Rest>
		struct is_linear_s<First, Second, Third, Rest...>
		{
			static constexpr bool value =
				Second - First == Third - Second &&
				is_linear_s<Second, Third, Rest...>::value;
		};
		template <int... Values>
		static constexpr bool is_linear = is_linear_s<Values...>::value;

		template <int...>
		struct linear_sequence { static_assert("Invalid linear sequence"); };
		template <int First, int Second, int... Rest>
		struct linear_sequence<First, Second, Rest...>
		{
			static constexpr bool linear = true;
			static constexpr int first = First;
			static constexpr int delta = Second - First;
		};

		
		template <class T>
		inline void assert_vector() { static_assert(is_vector<T>, "argument must be a vector"); }
		template <class T>
		inline void assert_first_vector() { static_assert(is_vector<T>, "first argument must be av vector"); }
		template <class T>
		inline void assert_second_vector() { static_assert(is_vector<T>, "second argument must be a vector"); }


		template <typename... Rest>
		inline constexpr int delta(size_t first, size_t second, Rest...) { return int(second) - int(first); }

		template <class...>
		struct element_count { static constexpr size_t value = 0; };
		template <class First, class... Rest>
		struct element_count<First, Rest...> { static constexpr size_t value = 1 + element_count<Rest...>::value; };
		template <class A, size_t N, int K, class... Rest>
		struct element_count<Vector<A, N, K>, Rest...> { static constexpr size_t value = N + element_count<Rest...>::value; };

		static_assert(element_count<float, float, double>::value == 3);
		static_assert(element_count<float, Vector<float, 3>>::value == 4);


		inline void write_vector(void*) { }
		template <class T, class First, class... Rest>
		void write_vector(T* dst, const First& first, const Rest&... rest)
		{
			*dst = T(first); ++dst;
			write_vector(dst, rest...);
		}
		template <class T, class First, size_t N, int K, class... Rest>
		void write_vector(T* dst, const Vector<First, N, K>& first, const Rest&... rest)
		{
			for (size_t i = 0; i < N; ++i, ++dst)
				*dst = T(first[i]);
			write_vector(dst, rest...);
		}

		template <class A, class B, size_t IA, size_t IB>
		struct component_op
		{
			static auto add(Component<A, IA> a, Component<B, IB> b)
			{
				using T = decltype(*a + *b);
				Vector<T, std::max(IA, IB) + 1> result;
				for (size_t i = 0; i < result.size(); ++i)
					switch (i)
					{
					case IA: result[i] = *a;
					case IB: result[i] = *b;
					default: result[i] = T(0);
					}
				return result;
			}
			static auto sub(Component<A, IA> a, Component<B, IB> b)
			{
				using T = decltype(*a - *b);
				Vector<T, std::max(IA, IB) + 1> result;
				for (size_t i = 0; i < result.size(); ++i)
					switch (i)
					{
					case IA: result[i] = *a;
					case IB: result[i] = -*b;
					default: result[i] = T(0);
					}
				return result;
			}
		};
		template <class A, class B, size_t I>
		struct component_op<A, B, I, I>
		{
			static auto add(Component<A, I> a, Component<B, I> b)
			{
				return Component<decltype(*a + *b), I>{ *a + *b };
			}
			static auto sub(Component<A, I> a, Component<B, I> b)
			{
				return Component<decltype(*a - *b), I>{ *a - *b };
			}
		};

		template <size_t N>
		inline void assert_all_less() { }
		template <size_t N, size_t First, size_t... Rest>
		inline void assert_all_less() { static_assert(First < N, "Indices must be less than N"); assert_all_less<N, Rest...>(); }

		template <size_t N, size_t I0, int K>
		struct slicer
		{
			static constexpr int Ifirst = int(I0);
			static constexpr int Ilast = Ifirst + K*(int(N) - 1);
			static constexpr int Imin = std::min(Ifirst, Ilast);
			static constexpr int Imax = std::max(Ifirst, Ilast);
			static_assert(Imin >= 0, "Lowest index must be greater or equal to zero");

			template <class A, size_t AN, int AK>
			static auto& on(Vector<A, AN, AK>& v)
			{
				static_assert(Imax < AN, "Highest index must be less than vector dimension");
				return reinterpret_cast<Vector<A, N, K*AK>&>(v[I0]);
			}
			template <class A, size_t AN, int AK>
			static auto& on(const Vector<A, AN, AK>& v)
			{
				static_assert(Imax < AN, "Highest index must be less than vector dimension");
				return reinterpret_cast<const Vector<A, N, K*AK>&>(v[I0]);
			}
		};

		template <size_t... Indices>
		struct copy_selector
		{
			template <class V>
			static auto on(V&& v)
			{
				assert_all_less<dim<V>, Indices...>();
				return vector(v[Indices]...);
			}
		};

		template <int... Indices>
		struct selector : public std::conditional_t<is_linear<Indices...>,
			slicer<sizeof...(Indices), linear_sequence<Indices...>::first, linear_sequence<Indices...>::delta>,
			copy_selector<Indices...>> { };

		static_assert(std::is_base_of_v<slicer<2, 0, 1>, selector<0, 1>>);


		template <size_t...>
		struct index_s;
		template <>
		struct index_s<> { static constexpr size_t value = 0; };
		template <size_t I0, size_t... I>
		struct index_s<I0, I...> { static constexpr size_t value = (1 << I0) | index_s<I...>::value; };


		template <size_t N>
		struct cross_product { static_assert(N == 2 || N == 3, "Cross product only defined for two- and three-dimensional vectors"); };
		template <>
		struct cross_product<2>
		{
			template <class A, class B>
			static auto of(const A& a, const B& b)
			{
				return a[0] * b[1] - a[1] * b[0];
			}
		};
		template <>
		struct cross_product<3>
		{
			template <class A, class B>
			static auto of(const A& a, const B& b)
			{
				return vector(
					a[1] * b[2] - a[2] * b[1],
					a[2] * b[0] - a[0] * b[2],
					a[0] * b[1] - a[1] * b[0]);
			}
		};

		template <class OP, class A, class B>
		auto vector_apply(const A& a, const B& b)
		{
			static constexpr auto N = require::equal<dim<A>, dim<B>>;
			OP op;
			Vector<type::of<OP, scalar<A>, scalar<B>>, N> result;
			for (size_t i = 0; i < N; ++i)
				result[i] = op(a[i], b[i]);
			return result;
		}
		template <class OP, class V, class S>
		auto scalar_apply(const V& v, S s)
		{
			static constexpr auto N = dim<V>;
			OP op;
			Vector<type::of<OP, scalar<V>, S>, N> result;
			for (size_t i = 0; i < N; ++i)
				result[i] = op(v[i], s);
			return result;
		}

		template <class T>
		struct vector_and
		{
			static_assert(is_scalar_v<T>, "Vector operation not specialized for type");
			
			template <class OP, class V>
			static auto apply(const V& v, T s) { return scalar_apply<OP>(v, s); }
		};
	}
}
#undef VECTOR_A
#undef VECTOR_B

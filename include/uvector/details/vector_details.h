#pragma once

#include "../vector.h"

#define VECTOR_A Vector<A, NA, KA>
#define VECTOR_B Vector<B, NB, KB>

namespace uv
{
	namespace details
	{
		template <size_t A, size_t B>
		struct equal_test { static_assert(A == B, "vectors must be of equal length"); };
		template <size_t A, size_t B>
		struct greater_test { static_assert(A > B, "vector too short"); };

		template <class T>
		struct dim_s;
		template <class T, size_t N, int K>
		struct dim_s<Vector<T, N, K>> { static constexpr size_t value = N; };
		template <class T>
		static constexpr auto dim = dim_s<std::remove_const_t<std::remove_reference_t<T>>>::value;


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


		template <class T, class = void>
		struct is_vector_s : public std::false_type { };
		template <class T>
		struct is_vector_s<T, std::void_t<typename std::remove_reference_t<T>::scalar_type>> : public std::true_type { };
		template <class T>
		static constexpr bool is_vector = details::is_vector_s<T>::value;

		template <class T>
		inline void assert_vector() { static_assert(is_vector<T>, "argument must be a vector"); }
		template <class T>
		inline void assert_first_vector() { static_assert(is_vector<T>, "first argument must be av vector"); }
		template <class T>
		inline void assert_second_vector() { static_assert(is_vector<T>, "second argument must be a vector"); }


		template <typename... Rest>
		inline constexpr int delta(size_t first, size_t second, Rest...) { return int(second) - int(first); }





		template <class T>
		struct scalar_of_s
		{
			static_assert(is_scalar_v<T>, "Expected a scalar or vector type");

			using type = T;
		};
		template <class T, size_t N, int K>
		struct scalar_of_s<Vector<T, N, K>> { using type = T; };

		template <class T>
		using scalar_of = typename scalar_of_s<T>::type;

		template <class OP, class A, class B>
		struct applier
		{
			static auto apply(A a, B b) { return OP{}(a, b); }
		};
		template <class OP, class A, class B, size_t NA, int KA>
		struct applier<OP, VECTOR_A, B>
		{
			static auto apply(const VECTOR_A& a, B b)
			{
				Vector<type::of<OP, A, B>, NA> result;
				for (size_t i = 0; i < NA; ++i)
					result[i] = OP{}(a[i], b);
				return result;
			}
		};
		template <class OP, class A, class B, size_t NA, int KA>
		struct applier<OP, B, VECTOR_A>
		{
			static auto apply(B b, const VECTOR_A& a)
			{
				Vector<type::of<OP, B, A>, NA> result;
				for (size_t i = 0; i < NA; ++i)
					result[i] = OP{}(b, a[i]);
				return result;
			}
		};
		template <class OP, class A, class B, size_t NA, size_t NB, int KA, int KB>
		struct applier<OP, VECTOR_A, VECTOR_B>
		{
			static auto apply(const VECTOR_A& a, const VECTOR_B& b)
			{
				static_assert(NA == NB, "Vector-vector operation requires equal vector lengths");
				Vector<type::of<OP, A, B>, NA> result;
				for (size_t i = 0; i < NA; ++i)
					result[i] = OP{}(a[i], b[i]);
				return result;
			}
		};

		template <class OP, class A, class B>
		inline auto apply(const A& a, const B& b) { return applier<OP, A, B>::apply(a, b); }

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



	}
}
#undef VECTOR_A
#undef VECTOR_B

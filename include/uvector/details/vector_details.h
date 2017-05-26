#pragma once

#include "../vector.h"

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

		template <size_t N>
		inline void assert_all_less() { }
		template <size_t N, size_t First, size_t... Rest>
		inline void assert_all_less() { static_assert(First < N, "Indices must be less than N"); assert_all_less<N, Rest...>(); }

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

		template <class V, size_t... Indices>
		inline void assert_all_less() { assert_vector<V>(); assert_all_less<dim<V>, Indices...>(); }

		template <typename... Rest>
		inline constexpr int delta(size_t first, size_t second, Rest...) { return int(second) - int(first); }



		template <size_t N, size_t I0, int K>
		struct slicer
		{
			static constexpr int Ifirst = int(I0);
			static constexpr int Ilast = Ifirst + K*(int(N) - 1);
			static constexpr int Imin = std::min(Ifirst, Ilast);
			static constexpr int Imax = std::max(Ifirst, Ilast);
			static_assert(Imin >= 0, "Lowest index must be greater or equal to zero");

			template <class V>
			static auto& on(V&& v)
			{
				assert_vector<V>();
				static_assert(Imax < dim<V>, "Highest index must be less than vector dimension");
				return uv::slice<N, K, I0>(v);
			}
		};

		template <size_t... Indices>
		struct copy_selector
		{
			template <class V>
			static auto on(V&& v)
			{
				assert_all_less<V, Indices...>();
				return vector(v[Indices]...);
			}
		};

		template <int... Indices>
		struct selector : public std::conditional_t<is_linear<Indices...>,
			slicer<sizeof...(Indices), linear_sequence<Indices...>::first, linear_sequence<Indices...>::delta>,
			copy_selector<Indices...>> { };

		static_assert(std::is_base_of_v<slicer<2, 0, 1>, selector<0, 1>>);

		template <size_t N, class OP, class A, class B>
		inline auto apply(A&& a, B&& b)
		{
			Vector<type::of<OP, decltype(a[0]), decltype(b[0])>, N> result;
			for (size_t i = 0; i < N; ++i)
				result[i] = OP{}(a[i], b[i]);
			return result;
		}

		template <class...>
		struct element_count { static constexpr size_t value = 0; };
		template <class First, class... Rest>
		struct element_count<First, Rest...> { static constexpr size_t value = 1 + element_count<Rest...>::value; };
		template <class A, size_t N, int K, class... Rest>
		struct element_count<Vector<A, N, K>, Rest...> { static constexpr size_t value = N + element_count<Rest...>::value; };

		static_assert(element_count<float, float, double>::value == 3);
		static_assert(element_count<float, Vector<float, 3>>::value == 4);

		template <class T>
		struct scalar_of_s { using type = T; };
		template <class T, size_t N, int K>
		struct scalar_of_s<Vector<T, N, K>> { using type = T; };

		template <class T>
		using scalar_of = typename scalar_of_s<T>::type;

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

	}
}

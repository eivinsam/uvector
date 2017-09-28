#pragma once

#include <array>
#include <iterator>
#include <ostream>

#include "scalar.h"

#include "../base/gsl.h"

namespace uv
{
	namespace details
	{
		template <size_t A, size_t B>
		struct is_equal
		{
			static_assert(A == B, "Vectors must be of equal length");
			static constexpr size_t value = A;
		};
	}
	namespace require
	{
		template <size_t A, size_t B>
		static constexpr size_t equal = details::is_equal<A, B>::value;
	}

	template <class T, size_t N, int K = 1>
	class Vec;
	template <class T, size_t N>
	class Dir;

	template <size_t N, class T> struct is_vector : std::false_type { };
	template <size_t N, class T> struct is_vector<N, T&> : is_vector<N, T> { };
	template <size_t N, class T> struct is_vector<N, const T&> : is_vector<N, T> { };

	template <size_t N, class T> static constexpr bool is_vector_v = is_vector<N, T>::value;
	template <size_t N, class T, class R = void> 
	using if_vector_t = std::enable_if_t<is_vector<N, T>::value, R>;

	template <class T, size_t N, int K>
	struct is_vector<N, Vec<T, N, K>> : std::true_type { };

	template <size_t N, class... S>
	struct is_scalars : std::false_type { };
	template <size_t N, class T, class... S>
	struct is_scalars<N, T, S...> { static constexpr bool value = is_scalar_v<T> && is_scalars<N-1, S...>::value; };
	template <>
	struct is_scalars<0> : std::true_type { };
	template <size_t N, class... S>
	static constexpr bool is_scalars_v = is_scalars<N, S...>::value;
	
	template <size_t N, class... S>
	using if_scalars_t = typename std::enable_if<is_scalars<N, S...>::value>::type;


	template <size_t... I>
	struct Axes
	{
		template <size_t... J>
		constexpr Axes<I..., J...> operator|(Axes<J...>) const { return {}; }
	};

	namespace details
	{
		template <size_t... I>
		struct Dim<Axes<I...>> : std::integral_constant<size_t, sizeof...(I)> { };

		template <class T>
		static constexpr bool always_false = false;

	}


	template <class T, size_t I>
	class Component
	{
		T _value;
	public:
		static_assert(is_scalar_v<T>, "Scalar type required");
		Component() { }
		constexpr Component(T value) : _value(value) { }

		constexpr       T& operator*() { return _value; }
		constexpr const T& operator*() const { return _value; }

		constexpr Component operator-() const { return Component{ -_value }; }

		template <class S, class = if_scalar_t<S>> friend constexpr Component<type::mul<T, S>, I> operator*(Component c, S s) { return { c._value * s }; }
		template <class S, class = if_scalar_t<S>> friend constexpr Component<type::div<T, S>, I> operator/(Component c, S s) { return { c._value / s }; }
		template <class S, class = if_scalar_t<S>> friend constexpr Component<type::mul<S, T>, I> operator*(S s, Component c) { return { s * c._value }; }

		template <class S, size_t J>
		constexpr Component<type::mul<T, S>, I> operator*(Component<S, J> c)
		{
			static_assert(I == J, "Component-component multiplication requires equal indices");
			return { _value * c._value };
		}

		const T operator[](size_t i) const { return i == I ? _value : T(0); }
	};
	template <size_t N, class T, size_t I>
	struct is_vector<N, Component<T, I>> { static constexpr bool value = I < N; };

	template <size_t I>
	struct Axes<I> : Component<Unit, I>
	{
		constexpr Axes() : Component<Unit, I>{ Unit{} } { }

		template <size_t... J>
		constexpr Axes<I, J...> operator|(Axes<J...>) const { return {}; }

		template <class S, class = if_scalar_t<S>> friend constexpr Component<S, I> operator*(Axes, S s) { return { s }; }
		template <class S, class = if_scalar_t<S>> friend constexpr Component<type::div<int, S>, I> operator/(Axes, S s) { return { 1 / s }; }
		template <class S, class = if_scalar_t<S>> friend constexpr Component<S, I> operator*(S s, Axes) { return { s }; }
	};
	template <size_t N, size_t I> struct is_vector<N, Axes<I>> { static constexpr bool value = I < N; };
	template <size_t N, size_t I> struct   is_unit<N, Axes<I>> { static constexpr bool value = I < N; };

	namespace axes
	{
		static constexpr Axes<> Nought = {};

		static constexpr Axes<0> X = {};
		static constexpr Axes<1> Y = {};
		static constexpr Axes<2> Z = {};
		static constexpr Axes<3> W = {};

		static constexpr auto XY = X | Y;
		static constexpr auto YZ = Y | Z;
		static constexpr auto ZW = Z | W;
		static constexpr auto WX = W | X;

		static constexpr auto XZ = X | Z;
		static constexpr auto YW = Y | W;
		static constexpr auto ZX = Z | X;
		static constexpr auto WY = W | Y;

		static constexpr auto XW = X | W;
		static constexpr auto YX = Y | X;
		static constexpr auto ZY = Z | Y;
		static constexpr auto WZ = W | Z;

		static constexpr auto XYZ = X | Y | Z;
	}


	namespace details
	{

		template <class T, int K>
		class stride_iterator
		{
			T* _ptr = nullptr;
		public:
			using iterator_category = std::random_access_iterator_tag;
			using difference_type = ptrdiff_t;
			using value_type = T;
			using reference = T&;
			using pointer = T*;

			stride_iterator() = default;
			stride_iterator(T* ptr) : _ptr(ptr) { }

			stride_iterator& operator++() { _ptr += K; return *this; }
			stride_iterator& operator--() { _ptr += K; return *this; }
			stride_iterator operator++(int) { return { _ptr + K }; }
			stride_iterator operator--(int) { return { _ptr - K }; }

			stride_iterator& operator+=(ptrdiff_t n) { _ptr += K*n; return *this; }
			stride_iterator& operator-=(ptrdiff_t n) { _ptr -= K*n; return *this; }

			stride_iterator operator+(ptrdiff_t n) { stride_iterator result(*this); result += n; return result; }
			stride_iterator operator-(ptrdiff_t n) { stride_iterator result(*this); result -= n; return result; }

			ptrdiff_t operator-(const stride_iterator& other) const { return (_ptr - other._ptr) / K; }

			reference operator[](ptrdiff_t n) const { return _ptr[n*K]; }
			reference operator[](size_t    n) const { return _ptr[n*K]; }
			reference operator* () const { return *_ptr; }
			pointer   operator->() const { return  _ptr; }

			bool operator==(const stride_iterator& other) const { return _ptr == other._ptr; }
			bool operator!=(const stride_iterator& other) const { return _ptr != other._ptr; }

			bool operator< (const stride_iterator& other) const { return _ptr < other._ptr; }
			bool operator<=(const stride_iterator& other) const { return _ptr <= other._ptr; }
			bool operator>=(const stride_iterator& other) const { return _ptr >= other._ptr; }
			bool operator> (const stride_iterator& other) const { return _ptr > other._ptr; }
		};

		template <class T>
		class repeat_iterator
		{
			T* _ptr = nullptr;
			ptrdiff_t _i = 0;

			repeat_iterator(T* ptr, ptrdiff_t i) : _ptr(ptr), _i(i) { }
		public:
			using iterator_category = std::random_access_iterator_tag;
			using difference_type = ptrdiff_t;
			using value_type = T;
			using reference = T&;
			using pointer = T*;

			repeat_iterator() = default;
			repeat_iterator(T* ptr) : _ptr(ptr) { }

			repeat_iterator& operator++() { ++_i; return *this; }
			repeat_iterator& operator--() { --_i; return *this; }
			repeat_iterator operator++(int) { return { _ptr, _i + 1 }; }
			repeat_iterator operator--(int) { return { _ptr, _i - 1 }; }

			repeat_iterator& operator+=(ptrdiff_t n) { _i += n; return *this; }
			repeat_iterator& operator-=(ptrdiff_t n) { _i -= n; return *this; }

			repeat_iterator operator+(ptrdiff_t n) { repeat_iterator result(*this); result += n; return result; }
			repeat_iterator operator-(ptrdiff_t n) { repeat_iterator result(*this); result -= n; return result; }

			ptrdiff_t operator-(const repeat_iterator& other) const { return _i - other._i; }

			reference operator[](ptrdiff_t) const { return *_ptr; }
			reference operator[](size_t) const { return *_ptr; }
			reference operator* () const { return *_ptr; }
			pointer   operator->() const { return  _ptr; }

			bool operator==(const repeat_iterator& other) const { return _i == other._i; }
			bool operator!=(const repeat_iterator& other) const { return _i != other._i; }
			bool operator< (const repeat_iterator& other) const { return _i < other._i; }
			bool operator<=(const repeat_iterator& other) const { return _i <= other._i; }
			bool operator>=(const repeat_iterator& other) const { return _i >= other._i; }
			bool operator> (const repeat_iterator& other) const { return _i > other._i; }
		};

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
		template <typename... Rest>
		inline constexpr int delta(size_t first, size_t second, Rest...) { return int(second) - int(first); }

		template <class V, size_t N, size_t I0, int K>
		struct slicer
		{
			static_assert(std::is_reference_v<V>, "Slicer vector type should be a reference type");
			static constexpr int Ifirst = int(I0);
			static constexpr int Ilast = Ifirst + K*(int(N) - 1);
			static constexpr int Imin = std::min(Ifirst, Ilast);
			static constexpr int Imax = std::max(Ifirst, Ilast);
			static_assert(Imin >= 0, "Lowest index must be greater or equal to zero");

			template <class T>
			struct result { };
			template <class T> struct result<T&> { using type = typename result<T>::type&; };
			template <class T> struct result<const T> { using type = const typename result<T>::type; };
			template <class A, size_t AN, int AK> struct result<Vec<A, AN, AK>> { using type = Vec<A, N, K*AK>; };

			using type = typename result<V>::type;

			static type on(V&& v)
			{
				static_assert(Imax < dim<V>, "Highest index must be less than vector dimension");
				return reinterpret_cast<type>(v[Ifirst]);
			}
		};
		static_assert(std::is_same_v<slicer<      Vec<int, 4>&, 2, 0, 2>::type,       Vec<int, 2, 2>&>);
		static_assert(std::is_same_v<slicer<const Vec<int, 4>&, 2, 1, 2>::type, const Vec<int, 2, 2>&>);

		template <size_t N>
		inline void assert_all_less() { }
		template <size_t N, size_t First, size_t... Rest>
		inline void assert_all_less() { static_assert(First < N, "Indices must be less than N"); assert_all_less<N, Rest...>(); }

		template <class V, size_t... I>
		struct copy_selector { };
		template <class V, size_t... I>
		struct copy_selector<V&, I...> : copy_selector<V, I...> { };
		template <class V, size_t... I>
		struct copy_selector<const V&, I...> : copy_selector<V, I...> { };
		template <class T, size_t N, int K, size_t... I>
		struct copy_selector<Vec<T, N, K>, I...>
		{
			using type = Vec<T, sizeof...(I)>;
			static type on(const Vec<T, N, K>& v)
			{
				assert_all_less<N, I...>();
				return vector( v[I]... );
			}
		};


		template <class V, int... I>
		struct multiple_selector : public std::conditional_t<is_linear<I...>,
			slicer<V, sizeof...(I), linear_sequence<I...>::first, linear_sequence<I...>::delta>,
			copy_selector<V, I...>> { };

		template <class V, size_t I>
		struct single_selector
		{
			using type = std::conditional_t<std::is_const_v<std::remove_reference_t<V>>, const scalar<V>&, scalar<V>&>;

			static type on(V&& v)
			{
				static_assert(I < dim<V>, "Index must be lower than vector dimension");
				return v[I];
			}
		};

		template <class V, int First, int... Rest>
		struct selector : public std::conditional_t<(sizeof...(Rest) == 0),
			single_selector<V, First>,
			multiple_selector<V, First, Rest...>> { };


		template <class V, int... Indices>
		using select_t = typename selector<V, Indices...>::type;


		template <class...>
		struct element_count { static constexpr size_t value = 0; };
		template <class First, class... Rest>
		struct element_count<First, Rest...> { static constexpr size_t value = 1 + element_count<Rest...>::value; };
		template <class A, size_t N, int K, class... Rest>
		struct element_count<Vec<A, N, K>, Rest...> { static constexpr size_t value = N + element_count<Rest...>::value; };

		static_assert(element_count<float, float, double>::value == 3);
		static_assert(element_count<float, Vec<float, 3>>::value == 4);

		class Element
		{
			size_t _i;
		public:
			constexpr Element(size_t i) : _i(i) { }

			template <class T> 
			auto& of(T&& value) const
			{
				if constexpr (dim<T> == 1)
					return value;
				else
					return value[_i];
			}
		};

		inline void write_vector(void*) { }
		template <class T, class First, class... Rest>
		void write_vector(T* dst, const First& first, const Rest&... rest)
		{
			for (size_t i = 0; i < dim<First>; ++i, ++dst)
				*dst = T(Element(i).of(first));
			write_vector(dst, rest...);
		}

		template <class T, size_t N, int K>
		class VectorData
		{
			T _first;
		public:
			using       iterator = details::stride_iterator<T, K>;
			using const_iterator = details::stride_iterator<const T, K>;

			VectorData() = delete;
			VectorData(const VectorData&) = delete;

			template <int KB>
			VectorData& operator=(const VectorData<T, N, KB>& other) { for (size_t i = 0; i < N; ++i) (*this)[i] = other[i]; return *this; }
			VectorData& operator=(T value) { for (size_t i = 0; i < N; ++i) (*this)[i] = value;    return *this; }

			auto begin() { return       iterator{ &_first }; }
			auto begin() const { return const_iterator{ &_first }; }
			auto end() { return begin() += N; }
			auto end() const { return begin() += N; }

			T& operator[](size_t i) { return begin()[i]; }
			const T& operator[](size_t i) const { return begin()[i]; }

			explicit operator bool() const { for (size_t i = 0; i < N; ++i) if (!(*this)[i]) return false; return true; }
		};

		template <class T, size_t N>
		class VectorData<T, N, 0>
		{
			T _value;
		public:
			using       iterator = details::repeat_iterator<T>;
			using const_iterator = details::repeat_iterator<const T>;

			VectorData() { }
			constexpr VectorData(const VectorData&) = default;
			constexpr VectorData(T value) : _value(value) { }

			constexpr VectorData& operator=(const VectorData& other) = default;
			constexpr VectorData& operator=(T value) { _value = value; }

			auto begin() { return       iterator{ &_value }; }
			auto begin() const { return const_iterator{ &_value }; }

			auto end() { return begin() += N; }
			auto end() const { return begin() += N; }

			T& operator[](size_t) { return _value; }
			const T& operator[](size_t) const { return _value; }

			explicit constexpr operator bool() const { return bool(_value); }
		};


		template <class T, size_t N>
		class VectorData<T, N, 1> : public std::array<T, N>
		{
		public:
			VectorData() { }

			VectorData(T value)
			{
				for (size_t i = 0; i < N; ++i)
					(*this)[i] = value;
			}
			//template <class... Args, class = std::void_t<if_scalar_t<Args>...>>
			//constexpr explicit VectorData(Args... args)
			//{
			//	static_assert(sizeof...(Args) == N, "invalid number of elements");
			//	std::array<T, N>::operator=({ T(args)... });
			//}
			template <class B, int KB>
			explicit VectorData(const VectorData<B, N, KB>& other) { for (size_t i = 0; i < N; ++i) (*this)[i] = T(other[i]); }
			template <int KB>
			VectorData(const VectorData<T, N, KB>& other) { for (size_t i = 0; i < N; ++i) (*this)[i] = other[i]; }

			template <int KB>
			VectorData& operator=(const VectorData<T, N, KB>& other) { for (size_t i = 0; i < N; ++i) (*this)[i] = other[i]; return *this; }
			VectorData& operator=(T value) { for (size_t i = 0; i < N; ++i) (*this)[i] = value; return *this; }

			explicit operator bool() const { for (size_t i = 0; i < N; ++i) if (!(*this)[i]) return false; return true; }
		};
	}

	template <size_t... I, class V>
	details::select_t<V, I...> select(V&& source) { return details::selector<V, I...>::on(std::forward<V>(source)); }

	template <class T, size_t N, int K>
	class Vec
	{
		using Data = details::VectorData<T, N, K>;
	public:
		static_assert(is_scalar_v<T>, "T must be a scalar type");
		static_assert(N > 1, "vectors must have at least two dimensions");

		using size_type = size_t;
		using       iterator = typename Data::iterator;
		using const_iterator = typename Data::const_iterator;
		using       reference = T&;
		using const_reference = const T&;
		using difference_type = ptrdiff_t;
		using value_type = T;
		using scalar_type = T;

		static constexpr size_t dim = N;
	private:
		Data _data;

		template <class OP, class S>
		auto _scalar_apply(S s) const
		{
			static_assert(is_scalar_v<S>, "Invalid type for vector operation");
			OP op;
			Vec<type::of<OP, T, S>, N> result;
			for (size_t i = 0; i < N; ++i)
				result[i] = op((*this)[i], s);
			return result;
		}
		template <class OP, class B, size_t NB, int KB>
		auto _vector_apply(const Vec<B, NB, KB>& b) const
		{
			static_assert(N == NB, "Vector-vector operation requires equal dimensionality");
			OP op;
			Vec<type::of<OP, T, B>, N> result;
			for (size_t i = 0; i < N; ++i)
				result[i] = op((*this)[i], b[i]);
			return result;
		}
	public:

		Vec() { }

		Vec(const Vec&) = default;
		Vec(T s) : _data(s) { }

		template <class S, size_t M, int L>
		Vec(const Vec<S, M, L>& b) { *this = b; }
		template <size_t I>
		Vec(Component<T, I> c) { *this = c; }

		Vec& operator=(const Vec&) = default;
		Vec& operator=(T s) { _data = s; }

		template <class S, size_t M, int L>
		Vec& operator=(const Vec<S, M, L>& b)
		{
			static_assert(N == M, "Vector assignment requires equal dimensionality");
			for (size_t i = 0; i < N; ++i)
				(*this)[i] = b[i];
			return *this;
		}
		template <size_t I>
		Vec& operator=(Component<T, I> c)
		{
			static_assert(I < N, "Cannot asign component of higher index than vertex dimensionality");
			for (size_t i = 0; i < N; ++i)
				(*this)[i] = i == I ? *c : T(0);
			return *this;
		}


		auto begin()       { return _data.begin(); }
		auto begin() const { return _data.begin(); }
		auto end()       { return _data.end(); }
		auto end() const { return _data.end(); }

		      T& operator[](size_t i)       { return _data[i]; }
		const T& operator[](size_t i) const { return _data[i]; }

		constexpr size_t size() const { return N; }

		Vec operator-() const { Vec result; for (size_t i = 0; i < N; ++i) result[i] = -(*this)[i]; return result; }

		friend T sum(const Vec& v) { T s = T(0); for (size_t i = 0; i < N; ++i) s += v[i]; return s; }

		friend type::mul<T> square(const Vec& v) { return sum(v*v); }

		template <class S, size_t M, int L> 
		friend type::mul<T, S> dot(const Vec& a, const Vec<S, M, L>& b) { return sum(a*b); }
		
		template <size_t I> friend T dot(const Vec& v, Axes<I>)
		{ 
			static_assert(I < N, "Cannot dot vector with higher-dimensional axis");
			return v[I]; 
		}
		template <size_t I> friend T dot(Axes<I> a, const Vec& v) { return v*a; }
		template <class S, size_t I> friend type::mul<T, S> dot(const Vec& v, Component<S, I> c) { return (v*Axes<I>{0}) * *c; }
		template <class S, size_t I> friend type::mul<S, T> dot(const Vec& v, Component<S, I> c) { return *c * (Axes<I>{0}*v); }

		template <class S, size_t M, int L> friend auto angle(const Vec& a, const Vec<S, M, L>& b)
		{
			using R = type::identity<type::mul<T, S>>;
			return acos(std::clamp(dot(a, b) / sqrt(square(a)*square(b)), R(-1), R(1)));
		}

		friend bool isfinite(const Vec& a) { for (size_t i = 0; i < NA; ++i) if (!isfinite(a[i])) return false; return true; }

		friend Vec abs(const Vec& a)
		{
			using namespace std;
			Vec result;
			for (size_t i = 0; i < N; ++i)
				result[i] = abs(a[i]);
			return result;
		}

		template <size_t... I> friend details::select_t<Vec&, I...> operator*(Vec& v, Axes<I...>) { return select<I...>(v); }
		template <size_t... I> friend details::select_t<Vec&, I...> operator*(Axes<I...>, Vec& v) { return select<I...>(v); }
		template <size_t... I> friend details::select_t<const Vec&, I...> operator*(const Vec& v, Axes<I...>) { return select<I...>(v); }
		template <size_t... I> friend details::select_t<const Vec&, I...> operator*(Axes<I...>, const Vec& v) { return select<I...>(v); }

		template <class S, size_t I> friend auto operator*(const Vec& v, Component<S, I> c) { return v[I] * *c; }
		template <class S, size_t I> friend auto operator*(Component<S, I> c, const Vec& v) { return *c * v[I]; }

		template <class S, size_t I> friend auto operator+(const Vec& v, Component<S, I> c) { Vec<type::add<T, S>, N> r = v; r[I] = r[I] + *c; return r; }
		template <class S, size_t I> friend auto operator-(const Vec& v, Component<S, I> c) { Vec<type::add<T, S>, N> r = v; r[I] = r[I] - *c; return r; }
		template <class S, size_t I> friend auto operator+(Component<S, I> c, const Vec& v) { Vec<type::add<T, S>, N> r = v; r[I] = r[I] + *c; return r; }
		template <class S, size_t I> friend auto operator-(Component<S, I> c, const Vec& v) { Vec<type::add<T, S>, N> r = -v; r[I] = r[I] + *c; return r; }

		template <class S, class = if_scalar_t<S>> friend auto operator+(const Vec& v, S s) { return v._scalar_apply<op::add>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator-(const Vec& v, S s) { return v._scalar_apply<op::sub>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator*(const Vec& v, S s) { return v._scalar_apply<op::mul>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator/(const Vec& v, S s) { return v._scalar_apply<op::div>(s); }

		template <class S, class = if_scalar_t<S>> friend auto operator==(const Vec& v, S s) { return v._scalar_apply<op::eq>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator!=(const Vec& v, S s) { return v._scalar_apply<op::ne>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator< (const Vec& v, S s) { return v._scalar_apply<op::sl>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator<=(const Vec& v, S s) { return v._scalar_apply<op::le>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator>=(const Vec& v, S s) { return v._scalar_apply<op::ge>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator> (const Vec& v, S s) { return v._scalar_apply<op::sg>(s); }

		template <class S, class = if_scalar_t<S>> friend auto operator+(S s, const Vec& v) { return v._scalar_apply<op::rev<op::add>>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator-(S s, const Vec& v) { return v._scalar_apply<op::rev<op::sub>>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator*(S s, const Vec& v) { return v._scalar_apply<op::rev<op::mul>>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator/(S s, const Vec& v) { return v._scalar_apply<op::rev<op::div>>(s); }

		template <class S, class = if_scalar_t<S>> friend auto operator==(S s, const Vec& v) { return v._scalar_apply<op::rev<op::eq>>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator!=(S s, const Vec& v) { return v._scalar_apply<op::rev<op::ne>>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator< (S s, const Vec& v) { return v._scalar_apply<op::rev<op::sl>>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator<=(S s, const Vec& v) { return v._scalar_apply<op::rev<op::le>>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator>=(S s, const Vec& v) { return v._scalar_apply<op::rev<op::ge>>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator> (S s, const Vec& v) { return v._scalar_apply<op::rev<op::sg>>(s); }

		template <class S, size_t M, int L> Vec<type::add<S, T>, N> operator+(const Vec<S, M, L>& v) const { return _vector_apply<op::add>(v); }
		template <class S, size_t M, int L> Vec<type::add<S, T>, N> operator-(const Vec<S, M, L>& v) const { return _vector_apply<op::sub>(v); }
		template <class S, size_t M, int L> Vec<type::mul<S, T>, N> operator*(const Vec<S, M, L>& v) const { return _vector_apply<op::mul>(v); }
		template <class S, size_t M, int L> Vec<type::div<S, T>, N> operator/(const Vec<S, M, L>& v) const { return _vector_apply<op::div>(v); }

		template <class S, size_t M, int L> Vec<bool, N> operator==(const Vec<S, M, L>& v) const { return _vector_apply<op::eq>(v); }
		template <class S, size_t M, int L> Vec<bool, N> operator!=(const Vec<S, M, L>& v) const { return _vector_apply<op::ne>(v); }
		template <class S, size_t M, int L> Vec<bool, N> operator< (const Vec<S, M, L>& v) const { return _vector_apply<op::sl>(v); }
		template <class S, size_t M, int L> Vec<bool, N> operator>=(const Vec<S, M, L>& v) const { return _vector_apply<op::le>(v); }
		template <class S, size_t M, int L> Vec<bool, N> operator<=(const Vec<S, M, L>& v) const { return _vector_apply<op::ge>(v); }
		template <class S, size_t M, int L> Vec<bool, N> operator> (const Vec<S, M, L>& v) const { return _vector_apply<op::sg>(v); }

		explicit operator bool() const { return bool(_data); }
	};

	template <class T, int K = 1> using Vec2 = Vec<T, 2, K>;
	template <class T, int K = 1> using Vec3 = Vec<T, 3, K>;
	template <class T, int K = 1> using Vec4 = Vec<T, 4, K>;

	using bool2 = Vec<bool, 2>;
	using bool3 = Vec<bool, 3>;
	using bool4 = Vec<bool, 4>;

	using float2 = Vec<float, 2>;
	using float3 = Vec<float, 3>;
	using float4 = Vec<float, 4>;

	using double2 = Vec<double, 2>;
	using double3 = Vec<double, 3>;
	using double4 = Vec<double, 4>;

	template <class First, class... Rest>
	inline Vec<scalar<First>, details::element_count<First, Rest...>::value> vector(const First& first, const Rest&... rest)
	{
		decltype(vector(first, rest...)) result;
		details::write_vector(&result[0], first, rest...);
		return result;
	}
	template <class T, class... Args>
	inline Vec<T, details::element_count<Args...>::value> vector(Args... args)
	{
		decltype(vector<T>(args...)) result;
		details::write_vector(&result[0], args...);
		return result;
	}


	namespace details
	{
		template <class T, size_t N, int K>
		struct Scalar<Vec<T, N, K>> { using type = T; };
		template <class T, size_t N>
		struct Scalar<Dir<T, N>> { using type = T; };
		template <class T, size_t I>
		struct Scalar<Component<T, I>> { using type = T; };

		template <class A> struct fail { static_assert(details::always_false<A>, "Intended failure"); };

		template <class A, class B, size_t IA, size_t IB>
		struct component_op
		{
			static auto add(Component<A, IA> a, Component<B, IB> b)
			{
				using T = type::add<A, B>;
				Vec<T, std::max(IA, IB) + 1> result;
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
				using T = type::add<A, B>;
				Vec<T, std::max(IA, IB) + 1> result;
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
				return Component<type::add<A, B>, I>{ *a + *b };
			}
			static auto sub(Component<A, I> a, Component<B, I> b)
			{
				return Component<type::add<A, B>, I>{ *a - *b };
			}
		};


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
	}



	template <class A, class B, size_t NA, size_t NB, size_t NC, int KA, int KB, int KC>
	auto ifelse(const Vec<bool, NC, KC>& cond, const Vec<A, NA, KA>& a, const Vec<B, NB, KB>& b)
	{
		static constexpr size_t N = require::equal<NC, require::equal<NA, NB>>;
		Vec<type::common<A, B>, N> result;
		for (size_t i = 0; i < N; ++i)
			result[i] = cond[i] ? a[i] : b[i];
		return result;
	}
	template <class A, class B, size_t NA, size_t NC, int KA, int KC>
	auto ifelse(const Vec<bool, NC, KC>& cond, const Vec<A, NA, KA>& a, B b)
	{
		static_assert(is_scalar_v<B>);
		static constexpr size_t N = require::equal<NA, NC>;
		Vec<type::common<A, B>, N> result;
		for (size_t i = 0; i < N; ++i)
			result[i] = cond[i] ? a[i] : b;
		return result;
	}
	template <class A, class B, size_t NB, size_t NC, int KB, int KC>
	auto ifelse(const Vec<bool, NC, KC>& cond, A a, const Vec<B, NB, KB>& b)
	{
		static_assert(is_scalar_v<A>);
		static constexpr size_t N = require::equal<NB, NC>;
		Vec<type::common<A, B>, N> result;
		for (size_t i = 0; i < N; ++i)
			result[i] = cond[i] ? a : b[i];
		return result;
	}
	template <class A, class B, size_t NC, int KC>
	auto ifelse(const Vec<bool, NC, KC>& cond, A a, B b)
	{
		static_assert(is_scalar_v<A>);
		static_assert(is_scalar_v<B>);
		static constexpr size_t N = NC;
		Vec<type::common<A, B>, N> result;
		for (size_t i = 0; i < N; ++i)
			result[i] = cond[i] ? a : b;
		return result;
	}

	template <class A, class B>
	auto ifelse(bool cond, const A& a, const B& b)
	{
		return cond ? type::common<A, B>(a) : type::common<A, B>(b);
	}



	template <class A, class B, size_t N, int K, size_t I>
	auto operator+(const Vec<A, N, K>& v, Component<B, I> c)
	{
		Vec<type::of<op::add, A, B>, std::max(N, I + 1)> result;
		for (int i = 0; i < N; ++i)
			result[i] = v[i];
		for (int i = N; i <= I; ++i)
			result[i] = A(0);
		result[I] += *c;
		return result;
	}
	template <class A, class B, size_t N, int K, size_t I>
	auto operator+(Component<B, I> c, const Vec<A, N, K>& v) { return v + c; }
	template <class A, class B, size_t N, int K, size_t I>
	auto operator-(const Vec<A, N, K>& v, Component<B, I> c)
	{
		Vec<type::of<op::sub, A, B>, std::max(N, I + 1)> result;
		for (int i = 0; i < N; ++i)
			result[i] = v[i];
		for (int i = N; i <= I; ++i)
			result[i] = A(0);
		result[I] -= *c;
		return result;
	}
	template <class A, class B, size_t N, int K, size_t I>
	auto operator-(Component<B, I> c, const Vec<A, N, K>& v)
	{
		Vec<type::of<op::sub, B, A>, std::max(N, I + 1)> result;
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
	auto operator*(Component<B, I> c, const Vec<A, N, K>& v) { return v*c; }

	template <class A, class B, size_t I>
	bool operator==(Component<A, I> a, B b) { return a.value == b; }

	template <class A, size_t NA, int KA> auto& rest(      Vec<A, NA, KA>& v) { return reinterpret_cast<      Vec<A, NA - 1, KA>&>(v[1]); }
	template <class A, size_t NA, int KA> auto& rest(const Vec<A, NA, KA>& v) { return reinterpret_cast<const Vec<A, NA - 1, KA>&>(v[1]); }

	template <int K>
	bool any(const Vec<bool, 2, K>& v) { return v[0] | v[1]; }
	template <size_t N, int K>
	bool any(const Vec<bool, N, K>& v) { return v[0] | any(rest(v)); }

	template <int K>
	bool all(const Vec<bool, 2, K>& v) { return v[0] & v[1]; }
	template <size_t N, int K>
	bool all(const Vec<bool, N, K>& v) { return v[0] & all(rest(v)); }

	template <class T, int K>
	T maxComponent(const Vec<T, 2, K>& v) { using namespace std; return max(v[0], v[1]); }
	template <class T, size_t N, int K>
	T maxComponent(const Vec<T, N, K>& v) { using namespace std; return max(v[0], maxComponent(rest(v))); }
	template <class T, int K>
	T minComponent(const Vec<T, 2, K>& v) { using namespace std; return min(v[0], v[1]); }
	template <class T, size_t N, int K>
	T minComponent(const Vec<T, N, K>& v) { using namespace std; return min(v[0], minComponent(rest(v))); }

	template <class T, int K>
	type::mul<T> product(const Vec<T, 2, K>& v) { return v[0] * v[1]; }
	template <class T, size_t N, int K>
	auto product(const Vec<T, N, K>& v) { return v[0] * product(rest(v)); }

	template <class A, size_t NA, int KA>
	Vec<type::add<A>, NA - 1> differences(const Vec<A, NA, KA>& a)
	{
		Vec<type::add<A>, NA - 1> result;
		for (size_t i = 0; i < NA - 1; ++i)
			result[i] = a[i + 1] - a[i];
		return result;
	}

	template <size_t N, int K> inline Vec<bool, N> operator!(const Vec<bool, N, K>& a) { Vec<bool, N> r; for (size_t i = 0; i < N; ++i) r[i] = !a[i]; return r; }


	// Counter-clockwise angle from 'a' to 'b' in (-pi, pi)
	template <class A, class B, int KA, int KB> auto signed_angle(const Vec<A, 2, KA>& a, const Vec<B, 2, KB>& b)
	{
		return atan2(cross(a, b), dot(a, b));
	}
	// Counter-clockwise angle from 'a' to 'b' in (-pi, pi) around 'axis'
	template <class A, class B, class C, int KA, int KB, int KC> auto signed_angle(const Vec<A, 3, KA>& a, const Vec<B, 3, KB>& b, const Vec<C, 3, KC>& axis)
	{
		const auto Z = direction(axis);
		const auto X = a - Z*dot(a, Z);
		const auto Y = cross(Z, X);
		return atan2(dot(Y, b), dot(X, b));
	}

	template <class A, class B, size_t N, int KA, int KB>
	Vec<type::add<A, B>, N> sum(const Vec<A, N, KA>& a, const Vec<B, N, KB>& b) { return a + b; }
	template <class First, class... Rest, size_t N, int KF>
	auto sum(const Vec<First, N, KF>& first, const Rest&... rest) { return first + sum(rest...); }

	template <class A, class B, size_t NA, size_t NB, int KA, int KB>
	auto cross(const Vec<A, NA, KA>& u, const Vec<B, NB, KB>& v)
	{
		static_assert(NA == NB);
		return details::cross_product<NA>::of(u, v);
	}

	template <class T, int K> T cross(Axes<0>, const Vec<T, 2, K>& v) { return +v[1]; }
	template <class T, int K> T cross(Axes<1>, const Vec<T, 2, K>& v) { return -v[0]; }
	template <class T, int K> T cross(const Vec<T, 2, K>& v, Axes<0>) { return -v[1]; }
	template <class T, int K> T cross(const Vec<T, 2, K>& v, Axes<1>) { return +v[0]; }

	template <class T, int K> Vec<T, 3> cross(Axes<0>, const Vec<T, 3, K>& a) { return vector<T>(0, -a[2], +a[1]); }
	template <class T, int K> Vec<T, 3> cross(const Vec<T, 3, K>& a, Axes<0>) { return vector<T>(0, +a[2], -a[1]); }
	template <class T, int K> Vec<T, 3> cross(Axes<1>, const Vec<T, 3, K>& a) { return vector<T>(+a[2], 0, -a[0]); }
	template <class T, int K> Vec<T, 3> cross(const Vec<T, 3, K>& a, Axes<1>) { return vector<T>(-a[2], 0, +a[0]); }
	template <class T, int K> Vec<T, 3> cross(Axes<2>, const Vec<T, 3, K>& a) { return vector<T>(-a[1], +a[0], 0); }
	template <class T, int K> Vec<T, 3> cross(const Vec<T, 3, K>& a, Axes<2>) { return vector<T>(+a[1], -a[0], 0); }

	template <class V, class B, size_t IB> auto cross(const V& a, Component<B, IB> b) { return cross(a, Axes<IB>{}) * *b; }
	template <class V, class B, size_t IB> auto cross(Component<B, IB> b, const V& a) { return *b * cross(Axes<IB>{}, a); }




	template <size_t N, int K>
	constexpr size_t index(const Vec<bool, N, K>& v)
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
	Vec<bool, N> from_index(size_t idx)
	{
		Vec<bool, N> result;
		for (size_t i = 0; i < N; ++i)
			result[i] = (idx & (1 << i)) != 0;
		return result;
	}


	template <class T, size_t N, int K>
	std::ostream& operator<<(std::ostream& out, const Vec<T, N, K>& v)
	{
		out << '[' << v[0];
		for (size_t i = 1; i < N; ++i)
			out << ", " << v[i];
		return out << ']';
	}

	template <class T, size_t N>
	class Dir : public Vec<T, N>
	{
		Dir(const Vec<T, N>& v) : Vec<T, N>(v) { }
	public:
		template <int K>
		static Dir fromUnchecked(const Vec<T, N, K>& v) { return { v }; }
	};
	template <class T, size_t N, int K>
	Dir<T, N> uncheckedDir(const Vec<T, N, K>& v) { return Dir<T, N>::fromUnchecked(v); }

	template <class T, size_t N>
	struct is_unit<N, Dir<T, N>> : std::true_type { };

	template <class T, size_t N, int K>
	struct Decomposed<Vec<T, N, K>>
	{
		using D = Dir<type::identity<T>, N>;
		
		T length;
		D direction;

		Decomposed(const Vec<T, N, K>& v) : length(uv::length(v)), direction(D::fromUnchecked(v/length)) { }
	};
	template <class T, size_t N>
	struct Decomposed<Dir<T, N>>
	{
		static constexpr T length = T(1);
		Dir<T, N> direction;

		Decomposed(const Dir<T, N>& d) : direction(d) { }
	};
}

namespace std
{
	template <class A, class B, size_t N, int K>
	struct common_type<uv::Vec<A, N, K>, B>
	{
		using type = uv::Vec<common_type_t<A, B>, N>;
	};
	template <class A, class B, size_t N, int K>
	struct common_type<A, uv::Vec<B, N, K>>
	{
		using type = uv::Vec<common_type_t<A, B>, N>;
	};
	template <class A, class B, size_t N, int KA, int KB>
	struct common_type<uv::Vec<A, N, KA>, uv::Vec<B, N, KB>>
	{
		using type = uv::Vec<common_type_t<A, B>, N>;
	};
}

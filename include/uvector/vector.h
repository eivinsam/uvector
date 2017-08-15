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
	class Vector;

	template <size_t... I>
	struct Axes
	{
		template <size_t... J>
		constexpr Axes<I..., J...> operator+(Axes<J...>) const { return {}; }
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

		template <class S, size_t J>
		constexpr Component<type::mul<T, S>, I> operator*(Component<S, J> c)
		{
			static_assert(I == J, "Component-component multiplication requires equal indices");
			return { _value * c._value };
		}
	};
	template <class T, size_t I, class = if_scalar_t<T>> Component<T, I> operator*(Axes<I>, T value) { return { value }; }
	template <class T, size_t I, class = if_scalar_t<T>> Component<T, I> operator*(T value, Axes<I>) { return { value }; }

	namespace axes
	{
		static constexpr Axes<> Nought = {};

		static constexpr Axes<0> X = {};
		static constexpr Axes<1> Y = {};
		static constexpr Axes<2> Z = {};
		static constexpr Axes<3> W = {};

		static constexpr auto XY = X + Y;
		static constexpr auto YZ = Y + Z;
		static constexpr auto ZW = Z + W;
		static constexpr auto WX = W + X;

		static constexpr auto XZ = X + Z;
		static constexpr auto YW = Y + W;
		static constexpr auto ZX = Z + X;
		static constexpr auto WY = W + Y;

		static constexpr auto XW = X + W;
		static constexpr auto YX = Y + X;
		static constexpr auto ZY = Z + Y;
		static constexpr auto WZ = W + Z;

		static constexpr auto XYZ = X + Y + Z;
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
			template <class A, size_t AN, int AK> struct result<Vector<A, AN, AK>> { using type = Vector<A, N, K*AK>; };

			using type = typename result<V>::type;

			static type on(V&& v)
			{
				static_assert(Imax < dim<V>, "Highest index must be less than vector dimension");
				return reinterpret_cast<type>(v[Ifirst]);
			}
		};
		static_assert(std::is_same_v<slicer<      Vector<int, 4>&, 2, 0, 2>::type,       Vector<int, 2, 2>&>);
		static_assert(std::is_same_v<slicer<const Vector<int, 4>&, 2, 1, 2>::type, const Vector<int, 2, 2>&>);

		template <size_t N>
		inline void assert_all_less() { }
		template <size_t N, size_t First, size_t... Rest>
		inline void assert_all_less() { static_assert(First < N, "Indices must be less than N"); assert_all_less<N, Rest...>(); }

		template <class V, int... I>
		struct copy_selector { };
		template <class T, size_t N, int K, int... I>
		struct copy_selector<Vector<T, N, K>, I...>
		{
			using type = Vector<T, sizeof...(I)>;
			static type on(const Vector<T, N, K>& v)
			{
				assert_all_less<N, I...>();
				return { v[I...] };
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
	class Vector
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
			Vector<type::of<OP, T, S>, N> result;
			for (size_t i = 0; i < N; ++i)
				result[i] = op((*this)[i], s);
			return result;
		}
		template <class OP, class B, size_t NB, int KB>
		auto _vector_apply(const Vector<B, NB, KB>& b) const
		{
			static_assert(N == NB, "Vector-vector operation requires equal dimensionality");
			OP op;
			Vector<type::of<OP, T, B>, N> result;
			for (size_t i = 0; i < N; ++i)
				result[i] = op((*this)[i], b[i]);
			return result;
		}
	public:

		Vector() { }

		Vector(const Vector&) = default;
		Vector(T s) : _data(s) { }

		template <class S, size_t M, int L>
		Vector(const Vector<S, M, L>& b) { *this = b; }

		Vector& operator=(const Vector&) = default;
		Vector& operator=(T s) { _data = s; }

		template <class S, size_t M, int L>
		Vector& operator=(const Vector<S, M, L>& b)
		{
			static_assert(N == M, "Vector assignment requires equal dimensionality");
			for (size_t i = 0; i < N; ++i)
				(*this)[i] = b[i];
			return *this;
		}


		auto begin()       { return _data.begin(); }
		auto begin() const { return _data.begin(); }
		auto end()       { return _data.end(); }
		auto end() const { return _data.end(); }

		      T& operator[](size_t i)       { return _data[i]; }
		const T& operator[](size_t i) const { return _data[i]; }

		constexpr size_t size() const { return N; }

		Vector operator-() const { Vector result; for (size_t i = 0; i < N; ++i) result[i] = -(*this)[i]; return result; }

		friend T sum(const Vector& v) { T s = T(0); for (size_t i = 0; i < N; ++i) s += v[i]; return s; }

		friend type::mul<T> square(const Vector& v) { return sum(v*v); }

		template <class S, size_t M, int L> 
		friend type::mul<T, S> dot(const Vector& a, const Vector<S, M, L>& b) { return sum(a*b); }
		
		template <size_t I> friend T dot(const Vector& v, Axes<I>)
		{ 
			static_assert(I < N, "Cannot dot vector with higher-dimensional axis");
			return v[I]; 
		}
		template <size_t I> friend T dot(Axes<I> a, const Vector& v) { return v*a; }
		template <class S, size_t I> friend type::mul<T, S> dot(const Vector& v, Component<S, I> c) { return (v*Axes<I>{0}) * *c; }
		template <class S, size_t I> friend type::mul<S, T> dot(const Vector& v, Component<S, I> c) { return *c * (Axes<I>{0}*v); }

		template <class S, size_t M, int L> friend auto angle(const Vector& a, const Vector<S, M, L>& b)
		{
			using R = type::identity<type::mul<T, S>>;
			return acos(std::clamp(dot(a, b) / sqrt(square(a)*square(b)), R(-1), R(1)));
		}

		friend bool isfinite(const Vector& a) { for (size_t i = 0; i < NA; ++i) if (!isfinite(a[i])) return false; return true; }

		friend Vector abs(const Vector& a)
		{
			using namespace std;
			Vector result;
			for (size_t i = 0; i < N; ++i)
				result[i] = abs(a[i]);
			return result;
		}

		template <size_t... I> friend details::select_t<Vector&, I...> operator*(Vector& v, Axes<I...>) { return select<I...>(v); }
		template <size_t... I> friend details::select_t<Vector&, I...> operator*(Axes<I...>, Vector& v) { return select<I...>(v); }
		template <size_t... I> friend details::select_t<const Vector&, I...> operator*(const Vector& v, Axes<I...>) { return select<I...>(v); }
		template <size_t... I> friend details::select_t<const Vector&, I...> operator*(Axes<I...>, const Vector& v) { return select<I...>(v); }

		template <size_t I> friend Vector operator+(Vector v, Axes<I>) { v[I] = v[I] + T(1); return v; }
		template <size_t I> friend Vector operator-(Vector v, Axes<I>) { v[I] = v[I] - T(1); return v; }
		template <size_t I> friend Vector operator+(Axes<I>, Vector v) { v[I] = v[I] + T(1); return v; }
		template <size_t I> friend Vector operator-(Axes<I>, Vector v) { v = -v; v[I] = v[I] + T(1); return v; }

		template <class S, size_t I> friend Component<type::mul<T, S>, I> operator*(const Vector& v, Component<S, I> c) { return v[I] * *c; }

		template <class S, size_t I> friend auto operator+(const Vector& v, Component<S, I> c) { Vector<type::add<T, S>, N> r = v; r[I] = r[I] + *c; return r; }
		template <class S, size_t I> friend auto operator-(const Vector& v, Component<S, I> c) { Vector<type::add<T, S>, N> r = v; r[I] = r[I] - *c; return r; }
		template <class S, size_t I> friend auto operator+(Component<S, I> c, const Vector& v) { Vector<type::add<T, S>, N> r = v; r[I] = r[I] + *c; return r; }
		template <class S, size_t I> friend auto operator-(Component<S, I> c, const Vector& v) { Vector<type::add<T, S>, N> r = -v; r[I] = r[I] + *c; return r; }

		template <class S, class = if_scalar_t<S>> friend auto operator+(const Vector& v, S s) { return v._scalar_apply<op::add>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator-(const Vector& v, S s) { return v._scalar_apply<op::sub>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator*(const Vector& v, S s) { return v._scalar_apply<op::mul>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator/(const Vector& v, S s) { return v._scalar_apply<op::div>(s); }

		template <class S, class = if_scalar_t<S>> friend auto operator==(const Vector& v, S s) { return v._scalar_apply<op::eq>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator!=(const Vector& v, S s) { return v._scalar_apply<op::ne>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator< (const Vector& v, S s) { return v._scalar_apply<op::sl>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator<=(const Vector& v, S s) { return v._scalar_apply<op::le>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator>=(const Vector& v, S s) { return v._scalar_apply<op::ge>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator> (const Vector& v, S s) { return v._scalar_apply<op::sg>(s); }

		template <class S, class = if_scalar_t<S>> friend auto operator+(S s, const Vector& v) { return v._scalar_apply<op::rev<op::add>>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator-(S s, const Vector& v) { return v._scalar_apply<op::rev<op::sub>>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator*(S s, const Vector& v) { return v._scalar_apply<op::rev<op::mul>>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator/(S s, const Vector& v) { return v._scalar_apply<op::rev<op::div>>(s); }

		template <class S, class = if_scalar_t<S>> friend auto operator==(S s, const Vector& v) { return v._scalar_apply<op::rev<op::eq>>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator!=(S s, const Vector& v) { return v._scalar_apply<op::rev<op::ne>>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator< (S s, const Vector& v) { return v._scalar_apply<op::rev<op::sl>>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator<=(S s, const Vector& v) { return v._scalar_apply<op::rev<op::le>>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator>=(S s, const Vector& v) { return v._scalar_apply<op::rev<op::ge>>(s); }
		template <class S, class = if_scalar_t<S>> friend auto operator> (S s, const Vector& v) { return v._scalar_apply<op::rev<op::sg>>(s); }

		template <class S, size_t M, int L> Vector<type::add<S, T>, N> operator+(const Vector<S, M, L>& v) const { return _vector_apply<op::add>(v); }
		template <class S, size_t M, int L> Vector<type::add<S, T>, N> operator-(const Vector<S, M, L>& v) const { return _vector_apply<op::sub>(v); }
		template <class S, size_t M, int L> Vector<type::mul<S, T>, N> operator*(const Vector<S, M, L>& v) const { return _vector_apply<op::mul>(v); }
		template <class S, size_t M, int L> Vector<type::div<S, T>, N> operator/(const Vector<S, M, L>& v) const { return _vector_apply<op::div>(v); }

		template <class S, size_t M, int L> Vector<bool, N> operator==(const Vector<S, M, L>& v) const { return _vector_apply<op::eq>(v); }
		template <class S, size_t M, int L> Vector<bool, N> operator!=(const Vector<S, M, L>& v) const { return _vector_apply<op::ne>(v); }
		template <class S, size_t M, int L> Vector<bool, N> operator< (const Vector<S, M, L>& v) const { return _vector_apply<op::sl>(v); }
		template <class S, size_t M, int L> Vector<bool, N> operator>=(const Vector<S, M, L>& v) const { return _vector_apply<op::le>(v); }
		template <class S, size_t M, int L> Vector<bool, N> operator<=(const Vector<S, M, L>& v) const { return _vector_apply<op::ge>(v); }
		template <class S, size_t M, int L> Vector<bool, N> operator> (const Vector<S, M, L>& v) const { return _vector_apply<op::sg>(v); }

		explicit operator bool() const { return bool(_data); }
	};
	using bool2 = Vector<bool, 2>;
	using bool3 = Vector<bool, 3>;
	using bool4 = Vector<bool, 4>;

	using float2 = Vector<float, 2>;
	using float3 = Vector<float, 3>;
	using float4 = Vector<float, 4>;

	using double2 = Vector<double, 2>;
	using double3 = Vector<double, 3>;
	using double4 = Vector<double, 4>;

	template <class First, class... Rest>
	inline Vector<scalar<First>, details::element_count<First, Rest...>::value> vector(const First& first, const Rest&... rest)
	{
		decltype(vector(first, rest...)) result;
		details::write_vector(&result[0], first, rest...);
		return result;
	}
	template <class T, class... Args>
	inline Vector<T, details::element_count<Args...>::value> vector(Args... args)
	{
		decltype(vector<T>(args...)) result;
		details::write_vector(&result[0], args...);
		return result;
	}


	namespace details
	{
		template <class T, size_t N, int K>
		struct Scalar<Vector<T, N, K>> { using type = T; };

		template <class A> struct fail { static_assert(details::always_false<A>, "Intended failure"); };

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
	auto ifelse(const Vector<bool, NC, KC>& cond, const Vector<A, NA, KA>& a, const Vector<B, NB, KB>& b)
	{
		static constexpr size_t N = require::equal<NC, require::equal<NA, NB>>;
		Vector<type::common<A, B>, N> result;
		for (size_t i = 0; i < N; ++i)
			result[i] = cond[i] ? a[i] : b[i];
		return result;
	}
	template <class A, class B, size_t NA, size_t NC, int KA, int KC>
	auto ifelse(const Vector<bool, NC, KC>& cond, const Vector<A, NA, KA>& a, B b)
	{
		static_assert(is_scalar_v<B>);
		static constexpr size_t N = require::equal<NA, NC>;
		Vector<type::common<A, B>, N> result;
		for (size_t i = 0; i < N; ++i)
			result[i] = cond[i] ? a[i] : b;
		return result;
	}
	template <class A, class B, size_t NB, size_t NC, int KB, int KC>
	auto ifelse(const Vector<bool, NC, KC>& cond, A a, const Vector<B, NB, KB>& b)
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

	template <class A, class B>
	auto ifelse(bool cond, const A& a, const B& b)
	{
		return cond ? type::common<A, B>(a) : type::common<A, B>(b);
	}



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
	auto operator*(Component<B, I> c, const Vector<A, N, K>& v) { return v*c; }

	template <class A, class B, size_t I>
	bool operator==(Component<A, I> a, B b) { return a.value == b; }

	template <class A, size_t NA, int KA> auto& rest(      Vector<A, NA, KA>& v) { return reinterpret_cast<      Vector<A, NA - 1, KA>&>(v[1]); }
	template <class A, size_t NA, int KA> auto& rest(const Vector<A, NA, KA>& v) { return reinterpret_cast<const Vector<A, NA - 1, KA>&>(v[1]); }

	template <int K>
	bool any(const Vector<bool, 2, K>& v) { return v[0] | v[1]; }
	template <size_t N, int K>
	bool any(const Vector<bool, N, K>& v) { return v[0] | any(rest(v)); }

	template <int K>
	bool all(const Vector<bool, 2, K>& v) { return v[0] & v[1]; }
	template <size_t N, int K>
	bool all(const Vector<bool, N, K>& v) { return v[0] & all(rest(v)); }

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
	Vector<type::add<A>, NA - 1> differences(const Vector<A, NA, KA>& a)
	{
		Vector<type::add<A>, NA - 1> result;
		for (size_t i = 0; i < NA - 1; ++i)
			result[i] = a[i + 1] - a[i];
		return result;
	}

	template <size_t N, int K> inline Vector<bool, N> operator!(const Vector<bool, N, K>& a) { Vector<bool, N> r; for (size_t i = 0; i < N; ++i) r[i] = !a[i]; return r; }


	// Counter-clockwise angle from 'a' to 'b' in (-pi, pi)
	template <class A, class B, int KA, int KB> auto signed_angle(const Vector<A, 2, KA>& a, const Vector<B, 2, KB>& b)
	{
		return atan2(cross(a, b), dot(a, b));
	}
	// Counter-clockwise angle from 'a' to 'b' in (-pi, pi) around 'axis'
	template <class A, class B, class C, int KA, int KB, int KC> auto signed_angle(const Vector<A, 3, KA>& a, const Vector<B, 3, KB>& b, const Vector<C, 3, KC>& axis)
	{
		const auto Z = direction(axis);
		const auto X = a - Z*dot(a, Z);
		const auto Y = cross(Z, X);
		return atan2(dot(Y, b), dot(X, b));
	}

	template <class A, class B, size_t N, int KA, int KB>
	Vector<type::add<A, B>, N> sum(const Vector<A, N, KA>& a, const Vector<B, N, KB>& b) { return a + b; }
	template <class First, class... Rest, size_t N, int KF>
	auto sum(const Vector<First, N, KF>& first, const Rest&... rest) { return first + sum(rest...); }

	template <class A, class B, size_t NA, size_t NB, int KA, int KB>
	auto cross(const Vector<A, NA, KA>& u, const Vector<B, NB, KB>& v)
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


	template <class T, size_t N, int K>
	std::ostream& operator<<(std::ostream& out, const Vector<T, N, K>& v)
	{
		out << '[' << v[0];
		for (size_t i = 1; i < N; ++i)
			out << ", " << v[i];
		return out << ']';
	}

}

namespace std
{
	template <class A, class B, size_t N, int K>
	struct common_type<uv::Vector<A, N, K>, B>
	{
		using type = uv::Vector<common_type_t<A, B>, N>;
	};
	template <class A, class B, size_t N, int K>
	struct common_type<A, uv::Vector<B, N, K>>
	{
		using type = uv::Vector<common_type_t<A, B>, N>;
	};
	template <class A, class B, size_t N, int KA, int KB>
	struct common_type<uv::Vector<A, N, KA>, uv::Vector<B, N, KB>>
	{
		using type = uv::Vector<common_type_t<A, B>, N>;
	};
}

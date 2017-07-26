#pragma once

#include <array>
#include <iterator>
#include <ostream>

#include "scalar.h"

#include "../base/gsl.h"

namespace uv
{
	template <class T> struct is_vector : std::false_type { };
	template <class T> struct is_vector<T&> : is_vector<T> { };
	template <class T> struct is_vector<const T> : is_vector<T> { };
	template <class T> static constexpr bool is_vector_v = is_vector<T>::value;

	template <class T, class R = void> struct if_vector : std::enable_if<is_vector_v<T>, R> { };
	template <class T, class R = void> using if_vector_t = typename if_vector<T, R>::type;

	template <class T, size_t N, int K>
	class Vector;

	template <class T, size_t N, int K> struct is_vector<Vector<T, N, K>> : std::true_type { };

	template <size_t... I>
	struct Axes { };

	template <size_t... IA, size_t... IB>
	constexpr Axes<IA..., IB...> operator+(Axes<IA...>, Axes<IB...>) { return {}; }

	namespace details
	{
		template <size_t... I>
		struct Dim<Axes<I...>> : std::integral_constant<size_t, sizeof...(I)> { };
	}

	namespace axes
	{
		static constexpr Axes<> Nought;

		static constexpr Axes<0> X;
		static constexpr Axes<1> Y;
		static constexpr Axes<2> Z;
		static constexpr Axes<3> W;

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

	template <class T, size_t I>
	class Component
	{
		T _value;
	public:
		Component() { }
		constexpr Component(T value) : _value(value) { }

		constexpr       T& operator*()       { return _value; }
		constexpr const T& operator*() const { return _value; }

		Component operator-() const { return Component{ -_value }; }
	};


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

	}

	template <class T, size_t N, int K = 1>
	class Vector
	{
		T _first;
	public:
		static_assert(is_scalar_v<T>, "T must be a scalar type");
		static_assert(N > 1, "vectors must have at least two dimensions");

		using size_type = size_t;
		using       iterator = details::stride_iterator<T, K>;
		using const_iterator = details::stride_iterator<const T, K>;
		using       reference = T&;
		using const_reference = const T&;
		using difference_type = ptrdiff_t;
		using value_type = T;
		using scalar_type = T;

		static constexpr size_t dim = N;

		Vector() = delete;
		Vector(const Vector&) = delete;

		template <int KB>
		Vector& operator=(const Vector<T, N, KB>& other) { for (size_t i = 0; i < N; ++i) (*this)[i] = other[i]; return *this; }
		Vector& operator=(T value)                       { for (size_t i = 0; i < N; ++i) (*this)[i] = value;    return *this; }

		auto begin()       { return       iterator{ &_first }; }
		auto begin() const { return const_iterator{ &_first }; }
		auto end()       { return begin() += N; }
		auto end() const { return begin() += N; }

		auto& operator[](size_t i)       { return begin()[i]; }
		auto& operator[](size_t i) const { return begin()[i]; }

		constexpr size_t size() const { return N; }

		explicit operator bool() const { for (size_t i = 0; i < N; ++i) if (!(*this)[i]) return false; return true; }
	};

	template <class T, size_t N>
	class Vector<T, N, 0>
	{
		T _value;
	public:
		static_assert(is_scalar_v<T>, "T must be a scalar type");
		static_assert(N > 1, "vectors must have at least two dimensions");

		using size_type = size_t;
		using       iterator = details::repeat_iterator<T>;
		using const_iterator = details::repeat_iterator<const T>;
		using       reference = T&;
		using const_reference = const T&;
		using difference_type = ptrdiff_t;
		using value_type = T;
		using scalar_type = T;

		static constexpr size_t dim = N;

		Vector() { }
		constexpr Vector(const Vector&) = default;
		constexpr Vector(T value) : _value(value) { }

		constexpr Vector& operator=(const Vector& other) = default;
		constexpr Vector& operator=(T value) { _value = value; }

		auto begin()       { return       iterator{ &_value }; }
		auto begin() const { return const_iterator{ &_value }; }

		auto end()       { return begin() += N; }
		auto end() const { return begin() += N; }

		auto& operator[](size_t)       { return _value; }
		auto& operator[](size_t) const { return _value; }

		constexpr size_t size() const { return N; }

		explicit constexpr operator bool() const { return bool(_value); }
	};


	template <class T, size_t N>
	class Vector<T, N, 1> : public std::array<T, N>
	{
	public:
		using scalar_type = T;
		static constexpr size_t dim = N;

		static_assert(is_scalar_v<T>, "T must be a scalar type");
		static_assert(N > 1, "vectors must have at least two dimensions");

		Vector() { }

		template <class Arg, class = if_scalar_t<Arg>>
		explicit Vector(Arg arg)
		{
			for (size_t i = 0; i < N; ++i)
				(*this)[i] = T(arg);
		}
		template <class... Args, class = std::void_t<if_scalar_t<Args>...>>
		constexpr explicit Vector(Args... args) 
		{
			static_assert(sizeof...(Args) == N, "invalid number of elements"); 
			std::array<T, N>::operator=({ T(args)... });
		}
		template <class B, int KB>
		explicit Vector(const Vector<B, N, KB>& other) { for (size_t i = 0; i < N; ++i) (*this)[i] = T(other[i]); }
		template <int KB>
		Vector(const Vector<T, N, KB>& other) { for (size_t i = 0; i < N; ++i) (*this)[i] = other[i]; }

		template <int KB>
		Vector& operator=(const Vector<T, N, KB>& other) { for (size_t i = 0; i < N; ++i) (*this)[i] = other[i]; return *this; }
		Vector& operator=(T value)                       { for (size_t i = 0; i < N; ++i) (*this)[i] = value;    return *this; }

		explicit operator bool() const { for (size_t i = 0; i < N; ++i) if (!(*this)[i]) return false; return true; }
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
}


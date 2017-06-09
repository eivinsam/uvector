#pragma once

#include <array>
#include <iterator>
#include <ostream>

#include "scalar.h"

#include "../base/gsl.h"

namespace uv
{
	template <class T, size_t N, int K>
	class Vector;

	template <size_t... I>
	struct Axes { };

	template <size_t... IA, size_t... IB>
	constexpr Axes<IA..., IB...> operator+(Axes<IA...>, Axes<IB...>) { return {}; }

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
		Component(T value) : _value(value) { }

		T& operator*() { return _value; }
		const T& operator*() const { return _value; }

		Component operator-() const { return Component{ -_value }; }
	};


	namespace type
	{
		template <class OP, class A, class B = A> using of = decltype(OP{}(std::declval<A>(), std::declval<B>()));

		template <class A, class B = A>
		using inner_product = of<op::add, of<op::mul, A, B>>;

		template <class T>
		using identity = of<op::div, T, T>;
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

		template <class C, size_t N>
		class indexable_from_begin
		{
			C* self() { return reinterpret_cast<C*>(this); }
			const C* self() const { return reinterpret_cast<const C*>(this); }
		public:

			auto end() { return self()->begin() + N; }
			auto end() const { return self()->begin() + N; }

			auto& at(size_t i) { if (i >= N) throw std::out_of_range("strided vector element out of range"); return self()->begin()[i]; }
			auto& at(size_t i) const { if (i >= N) throw std::out_of_range("strided vector element out of range"); return self()->begin()[i]; }
			auto& operator[](size_t i) { return self()->begin()[i]; }
			auto& operator[](size_t i) const { return self()->begin()[i]; }
		};
		template <class T, size_t N, int K>
		class indexable_from_begin_vec : public indexable_from_begin<Vector<T, N, K>, N> { };
	}

	template <class T, size_t N, int K = 1>
	class Vector : public details::indexable_from_begin_vec<T,N,K>
	{
		T _first;
	public:
		static_assert(is_scalar_v<T>, "T must be a scalar type");
		static_assert(N > 1, "vectors must have at least two dimensions");

		using details::indexable_from_begin_vec<T, N, K>::operator[];

		using size_type = size_t;
		using       iterator = details::stride_iterator<T, K>;
		using const_iterator = details::stride_iterator<const T, K>;
		using       reference = T&;
		using const_reference = const T&;
		using difference_type = ptrdiff_t;
		using value_type = T;
		using scalar_type = T;

		Vector() = delete;
		Vector(const Vector&) = delete;

		template <int KB>
		Vector& operator=(const Vector<T, N, KB>& other) { for (int i = 0; i < N; ++i) (*this)[i] = other[i]; return *this; }
		Vector& operator=(T value)                    { for (int i = 0; i < N; ++i) (*this)[i] = value;    return *this; }

		auto begin()       { return       iterator{ reinterpret_cast<      T*>(this) }; }
		auto begin() const { return const_iterator{ reinterpret_cast<const T*>(this) }; }

		constexpr size_t size() const { return N; }

		explicit operator bool() const { for (size_t i = 0; i < N; ++i) if (!(*this)[i]) return false; return true; }
	};

	template <class T, size_t N>
	class Vector<T, N, 0> : public details::indexable_from_begin_vec<T, N, 0>
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

		Vector() { }
		Vector(const Vector&) = default;
		Vector(T value) : _value(value) { }

		Vector& operator=(const Vector& other) { _value = other._value; };
		Vector& operator=(T value) { _value = value; }

		auto begin()       { return       iterator{ reinterpret_cast<      T*>(this) }; }
		auto begin() const { return const_iterator{ reinterpret_cast<const T*>(this) }; }

		constexpr size_t size() const { return N; }

		explicit operator bool() const { return bool(_value); }
	};

	template <class T, size_t N>
	class Vector<T, N, 1> : public std::array<T, N>
	{
	public:
		using scalar_type = T;

		static_assert(is_scalar_v<T>, "T must be a scalar type");
		static_assert(N > 1, "vectors must have at least two dimensions");

		template <class... Args>
		Vector(Args... args) 
		{
			static_assert(sizeof...(Args) == N, "invalid number of elements"); 
			std::array<T, N>::operator=({ T(args)... });
		}

		Vector() { }
		template <int KB>
		Vector(const Vector<T, N, KB>& other) { for (size_t i = 0; i < N; ++i) (*this)[i] = other[i]; }

		template <int K>
		Vector& operator=(const Vector<T, N, K>& other) { for (size_t i = 0; i < N; ++i) (*this)[i] = other[i]; return *this; }
		Vector& operator=(T value) { for (size_t i = 0; i < N; ++i) (*this)[i] = value; return *this; }

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

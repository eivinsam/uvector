#pragma once

#include <array>
#include <iterator>
#include <ostream>

#include "../base/gsl.h"

#define TEMPLATE_VECTOR_A template <class A, size_t NA, int KA>
#define TEMPLATE_VECTORS_AB template <class A, class B, size_t NA, size_t NB, int KA, int KB>
#define TEMPLATE_VECTOR_A_SCALAR_B template <class A, class B, size_t NA, int KA>
#define VECTOR_A vec<A, NA, KA> 
#define VECTOR_B vec<B, NB, KB>
#define AS_VECTOR_B reinterpret_cast<vec<B, NA, 0>&>


namespace uv
{
	template <size_t A, size_t B>
	struct equal_test { static_assert(A == B, "vectors must be of equal length"); };
	template <size_t A, size_t B>
	struct greater_test { static_assert(A > B, "vector too short"); };

	template <size_t A, size_t B>
	constexpr equal_test<A, B> require_equal = {};


	template <class A, class B>
	struct add_result { using type = decltype(std::declval<A>() + std::declval<B>()); };
	template <class A, class B>
	struct sub_result { using type = decltype(std::declval<A>() - std::declval<B>()); };
	template <class A, class B>
	struct mul_result { using type = decltype(std::declval<A>() * std::declval<B>()); };
	template <class A, class B>
	struct div_result { using type = decltype(std::declval<A>() / std::declval<B>()); };

	template <class A, class B = A>
	using add_result_t = typename add_result<A, B>::type;
	template <class A, class B = A>
	using sub_result_t = typename sub_result<A, B>::type;
	template <class A, class B = A>
	using mul_result_t = typename mul_result<A, B>::type;
	template <class A, class B = A>
	using div_result_t = typename div_result<A, B>::type;

	template <class A, class B = A>
	using inner_product_t = add_result_t<mul_result_t<A, B>>;

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

		ptrdiff_t operator-(const stride_iterator& other) const { return (_ptr - other._ptr)/K; }

		reference operator[](ptrdiff_t n) const { return _ptr[n*K]; }
		reference operator[](size_t    n) const { return _ptr[n*K]; }
		reference operator* () const { return *_ptr; }
		pointer   operator->() const { return  _ptr; }

		bool operator==(const stride_iterator& other) const { return _ptr == other._ptr; }
		bool operator!=(const stride_iterator& other) const { return _ptr != other._ptr; }

		bool operator< (const stride_iterator& other) const { return _ptr <  other._ptr; }
		bool operator<=(const stride_iterator& other) const { return _ptr <= other._ptr; }
		bool operator>=(const stride_iterator& other) const { return _ptr >= other._ptr; }
		bool operator> (const stride_iterator& other) const { return _ptr >  other._ptr; }
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
		reference operator[](size_t   ) const { return *_ptr; }
		reference operator* () const { return *_ptr; }
		pointer   operator->() const { return  _ptr; }

		bool operator==(const repeat_iterator& other) const { return _i == other._i; }
		bool operator!=(const repeat_iterator& other) const { return _i != other._i; }
		bool operator< (const repeat_iterator& other) const { return _i <  other._i; }
		bool operator<=(const repeat_iterator& other) const { return _i <= other._i; }
		bool operator>=(const repeat_iterator& other) const { return _i >= other._i; }
		bool operator> (const repeat_iterator& other) const { return _i >  other._i; }
	};

	template <class C, size_t N>
	class indexable_from_begin
	{
		      C* self()       { return reinterpret_cast<      C*>(this); }
		const C* self() const { return reinterpret_cast<const C*>(this); }
	public:

		auto end()       { return self()->begin() + N; }
		auto end() const { return self()->begin() + N; }

		auto& at(size_t i)       { if (i >= N) throw std::out_of_range("strided vector element out of range"); return self()->begin()[i]; }
		auto& at(size_t i) const { if (i >= N) throw std::out_of_range("strided vector element out of range"); return self()->begin()[i]; }
		auto& operator[](size_t i)       { return self()->begin()[i]; }
		auto& operator[](size_t i) const { return self()->begin()[i]; }
	};
	template <class T, size_t N, int K>
	class vec;
	template <class T, size_t N, int K>
	class indexable_from_begin_vec : public indexable_from_begin<vec<T, N, K>, N> { };

	template <class T, size_t N, int K = 1>
	class vec : public indexable_from_begin_vec<T,N,K>
	{
		T _first;
	public:
		static_assert(N > 1, "vectors must have at least two dimensions");

		using indexable_from_begin_vec<T, N, K>::operator[];

		using size_type = size_t;
		using       iterator = stride_iterator<T, K>;
		using const_iterator = stride_iterator<const T, K>;
		using       reference = T&;
		using const_reference = const T&;
		using difference_type = ptrdiff_t;
		using value_type = T;

		vec() = delete;
		vec(const vec&) = delete;

		template <int KB>
		vec& operator=(const vec<T, N, KB>& other) { for (int i = 0; i < N; ++i) (*this)[i] = other[i]; return *this; }
		vec& operator=(T value)                    { for (int i = 0; i < N; ++i) (*this)[i] = value;    return *this; }

		auto begin()       { return       iterator{ reinterpret_cast<      T*>(this) }; }
		auto begin() const { return const_iterator{ reinterpret_cast<const T*>(this) }; }

		constexpr size_t size() const { return N; }

		explicit operator bool() const { for (size_t i = 0; i < N; ++i) if (!(*this)[i]) return false; return true; }
	};

	template <class T, size_t N>
	class vec<T, N, 0> : public indexable_from_begin_vec<T, N, 0>
	{
		T _value;
	public:
		static_assert(N > 1, "vectors must have at least two dimensions");

		using size_type = size_t;
		using       iterator = repeat_iterator<T>;
		using const_iterator = repeat_iterator<const T>;
		using       reference = T&;
		using const_reference = const T&;
		using difference_type = ptrdiff_t;
		using value_type = T;

		vec() { }
		vec(const vec& other) = default;
		vec(T value) : _value(value) { }

		template <int K>
		vec& operator=(const vec<T, N, K>& other) { _value = other[0]; };
		vec& operator=(T value) { _value = value; }

		auto begin()       { return       iterator{ reinterpret_cast<      T*>(this) }; }
		auto begin() const { return const_iterator{ reinterpret_cast<const T*>(this) }; }

		constexpr size_t size() const { return N; }

		explicit operator bool() const { return bool(_value); }
	};

	template <class T, size_t N>
	class vec<T, N, 1> : public std::array<T, N>
	{
	public:
		static_assert(N > 1, "vectors must have at least two dimensions");

		template <class... Args>
		explicit vec(Args... args) 
		{
			static_assert(sizeof...(Args) == N, "invalid number of elements"); 
			std::array<T, N>::operator=({ T(args)... });
		}

		vec() { }
		vec(const vec& other) = default;

		template <int K>
		vec& operator=(const vec<T, N, K>& other) { for (size_t i = 0; i < N; ++i) (*this)[i] = other[i]; return *this; }
		vec& operator=(T value) { for (size_t i = 0; i < N; ++i) (*this)[i] = value; return *this; }

		explicit operator bool() const { for (size_t i = 0; i < N; ++i) if (!(*this)[i]) return false; return true; }
	};

	template <class T, size_t N>       T* data(vec<T, N>& v) { return v.begin(); }
	template <class T, size_t N> const T* data(const vec<T, N>& v) { return v.begin(); }

	template <size_t Size, int Stride, size_t Offset, class T, size_t N, int K>
	auto& slice(vec<T, N, K>& v) { return reinterpret_cast<vec<T, Size, K*Stride>&>(v[Offset]); }
	template <size_t Size, int Stride, size_t Offset, class T, size_t N, int K>
	auto& slice(const vec<T, N, K>& v) { return reinterpret_cast<const vec<T, Size, K*Stride>&>(v[Offset]); }

	template <class T, size_t N, int K>
	vec<T, (N - 1), K>& rest(vec<T, N, K>& v) { return slice<(N - 1), K, 1>(v); }
	template <class T, size_t N, int K>
	const vec<T, (N - 1), K>& rest(const vec<T, N, K>& v) { return slice<(N - 1), K, 1>(v); }

	template <int K>
	bool any(const vec<bool, 2, K>& v) { return v[0] | v[1]; }
	template <size_t N, int K>
	bool any(const vec<bool, N, K>& v) { return v[0] | any(rest(v)); }
	template <int K>
	bool all(const vec<bool, 2, K>& v) { return v[0] & v[1]; }
	template <size_t N, int K>
	bool all(const vec<bool, N, K>& v) { return v[0] & all(rest(v)); }
	template <class T, int K>
	auto sum(const vec<T, 2, K>& v) { return v[0] + v[1]; }
	template <class T, size_t N, int K>
	auto sum(const vec<T, N, K>& v) { return v[0] + sum(rest(v)); }
	template <class T, int K>
	auto product(const vec<T, 2, K>& v) { return v[0] * v[1]; }
	template <class T, size_t N, int K>
	auto product(const vec<T, N, K>& v) { return v[0] * product(rest(v)); }

	TEMPLATE_VECTOR_A
	auto differences(const VECTOR_A& a)
	{
		vec<sub_result_t<A>, NA - 1> result;
		for (size_t i = 0; i < NA - 1; ++i)
			result[i] = a[i + 1] - a[i];
		return result;
	}

	template <size_t N, int K> inline auto operator!(const vec<bool, N, K>& a) { vec<bool, N> r; for (size_t i = 0; i < N; ++i) r[i] = !a[i]; return r; }

	TEMPLATE_VECTORS_AB 
	inline auto operator==(const VECTOR_A& a, const VECTOR_B& b)
	{
		require_equal<NA, NB>;
		vec<bool, NA> result;
		for (size_t i = 0; i < NA; ++i)
			result[i] = a[i] == b[i];
		return result;
	}
	TEMPLATE_VECTORS_AB
	inline auto operator!=(const VECTOR_A& a, const VECTOR_B& b) 
	{ 
		require_equal<NA, NB>;
		vec<bool, NA> result;
		for (size_t i = 0; i < NA; ++i)
			result[i] = a[i] != b[i];
		return result;
	}

	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator==(const VECTOR_A& a, B c) { vec<bool, NA> r; for (size_t i = 0; i < NA; ++i) r[i] = a[i] == c; return r; }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator!=(const VECTOR_A& a, B c) { vec<bool, NA> r; for (size_t i = 0; i < NA; ++i) r[i] = a[i] != c; return r; }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator==(B c, const VECTOR_A& a) { return a == c; }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator!=(B c, const VECTOR_A& a) { return a != c; }

	TEMPLATE_VECTORS_AB
	inline auto operator+(const VECTOR_A& a, const VECTOR_B& b)
	{
		require_equal<NA, NB>;
		vec<add_result_t<A,B>, NA> result;
		for (size_t i = 0; i < NA; ++i)
			result[i] = a[i] + b[i];
		return result;
	}
	TEMPLATE_VECTORS_AB
	inline auto operator-(const VECTOR_A& a, const VECTOR_B& b)
	{
		require_equal<NA, NB>;
		vec<sub_result_t<A,B>, NA> result;
		for (size_t i = 0; i < NA; ++i)
			result[i] = a[i] - b[i];
		return result;
	}
	TEMPLATE_VECTORS_AB
	inline auto operator*(const VECTOR_A& a, const VECTOR_B& b)
	{
		require_equal<NA, NB>;
		vec<mul_result_t<A, B>, NA> result;
		for (size_t i = 0; i < NA; ++i)
			result[i] = a[i] * b[i];
		return result;
	}

	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator+(const VECTOR_A& a, B b) { return a + AS_VECTOR_B(b); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator+(B b, const VECTOR_A& a) { return AS_VECTOR_B(a) + b; }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator-(const VECTOR_A& a, B b) { return a - AS_VECTOR_B(b); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator-(B b, const VECTOR_A& a) { return AS_VECTOR_B(b) - a; }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator*(const VECTOR_A& a, B b) { return a * AS_VECTOR_B(b); }
	TEMPLATE_VECTOR_A_SCALAR_B inline auto operator*(B b, const VECTOR_A& a) { return AS_VECTOR_B(b) * a; }

	TEMPLATE_VECTOR_A_SCALAR_B
	inline auto operator/(const VECTOR_A& a, B b)
	{
		vec<div_result_t<A, B>, NA> result;
		for (size_t i = 0; i < NA; ++i)
			result[i] = a[i] / b;
		return result;
	}

	namespace details
	{
		template <class A, class B, size_t N, int KA, int KB>
		struct binary_op
		{
			static auto dot(const vec<A, N, KA>& a, const vec<B, N, KB>& b)
			{
				return sum(a*b);
			}
		};
		template <class A, class B, int KA, int KB>
		struct binary_op<A, B, 2, KA, KB>
		{
			static auto dot(const vec<A, 2, KA>& a, const vec<B, 2, KB>& b)
			{
				return a[0] * b[0] + a[1] * b[1];
			}
			static auto cross(const vec<A, 2, KA>& a, const vec<B, 2, KB>& b)
			{
				return a[0] * b[1] - a[1] * b[0];
			}
		};
		template <class A, class B, int KA, int KB>
		struct binary_op<A, B, 3, KA, KB>
		{
			static auto dot(const vec<A, 3, KA>& a, const vec<B, 3, KB>& b)
			{
				return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
			}
			static auto cross(const vec<A, 3, KA>& a, const vec<B, 3, KB>& b)
			{
				return vec<decltype(a[0] * b[0] - a[0] * b[0]), 3> 
				{
					a[1] * b[2] - a[2] * b[1],
					a[2] * b[0] - a[0] * b[2],
					a[0] * b[1] - a[1] * b[0]
				};
			}
		};
	}

	TEMPLATE_VECTORS_AB
	inner_product_t<A, B> dot(const VECTOR_A& a, const VECTOR_B& b)
	{
		require_equal<NA, NB>;
		return details::binary_op<A, B, NA, KA, KB>::dot(a, b);
	}
	TEMPLATE_VECTORS_AB
	auto cross(const VECTOR_A& a, const VECTOR_B& b)
	{
		require_equal<NA, NB>;
		static_assert(NA == 2 || NA == 3, "cross product only defined in 3 or 2 dimensions");
		return details::binary_op<A, B, NA, KA, KB>::cross(a, b);
	}
	TEMPLATE_VECTOR_A
	auto square(const VECTOR_A& a)
	{
		add_result_t<mul_result_t<A>> result(0);
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
			decltype(a/len) direction;
			decltype(len) length;
		};

		return result_t{ a / len, len };
	}

	using float2 = vec<float, 2>;
	using float3 = vec<float, 3>;
	using float4 = vec<float, 4>;


	template <class First, class... Rest>
	inline auto vector(First first, Rest... rest) { return vec<First, sizeof...(Rest)+1>{ first, rest... }; }
	template <class T, class... Args>
	inline auto vector(Args... args) { return vector(T(args)...); }


	template <size_t N>  greater_test<N, 0> requires_x;
	template <size_t N>  greater_test<N, 1> requires_y;
	template <size_t N>  greater_test<N, 2> requires_z;
	template <size_t N>  greater_test<N, 3> requires_w;

	namespace selectors
	{
		template <class T, size_t N, int K> T& x(vec<T, N, K>& v) { requires_x<N>; return v[0]; }
		template <class T, size_t N, int K> T& y(vec<T, N, K>& v) { requires_y<N>; return v[1]; }
		template <class T, size_t N, int K> T& z(vec<T, N, K>& v) { requires_z<N>; return v[2]; }
		template <class T, size_t N, int K> T& w(vec<T, N, K>& v) { requires_w<N>; return v[3]; }

		template <class T, size_t N, int K> auto& xy(vec<T, N, K>& v) { requires_y<N>; return slice<2, +1, 0>(v); }
		template <class T, size_t N, int K> auto& yz(vec<T, N, K>& v) { requires_z<N>; return slice<2, +1, 1>(v); }
		template <class T, size_t N, int K> auto& zw(vec<T, N, K>& v) { requires_w<N>; return slice<2, +1, 2>(v); }
		template <class T, size_t N, int K> auto& wx(vec<T, N, K>& v) { requires_w<N>; return slice<2, -3, 3>(v); }

		template <class T, size_t N, int K> auto& yx(vec<T, N, K>& v) { requires_y<N>; return slice<2, -1, 1>(v); }
		template <class T, size_t N, int K> auto& zy(vec<T, N, K>& v) { requires_z<N>; return slice<2, -1, 2>(v); }
		template <class T, size_t N, int K> auto& wz(vec<T, N, K>& v) { requires_w<N>; return slice<2, -1, 3>(v); }
		template <class T, size_t N, int K> auto& xw(vec<T, N, K>& v) { requires_w<N>; return slice<2, +3, 0>(v); }

		template <class T, size_t N, int K> auto& xz(vec<T, N, K>& v) { requires_z<N>; return slice<2, +2, 0>(v); }
		template <class T, size_t N, int K> auto& yw(vec<T, N, K>& v) { requires_w<N>; return slice<2, +2, 1>(v); }
		template <class T, size_t N, int K> auto& zx(vec<T, N, K>& v) { requires_z<N>; return slice<2, -2, 2>(v); }
		template <class T, size_t N, int K> auto& wy(vec<T, N, K>& v) { requires_w<N>; return slice<2, -2, 3>(v); }

		template <class T, size_t N, int K> auto& xyz(vec<T, N, K>& v) { requires_z<N>; return slice<3, +1, 0>(v); }
		template <class T, size_t N, int K> auto& yzw(vec<T, N, K>& v) { requires_w<N>; return slice<3, +1, 1>(v); }
		template <class T, size_t N, int K> auto& zyx(vec<T, N, K>& v) { requires_z<N>; return slice<3, -1, 2>(v); }
		template <class T, size_t N, int K> auto& wzy(vec<T, N, K>& v) { requires_w<N>; return slice<3, -1, 3>(v); }

		template <class T, size_t N, int K> const T& x(const vec<T, N, K>& v) { requires_x<N>; return v[0]; }
		template <class T, size_t N, int K> const T& y(const vec<T, N, K>& v) { requires_y<N>; return v[1]; }
		template <class T, size_t N, int K> const T& z(const vec<T, N, K>& v) { requires_z<N>; return v[2]; }
		template <class T, size_t N, int K> const T& w(const vec<T, N, K>& v) { requires_w<N>; return v[3]; }

		template <class T, size_t N, int K> auto& xy(const vec<T, N, K>& v) { requires_y<N>; return slice<2, +1, 0>(v); }
		template <class T, size_t N, int K> auto& yz(const vec<T, N, K>& v) { requires_z<N>; return slice<2, +1, 1>(v); }
		template <class T, size_t N, int K> auto& zw(const vec<T, N, K>& v) { requires_w<N>; return slice<2, +1, 2>(v); }
		template <class T, size_t N, int K> auto& wx(const vec<T, N, K>& v) { requires_w<N>; return slice<2, -3, 3>(v); }

		template <class T, size_t N, int K> auto& yx(const vec<T, N, K>& v) { requires_y<N>; return slice<2, -1, 1>(v); }
		template <class T, size_t N, int K> auto& zy(const vec<T, N, K>& v) { requires_z<N>; return slice<2, -1, 2>(v); }
		template <class T, size_t N, int K> auto& wz(const vec<T, N, K>& v) { requires_w<N>; return slice<2, -1, 3>(v); }
		template <class T, size_t N, int K> auto& xw(const vec<T, N, K>& v) { requires_w<N>; return slice<2, +3, 0>(v); }

		template <class T, size_t N, int K> auto& xz(const vec<T, N, K>& v) { requires_z<N>; return slice<2, +2, 0>(v); }
		template <class T, size_t N, int K> auto& yw(const vec<T, N, K>& v) { requires_w<N>; return slice<2, +2, 1>(v); }
		template <class T, size_t N, int K> auto& zx(const vec<T, N, K>& v) { requires_z<N>; return slice<2, -2, 2>(v); }
		template <class T, size_t N, int K> auto& wy(const vec<T, N, K>& v) { requires_w<N>; return slice<2, -2, 3>(v); }

		template <class T, size_t N, int K> auto& xyz(const vec<T, N, K>& v) { requires_z<N>; return slice<3, +1, 0>(v); }
		template <class T, size_t N, int K> auto& yzw(const vec<T, N, K>& v) { requires_w<N>; return slice<3, +1, 1>(v); }
		template <class T, size_t N, int K> auto& zyx(const vec<T, N, K>& v) { requires_z<N>; return slice<3, -1, 2>(v); }
		template <class T, size_t N, int K> auto& wzy(const vec<T, N, K>& v) { requires_w<N>; return slice<3, -1, 3>(v); }
	}

	template <class T, size_t N, int K>
	std::ostream& operator<<(std::ostream& out, const vec<T, N, K>& v)
	{
		for (size_t i = 0; i < N; ++i)
			out << (i == 0 ? "[" : ", ") << v[i];
		return out << ']';
	}
}

#undef TEMPLATE_VECTOR_A
#undef TEMPLATE_VECTORS_AB
#undef TEMPLATE_VECTOR_A_SCALAR_B
#undef VECTOR_A
#undef VECTOR_B
#undef AS_VECTOR_B

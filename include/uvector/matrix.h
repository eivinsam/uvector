#pragma once

#include "vector.h"

namespace uv
{
	template <class T, size_t R, size_t C>
	class Matrix
	{
	public:
		using Row    = Vector<T, C, 1>;
		using Column = Vector<T, R, C>;
		static_assert(sizeof(Column) == sizeof(T));
	private:
		static constexpr size_t D = R < C ? R : C; // diagonal length
		std::array<Row, R> _rows;
	public:
		using scalar_type = T;
		static constexpr struct { size_t R, C; } dim = { R, C };

		Matrix() { }
		Matrix(T diagonal) { *this = diagonal; }
		template <int K>
		Matrix(const Vector<T, D, K>& diagonal) { *this = diagonal; }

		Matrix& operator=(T diagonal)
		{ 
			for (size_t i = 0; i < R; ++i)
				for (size_t j = 0; j < C; ++j)
					_rows[i][j] = i == j ? diagonal : T(0);
			return *this;
		}
		template <int K>
		Matrix& operator=(const Vector<T, D, K>& diagonal)
		{
			for (size_t i = 0; i < R; ++i)
				for (size_t j = 0; j < C; ++j)
					_rows[i][j] = i == j ? diagonal[i] : T(0);
			return *this;
		}

		auto data()       { return reinterpret_cast<      T*>(this); }
		auto data() const { return reinterpret_cast<const T*>(this); }

		explicit operator bool() const { for (size_t i = 0; i < R; ++i) if (!_rows[i]) return false; return true; }

	};

	template <class T> struct is_matrix : std::false_type { };
	template <class T> struct is_matrix<const T> : is_matrix<T> { };
	template <class T> struct is_matrix<T&>      : is_matrix<T> { };
	template <class T> 
	static constexpr bool is_matrix_v = is_matrix<T>::value;

	template <class T, size_t R, size_t C> 
	struct is_matrix<Matrix<T, R, C>> : std::true_type { };

	template <class T, class R = void> struct if_matrix : std::enable_if<is_matrix_v<T>, R> { };
	template <class T, class R = void> using if_matrix_t = typename if_matrix<T, R>::type;

	template <class T> struct Row { };
	template <class T> struct Row<const T> : Row<T> { };
	template <class T> struct Row<T&>      : Row<T> { };
	template <class T> using row = typename Row<T>::type;
	template <class T> struct Column { };
	template <class T> struct Column<const T> : Column<T> { };
	template <class T> struct Column<T&>      : Column<T> { };
	template <class T> using column = typename Column<T>::type;

	template <class T, size_t R, size_t C> struct    Row<Matrix<T, R, C>> { using type = typename Matrix<T, R, C>::Row; };
	template <class T, size_t R, size_t C> struct Column<Matrix<T, R, C>> { using type = typename Matrix<T, R, C>::Column; };

	template <class T, size_t R, size_t C> auto& diagonal(      Matrix<T, R, C>& m) { return reinterpret_cast<      Vector<T, std::min(R, C), C+1>&>(m); }
	template <class T, size_t R, size_t C> auto& diagonal(const Matrix<T, R, C>& m) { return reinterpret_cast<const Vector<T, std::min(R, C), C+1>&>(m); }

	template <class V, size_t M>
	class MatrixView
	{
		V _data[M];
	public:
		V* begin() { return _data; }
		const V* begin() const { return _data; }
		V* end() { return _data + M; }
		const V* end() const { return _data + M; }

		constexpr size_t size() const { return M; }

		auto& operator[](size_t i) { return _data[i]; }
		auto& operator[](size_t i) const { return _data[i]; }
	};

	template <class M, class = if_matrix_t<M>> auto& rows(M&& m) { return reinterpret_cast<details::copy_cr<M, MatrixView<   row<M>, dim<M>.R>>&>(m); }
	template <class M, class = if_matrix_t<M>> auto& cols(M&& m) { return reinterpret_cast<details::copy_cr<M, MatrixView<column<M>, dim<M>.C>>&>(m); }

	using float22 = Matrix<float, 2, 2>;
	using float33 = Matrix<float, 3, 3>;
	using float44 = Matrix<float, 4, 4>;

	using double22 = Matrix<double, 2, 2>;
	using double33 = Matrix<double, 3, 3>;
	using double44 = Matrix<double, 4, 4>;
}

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
	private:
		static constexpr size_t D = R < C ? R : C; // diagonal length
		std::array<Row, R> _rows;
	public:
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


	using float22 = Matrix<float, 2, 2>;
	using float33 = Matrix<float, 3, 3>;
	using float44 = Matrix<float, 4, 4>;

	using double22 = Matrix<double, 2, 2>;
	using double33 = Matrix<double, 3, 3>;
	using double44 = Matrix<double, 4, 4>;
}

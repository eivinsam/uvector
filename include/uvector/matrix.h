#pragma once

#include "vector.h"

namespace uv
{
	template <class T, size_t R, size_t C>
	class mat
	{
	public:
		using Row    = vec<T, C, 1>;
		using Column = vec<T, R, C>;
	private:
		static constexpr size_t D = R < C ? R : C; // diagonal length
		std::array<Row, R> _rows;
	public:
		mat() { }
		mat(T c)
		{
			for (size_t i = 0; i < R; ++i)
				for (size_t j = 0; j < C; ++j)
					_rows[i][j] = i == j ? c : T(0);
		}
		template <int K>
		mat(const vec<T, D, K>& diagonal)
		{
			for (size_t i = 0; i < R; ++i)
				for (size_t j = 0; j < C; ++j)
					_rows[i][j] = i == j ? diagonal[i] : T(0);
		}


		explicit operator bool() const { for (size_t i = 0; i < R; ++i) if (!row(i)) return false; return true; }
	};


	using float22 = mat<float, 2, 2>;
	using float33 = mat<float, 3, 3>;
	using float44 = mat<float, 4, 4>;

	using double22 = mat<double, 2, 2>;
	using double33 = mat<double, 3, 3>;
	using double44 = mat<double, 4, 4>;
}

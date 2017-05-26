#pragma once

#include "../matrix.h"

namespace uv
{
	namespace details
	{
		template <class M>
		void assign_rows(M&) { }

		template <class T, size_t R, size_t C, int K0, int... KN>
		void assign_rows(Matrix<T, R, C>& m, const Vector<T, C, K0>& first, const Vector<T, C, KN>&... rest)
		{
			reinterpret_cast<typename Matrix<T, R, C>::Row*>(&m)[R - (1 + sizeof...(KN))] = first;
			assign_rows(m, rest...);
		}

		template <class M>
		void assign_cols(M&) { }

		template <class T, size_t R, size_t C, int K0, int... KN>
		void assign_cols(Matrix<T, R, C>& m, const Vector<T, R, K0>& first, const Vector<T, R, KN>&... rest)
		{
			reinterpret_cast<typename Matrix<T, R, C>::Column*>(&m)[C - (1 + sizeof...(KN))] = first;
			assign_cols(m, rest...);
		}
	}
}

#pragma once

#include "../vector_operations.h"
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




		template <size_t R, size_t C>
		struct square_op
		{
			static_assert(R == C, "Matrix must be square");

			template <class T>
			static auto det(const Matrix<T, R, C>& m);
			template <class T>
			static auto inv(const Matrix<T, R, C>& m);
		};
		template <>
		struct square_op<2, 2>
		{
			template <class T>
			static auto det(const Matrix<T, 2, 2>& m)
			{
				return cross(cols(m)[0], cols(m)[1]);
			}
			template <class T>
			static auto inv(const Matrix<T, 2, 2>& m)
			{
				return rows(
					vector(rows(m)[1][1], rows(m)[1][0]),
					vector(rows(m)[0][1], rows(m)[0][0])
				) / det(m);
			}
		};
		template <>
		struct square_op<3, 3>
		{
			template <class T>
			static auto det(const Matrix<T, 3, 3>& m)
			{
				return dot(rows(m)[0], cross(rows(m)[1], rows(m)[2]));
			}
			template <class T>
			static auto inv(const Matrix<T, 3, 3>& m)
			{
				auto& mc0 = cols(m)[0];
				auto& mc1 = cols(m)[1];
				auto& mc2 = cols(m)[2];
				return rows(
					cross(mc1, mc2),
					cross(mc2, mc0),
					cross(mc0, mc1)
				) / det(m);
			}
		};

		template <class A, class B>
		auto rotate(const A& from, const B& to)
		{
			using namespace axes;
			using T = typename decltype(cross(from, to))::scalar_type;
			Matrix<T, 3, 3> result(1);
			if (square(from - to) < 0.00001f)
				return result;

			auto v = cross(from, to);
			auto coef = (1 - dot(from, to)) / square(v);
			auto sscv = cols(
				vector<T>(0, v*Z, -v*Y),
				vector<T>(-v*Z, 0, v*X),
				vector<T>(v*Y, -v*X, 0));

			result = result + sscv + (sscv*sscv)*coef;
			return result;
		}
	}
}

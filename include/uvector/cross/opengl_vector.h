#pragma once

#include <GL/GL.h>

#include "../vector.h"

namespace uv
{
	template <class T, size_t N>
	void glColoruv(const uv::Vec<T, N>& v)
	{
		if constexpr (std::is_same_v<float, T>)
		{
			if constexpr (N == 2) glColor2fv(&v[0]);
			else if constexpr (N == 3) glColor3fv(&v[0]);
			else if constexpr (N == 4) glColor4fv(&v[0]);
			else
				static_assert(false, "Dimensionality not supported");
		}
		else
			static_assert(false, "Type not supported");
	}
	template <class T, size_t N>
	void glVertexuv(const uv::Vec<T, N>& v)
	{
		if constexpr (std::is_same_v<float, T>)
		{
			if constexpr (N == 2) glVertex2fv(&v[0]);
			else if constexpr (N == 3) glVertex3fv(&v[0]);
			else if constexpr (N == 4) glVertex4fv(&v[0]);
			else
				static_assert(false, "Dimensionality not supported");
		}
		else
			static_assert(false, "Type not supported");
	}
}

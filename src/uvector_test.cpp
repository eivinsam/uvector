#include <uvector/vector.h>
#include <uvector/matrix.h>
#include <uvector/transform.h>
#include <uvector/bounds.h>
#include <uvector/complex.h>
#include <units.h>

#include <tester_with_macros.h>
#include <random>

using namespace uv::axes;

static std::mt19937 rng;

using tester::Repeat;
using tester::Subcase;

template <class T, size_t N, int K>
struct tester::Magnitude<uv::Vec<T, N, K>> { double operator()(const uv::Vec<T, N, K>& v) const { return double(length(v)); } };
template <class T, size_t N>
struct tester::Magnitude<uv::Dir<T, N>> { constexpr double operator()(const uv::Dir<T, N>&) const { return 1.0; } };
template <class T>
struct tester::Magnitude<uv::Quat<T>> { double operator()(const uv::Quat<T>& q) const { return double(length(q)); } };
template <class T>
struct tester::Magnitude<uv::Imaginary<T>> { double operator()(const uv::Imaginary<T>& i) const { return double(std::abs(i.value)); } };
template <class T>
struct tester::Magnitude<uv::Complex<T>> { double operator()(const uv::Complex<T>& c) const { return double(length(c)); } };

namespace uv
{

	namespace test
	{
		size_t fuzzing_iterations = 1000;
	}
}

float fuzzy_float()
{
	constexpr auto lo = (uint32_t(127 - 30)) << 24U;
	constexpr auto hi = (uint32_t(127 + 30)) << 24U;

	auto r = std::uniform_int_distribution<uint32_t>(lo, hi)(rng);
	r = (r >> 1) | (r << 31);
	return reinterpret_cast<float&>(r);
}

float signed_unit_float()
{
	return std::uniform_real_distribution<float>{-1.f, 1.f}(rng);
}


template <class T>
void test_selectors(T& a)
{
	auto check_selector = [](const auto& sv, const auto& v, std::initializer_list<size_t> vi)
	{
		assert(sv.size() == vi.size());
		auto vit = vi.begin();
		for (size_t k = 0; k < sv.size(); ++k, ++vit)
			CHECK(&sv[k] == &v[*vit]);
	};

	CHECK(&(a[X]) == &a[0]);
	CHECK(&(a[Y]) == &a[1]);
	CHECK(&(a[Z]) == &a[2]);
	CHECK(&(a[W]) == &a[3]);

	check_selector(a[XY], a, { 0, 1 });
	check_selector(a[YZ], a, { 1, 2 });
	check_selector(a[ZW], a, { 2, 3 });
	check_selector(a[WX], a, { 3, 0 });

	check_selector(a[YX], a, { 1, 0 });
	check_selector(a[ZY], a, { 2, 1 });
	check_selector(a[WZ], a, { 3, 2 });
	check_selector(a[XW], a, { 0, 3 });

	check_selector(a[XZ], a, { 0, 2 });
	check_selector(a[YW], a, { 1, 3 });
	check_selector(a[ZX], a, { 2, 0 });
	check_selector(a[WY], a, { 3, 1 });

	check_selector(a[X|Y|Z], a, { 0, 1, 2 });
	check_selector(a[Y|Z|W], a, { 1, 2, 3 });
	check_selector(a[Z|Y|X], a, { 2, 1, 0 });
	check_selector(a[W|Z|Y], a, { 3, 2, 1 });

	CHECK(uv::vector(a[Z], a[XY]) == a[Z | XY]);
}

template <class T>
void test_arithmetics(const T& a, const T& b, const typename T::value_type c)
{
	CHECK(a + b == uv::vector(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3]));
	CHECK(a - b == uv::vector(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3]));

	CHECK(a * c == uv::vector(a[0] * c, a[1] * c, a[2] * c, a[3] * c));
	CHECK(c * a == a * c);
	CHECK_APPROX(a / c == uv::vector(a[0] / c, a[1] / c, a[2] / c, a[3] / c));
}
template <class T>
void test_dot_product(const T& a, const T& b)
{
	CHECK_APPROX(dot(a, b) == (a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3]));
	CHECK_APPROX(dot(a[XYZ], b[XYZ]) == (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]));
	CHECK_APPROX(dot(a[XY],  b[XY])  == (a[0] * b[0] + a[1] * b[1]));
	CHECK_APPROX(dot(a[ZW],  b[ZW])  == (a[2] * b[2] + a[3] * b[3]));

}
template <class T>
void test_cross_product(const T& a, const T& b)
{
	CHECK(cross(a[XYZ], b[XYZ]) == uv::vector(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]));
	CHECK(cross(a[XY],  b[XY]) == a[0] * b[1] - a[1] * b[0]);
	CHECK(cross(a[XYZ], b[XYZ]) == uv::vector(cross(a[YZ], b[YZ]), cross(a[ZX], b[ZX]), cross(a[XY], b[XY])));
}
template <class T>
void test_decomposition(const T& a)
{
	auto d = decompose(a);
	CHECK_APPROX(d.length == sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2] + a[3] * a[3]));
	CHECK_APPROX(d.direction == uv::vector(a[0] / d.length, a[1] / d.length, a[2] / d.length, a[3] / d.length));
	CHECK_APPROX(a == d.direction * d.length);
}
template <class T, size_t N>
void test_components(const uv::Vec<T,N>& a, const T c)
{
	CHECK(a + X*c == uv::vector(a[0] + c, a[1], a[2], a[3]));
	CHECK(a + Y*c == uv::vector(a[0], a[1] + c, a[2], a[3]));
	CHECK(a + Z*c == uv::vector(a[0], a[1], a[2] + c, a[3]));
	CHECK(a + W*c == uv::vector(a[0], a[1], a[2], a[3] + c));

	uv::Mat<T, N, N> m;
	T* md = m.data();
	for (size_t i = 0; i < N; ++i)
	{
		CHECK(&rows(m)[0][i] == &(X*m)[i] == md + i*N + 0);
		CHECK(&rows(m)[1][i] == &(Y*m)[i] == md + i*N + 1);
		CHECK(&rows(m)[2][i] == &(Z*m)[i] == md + i*N + 2);
		CHECK(&rows(m)[3][i] == &(W*m)[i] == md + i*N + 3);

		CHECK(&cols(m)[0][i] == &(m*X)[i] == md + i + 0*N);
		CHECK(&cols(m)[1][i] == &(m*Y)[i] == md + i + 1*N);
		CHECK(&cols(m)[2][i] == &(m*Z)[i] == md + i + 2*N);
		CHECK(&cols(m)[3][i] == &(m*W)[i] == md + i + 3*N);
	}

	CHECK(typeid(uv::Vec<T, 2>) == typeid(X*c + Y*c));
}

template <class T, size_t N>
void test_bounds(const uv::Vec<T, N>& a, const uv::Vec<T, N>& b, const T c)
{
	const auto ab = bounds(a, b);
	const auto ac = bounds(a, c);
	const auto abc = bounds(a, b, c);

	CHECK(min(ab) == min(a, b));
	CHECK(max(ab) == max(a, b));

	CHECK(min(ac) == min(a, c));
	CHECK(max(ac) == max(a, c));

	CHECK(min(abc) == min(min(a, b), c));
	CHECK(max(abc) == max(max(a, b), c));

	CHECK(min(ab[0]) >= min(abc[0]));
}

template <class T, size_t N>
void test_matrix(const uv::Vec<T, N>& a)
{
	static_assert(N == 4);


	uv::Vec<float, N> d;
	for (auto& c : d)
		c = signed_unit_float();

	const uv::Mat<float, N, N> A = d;

	const auto Aa = A*a;

	CHECK_EACH(Aa == uv::vector(rows(A)[0][0]*a[0], rows(A)[1][1]*a[1], rows(A)[2][2]*a[2], rows(A)[3][3]*a[3]));
	CHECK_EACH(Aa == diagonal(A)*a);
	uv::Mat<float, N, N> B = 0.0f;

	CHECK_EACH(rows(B) == 0);

	for (size_t i = 0; i < N; ++i)
		rows(B)[i][i] = d[i];

	CHECK_EACH(rows(A) == rows(B));

	for (size_t i = 0; i < N; ++i)
		for (size_t j = 0; j < i; ++j)
			rows(B)[i][j] = rows(B)[j][i] = signed_unit_float();

	CHECK_EACH(rows(B) == cols(B));
	
	CHECK_EACH(rows(A*B) == rows(rows(d[0] * rows(B)[0], d[1] * rows(B)[1], d[2] * rows(B)[2], d[3] * rows(B)[3])));
}

void test_pi()
{
	tester::section = "pi";
	tester::presicion = 5e-15;

	const auto r2 = rotation(uv::pi / 2);

	CHECK(r2*X == uv::vector<double>(0, +1));
	CHECK(r2*Y == uv::vector<double>(-1, 0));

	CHECK_APPROX(r2.about(X)*X == uv::vector<double>(+1, 0, 0));
	CHECK_APPROX(r2.about(X)*Y == uv::vector<double>(0, 0, +1));
	CHECK_APPROX(r2.about(X)*Z == uv::vector<double>(0, -1, 0));

	CHECK_APPROX(r2.about(Y)*Y == uv::vector<double>(0, +1, 0));
	CHECK_APPROX(r2.about(Y)*Z == uv::vector<double>(+1, 0, 0));
	CHECK_APPROX(r2.about(Y)*X == uv::vector<double>(0, 0, -1));

	CHECK_APPROX(r2.about(Z)*Z == uv::vector<double>(0, 0, +1));
	CHECK_APPROX(r2.about(Z)*X == uv::vector<double>(0, +1, 0));
	CHECK_APPROX(r2.about(Z)*Y == uv::vector<double>(-1, 0, 0));


	for (int i = 0; i < 36; ++i)
	{
		CHECK_APPROX(cos(i*uv::pi/12) == std::cos(double(i*uv::pi/12)));
		CHECK_APPROX(sin(i*uv::pi/12) == std::sin(double(i*uv::pi/12)));
	}
}

void test_quaternion(const uv::Vec<float, 4>& v)
{
	tester::presicion = 5e-5f;
	auto a = 4 * signed_unit_float();
	auto R = uv::rotation(a).about(Z);

	CHECK_APPROX(R*X == uv::vector<float>(cos(a), sin(a), 0));
	CHECK_APPROX(R*Y == uv::vector<float>(-sin(a), cos(a), 0));
	CHECK_APPROX(R*Z == uv::vector<float>(0, 0, 1));

	static_assert(!uv::is_scalar_v<uv::Quat<float>>);
	CHECK_APPROX(uv::rotation(a).about(direction(v[XYZ])) * (v[XYZ]) == v[XYZ]);

	tester::presicion = 2e-4f;

	auto r = rotation(uv::quaternion(abs(signed_unit_float()), uv::vector(signed_unit_float(), signed_unit_float(), signed_unit_float())));
	auto rm = matrix(r);
	CHECK_APPROX(r * X == rm * X);
	CHECK_APPROX(r * Y == rm * Y);
	CHECK_APPROX(r * Z == rm * Z);
	auto rmr = rotation(rm);
	CHECK_APPROX(uv::vector(quaternion(r).re, quaternion(r).im) == uv::vector(quaternion(rmr).re, quaternion(rmr).im));
}
void test_quaternion(const uv::Vec<units::Distance<float>, 4>&)
{ 
}
void test_complex(const uv::Vec<float, 4>& v)
{
	(void)v;

	constexpr auto i = uv::imaginary;

	const auto ca = v[0] + i*v[1];
	const auto cb = v[2] + i*v[3];
	const auto cbsq = uv::square(v[2]) + uv::square(v[3]);
	CHECK(square(cb) == cbsq);

	{ const auto cr = ca + cb; CHECK(cr.re == v[0] + v[2]); CHECK(cr.im == i*(v[1] + v[3])); }
	{ const auto cr = ca - cb; CHECK(cr.re == v[0] - v[2]); CHECK(cr.im == i*(v[1] - v[3])); }
	{ const auto cr = ca * cb; CHECK(cr.re == (v[0]*v[2] - v[1]*v[3])); CHECK(cr.im == i*(v[1]*v[2] + v[0]*v[3])); }
	{ const auto cr = ca / cb; 
	CHECK_APPROX(cr.re == ((v[0]*v[2] + v[1]*v[3])/cbsq)); 
	CHECK_APPROX(cr.im == i*((v[1]*v[2] - v[0]*v[3])/cbsq)); }
}
void test_complex(const uv::Vec<units::Distance<float>, 4>&)
{
}

void test_rotate(const uv::Vec<float, 4>&)
{
	auto v = uv::vector<float>(signed_unit_float(), signed_unit_float(), signed_unit_float());
	auto ax = decompose(uv::vector<float>(1, 0, 0)).direction;
	auto vd = decompose(v).direction;
	auto R = rotation(ax, vd);

	tester::presicion = 5e-6f;
	CHECK_APPROX(square(reinterpret_cast<uv::Quat<float>&>(R)) == 1);
	CHECK_APPROX(vd == R*ax);
}
void test_rotate(const uv::Vec<units::Distance<float>, 4>&)
{

}


template <class T>
void fuzz_vectors()
{
	Repeat(uv::test::fuzzing_iterations) << []
	{
		auto a = uv::vector<T>(fuzzy_float(), fuzzy_float(), fuzzy_float(), fuzzy_float());
		const auto b = uv::vector<T>(fuzzy_float(), fuzzy_float(), fuzzy_float(), fuzzy_float());
		const T c = T(fuzzy_float());
		Subcase("selectors")       << [&] { test_selectors<uv::Vec<T, 4>>(a); };
		Subcase("const selectors") << [&] { test_selectors<const uv::Vec<T, 4>>(a); };
		Subcase("arithmetics")     << [&] { test_arithmetics(a, b, c); };
		Subcase("dot product")   << [&] { test_dot_product(a, b); };
		Subcase("cross product") << [&] { test_cross_product(a, b); };
		Subcase("decomposition") << [&] { test_decomposition(a); };
		Subcase("components")    << [&] { test_components(a, c); };
		Subcase("bounds")     << [&] { test_bounds(a, b, c); };
		Subcase("matrix")     << [&] { test_matrix(a); };
		Subcase("complex")    << [&] { test_complex(a); };
		Subcase("quaternion") << [&] { test_quaternion(a); };
		Subcase("rotate")     << [&] { test_rotate(a); };
	};
}

TEST_CASE("uvector")
{
	tester::presicion = tester::default_float_presicion;

	Subcase("logic") << []
	{
		for (int x = 0; x < 16; ++x)
		{
			const auto boolv = uv::vector(x & 1, x & 2, x & 4, x & 8) != 0;
			CHECK(all(boolv) == (x == 0b1111));
			CHECK(any(boolv) == (x != 0));
		}
	};

	Subcase("constants") << []
	{
		test_pi();
	};

	Subcase("float") << fuzz_vectors<float>;
	Subcase("Distance") << fuzz_vectors<units::Distance<float>>;
};

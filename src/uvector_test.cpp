#include <uvector/vector_operations.h>
#include <uvector/component_operations.h>
#include <uvector/matrix_operations.h>
#include <units.h>


#include <tester_with_macros.h>
#include <random>

using namespace uv;
using namespace uv::selectors;

static std::mt19937 rng;

namespace uv
{
	namespace test
	{
		size_t fuzzing_iterations = 1000;
	}
}

float fuzzy_float()
{
	constexpr auto lo = (uint32_t(127 - 60)) << 24U;
	constexpr auto hi = (uint32_t(127 + 60)) << 24U;

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
	tester::section = "selectors";

	CHECK(&x(a) == &a[0]);
	CHECK(&y(a) == &a[1]);
	CHECK(&z(a) == &a[2]);
	CHECK(&w(a) == &a[3]);

	check_selector(xy(a), a, { 0, 1 });
	check_selector(yz(a), a, { 1, 2 });
	check_selector(zw(a), a, { 2, 3 });
	check_selector(wx(a), a, { 3, 0 });

	check_selector(yx(a), a, { 1, 0 });
	check_selector(zy(a), a, { 2, 1 });
	check_selector(wz(a), a, { 3, 2 });
	check_selector(xw(a), a, { 0, 3 });

	check_selector(xz(a), a, { 0, 2 });
	check_selector(yw(a), a, { 1, 3 });
	check_selector(zx(a), a, { 2, 0 });
	check_selector(wy(a), a, { 3, 1 });

	check_selector(xyz(a), a, { 0, 1, 2 });
	check_selector(yzw(a), a, { 1, 2, 3 });
	check_selector(zyx(a), a, { 2, 1, 0 });
	check_selector(wzy(a), a, { 3, 2, 1 });
}

template <class T>
void test_arithmetics(const T& a, const T& b, const typename T::value_type c)
{
	tester::section = "arithmetics";

	CHECK_EACH(a + b == vector(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3]));
	CHECK_EACH(a - b == vector(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3]));
	CHECK_EACH(a * c == vector(a[0] * c, a[1] * c, a[2] * c, a[3] * c));
	CHECK_EACH(c * a == a * c);
	CHECK_EACH(a / c == vector(a[0] / c, a[1] / c, a[2] / c, a[3] / c));
}
template <class T>
void test_dot_product(const T& a, const T& b)
{
	tester::section = "dot product";
	
	CHECK_APPROX(dot(a, b) == (a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3]));
	CHECK_APPROX(dot(xyz(a), xyz(b)) == (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]));
	CHECK_APPROX(dot(xy(a), xy(b)) == (a[0] * b[0] + a[1] * b[1]));
	CHECK_APPROX(dot(zw(a), zw(b)) == (a[2] * b[2] + a[3] * b[3]));

}
template <class T>
void test_cross_product(const T& a, const T& b)
{
	tester::section = "cross product";

	CHECK_EACH(cross(xyz(a), xyz(b)) == vector(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]));
	CHECK(cross(xy(a), xy(b)) == a[0] * b[1] - a[1] * b[0]);
	CHECK_EACH(cross(xyz(a), xyz(b)) == vector(cross(yz(a), yz(b)), cross(zx(a), zx(b)), cross(xy(a), xy(b))));
}
template <class T>
void test_decomposition(const T& a)
{
	using V = typename T::value_type;

	tester::section = "decomposition";

	auto d = decompose(a);
	CHECK(d.length == sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2] + a[3] * a[3]));
	CHECK_EACH(d.direction == vector(a[0] / d.length, a[1] / d.length, a[2] / d.length, a[3] / d.length));
	CHECK_EACH_APPROX(a == d.direction * d.length);
}
template <class T, size_t N>
void test_components(const Vector<T,N>& a, const T c)
{
	tester::section = "components";
	
	using namespace uv::components;
	CHECK_EACH(a + X*c == vector(a[0] + c, a[1], a[2], a[3]));
	CHECK_EACH(a + Y*c == vector(a[0], a[1] + c, a[2], a[3]));
	CHECK_EACH(a + Z*c == vector(a[0], a[1], a[2] + c, a[3]));
	CHECK_EACH(a + W*c == vector(a[0], a[1], a[2], a[3] + c));

	Matrix<T, N, N> m;
	T* md = m.data();
	for (size_t i = 0; i < N; ++i)
	{
		CHECK(&rows(m)[0][i] == &(X*m)[i] == md + i + 0*N);
		CHECK(&rows(m)[1][i] == &(Y*m)[i] == md + i + 1*N);
		CHECK(&rows(m)[2][i] == &(Z*m)[i] == md + i + 2*N);
		CHECK(&rows(m)[3][i] == &(W*m)[i] == md + i + 3*N);

		CHECK(&cols(m)[0][i] == &(m*X)[i] == md + i*N + 0);
		CHECK(&cols(m)[1][i] == &(m*Y)[i] == md + i*N + 1);
		CHECK(&cols(m)[2][i] == &(m*Z)[i] == md + i*N + 2);
		CHECK(&cols(m)[3][i] == &(m*W)[i] == md + i*N + 3);
	}

	CHECK(typeid(Vector<T, 2>) == typeid(X*c + Y*c));
}

template <class T, size_t N>
void test_matrix(const Vector<T, N>& a)
{
	static_assert(N == 4);
	using U = decltype(T()/T());
	tester::section = "matrix";

	Vector<U, N> d;
	for (auto& c : d)
		c = U(signed_unit_float());

	const Matrix<U, N, N> A = d;

	CHECK_EACH(A*a == vector(rows(A)[0][0]*a[0], rows(A)[1][1]*a[1], rows(A)[2][2]*a[2], rows(A)[3][3]*a[3]));
	CHECK_EACH(A*a == diagonal(A)*a);
	Matrix<U, N, N> B = U(0);

	CHECK_EACH(rows(B) == U(0));

	for (size_t i = 0; i < N; ++i)
		rows(B)[i][i] = d[i];

	CHECK_EACH(rows(A) == rows(B));

	for (size_t i = 0; i < N; ++i)
		for (size_t j = 0; j < i; ++j)
			rows(B)[i][j] = rows(B)[j][i] = U(signed_unit_float());

	CHECK_EACH(rows(B) == cols(B));
	
	CHECK_EACH(rows(A*B) == rows(rows(d[0] * rows(B)[0], d[1] * rows(B)[1], d[2] * rows(B)[2], d[3] * rows(B)[3])));
}

template <class T>
void fuzz_vectors()
{
	for (auto i : tester::Repeat(test::fuzzing_iterations))
	{
		i;
		auto a = vector<T>(fuzzy_float(), fuzzy_float(), fuzzy_float(), fuzzy_float());
		const auto b = vector<T>(fuzzy_float(), fuzzy_float(), fuzzy_float(), fuzzy_float());
		const T c = T(fuzzy_float());
		test_selectors<Vector<T, 4>>(a);
		test_selectors<const Vector<T, 4>>(a);
		test_arithmetics(a, b, c);
		test_dot_product(a, b);
		test_cross_product(a, b);
		test_decomposition(a);
		test_components(a, c);
		test_matrix(a);
	}
}


namespace units_vector_test
{
	using Distancef = units::Distance<float>;
	using Areaf = units::Area<float>;
	static_assert(std::is_same_v<Distancef, decltype(vector<Distancef>(1, 2) + vector<Distancef>(3, 4))::value_type>, "vector addition should have the same output type");
	static_assert(std::is_same_v<Areaf, decltype(dot(vector<Distancef>(1, 2), vector<Distancef>(3, 4)))>, "vector dot product should produce squared output type");
}

TEST_CASE("uvector")
{
	tester::presicion = tester::default_float_presicion;

	SUBCASE("logic")
	{
		for (int x = 0; x < 16; ++x)
		{
			const auto boolv = vector((x & 1) != 0, (x & 2) != 0, (x & 4) != 0, (x & 8) != 0);
			CHECK(all(boolv) == (x == 0b1111));
			CHECK(any(boolv) == (x != 0));
		}
	}

	SUBCASE("float")
		fuzz_vectors<float>();
	SUBCASE("unit::Distance<float>")
		fuzz_vectors<units::Distance<float>>();
};

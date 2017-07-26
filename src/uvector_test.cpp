#include <uvector/vector_operations.h>
#include <uvector/matrix_operations.h>
#include <uvector/quaternion_operations.h>
#include <uvector/bounds_operations.h>
#include <units.h>

#include <tester_with_macros.h>
#include <random>

using namespace uv;
using namespace uv::axes;

static std::mt19937 rng;

template <class T, size_t N, int K>
struct tester::Magnitude<Vector<T, N, K>> { double operator()(const Vector<T, N, K>& v) const { return double(length(v)); } };

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

	CHECK(&(a*X) == &a[0]);
	CHECK(&(a*Y) == &a[1]);
	CHECK(&(a*Z) == &a[2]);
	CHECK(&(a*W) == &a[3]);

	check_selector(a*XY, a, { 0, 1 });
	check_selector(a*YZ, a, { 1, 2 });
	check_selector(a*ZW, a, { 2, 3 });
	check_selector(a*WX, a, { 3, 0 });

	check_selector(a*YX, a, { 1, 0 });
	check_selector(a*ZY, a, { 2, 1 });
	check_selector(a*WZ, a, { 3, 2 });
	check_selector(a*XW, a, { 0, 3 });

	check_selector(a*XZ, a, { 0, 2 });
	check_selector(a*YW, a, { 1, 3 });
	check_selector(a*ZX, a, { 2, 0 });
	check_selector(a*WY, a, { 3, 1 });

	check_selector(a*(X+Y+Z), a, { 0, 1, 2 });
	check_selector(a*(Y+Z+W), a, { 1, 2, 3 });
	check_selector(a*(Z+Y+X), a, { 2, 1, 0 });
	check_selector(a*(W+Z+Y), a, { 3, 2, 1 });
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
	CHECK_APPROX(dot(a*XYZ, b*XYZ) == (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]));
	CHECK_APPROX(dot(a*XY,  b*XY)    == (a[0] * b[0] + a[1] * b[1]));
	CHECK_APPROX(dot(a*ZW,  b*ZW) == (a[2] * b[2] + a[3] * b[3]));

}
template <class T>
void test_cross_product(const T& a, const T& b)
{
	tester::section = "cross product";

	CHECK_EACH(cross(a*XYZ, b*XYZ) == vector(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]));
	CHECK(cross(a*XY, b*XY) == a[0] * b[1] - a[1] * b[0]);
	CHECK_EACH(cross(a*XYZ, b*XYZ) == vector(cross(a*YZ, b*YZ), cross(a*ZX, b*ZX), cross(a*XY, b*XY)));
}
template <class T>
void test_decomposition(const T& a)
{
	using V = typename T::value_type;

	tester::section = "decomposition";

	auto d = decompose(a);
	CHECK_APPROX(d.length == sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2] + a[3] * a[3]));
	CHECK_EACH(d.direction == vector(a[0] / d.length, a[1] / d.length, a[2] / d.length, a[3] / d.length));
	CHECK_EACH_APPROX(a == d.direction * d.length);
}
template <class T, size_t N>
void test_components(const Vector<T,N>& a, const T c)
{
	tester::section = "components";
	
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
void test_bounds(const Vector<T, N>& a, const Vector<T, N>& b, const T c)
{
	tester::section = "bounds";

	const auto ab = bounds(a, b);
	const auto ac = bounds(a, c);
	const auto abc = bounds(a, b, c);

	CHECK_EACH(min(ab) == uv::min(a, b));
	CHECK_EACH(max(ab) == uv::max(a, b));

	CHECK_EACH(min(ac) == uv::min(a, c));
	CHECK_EACH(max(ac) == uv::max(a, c));

	CHECK_EACH(min(abc) == uv::min(uv::min(a, b), c));
	CHECK_EACH(max(abc) == uv::max(uv::max(a, b), c));
}

template <class T, size_t N>
void test_matrix(const Vector<T, N>& a)
{
	static_assert(N == 4);
	tester::section = "matrix";

	Vector<float, N> d;
	for (auto& c : d)
		c = signed_unit_float();

	const Matrix<float, N, N> A = d;

	CHECK_EACH(A*a == vector(rows(A)[0][0]*a[0], rows(A)[1][1]*a[1], rows(A)[2][2]*a[2], rows(A)[3][3]*a[3]));
	CHECK_EACH(A*a == diagonal(A)*a);
	Matrix<float, N, N> B = 0;

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

void test_quaternion(const Vector<float, 4>& v)
{
	tester::section = "quaternion";
	using namespace axes;
	auto a = 1.0f;
	auto R = rotate<float>(a, Z);

	CHECK_EACH_APPROX(R*X == vector<float>(cos(a), sin(a), 0));
	CHECK_EACH_APPROX(R*Y == vector<float>(-sin(a), cos(a), 0));
	CHECK_EACH(R*Z == vector<float>(0, 0, 1));

	CHECK_APPROX(rotate(a, v*XYZ) * (v*XYZ) == v*XYZ);

	auto old_p = tester::presicion;
	tester::presicion = 2e-4f;

	auto q = quaternion(signed_unit_float(), signed_unit_float(), signed_unit_float(), signed_unit_float());
	if (real(q) < 0)
		q = -q;
	auto qm = matrix(q);
	CHECK_APPROX(q * X == qm * X);
	CHECK_APPROX(q * Y == qm * Y);
	CHECK_APPROX(q * Z == qm * Z);
	auto qmq = quaternion(qm);
	CHECK_APPROX(q.v == qmq.v);

	tester::presicion = old_p;
}
void test_quaternion(const Vector<units::Distance<float>, 4>&)
{ 
}

void test_rotate(const Vector<float, 4>&)
{
	auto v = vector<float>(signed_unit_float(), signed_unit_float(), signed_unit_float());
	auto ax = decompose(vector<float>(1, 0, 0)).direction;
	tester::section = "rotate";
	auto vd = decompose(v).direction;
	auto R = rotate(ax, vd);
	CHECK_APPROX(det(R) == 1);
	CHECK_APPROX(vd == R*ax);

	CHECK_EACH(cols(R) == cols(rotate(X, vd)));
	CHECK_EACH_APPROX(cols(R) == cols(transpose(rotate(vd, X))));
}
void test_rotate(const Vector<units::Distance<float>, 4>&)
{

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
		test_bounds(a, b, c);
		test_matrix(a);
		test_quaternion(a);
		test_rotate(a);
	}
}

template <class T>
void test_type_tests()
{
	static_assert(is_scalar_v<T> == true);
	static_assert(is_scalar_v<const T&> == true);
	static_assert(is_scalar_v<std::vector<T>> == false);
	static_assert(is_vector_v<T> == false);
	static_assert(is_matrix_v<T> == false);

	using V = Vector<T, 3>;
	static_assert(is_scalar_v<V> == false);
	static_assert(is_vector_v<V> == true);
	static_assert(is_vector_v<const V&> == true);
	static_assert(is_vector_v<std::vector<V>> == false);
	static_assert(is_matrix_v<V> == false);

	using M = Matrix<T, 3, 3>;
	static_assert(is_scalar_v<M> == false);
	static_assert(is_vector_v<M> == false);
	static_assert(is_matrix_v<M> == true);
	static_assert(is_matrix_v<const M&> == true);
	static_assert(is_matrix_v<std::vector<M>> == false);
}

//static_assert(!is_vector_v<units::Distance<float>>);

TEST_CASE("uvector")
{
	tester::presicion = tester::default_float_presicion;

	SUBCASE("logic")
	{
		for (int x = 0; x < 16; ++x)
		{
			const auto boolv = vector(x & 1, x & 2, x & 4, x & 8) != 0;
			CHECK(all(boolv) == (x == 0b1111));
			CHECK(any(boolv) == (x != 0));
		}
	}

	SUBCASE("float")
	{
		test_type_tests<float>();
		fuzz_vectors<float>();
	}
	SUBCASE("Distance")
	{
		//test_type_tests<units::Distance<float>>();
		fuzz_vectors<units::Distance<float>>();
	}
};

// Sanitizer/self-check target: exercises the vector headers so the ASan build
// (scripts/check.ps1 -Asan) has something real to link and run.
#include "utils/Matrix3.hpp"
#include "utils/Matrix4.hpp"
#include "utils/Vector2.hpp"
#include "utils/Vector3.hpp"
#include "utils/Vector4.hpp"

#include <cmath>
#include <cstdio>
#include <limits>

namespace {

int failures = 0;

void check(bool cond, const char* what){
  if (!cond) {
    std::printf("FAIL: %s\n", what);
    ++failures;
  }
}

bool finite(const Vec3& v){
  return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
}

} // namespace

int main(){
  const Vec3 a{3.0f, 4.0f, 0.0f};
  const Vec3 b{1.0f, 0.0f, 0.0f};

  check(a.len() == 5.0f, "Vec3::len");
  check(a.normalized().approx_equal(Vec3{0.6f, 0.8f, 0.0f}), "Vec3::normalized");
  check(a.dot(b) == 3.0f, "Vec3::dot");
  check(a.cross(b).approx_equal(Vec3{0.0f, -0.0f, -4.0f}), "Vec3::cross");
  check((a * 2.0f).x == 6.0f, "Vec3 scalar multiply");
  check((2.0f * a).x == 6.0f, "float * Vec3");
  check(a.safe_divide(Vec3{}, 0.0f).approx_equal(Vec3{}), "Vec3 divide by zero is finite");
  check(finite(a.safe_divide(0.0f, 0.0f)), "Vec3 divide by zero scalar is finite");
  check(Vec3{}.normalized().approx_equal(Vec3{}), "zero vector normalizes to zero");
  check(Vec3{1e-30f, 0.0f, 0.0f}.normalized() == Vec3{1e-30f, 0.0f, 0.0f}, "near-zero vector stays unchanged");
  check(-a == Vec3{-3.0f, -4.0f, 0.0f}, "Vec3 unary minus");
  check(a != b, "Vec3 exact inequality");

  const Vec2 p{3.0f, 4.0f};
  const Vec2 q{1.0f, 0.0f};

  check(p.len() == 5.0f, "Vec2::len");
  check(p.normalized().approx_equal(Vec2{0.6f, 0.8f}), "Vec2::normalized");
  check(p.cross(q) == -4.0f, "Vec2::cross");
  check(p.safe_divide(Vec2{}, 0.0f).approx_equal(Vec2{}), "Vec2 divide by zero is finite");
  check(p.dot(q) == 3.0f, "Vec2::dot");

  const Vec4 h{1.0f, 2.0f, 2.0f, 4.0f};
  const Vec4 k{1.0f, 0.0f, 0.0f, 0.0f};

  check(h.len() == 5.0f, "Vec4::len");
  check(h.dot(k) == 1.0f, "Vec4::dot");
  check((h * 2.0f).w == 8.0f, "Vec4 scalar multiply");
  check((2.0f * h).w == 8.0f, "float * Vec4");
  check((-h).w == -4.0f, "Vec4 unary minus");
  check(h.safe_divide(Vec4{}, 0.0f).approx_equal(Vec4{}), "Vec4 divide by zero is finite");
  check(std::abs(h.normalized().len() - 1.0f) < 0.0001f, "Vec4::normalized is unit length");
  check(Vec4{}.normalized() == Vec4{}, "zero Vec4 normalizes to zero");
  check(Vec4{0.00001f, 1.0f, 2.0f, 3.0f}.zero_clamp(0.0001f).x == 0.0f, "Vec4::zero_clamp");
  check(h != k, "Vec4 exact inequality");

  check((Vec2{1e-5f, 2e-5f} / 1e-5f).approx_equal(Vec2{1.0f, 2.0f}),
        "division keeps small nonzero divisors");
  check(a.safe_divide(Vec3{1e-5f, 2.0f, 0.0f}, 1e-5f) == Vec3{0.0f, 2.0f, 0.0f},
        "guarded division applies the supplied cutoff per component");
  check(std::isfinite(Vec2{1e20f, 0.0f}.len()), "representable length survives large components");
  check(Vec3{1e20f, 0.0f, 0.0f}.normalized() == Vec3{1.0f, 0.0f, 0.0f},
        "large finite vector normalizes to unit length");
  const float max = std::numeric_limits<float>::max();
  check(std::abs(Vec4{max, max, max, max}.normalized().len() - 1.0f) < 0.0001f,
        "normalization works even when the length exceeds float range");

  const float half_pi = 1.57079632679f;

  const Mat4 id = Mat4::identity();
  const Mat4 t  = Mat4::translation(1.0f, 2.0f, 3.0f);
  const Mat4 s  = Mat4::scale(2.0f, 3.0f, 4.0f);

  check((id * t) == t, "identity * M == M");
  check((t * id) == t, "M * identity == M");
  check(t.transform_point(Vec3{}).approx_equal(Vec3{1.0f, 2.0f, 3.0f}), "translation moves points");
  check(t.transform_direction(Vec3{1.0f, 1.0f, 1.0f}).approx_equal(Vec3{1.0f, 1.0f, 1.0f}),
        "translation ignores directions");
  check(s.transform_point(Vec3{1.0f, 1.0f, 1.0f}).approx_equal(Vec3{2.0f, 3.0f, 4.0f}), "scale");
  check((t * s).transform_point(Vec3{1.0f, 1.0f, 1.0f}).approx_equal(Vec3{3.0f, 5.0f, 7.0f}),
        "T * S applies the scale first");
  check((t * s).transposed().approx_equal(s.transposed() * t.transposed()), "(AB)^T == B^T A^T");
  check(Mat4::rotationX(half_pi).transform_point(Vec3{0.0f, 1.0f, 0.0f}).approx_equal(Vec3{0.0f, 0.0f, 1.0f}),
        "rotationX");
  check(Mat4::rotationY(half_pi).transform_point(Vec3{1.0f, 0.0f, 0.0f}).approx_equal(Vec3{0.0f, 0.0f, -1.0f}),
        "rotationY");
  check(Mat4::rotationZ(half_pi).transform_point(Vec3{1.0f, 0.0f, 0.0f}).approx_equal(Vec3{0.0f, 1.0f, 0.0f}),
        "rotationZ");
  check(Mat4::rotationZ(half_pi).transform_direction(Vec3{0.0f, 0.0f, 1.0f})
          .approx_equal(Vec3{0.0f, 0.0f, 1.0f}), "rotation about Z keeps the Z axis");
  check((Mat4::rotationZ(0.7f) * Mat4::rotationZ(0.7f).transposed()).approx_equal(id),
        "rotation matrices are orthonormal");

  Mat4 edited = {};
  edited(0, 3) = 7.0f;
  check(edited(0, 3) == 7.0f, "operator() reads/writes row-major");
  check(edited.m[3] == 7.0f, "operator() matches m[] layout");

  const Mat3 id3 = Mat3::identity();
  const Mat3 s3  = Mat3::scale(2.0f, 3.0f, 4.0f);

  check((id3 * s3) == s3, "Mat3 identity * M == M");
  check((s3 * id3) == s3, "Mat3 M * identity == M");
  check(s3.transform(Vec3{1.0f, 1.0f, 1.0f}).approx_equal(Vec3{2.0f, 3.0f, 4.0f}), "Mat3 scale");
  check(Mat3::rotationX(half_pi).transform(Vec3{0.0f, 1.0f, 0.0f}).approx_equal(Vec3{0.0f, 0.0f, 1.0f}),
        "Mat3 rotationX");
  check(Mat3::rotationY(half_pi).transform(Vec3{1.0f, 0.0f, 0.0f}).approx_equal(Vec3{0.0f, 0.0f, -1.0f}),
        "Mat3 rotationY");
  check(Mat3::rotationZ(half_pi).transform(Vec3{1.0f, 0.0f, 0.0f}).approx_equal(Vec3{0.0f, 1.0f, 0.0f}),
        "Mat3 rotationZ");
  check((Mat3::rotationX(0.7f) * Mat3::rotationX(0.7f).transposed()).approx_equal(id3),
        "Mat3 rotations are orthonormal");
  check(s3.transposed().transposed() == s3, "Mat3 double transpose is the identity operation");
  check((s3 * Mat3::rotationZ(0.3f)).transposed().approx_equal(
          Mat3::rotationZ(0.3f).transposed() * s3.transposed()), "Mat3 (AB)^T == B^T A^T");
  check(id3.determinant() == 1.0f, "Mat3 determinant of identity");
  check(std::abs(s3.determinant() - 24.0f) < 0.0001f, "Mat3 determinant of scale");
  check(std::abs(Mat3::rotationY(0.9f).determinant() - 1.0f) < 0.0001f,
        "Mat3 determinant of a rotation is 1");

  const float nan = std::numeric_limits<float>::quiet_NaN();
  Mat3 nan3 = id3;
  Mat4 nan4 = id;
  nan3(0, 0) = nan;
  nan4(0, 0) = nan;
  check(!nan3.approx_equal(id3), "Mat3 rejects NaN in approximate comparison");
  check(!nan4.approx_equal(id), "Mat4 rejects NaN in approximate comparison");

  if (failures != 0) {
    std::printf("%d check(s) failed\n", failures);
    return 1;
  }
  std::printf("smoke: all checks passed\n");
  return 0;
}

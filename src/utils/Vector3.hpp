#pragma once
#include <cmath>

struct Vec3{
  float x;
  float y;
  float z;

  [[nodiscard]] Vec3 operator+(const Vec3& v) const{
    return {x + v.x, y + v.y, z + v.z};
  }

  [[nodiscard]] Vec3 operator-(const Vec3& v) const{
    return {x - v.x, y - v.y, z - v.z};
  }

  [[nodiscard]] Vec3 operator-() const{
    return {-x, -y, -z};
  }

  // element-wise product; use dot() for the scalar product
  [[nodiscard]] Vec3 operator*(const Vec3& v) const{
    return {x * v.x, y * v.y, z * v.z};
  }

  [[nodiscard]] Vec3 operator*(float s) const{
    return {x * s, y * s, z * s};
  }

  [[nodiscard]] friend Vec3 operator*(float s, const Vec3& v){
    return {s * v.x, s * v.y, s * v.z};
  }

  // component-wise divide; components whose divisor is ~0 yield 0 instead of
  // inf/NaN, so a degenerate divisor can never propagate
  [[nodiscard]] Vec3 operator/(const Vec3& v) const{
    return {safe_div(x, v.x), safe_div(y, v.y), safe_div(z, v.z)};
  }

  [[nodiscard]] Vec3 operator/(float s) const{
    return {safe_div(x, s), safe_div(y, s), safe_div(z, s)};
  }

  // exact comparison; use approx_equal() for a tolerance test
  [[nodiscard]] bool operator==(const Vec3& v) const{
    return x == v.x && y == v.y && z == v.z;
  }

  [[nodiscard]] bool approx_equal(const Vec3& v, float eps = 0.0001f) const{
    return std::abs(x - v.x) < eps
        && std::abs(y - v.y) < eps
        && std::abs(z - v.z) < eps;
  }

  [[nodiscard]] float dot(const Vec3& v) const{
    return x * v.x + y * v.y + z * v.z;
  }

  [[nodiscard]] Vec3 cross(const Vec3& v) const{
    return {
      y * v.z - z * v.y,
      z * v.x - x * v.z,
      x * v.y - y * v.x
    };
  }

  [[nodiscard]] float len() const{
    return std::sqrt(dot(*this));
  }

  // returns *this unchanged when the length is not above eps, so a zero (or
  // near-zero) vector never produces inf/NaN
  [[nodiscard]] Vec3 normalized(float eps = 0.00000001f) const{
    const float l = len();
    return l > eps ? *this * (1.0f / l) : *this;
  }

  static float safe_div(float num, float den, float eps = 0.0001f){
    return std::abs(den) > eps ? num / den : 0.0f;
  }

  static float apply_deadzone(float value, float eps){
    return (std::abs(value) < eps) ? 0.0f : value;
  }

  // zeroes components with magnitude below eps
  [[nodiscard]] Vec3 zero_clamp(float eps) const{
    Vec3 result{};

    result.x = apply_deadzone(x, eps);
    result.y = apply_deadzone(y, eps);
    result.z = apply_deadzone(z, eps);
    return result;
  }
};

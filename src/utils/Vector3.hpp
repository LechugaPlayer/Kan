#pragma once
#include <cmath>
#include <limits>

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

  [[nodiscard]] Vec3 operator/(const Vec3& v) const{
    return {x / v.x, y / v.y, z / v.z};
  }

  [[nodiscard]] Vec3 operator/(float s) const{
    return {x / s, y / s, z / s};
  }

  // Zeroes components whose divisor magnitude is at most eps.
  // The caller supplies a nonnegative tolerance.
  [[nodiscard]] Vec3 safe_divide(const Vec3& v, float eps) const{
    return {safe_div(x, v.x, eps), safe_div(y, v.y, eps), safe_div(z, v.z, eps)};
  }

  [[nodiscard]] Vec3 safe_divide(float s, float eps) const{
    return std::abs(s) > eps ? Vec3{x / s, y / s, z / s} : Vec3{};
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
    const double l = wide_length();
    return l > static_cast<double>(std::numeric_limits<float>::max())
        ? std::numeric_limits<float>::infinity()
        : static_cast<float>(l);
  }

  // returns *this unchanged when the length is not above eps, so a zero (or
  // near-zero) vector never produces inf/NaN
  [[nodiscard]] Vec3 normalized(float eps = 0.00000001f) const{
    const double l = wide_length();
    if (!(l > static_cast<double>(eps))) {
      return *this;
    }
    return {
      static_cast<float>(static_cast<double>(x) / l),
      static_cast<float>(static_cast<double>(y) / l),
      static_cast<float>(static_cast<double>(z) / l)
    };
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

private:
  [[nodiscard]] double wide_length() const{
    const double dx = static_cast<double>(x);
    const double dy = static_cast<double>(y);
    const double dz = static_cast<double>(z);
    return std::sqrt(dx * dx + dy * dy + dz * dz);
  }
};

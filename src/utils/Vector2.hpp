#pragma once
#include <cmath>
#include <limits>

struct Vec2{
  float x;
  float y;

  [[nodiscard]] Vec2 operator+(const Vec2& v) const{
    return {x + v.x, y + v.y};
  }

  [[nodiscard]] Vec2 operator-(const Vec2& v) const{
    return {x - v.x, y - v.y};
  }

  [[nodiscard]] Vec2 operator-() const{
    return {-x, -y};
  }

  // element-wise product; use dot() for the scalar product
  [[nodiscard]] Vec2 operator*(const Vec2& v) const{
    return {x * v.x, y * v.y};
  }

  [[nodiscard]] Vec2 operator*(float s) const{
    return {x * s, y * s};
  }

  [[nodiscard]] friend Vec2 operator*(float s, const Vec2& v){
    return v * s;
  }

  [[nodiscard]] Vec2 operator/(const Vec2& v) const{
    return {x / v.x, y / v.y};
  }

  [[nodiscard]] Vec2 operator/(float s) const{
    return {x / s, y / s};
  }

  // Zeroes components whose divisor magnitude is at most eps.
  // The caller supplies a nonnegative tolerance.
  [[nodiscard]] Vec2 safe_divide(const Vec2& v, float eps) const{
    return {safe_div(x, v.x, eps), safe_div(y, v.y, eps)};
  }

  [[nodiscard]] Vec2 safe_divide(float s, float eps) const{
    return std::abs(s) > eps ? Vec2{x / s, y / s} : Vec2{};
  }

  // exact comparison; use approx_equal() for a tolerance test
  [[nodiscard]] bool operator==(const Vec2& v) const{
    return x == v.x && y == v.y;
  }

  [[nodiscard]] bool approx_equal(const Vec2& v, float eps = 0.0001f) const{
    return std::abs(x - v.x) < eps
        && std::abs(y - v.y) < eps;
  }

  [[nodiscard]] float dot(const Vec2& v) const{
    return x * v.x + y * v.y;
  }

  // scalar z component of the 3D cross product
  [[nodiscard]] float cross(const Vec2& v) const{
    return x * v.y - y * v.x;
  }

  [[nodiscard]] float len() const{
    const double l = wide_length();
    return l > static_cast<double>(std::numeric_limits<float>::max())
        ? std::numeric_limits<float>::infinity()
        : static_cast<float>(l);
  }

  // returns *this unchanged when the length is not above eps, so a zero (or
  // near-zero) vector never produces inf/NaN
  [[nodiscard]] Vec2 normalized(float eps = 0.00000001f) const{
    const double l = wide_length();
    if (!(l > static_cast<double>(eps))) {
      return *this;
    }
    return {
      static_cast<float>(static_cast<double>(x) / l),
      static_cast<float>(static_cast<double>(y) / l)
    };
  }

  static float safe_div(float num, float den, float eps = 0.0001f){
    return std::abs(den) > eps ? num / den : 0.0f;
  }

  static float apply_deadzone(float value, float eps){
    return (std::abs(value) < eps) ? 0.0f : value;
  }

  // zeroes components with magnitude below eps
  [[nodiscard]] Vec2 zero_clamp(float eps) const{
    Vec2 result{};

    result.x = apply_deadzone(x, eps);
    result.y = apply_deadzone(y, eps);
    return result;
  }

private:
  [[nodiscard]] double wide_length() const{
    const double dx = static_cast<double>(x);
    const double dy = static_cast<double>(y);
    return std::sqrt(dx * dx + dy * dy);
  }
};

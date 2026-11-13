#pragma once
#include <cmath>

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

  // component-wise divide; components whose divisor is ~0 yield 0 instead of
  // inf/NaN, so a degenerate divisor can never propagate
  [[nodiscard]] Vec2 operator/(const Vec2& v) const{
    return {safe_div(x, v.x), safe_div(y, v.y)};
  }

  [[nodiscard]] Vec2 operator/(float s) const{
    return {safe_div(x, s), safe_div(y, s)};
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
    return std::sqrt(dot(*this));
  }

  // returns *this unchanged when the length is not above eps, so a zero (or
  // near-zero) vector never produces inf/NaN
  [[nodiscard]] Vec2 normalized(float eps = 0.00000001f) const{
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
  [[nodiscard]] Vec2 zero_clamp(float eps) const{
    Vec2 result{};

    result.x = apply_deadzone(x, eps);
    result.y = apply_deadzone(y, eps);
    return result;
  }
};

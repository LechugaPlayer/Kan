#pragma once

#include <array>
#include <cmath>
#include <cstddef>

#include "Vector3.hpp"

// 4x4 matrix, stored row-major: element (row, col) lives at m[row * 4 + col].
// Matrices are applied to column vectors, v' = M * v, with right-handed
// rotations and angles in radians.
struct Mat4{
  std::array<float, 16> m = {};

  static constexpr std::size_t Dim = 4;

  [[nodiscard]] float operator()(std::size_t row, std::size_t col) const{
    return m[row * Dim + col];
  }

  float& operator()(std::size_t row, std::size_t col){
    return m[row * Dim + col];
  }

  [[nodiscard]] static Mat4 identity(){
    return {1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1};
  }

  [[nodiscard]] static Mat4 translation(float tx, float ty, float tz){
    return {1,0,0,tx,  0,1,0,ty,  0,0,1,tz,  0,0,0,1};
  }

  [[nodiscard]] static Mat4 scale(float sx, float sy, float sz){
    return {sx,0,0,0,  0,sy,0,0,  0,0,sz,0,  0,0,0,1};
  }

  [[nodiscard]] static Mat4 rotationX(float angle){
    const float c = std::cos(angle);
    const float s = std::sin(angle);

    return {1,0,0,0,  0,c,-s,0,  0,s,c,0,  0,0,0,1};
  }

  [[nodiscard]] static Mat4 rotationY(float angle){
    const float c = std::cos(angle);
    const float s = std::sin(angle);

    return {c,0,s,0,  0,1,0,0,  -s,0,c,0,  0,0,0,1};
  }

  [[nodiscard]] static Mat4 rotationZ(float angle){
    const float c = std::cos(angle);
    const float s = std::sin(angle);

    return {c,-s,0,0,  s,c,0,0,  0,0,1,0,  0,0,0,1};
  }

  [[nodiscard]] Mat4 operator*(const Mat4& b) const{
    Mat4 result = {};

    for (std::size_t row = 0; row < Dim; row++) {
      for (std::size_t col = 0; col < Dim; col++) {
        float sum = 0.0f;
        for (std::size_t k = 0; k < Dim; k++) {
          sum += m[row * Dim + k] * b.m[k * Dim + col];
        }
        result.m[row * Dim + col] = sum;
      }
    }
    return result;
  }

  // applies the upper 3x4 (affine) part to a point; the homogeneous w is
  // assumed to be 1, so this is not a perspective divide
  [[nodiscard]] Vec3 transform(const Vec3& p) const{
    return {
      m[0] * p.x + m[1] * p.y + m[2]  * p.z + m[3],
      m[4] * p.x + m[5] * p.y + m[6]  * p.z + m[7],
      m[8] * p.x + m[9] * p.y + m[10] * p.z + m[11]
    };
  }

  // applies the upper 3x3 part to a direction: translation is ignored
  [[nodiscard]] Vec3 transform_direction(const Vec3& v) const{
    return {
      m[0] * v.x + m[1] * v.y + m[2]  * v.z,
      m[4] * v.x + m[5] * v.y + m[6]  * v.z,
      m[8] * v.x + m[9] * v.y + m[10] * v.z
    };
  }

  [[nodiscard]] Mat4 transposed() const{
    Mat4 result = {};

    for (std::size_t row = 0; row < Dim; row++) {
      for (std::size_t col = 0; col < Dim; col++) {
        result.m[col * Dim + row] = m[row * Dim + col];
      }
    }
    return result;
  }

  // exact comparison; use approx_equal() for a tolerance test
  [[nodiscard]] bool operator==(const Mat4& b) const{
    for (std::size_t i = 0; i < m.size(); i++) {
      if (m[i] != b.m[i]) {
        return false;
      }
    }
    return true;
  }

  [[nodiscard]] bool approx_equal(const Mat4& b, float eps = 0.0001f) const{
    for (std::size_t i = 0; i < m.size(); i++) {
      if (std::abs(m[i] - b.m[i]) >= eps) {
        return false;
      }
    }
    return true;
  }
};


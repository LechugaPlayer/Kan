#pragma once

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>

#include "Vector3.hpp"

// 3x3 matrix, stored row-major: element (row, col) lives at m[row * 3 + col].
// This is the linear part of a 4x4 transform (rotation/scale/shear, no
// translation). Under non-uniform scale, transform normals with the inverse
// transpose of the linear transform. Matrices are applied to column vectors,
// v' = M * v, with right-handed rotations and angles in radians.
struct Mat3{
  std::array<float, 9> m = {};

  static constexpr std::size_t Dim = 3;

  [[nodiscard]] float operator()(std::size_t row, std::size_t col) const{
    assert(row < Dim && col < Dim);
    return m[row * Dim + col];
  }

  float& operator()(std::size_t row, std::size_t col){
    assert(row < Dim && col < Dim);
    return m[row * Dim + col];
  }

  [[nodiscard]] static Mat3 identity(){
    return {1,0,0,  0,1,0,  0,0,1};
  }

  [[nodiscard]] static Mat3 scale(float sx, float sy, float sz){
    return {sx,0,0,  0,sy,0,  0,0,sz};
  }

  [[nodiscard]] static Mat3 rotationX(float angle){
    const float c = std::cos(angle);
    const float s = std::sin(angle);

    return {1,0,0,  0,c,-s,  0,s,c};
  }

  [[nodiscard]] static Mat3 rotationY(float angle){
    const float c = std::cos(angle);
    const float s = std::sin(angle);

    return {c,0,s,  0,1,0,  -s,0,c};
  }

  [[nodiscard]] static Mat3 rotationZ(float angle){
    const float c = std::cos(angle);
    const float s = std::sin(angle);

    return {c,-s,0,  s,c,0,  0,0,1};
  }

  [[nodiscard]] Mat3 operator*(const Mat3& b) const{
    Mat3 result = {};

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

  [[nodiscard]] Vec3 transform(const Vec3& v) const{
    return {
      m[0] * v.x + m[1] * v.y + m[2] * v.z,
      m[3] * v.x + m[4] * v.y + m[5] * v.z,
      m[6] * v.x + m[7] * v.y + m[8] * v.z
    };
  }

  [[nodiscard]] Mat3 transposed() const{
    Mat3 result = {};

    for (std::size_t row = 0; row < Dim; row++) {
      for (std::size_t col = 0; col < Dim; col++) {
        result.m[col * Dim + row] = m[row * Dim + col];
      }
    }
    return result;
  }

  [[nodiscard]] float determinant() const{
    return m[0] * (m[4] * m[8] - m[5] * m[7])
         - m[1] * (m[3] * m[8] - m[5] * m[6])
         + m[2] * (m[3] * m[7] - m[4] * m[6]);
  }

  // exact comparison; use approx_equal() for a tolerance test
  [[nodiscard]] bool operator==(const Mat3& b) const{
    for (std::size_t i = 0; i < m.size(); i++) {
      if (m[i] != b.m[i]) {
        return false;
      }
    }
    return true;
  }

  [[nodiscard]] bool approx_equal(const Mat3& b, float eps = 0.0001f) const{
    for (std::size_t i = 0; i < m.size(); i++) {
      if (!(std::abs(m[i] - b.m[i]) < eps)) {
        return false;
      }
    }
    return true;
  }
};

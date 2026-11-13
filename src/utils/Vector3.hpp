#pragma once
#include <cassert>

struct Vec3{
  float x;
  float y;
  float z;
  
  Vec3 operator+(const Vec3& v){
    return {x + v.x, y + v.y, z + v.z};
  }

  Vec3 operator-(const Vec3& v){
    return {x - v.x, y - v.y, z - v.z};
  }
  
  Vec3 operator*(const Vec3& v){
    return {x * v.x, y * v.y, z * v.z};
  }
  
  Vec3 operator/(const Vec3& vec){
    Vec3 v = vec.zero_clamp(0.00001f);
    assert(v.x == 0.0f || v.y == 0.0f || v.z == 0.0f);
    return {x / v.x, y / v.y, z / v.z};
  }
  
  float apply_deadzone(float value, float eps) const{
    return (std::abs(value)< eps) ? 0.0f : value;
  }
  
  Vec3 zero_clamp(float eps) const{
    Vec3 result{};
    
    result.x = apply_deadzone(x, eps);
    result.y = apply_deadzone(y, eps);
    result.z = apply_deadzone(z, eps);
    return result;
    
  }

  
};

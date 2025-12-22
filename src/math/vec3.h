#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <ostream>

struct Vec3 {
  float x = 0.f;
  float y = 0.f;
  float z = 0.f;

  constexpr Vec3() = default;
  constexpr Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

  float length() const {
    return std::sqrt(x * x + y * y + z * z);
  }

  float lengthSquared() const {
    return x * x + y * y + z * z;
  }

  Vec3 normalized() const {
    float len = length();
    if (len > 0.f) {
      return {x / len, y / len, z / len};
    }
    return {};
  }

  void normalize() {
    float len = length();
    if (len > 0.f) {
      x /= len;
      y /= len;
      z /= len;
    }
  }
};

inline Vec3 operator+(const Vec3 &a, const Vec3 &b) {
  return {a.x + b.x, a.y + b.y, a.z + b.z};
}

inline Vec3 operator-(const Vec3 &a, const Vec3 &b) {
  return {a.x - b.x, a.y - b.y, a.z - b.z};
}

inline Vec3 operator*(const Vec3 &v, float s) {
  return {v.x * s, v.y * s, v.z * s};
}

inline Vec3 operator*(float s, const Vec3 &v) {
  return v * s;
}

inline Vec3 operator/(const Vec3 &v, float s) {
  return {v.x / s, v.y / s, v.z / s};
}

inline float dot(const Vec3 &a, const Vec3 &b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vec3 cross(const Vec3 &a, const Vec3 &b) {
  return {
      a.y * b.z - a.z * b.y,
      a.z * b.x - a.x * b.z,
      a.x * b.y - a.y * b.x};
}

inline std::ostream &operator<<(std::ostream &os, const Vec3 &v) {
  os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
  return os;
}

#endif

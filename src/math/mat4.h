#ifndef MAT4_H
#define MAT4_H

#include "math/mat3.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include <cmath>
#include <ostream>
#include <iomanip>

struct Mat4 {
private:
  // data_[row][col]
  float data_[4][4];

public:
  float &operator()(int row, int col) { return data_[row][col]; }
  float operator()(int row, int col) const { return data_[row][col]; }

  static Mat4 identity() {
    Mat4 r{};
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        r.data_[i][j] = (i == j) ? 1.f : 0.f;
    return r;
  }
};

// Matrix * Matrix
inline Mat4 multiply(const Mat4 &left, const Mat4 &right) {
  Mat4 result{};
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      float sum = 0.f;
      for (int k = 0; k < 4; k++)
        sum += left(row, k) * right(k, col);
      result(row, col) = sum;
    }
  }
  return result;
}

// Matrix * Vector
inline Vec4 transform(const Mat4 &matrix, const Vec4 &v) {
  return {
      matrix(0, 0) * v.x + matrix(0, 1) * v.y + matrix(0, 2) * v.z + matrix(0, 3) * v.w,
      matrix(1, 0) * v.x + matrix(1, 1) * v.y + matrix(1, 2) * v.z + matrix(1, 3) * v.w,
      matrix(2, 0) * v.x + matrix(2, 1) * v.y + matrix(2, 2) * v.z + matrix(2, 3) * v.w,
      matrix(3, 0) * v.x + matrix(3, 1) * v.y + matrix(3, 2) * v.z + matrix(3, 3) * v.w};
}

// Extract the linear (upper-left) 3x3 part
inline Mat3 extractLinearPart3x3(const Mat4 &matrix) {
  Mat3 linear{};
  for (int row = 0; row < 3; row++)
    for (int col = 0; col < 3; col++)
      linear.m[row][col] = matrix(row, col); 
  return linear;
}

// assumes last row is [0 0 0 1]
inline Mat4 invertAffineTransform(const Mat4 &matrix) {
  const float a00 = matrix(0, 0), a01 = matrix(0, 1), a02 = matrix(0, 2);
  const float a10 = matrix(1, 0), a11 = matrix(1, 1), a12 = matrix(1, 2);
  const float a20 = matrix(2, 0), a21 = matrix(2, 1), a22 = matrix(2, 2);

  const float det =
      a00 * (a11 * a22 - a12 * a21) -
      a01 * (a10 * a22 - a12 * a20) +
      a02 * (a10 * a21 - a11 * a20);

  const float invDet = 1.f / det;

  Mat4 inverse = Mat4::identity();

  // inverse linear 3x3
  inverse(0, 0) = (a11 * a22 - a12 * a21) * invDet;
  inverse(0, 1) = -(a01 * a22 - a02 * a21) * invDet;
  inverse(0, 2) = (a01 * a12 - a02 * a11) * invDet;

  inverse(1, 0) = -(a10 * a22 - a12 * a20) * invDet;
  inverse(1, 1) = (a00 * a22 - a02 * a20) * invDet;
  inverse(1, 2) = -(a00 * a12 - a02 * a10) * invDet;

  inverse(2, 0) = (a10 * a21 - a11 * a20) * invDet;
  inverse(2, 1) = -(a00 * a21 - a01 * a20) * invDet;
  inverse(2, 2) = (a00 * a11 - a01 * a10) * invDet;

  // translation (t is last column)
  const float tx = matrix(0, 3);
  const float ty = matrix(1, 3);
  const float tz = matrix(2, 3);

  // new translation = -invA * t
  inverse(0, 3) = -(inverse(0, 0) * tx + inverse(0, 1) * ty + inverse(0, 2) * tz);
  inverse(1, 3) = -(inverse(1, 0) * tx + inverse(1, 1) * ty + inverse(1, 2) * tz);
  inverse(2, 3) = -(inverse(2, 0) * tx + inverse(2, 1) * ty + inverse(2, 2) * tz);

  return inverse;
}

inline std::ostream& operator<<(std::ostream& os, const Mat4& M) {
  std::ios oldState(nullptr);
  oldState.copyfmt(os); 

  os << std::fixed << std::setprecision(4);
  for (int r = 0; r < 4; ++r) {
    os << "[ ";
    for (int c = 0; c < 4; ++c) {
      os << std::setw(9) << M(r, c) << ' ';
    }
    os << "]";
    if (r < 3) os << '\n';
  }

  os.copyfmt(oldState); 
  return os;
}

#endif

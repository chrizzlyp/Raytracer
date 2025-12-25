#ifndef MAT4_H
#define MAT4_H

#include <cmath>
#include "math/vec3.h"
#include "math/vec4.h"
#include "math/mat3.h"

struct Mat4 {
  // row-major: m[row][col]
  float m[4][4];

  static Mat4 identity() {
    Mat4 r{};
    for (int i=0;i<4;i++)
      for (int j=0;j<4;j++)
        r.m[i][j] = (i==j) ? 1.f : 0.f;
    return r;
  }
};

inline Mat4 mul(const Mat4& A, const Mat4& B) {
  Mat4 R{};
  for (int i=0;i<4;i++) {
    for (int j=0;j<4;j++) {
      float s = 0.f;
      for (int k=0;k<4;k++) s += A.m[i][k] * B.m[k][j];
      R.m[i][j] = s;
    }
  }
  return R;
}

inline Vec4 mul(const Mat4& M, const Vec4& v) {
  return {
    M.m[0][0]*v.x + M.m[0][1]*v.y + M.m[0][2]*v.z + M.m[0][3]*v.w,
    M.m[1][0]*v.x + M.m[1][1]*v.y + M.m[1][2]*v.z + M.m[1][3]*v.w,
    M.m[2][0]*v.x + M.m[2][1]*v.y + M.m[2][2]*v.z + M.m[2][3]*v.w,
    M.m[3][0]*v.x + M.m[3][1]*v.y + M.m[3][2]*v.z + M.m[3][3]*v.w
  };
}

inline Mat3 upperLeft3x3(const Mat4& M) {
  Mat3 r{};
  for (int i=0;i<3;i++)
    for (int j=0;j<3;j++)
      r.m[i][j] = M.m[i][j];
  return r;
}

// Inversion for affine matrices (rotation/scale + translation), no projection/shear required.
// Assumes last row is [0 0 0 1].
inline Mat4 inverseAffine(const Mat4& M) {
  // Invert 3x3 part via general formula (works for rotation+scale; fails if singular)
  const float a00=M.m[0][0], a01=M.m[0][1], a02=M.m[0][2];
  const float a10=M.m[1][0], a11=M.m[1][1], a12=M.m[1][2];
  const float a20=M.m[2][0], a21=M.m[2][1], a22=M.m[2][2];

  const float det =
      a00*(a11*a22 - a12*a21) -
      a01*(a10*a22 - a12*a20) +
      a02*(a10*a21 - a11*a20);

  // You may want to handle det ~ 0 as error; here we assume valid transforms.
  const float invDet = 1.f / det;

  Mat4 R = Mat4::identity();
  // inverse of 3x3
  R.m[0][0] =  (a11*a22 - a12*a21) * invDet;
  R.m[0][1] = -(a01*a22 - a02*a21) * invDet;
  R.m[0][2] =  (a01*a12 - a02*a11) * invDet;

  R.m[1][0] = -(a10*a22 - a12*a20) * invDet;
  R.m[1][1] =  (a00*a22 - a02*a20) * invDet;
  R.m[1][2] = -(a00*a12 - a02*a10) * invDet;

  R.m[2][0] =  (a10*a21 - a11*a20) * invDet;
  R.m[2][1] = -(a00*a21 - a01*a20) * invDet;
  R.m[2][2] =  (a00*a11 - a01*a10) * invDet;

  // translation
  const float tx = M.m[0][3];
  const float ty = M.m[1][3];
  const float tz = M.m[2][3];

  // new translation = -invA * t
  R.m[0][3] = -(R.m[0][0]*tx + R.m[0][1]*ty + R.m[0][2]*tz);
  R.m[1][3] = -(R.m[1][0]*tx + R.m[1][1]*ty + R.m[1][2]*tz);
  R.m[2][3] = -(R.m[2][0]*tx + R.m[2][1]*ty + R.m[2][2]*tz);

  // last row already [0 0 0 1]
  return R;
}

#endif

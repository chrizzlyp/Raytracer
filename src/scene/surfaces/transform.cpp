#include "transform.h"
#include "math/mat4.h"
#include <cmath>

static float deg2rad(float deg) { return deg * 3.14159265358979323846f / 180.f; }

Transform::Transform():M_(Mat4::identity()), invM_(Mat4::identity()), normalM_(Mat3::identity()) {}

static Mat4 makeTranslation(const Vec3 &t) {
  Mat4 T = Mat4::identity();
  T.m[0][3] = t.x;
  T.m[1][3] = t.y;
  T.m[2][3] = t.z;
  return T;
}

static Mat4 makeScale(const Vec3 &s) {
  Mat4 S = Mat4::identity();
  S.m[0][0] = s.x;
  S.m[1][1] = s.y;
  S.m[2][2] = s.z;
  return S;
}

static Mat4 makeRotX(float deg) {
  float a = deg2rad(deg);
  float c = std::cos(a), s = std::sin(a);
  Mat4 R = Mat4::identity();
  R.m[1][1] = c;
  R.m[1][2] = -s;
  R.m[2][1] = s;
  R.m[2][2] = c;
  return R;
}

static Mat4 makeRotY(float deg) {
  float a = deg2rad(deg);
  float c = std::cos(a), s = std::sin(a);
  Mat4 R = Mat4::identity();
  R.m[0][0] = c;
  R.m[0][2] = s;
  R.m[2][0] = -s;
  R.m[2][2] = c;
  return R;
}

static Mat4 makeRotZ(float deg) {
  float a = deg2rad(deg);
  float c = std::cos(a), s = std::sin(a);
  Mat4 R = Mat4::identity();
  R.m[0][0] = c;
  R.m[0][1] = -s;
  R.m[1][0] = s;
  R.m[1][1] = c;
  return R;
}

// Compose in XML order: if XML says translate then scale, we want M = (Scale * Translate)?? depending on convention.
// With p' = M * p and applying operations in order, you should post-multiply: M = M * Op.
void Transform::translate(const Vec3 &t) {
  M_ = mul(M_, makeTranslation(t));
  recomputeCaches();
}

void Transform::scale(const Vec3 &s) {
  M_ = mul(M_, makeScale(s));
  recomputeCaches();
}

void Transform::rotateX(float deg) {
  M_ = mul(M_, makeRotX(deg));
  recomputeCaches();
}

void Transform::rotateY(float deg) {
  M_ = mul(M_, makeRotY(deg));
  recomputeCaches();
}

void Transform::rotateZ(float deg) {
  M_ = mul(M_, makeRotZ(deg));
  recomputeCaches();
}

void Transform::recomputeCaches() {
  invM_ = inverseAffine(M_);

  // normal matrix = transpose(inverse(upper-left 3x3))
  Mat3 invA = upperLeft3x3(invM_);
  normalM_ = transpose(invA);
}

Vec3 Transform::applyPoint(const Vec3 &p) const {
  Vec4 hp = mul(M_, Vec4{p.x, p.y, p.z, 1.f});
  // perspective not used; w should be 1.0
  return {hp.x, hp.y, hp.z};
}

Vec3 Transform::applyVector(const Vec3 &v) const {
  Vec4 hv = mul(M_, Vec4{v.x, v.y, v.z, 0.f});
  return {hv.x, hv.y, hv.z};
}

Vec3 Transform::applyNormal(const Vec3 &n) const {
  // normalM_ is 3x3
  Vec3 r{
      normalM_.m[0][0] * n.x + normalM_.m[0][1] * n.y + normalM_.m[0][2] * n.z,
      normalM_.m[1][0] * n.x + normalM_.m[1][1] * n.y + normalM_.m[1][2] * n.z,
      normalM_.m[2][0] * n.x + normalM_.m[2][1] * n.y + normalM_.m[2][2] * n.z};
  return r;
}

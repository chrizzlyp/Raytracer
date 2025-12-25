#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "math/vec3.h"
#include "math/vec4.h"
#include "math/mat3.h"
#include "math/mat4.h"

class Transform {
public:
  Transform(); 

  void translate(const Vec3 &t);
  void scale(const Vec3 &s);
  void rotateX(float deg);
  void rotateY(float deg);
  void rotateZ(float deg);

  const Mat4 &matrix() const { return M_; }
  const Mat4 &inverse() const { return invM_; }
  const Mat3 &normalMatrix() const { return normalM_; }

  Vec3 applyPoint(const Vec3 &p) const;
  Vec3 applyVector(const Vec3 &v) const;
  Vec3 applyNormal(const Vec3 &n) const;

private:
  void recomputeCaches(); // inv + normal matrix

  Mat4 M_;
  Mat4 invM_;
  Mat3 normalM_;
};

#endif
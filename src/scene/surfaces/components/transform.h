#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "math/vec3.h"
#include "math/vec4.h"
#include "math/mat3.h"
#include "math/mat4.h"
#include "math/ray.h"  

class Transform {
public:
  Transform();

  void translate(const Vec3& translation);
  void scale(const Vec3& scaleFactors);
  void rotateX(float angleDegrees);
  void rotateY(float angleDegrees);
  void rotateZ(float angleDegrees);

  const Mat4& transformMatrix() const { return transformMatrix_; }
  const Mat4& inverseTransformMatrix() const { return inverseTransformMatrix_; }
  const Mat3& normalTransformMatrix() const { return normalTransformMatrix_; }

  Vec3 applyPoint(const Vec3& point) const;
  Vec3 applyVector(const Vec3& direction) const;
  Vec3 applyNormal(const Vec3& normal) const;

  Ray toObjectRay(const Ray& worldRay) const;

  friend std::ostream& operator<<(std::ostream& os, const Transform& t);
  
private:
  void recomputeCachedMatrices(); // inverse + normal matrix

  // helpers for (toObjectRay) transforming world space ray -> object space ray
  Vec3 applyInversePoint(const Vec3& point) const;
  Vec3 applyInverseVector(const Vec3& direction) const;

  Mat4 transformMatrix_;
  Mat4 inverseTransformMatrix_;
  Mat3 normalTransformMatrix_;
};

#endif

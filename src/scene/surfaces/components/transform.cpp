#include "transform.h"
#include "math/mat4.h"
#include <cmath>
#include <ostream>
#include <iomanip>


static float degreesToRadians(float angleDegrees) {
  return angleDegrees * 3.14159265358979323846f / 180.f;
}

// Initialize as identity matrices
Transform::Transform(): transformMatrix_(Mat4::identity()), inverseTransformMatrix_(Mat4::identity()), normalTransformMatrix_(Mat3::identity()) {}

static Mat4 makeTranslationMatrix(const Vec3& translation) {
  Mat4 translationMat = Mat4::identity();
  translationMat(0,3) = translation.x;
  translationMat(1,3) = translation.y;
  translationMat(2,3) = translation.z;
  return translationMat;
}

static Mat4 makeScaleMatrix(const Vec3& scaleFactors) {
  Mat4 scaleMat = Mat4::identity();
  scaleMat(0,0) = scaleFactors.x;
  scaleMat(1,1) = scaleFactors.y;
  scaleMat(2,2) = scaleFactors.z;
  return scaleMat;
}

static Mat4 makeRotationXMatrix(float angleDegrees) {
  const float angleRad = degreesToRadians(angleDegrees);
  const float cosA = std::cos(angleRad);
  const float sinA = std::sin(angleRad);

  Mat4 rotationMat = Mat4::identity();
  rotationMat(1,1) = cosA;
  rotationMat(1,2) = -sinA;
  rotationMat(2,1) = sinA;
  rotationMat(2,2) = cosA;
  return rotationMat;
}

static Mat4 makeRotationYMatrix(float angleDegrees) {
  const float angleRad = degreesToRadians(angleDegrees);
  const float cosA = std::cos(angleRad);
  const float sinA = std::sin(angleRad);

  Mat4 rotationMat = Mat4::identity();
  rotationMat(0,0) = cosA;
  rotationMat(0,2) = sinA;
  rotationMat(2,0) = -sinA;
  rotationMat(2,2) = cosA;
  return rotationMat;
}

static Mat4 makeRotationZMatrix(float angleDegrees) {
  const float angleRad = degreesToRadians(angleDegrees);
  const float cosA = std::cos(angleRad);
  const float sinA = std::sin(angleRad);

  Mat4 rotationMat = Mat4::identity();
  rotationMat(0,0) = cosA;
  rotationMat(0,1) = -sinA;
  rotationMat(1,0) = sinA;
  rotationMat(1,1) = cosA;
  return rotationMat;
}

// With p' = M * p and applying operations in order, post-multiply: M = M * Op.
void Transform::translate(const Vec3& translation) {
  transformMatrix_ = multiply(transformMatrix_, makeTranslationMatrix(translation));
  recomputeCachedMatrices();
}

void Transform::scale(const Vec3& scaleFactors) {
  transformMatrix_ = multiply(transformMatrix_, makeScaleMatrix(scaleFactors));
  recomputeCachedMatrices();
}

void Transform::rotateX(float angleDegrees) {
  transformMatrix_ = multiply(transformMatrix_, makeRotationXMatrix(angleDegrees));
  recomputeCachedMatrices();
}

void Transform::rotateY(float angleDegrees) {
  transformMatrix_ = multiply(transformMatrix_, makeRotationYMatrix(angleDegrees));
  recomputeCachedMatrices();
}

void Transform::rotateZ(float angleDegrees) {
  transformMatrix_ = multiply(transformMatrix_, makeRotationZMatrix(angleDegrees));
  recomputeCachedMatrices();
}

void Transform::recomputeCachedMatrices() {
  inverseTransformMatrix_ = invertAffineTransform(transformMatrix_);

  // normal matrix = transpose(inverse(upper-left 3x3))
  const Mat3 inverseLinearPart = extractLinearPart3x3(inverseTransformMatrix_);
  normalTransformMatrix_ = transpose(inverseLinearPart);
}

Vec3 Transform::applyPoint(const Vec3& point) const {
  const Vec4 homPoint = transform(transformMatrix_, Vec4{point.x, point.y, point.z, 1.f});
  return {homPoint.x, homPoint.y, homPoint.z};
}

Vec3 Transform::applyVector(const Vec3& direction) const {
  const Vec4 homVector = transform(transformMatrix_, Vec4{direction.x, direction.y, direction.z, 0.f});
  return {homVector.x, homVector.y, homVector.z};
}

Vec3 Transform::applyNormal(const Vec3& normal) const {
  // Falls Mat3 noch m[][] hat, bleibt das so:
  Vec3 transformedNormal{
    normalTransformMatrix_.m[0][0] * normal.x +
    normalTransformMatrix_.m[0][1] * normal.y +
    normalTransformMatrix_.m[0][2] * normal.z,

    normalTransformMatrix_.m[1][0] * normal.x +
    normalTransformMatrix_.m[1][1] * normal.y +
    normalTransformMatrix_.m[1][2] * normal.z,

    normalTransformMatrix_.m[2][0] * normal.x +
    normalTransformMatrix_.m[2][1] * normal.y +
    normalTransformMatrix_.m[2][2] * normal.z
  };
  return transformedNormal;
}

//to transform a ray in world space to object space of the surface
Ray Transform::toObjectRay(const Ray& worldRay) const {
  Vec3 origin = applyInversePoint(worldRay.origin);
  Vec3 direction = applyInverseVector(worldRay.direction).normalized();
  return Ray{origin, direction};
}

Vec3 Transform::applyInversePoint(const Vec3& point) const {
  const Vec4 homogenPoint = transform(inverseTransformMatrix_, Vec4{point.x, point.y, point.z, 1.f});
  return {homogenPoint.x, homogenPoint.y, homogenPoint.z};
}

Vec3 Transform::applyInverseVector(const Vec3& direction) const {
  const Vec4 homVector = transform(inverseTransformMatrix_, Vec4{direction.x, direction.y, direction.z, 0.f});
  return {homVector.x, homVector.y, homVector.z};
}


std::ostream& operator<<(std::ostream& os, const Transform& t) {
  os << "Transform {\n";
  os << "  transformMatrix:\n"        << t.transformMatrix()        << "\n";
  os << "  inverseTransformMatrix:\n" << t.inverseTransformMatrix() << "\n";
  os << "  normalTransformMatrix:\n"  << t.normalTransformMatrix()  << "\n";
  os << "}";
  return os;
}

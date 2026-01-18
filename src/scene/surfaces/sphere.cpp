#include "scene/surfaces/sphere.h"
#include "math/vec3.h"
#include <cmath>

bool Sphere::intersect(const Ray &rayWorldSpace, Hit &hit) const {
  // transform the ray to object space
  Ray rayObjectSpace = transform_.toObjectRay(rayWorldSpace);

  Vec3 sphereCenterToRayOrigin = rayObjectSpace.origin - this->centerPosition_;

  // length^2 of my direction (if normalized = 1)
  float a = dot(rayObjectSpace.direction, rayObjectSpace.direction);

  // is the ray pointing in the direction of the ray (+: yes, -: no)
  float b = 2.0f * dot(sphereCenterToRayOrigin, rayObjectSpace.direction);

  // is the ray starting outside, inside or directly on the surface? (outside: c>0, inside: c<0 or on: c==0)
  // distance^2 - radius^2
  float c = dot(sphereCenterToRayOrigin, sphereCenterToRayOrigin) - radius_ * radius_;

  // ax^2 + bx + c = 0 -> discriminant = b^2 - 4ac
  float discriminant = b * b - 4.0f * a * c;

  // no hit (discriminant < 0)
  if (discriminant < 0.0f)
    return false;

  float sqrtDiscriminant = std::sqrt(discriminant);
  float factor_2a = 2.0f * a;

  // hitdistance = (-b +- sqrt(b^2 - 4ac)) / (factor_2a)
  float hitDistanceOne = (-b - sqrtDiscriminant) / factor_2a;
  float hitDistanceTwo = (-b + sqrtDiscriminant) / factor_2a;

  // smallest positive hit distance
  float START_DISTANCE = 1e30f; // really far away
  float closestHitDistance = START_DISTANCE;
  
  const float eps = 1e-4f; // prevents self injection
  if (hitDistanceOne > eps)
    closestHitDistance = hitDistanceOne;
  else if (hitDistanceTwo > eps)
    closestHitDistance = hitDistanceTwo;
  else
    return false; // unuseful hit

  // calculate hit in object space
  Vec3 hitpointObjectSpace = rayObjectSpace.origin + closestHitDistance * rayObjectSpace.direction;
  Vec3 hitpointNormalObjectSpace = (hitpointObjectSpace - centerPosition_).normalized();

  // transform the hitpoint + normal back to world space
  Vec3 hitpointWorldSpace = transform_.applyPoint(hitpointObjectSpace);
  Vec3 hitpointNormalWorldSpace = transform_.applyNormal(hitpointNormalObjectSpace).normalized();

  // distance of hitpoint to ray starting point in worldspace
  float hitpointRayOriginDistanceWorldSpace = (hitpointWorldSpace - rayWorldSpace.origin).length(); 

  // only saves the closest intersection
  if (hitpointRayOriginDistanceWorldSpace < hit.distanceClosestIntersection) {
    hit.distanceClosestIntersection = hitpointRayOriginDistanceWorldSpace;
    hit.positionWorldSpace = hitpointWorldSpace;
    hit.normalWorldSpace = hitpointNormalWorldSpace;
    hit.material = &material_;
    return true;
  }

  return false;
}

AABB Sphere::getWorldBoundingVolume() const {
  Vec3 sphereCenterWorldSpace = transform().applyPoint(centerPosition());

  float scaleX = transform().applyVector(Vec3(1,0,0)).length();
  float scaleY = transform().applyVector(Vec3(0,1,0)).length();
  float scaleZ = transform().applyVector(Vec3(0,0,1)).length();

  Vec3 halfExtents(radius() * scaleX, radius() * scaleY, radius() * scaleZ);

  return AABB(sphereCenterWorldSpace - halfExtents, sphereCenterWorldSpace + halfExtents);
}

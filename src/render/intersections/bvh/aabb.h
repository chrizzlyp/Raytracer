#ifndef AABB_H
#define AABB_H

#include "math/vec3.h"
#include "math/ray.h"

class AABB {
public:
  AABB() = default;
  AABB(const Vec3  boxMinAxis, const Vec3 &boxMaxAxis) : minCorner(boxMinAxis), maxCorner(boxMaxAxis) {}

  const Vec3 &min() const {
    return minCorner;
  }

  const Vec3 &max() const {
    return maxCorner;
  }

  bool isValid() const {
    return (minCorner.x <= maxCorner.x) &&
           (minCorner.y <= maxCorner.y) &&
           (minCorner.z <= maxCorner.z);
  }

  // extends the box that the point is included (for mesh boxes -> object space / for surface boxes -> world space)
  void includePoint(const Vec3 &vertex) {
    minCorner = Vec3(std::min(minCorner.x, vertex.x), std::min(minCorner.y, vertex.y), std::min(minCorner.z, vertex.z));
    maxCorner = Vec3(std::max(maxCorner.x, vertex.x), std::max(maxCorner.y, vertex.y), std::max(maxCorner.z, vertex.z));
  }

  // extends the box that another bounding box is included
  void includeBox(const AABB &b) {
    includePoint(b.minCorner);
    includePoint(b.maxCorner);
  }

  float surfaceArea() const {
    Vec3 size = maxCorner - minCorner;
    return 2.f * (size.x * size.y + size.y * size.z + size.z * size.x);
  }

  bool intersect(const Ray &ray, float rayDistanceStart, float rayDistanceEnd) const {
    // axis = 0 -> x / 1 -> y / 2 -> z
    for (int axis = 0; axis < 3; ++axis) {
      float originAxis = ray.origin[axis];
      float directionAxis = ray.direction[axis];

      // directionAxis=0 -> parallel to box in this axis
      if (std::abs(directionAxis) < 1e-8f) {
        float boxMinAxis = minCorner[axis];
        float boxMaxAxis = maxCorner[axis];

        // ray origin on this axis is outside of bounding box on this axis: can never slap the box
        if (originAxis < boxMinAxis || originAxis > boxMaxAxis)
          return false;
        // ray origin on this axis is inside: works
        continue;
      }

      float inverseDirectionAxis = 1.0f / directionAxis;
      float boxMinAxis = minCorner[axis];
      float boxMaxAxis = maxCorner[axis];

      float axisEntryDistance = (boxMinAxis - originAxis) * inverseDirectionAxis;
      float axisExitDistance = (boxMaxAxis - originAxis) * inverseDirectionAxis;
      // if ray direction is negative entry/exit swap
      if (inverseDirectionAxis < 0.0f)
        std::swap(axisEntryDistance, axisExitDistance);

      // if ray interval intersects with axis interval
      rayDistanceStart = axisEntryDistance > rayDistanceStart ? axisEntryDistance : rayDistanceStart;
      rayDistanceEnd = axisExitDistance < rayDistanceEnd ? axisExitDistance : rayDistanceEnd;
      // if interval is empty -> ray doesnt slap the box
      if (rayDistanceEnd < rayDistanceStart)
        return false;
    }
    return true;
  }

  static AABB empty() { return AABB(); }

private:
  Vec3 minCorner{+INFINITY, +INFINITY, +INFINITY};
  Vec3 maxCorner{-INFINITY, -INFINITY, -INFINITY};
};

#endif
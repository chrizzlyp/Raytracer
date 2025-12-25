#ifndef SPHERE_H
#define SPHERE_H

#include "math/vec3.h"
#include "scene/surfaces/surface.h"
#include <ostream>

class Sphere : public Surface {
public:
  SurfaceType type() const override {
    return SurfaceType::SPHERE;
  }
  void setCenterPosition(const Vec3 &centerPosition) {
    centerPosition_ = centerPosition;
  }
  void setRadius(float radius) {
    radius_ = radius;
  }

  const Vec3 &centerPosition() const{
    return centerPosition_;
  }

    const float &radius() const{
    return radius_;
  }

private:
  Vec3 centerPosition_{0, 0, 0};
  float radius_ = 1.f;
};

inline std::ostream& operator<<(std::ostream& os, const Sphere& s) {
  os << "Sphere{center=" << s.centerPosition() << ", radius=" << s.radius() << "}";
  return os;
}

#endif
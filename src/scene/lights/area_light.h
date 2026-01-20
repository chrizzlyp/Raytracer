#ifndef SCENE_LIGHTS_AREA_LIGHT_H
#define SCENE_LIGHTS_AREA_LIGHT_H

#include "math/vec3.h"
#include "scene/lights/light.h"
#include <cmath>

class AreaLight final : public Light {
public:
  LightType type() const override { return LightType::AREA; }

  const Vec3 &center() const { return center_; }
  void setCenter(const Vec3 &c) { center_ = c; }

  const Vec3 &halfWidthVec() const { return halfWidthVec_; }
  const Vec3 &halfHeightVec() const { return halfHeightVec_; }

  void setHalfWidthVec(const Vec3 &w) {
    halfWidthVec_ = w;
    recompute();
  }

  void setHalfHeightVec(const Vec3 &h) {
    halfHeightVec_ = h;
    recompute();
  }

  const Vec3 &normal() const { return normal_; }
  float area() const { return area_; }

  Vec3 samplePoint(float xi1, float xi2) const {
    float a = 2.0f * xi1 - 1.0f; // [-1, 1]
    float b = 2.0f * xi2 - 1.0f; // [-1, 1]
    return center_ + a * halfWidthVec_ + b * halfHeightVec_;
  }

private:
  void recompute() {
    Vec3 c = cross(halfWidthVec_, halfHeightVec_);
    float len = c.length();

    if (len > 0.0f) {
      normal_ = c / len;
    } else {
      normal_ = Vec3(0.f, 0.f, 1.f); // fallback
    }

    // full edges are (2*w) and (2*h) => area = |(2w)x(2h)| = 4|w x h|
    area_ = 4.0f * len;
  }

  Vec3 center_{0.f, 0.f, 0.f};

  Vec3 halfWidthVec_{0.5f, 0.f, 0.f};
  Vec3 halfHeightVec_{0.f, 0.5f, 0.f};

  Vec3 normal_{0.f, 0.f, 1.f};
  float area_{1.0f};
};

#endif

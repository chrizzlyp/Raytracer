#ifndef SCENE_LIGHTS_PARALLEL_LIGHT_H
#define SCENE_LIGHTS_PARALLEL_LIGHT_H

#include "math/vec3.h"
#include "scene/lights/light.h"

class ParallelLight final : public Light {
public:
  LightType type() const override {
    return LightType::PARALLEL;
  }

  const Vec3 &direction() const {
    return direction_;
  }
  void setDirection(Vec3 d) {
    d.normalize();
    direction_ = d;
  }

private:
  Vec3 direction_{0.f, 0.f, -1.f};
};

std::ostream &operator<<(std::ostream &os, const ParallelLight &l);

#endif

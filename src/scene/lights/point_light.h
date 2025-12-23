#ifndef SCENE_LIGHTS_POINT_LIGHT_H
#define SCENE_LIGHTS_POINT_LIGHT_H

#include "math/vec3.h"
#include "scene/lights/light.h"

class PointLight final : public Light {
public:
  LightType type() const override {
    return LightType::POINT;
  }

  const Vec3 &position() const {
    return position_;
  }
  
  void setPosition(const Vec3 &p) {
    position_ = p;
  }

private:
  Vec3 position_{};
};

std::ostream &operator<<(std::ostream &os, const PointLight &l);

#endif

#ifndef SCENE_LIGHTS_SPOT_LIGHT_H
#define SCENE_LIGHTS_SPOT_LIGHT_H

#include <stdexcept>

#include "math/vec3.h"
#include "scene/lights/light.h"

class SpotLight final : public Light {
public:
  LightType type() const override {
    return LightType::SPOT;
  }

  const Vec3 &position() const {
    return position_;
  }

  const Vec3 &direction() const {
    return direction_;
  }

  float alpha1() const {
    return alpha1_;
  }
  
  float alpha2() const {
    return alpha2_;
  }

  void setPosition(const Vec3 &p) {
    position_ = p;
  }

  void setDirection(Vec3 d) {
    d.normalize();
    direction_ = d;
  }

  void setFalloff(float a1, float a2) {
    if (a1 < 0.f || a2 < 0.f || a1 > a2)
      throw std::invalid_argument("Spot falloff must satisfy 0 <= alpha1 <= alpha2");
    alpha1_ = a1;
    alpha2_ = a2;
  }

private:
  Vec3 position_{};
  Vec3 direction_{0.f, 0.f, -1.f};
  float alpha1_ = 0.f;
  float alpha2_ = 0.f;
};

std::ostream &operator<<(std::ostream &os, const SpotLight &l);

#endif
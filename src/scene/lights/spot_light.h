#ifndef SCENE_LIGHTS_SPOT_LIGHT_H
#define SCENE_LIGHTS_SPOT_LIGHT_H

#include <stdexcept>
#include <cmath>

#include "math/vec3.h"
#include "scene/lights/light.h"

class SpotLight final : public Light {
public:
  LightType type() const override { return LightType::SPOT; }

  const Vec3& position() const { return position_; }
  const Vec3& direction() const { return direction_; }

  // inner cone angle (radians)
  float alpha1() const { return alpha1_; }
  // outer cone angle (radians)
  float alpha2() const { return alpha2_; }

  void setPosition(const Vec3& p) { position_ = p; }

  void setDirection(Vec3 d) {
    d.normalize();
    direction_ = d;
  }

  // a1 = inner angle, a2 = outer angle (radians), with 0 <= a1 <= a2
  void setFalloff(float a1, float a2) {
    if (a1 < 0.f || a2 < 0.f || a1 > a2)
      throw std::invalid_argument("Spot falloff must satisfy 0 <= alpha1 <= alpha2");
    alpha1_ = a1;
    alpha2_ = a2;

    // Precompute cosines for faster spot evaluation
    cosAlpha1_ = std::cos(alpha1_);
    cosAlpha2_ = std::cos(alpha2_);
  }

  // Returns spotlight attenuation in [0,1].
  float spotAttenuation(const Vec3& dirLightToPoint) const {
    // cos(theta) between spotlight direction and direction to point
    float cosTheta = dot(direction_, dirLightToPoint);

    // inside inner cone -> full intensity
    if (cosTheta >= cosAlpha1_) return 1.0f;

    // outside outer cone -> no light
    if (cosTheta <= cosAlpha2_) return 0.0f;

    // smooth falloff between outer and inner
    float t = (cosTheta - cosAlpha2_) / (cosAlpha1_ - cosAlpha2_);
    // smoother curve:
    t = t * t * (3.0f - 2.0f * t); // smoothstep
    return t;
  }

private:
  Vec3 position_{};
  Vec3 direction_{0.f, 0.f, -1.f};

  float alpha1_ = 0.f; 
  float alpha2_ = 0.f; 

  float cosAlpha1_ = 1.f;
  float cosAlpha2_ = 1.f;
};

std::ostream& operator<<(std::ostream& os, const SpotLight& l);

#endif

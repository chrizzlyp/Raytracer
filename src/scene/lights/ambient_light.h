#ifndef SCENE_LIGHTS_AMBIENT_LIGHT_H
#define SCENE_LIGHTS_AMBIENT_LIGHT_H

#include "scene/lights/light.h"

class AmbientLight final : public Light {
public:
  LightType type() const override {
    return LightType::AMBIENT;
  }
};

std::ostream &operator<<(std::ostream &os, const AmbientLight &l);

#endif

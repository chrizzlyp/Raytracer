#ifndef SCENE_LIGHTS_LIGHT_H
#define SCENE_LIGHTS_LIGHT_H

#include "math/color.h"

enum class LightType { AMBIENT, POINT, PARALLEL, SPOT };

class Light {
public:
  virtual ~Light() = default;
  virtual LightType type() const = 0;

  const Color &color() const {
    return color_;
  }
  
  void setColor(const Color &c) {
    color_ = c;
  }

private:
  Color color_{0.f, 0.f, 0.f};
};

std::ostream &operator<<(std::ostream &os, const Light &l);

#endif

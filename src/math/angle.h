#ifndef ANGLE_H
#define ANGLE_H

#include <cmath>

constexpr float PI_F = 3.14159265358979323846f;

constexpr float degToRad(float deg) {
  return deg * (PI_F / 180.0f);
}

constexpr float radToDeg(float rad) {
  return rad * (180.0f / PI_F);
}

#endif
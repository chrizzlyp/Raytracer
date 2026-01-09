#ifndef HIT_H
#define HIT_H

#include "math/vec3.h"
#include "scene/surfaces/components/material.h"

struct Hit {
  float distanceClosestIntersection = 1e30f;
  Vec3 positionWorldSpace;
  Vec3 normalWorldSpace;
  const Material *material = nullptr;

  float u = 0.0f;
  float v = 0.0f;
};

#endif
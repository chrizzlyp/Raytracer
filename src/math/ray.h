#ifndef RAY_H
#define RAY_H

#include "math/vec3.h"

struct Ray {
  Vec3 origin;
  Vec3 direction; //normalized
};

#endif // RAY_H

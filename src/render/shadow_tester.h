#ifndef SHADOW_TESTER_H
#define SHADOW_TESTER_H

#include "render/intersections/hit.h"
#include "math/ray.h"
#include "scene/scene.h"
#include "scene/lights/light.h"
#include "math/vec3.h"

class ShadowTester {
public:
  static bool isInShadow(const Scene& scene, const Vec3& P, const Vec3& N, const Light& light);

private:
  static bool castShadowRay(const Light &light, const Vec3 &P, const Vec3 &N, Ray &shadowRay, float &maxDistanceToLight);
  bool useBVH = false;
};

#endif

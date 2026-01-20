#ifndef SHADOW_TESTER_H
#define SHADOW_TESTER_H

#include "math/ray.h"
#include "math/vec3.h"
#include "render/intersections/hit.h"
#include "scene/lights/light.h"
#include "scene/scene.h"

class ShadowTester {
public:
  static bool isInShadow(const Scene &scene, const Vec3 &P, const Vec3 &N, const Light &light);
  static bool castShadowRayToSpecificPoint(const Vec3 &lightPoint, const Vec3 &P, const Vec3 &N, Ray &shadowRay, float &maxDistanceToLight);
  static float visibilityToAreaLight(const Scene &scene, const Vec3 &P, const Vec3 &N, const AreaLight &light, int samples);
  static bool occluded(const Scene &scene, const Ray &ray, float maxDist);

private:
  static bool castShadowRay(const Light &light, const Vec3 &P, const Vec3 &N, Ray &shadowRay, float &maxDistanceToLight);
};

#endif

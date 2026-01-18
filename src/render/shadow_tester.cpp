#include "render/shadow_tester.h"

#include "scene/lights/parallel_light.h"
#include "scene/lights/point_light.h"
#include <limits>

static constexpr float EPS = 1e-4f;

bool ShadowTester::castShadowRay(const Light &light, const Vec3 &P, const Vec3 &N, Ray &shadowRay, float &maxDistanceToLight) {
  // move the shadow ray along its hitpoint normal direction (outside of the eachSurface)
  Vec3 shadowRayOrigin = P + N * EPS;

  switch (light.type()) {
  case LightType::POINT: {
    auto &pointLight = static_cast<const PointLight &>(light);

    // direction to the light
    Vec3 L = pointLight.position() - shadowRayOrigin;
    float hitDistance = L.length();

    if (hitDistance <= EPS)
      return false; // eachSurface hitpoint is at the same position as the light source

    shadowRay.origin = shadowRayOrigin;
    shadowRay.direction = L / hitDistance; // normalize without normalize() call for performace
    maxDistanceToLight = hitDistance;      // upper bound: only hits closer than the distance to light can cast a shadow (if greater -> hit behind the light)

    return true;
  }

  case LightType::PARALLEL: {
    auto &parallelLight = static_cast<const ParallelLight &>(light);

    // direction to the light = -direction
    Vec3 L = -(parallelLight.direction());

    shadowRay.origin = shadowRayOrigin;
    shadowRay.direction = L;
    maxDistanceToLight = std::numeric_limits<float>::infinity(); // since parallel light is in infinity

    return true;
  }

  default:
    return false;
  }
}

bool ShadowTester::isInShadow(const Scene &scene, const Vec3 &P, const Vec3 &N, const Light &light) {
  Ray shadowRay;
  float maxDistanceToLight = 0.f;

  if (!castShadowRay(light, P, N, shadowRay, maxDistanceToLight))
    return false; // unimplemented lights or light at the same position as the hitpoint -> no shadow

  if (scene.useBVH()) {
    return scene.bvh().occluded(shadowRay, maxDistanceToLight);
  }

  Hit shadowHit;
  shadowHit.distanceClosestIntersection = 1e30f;

  for (const auto &eachSurface : scene.surfaces()) {
    Hit temp = shadowHit;
    if (eachSurface->intersect(shadowRay, temp)) {
      if (temp.distanceClosestIntersection > EPS &&
          temp.distanceClosestIntersection < maxDistanceToLight - EPS) {
        return true;
      }
      if (maxDistanceToLight == std::numeric_limits<float>::infinity() &&
          temp.distanceClosestIntersection > EPS) {
        return true;
      }
    }
  }

  return false;
}

#include "render/shadow_tester.h"

#include "scene/lights/area_light.h"
#include "scene/lights/parallel_light.h"
#include "scene/lights/point_light.h"
#include "scene/lights/spot_light.h"
#include <cstdlib>
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

  case LightType::SPOT: {
    auto &spot = static_cast<const SpotLight &>(light);

    Vec3 L = spot.position() - shadowRayOrigin;
    float hitDistance = L.length();
    if (hitDistance <= EPS)
      return false;

    shadowRay.origin = shadowRayOrigin;
    shadowRay.direction = L / hitDistance;
    maxDistanceToLight = hitDistance;
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
    return false;

  return occluded(scene, shadowRay, maxDistanceToLight);
}

bool ShadowTester::occluded(const Scene &scene, const Ray &ray, float maxDist) {
  if (scene.useBVH()) {
    return scene.bvh().occluded(ray, maxDist);
  }

  Hit shadowHit;
  shadowHit.distanceClosestIntersection = 1e30f;

  for (const auto &eachSurface : scene.surfaces()) {
    Hit temp = shadowHit;
    if (eachSurface->intersect(ray, temp)) {
      if (maxDist == std::numeric_limits<float>::infinity()) {
        if (temp.distanceClosestIntersection > EPS)
          return true;
      } else {
        if (temp.distanceClosestIntersection > EPS &&
            temp.distanceClosestIntersection < maxDist - EPS) {
          return true;
        }
      }
    }
  }
  return false;
}

bool ShadowTester::castShadowRayToSpecificPoint(const Vec3 &lightPoint, const Vec3 &P, const Vec3 &N, Ray &shadowRay, float &maxDistanceToLight) {
  Vec3 origin = P + N * EPS;
  Vec3 L = lightPoint - origin;
  float dist = L.length();
  if (dist <= EPS)
    return false;

  shadowRay.origin = origin;
  shadowRay.direction = L / dist;
  maxDistanceToLight = dist;

  return true;
}

static float rand01() {
  return (float)std::rand() / (float)(RAND_MAX + 1.0f);
}

float ShadowTester::visibilityToAreaLight(const Scene &scene, const Vec3 &P, const Vec3 &N, const AreaLight &light, int samples) {
  int unoccluded = 0;

  for (int i = 0; i < samples; i++) {
    Vec3 lightPoint = light.samplePoint(rand01(), rand01());

    Ray shadowRay;
    float maxDist = 0.f;
    if (!castShadowRayToSpecificPoint(lightPoint, P, N, shadowRay, maxDist))
      continue;

    if (!occluded(scene, shadowRay, maxDist))
      unoccluded++;
  }

  return (samples > 0) ? (float)unoccluded / (float)samples : 1.0f;
}

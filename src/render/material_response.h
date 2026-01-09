#ifndef MATERIAL_RESPONSE_H
#define MATERIAL_RESPONSE_H

#include "math/vec3.h"
#include <algorithm>
#include <cmath>

class MaterialResponse {
public:
  static Vec3 reflect(const Vec3 &I, const Vec3 &N) {
    return I - 2.0f * dot(I, N) * N;
  }

  // refraction (Snell)
  // I: incident direction (ray to the hitpoint)
  // N: surface normal
  // etaI: IOR of incident medium (e.g., air = 1.0)
  // etaT: IOR of transmitted medium (e.g., glass = 1.5)
  static bool refract(const Vec3 &I, const Vec3 &N, float etaI, float etaT, Vec3 &refractedDirection) {
    Vec3 incidentDirection = I.normalized();
    Vec3 surfaceNormal = N.normalized();

    // Orient normal so it faces against the incident ray
    Vec3 n = (dot(incidentDirection, surfaceNormal) < 0.0f) ? surfaceNormal : -surfaceNormal;

    // cos(theta1) should be positive
    float cos_angleIncidence = std::clamp(-dot(incidentDirection, n), 0.0f, 1.0f);

    float eta = etaI / etaT;

    // k = 1 - eta^2 * (1 - cos^2(theta1))
    float k = 1.0f - eta * eta * (1.0f - cos_angleIncidence * cos_angleIncidence);
    if (k < 0.0f)
      return false; // Total Internal Reflection

    float cos_angleTransmitted = std::sqrt(k);

    refractedDirection = eta * incidentDirection + (eta * cos_angleIncidence - cos_angleTransmitted) * n;

    refractedDirection = refractedDirection.normalized();
    return true;
  }
};

#endif

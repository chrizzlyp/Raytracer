#include "render/shader.h"
#include "render/shadow_tester.h"
#include "scene/lights/area_light.h"
#include "scene/lights/spot_light.h"
#include "scene/surfaces/components/texture.h"
#include "textures/texture_manager.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

static float rand01() {
  return (float)std::rand() / (float)(RAND_MAX + 1.0f);
}

Color Shader::shadePhong(const Scene &scene, const Hit &hit) {
  const Material &materialSurface = *hit.material;

  const Vec3 P = hit.positionWorldSpace; // hitpoint on the surface
  Vec3 N = hit.normalWorldSpace;         // normal vector on the hitpoint
  const Vec3 V = (scene.camera().position() - P).normalized();

  if (materialSurface.isBumped()) {
    N = applyBumpMapping(scene, hit, N);
  }

  Color resultColor(0.f, 0.f, 0.f);
  resultColor += shadeAmbient(scene, hit);
  resultColor += shadeDiffuse(scene, hit, N);
  resultColor += shadeSpecular(scene, materialSurface, P, N, V);

  return resultColor;
}

Color Shader::shadeAmbient(const Scene &scene, const Hit &hit) {
  Color result(0.f, 0.f, 0.f);

  if (!scene.ambientLight().has_value())
    return result; // no ambient light :(

  const Material &material = *hit.material;
  const PhongParams &phongParams = material.phong();
  Color surfaceColor = material.color();
  const Color &lightColor = scene.ambientLight()->color();

  // if material is textured use texture instead of color
  if (material.isTextured()) {
    const Texture &texture = scene.textureManager().getTexture(material.textureName());
    surfaceColor = texture.sampleColor(hit.u, hit.v);
  }

  result += phongParams.kAmbient * lightColor * surfaceColor;
  return result;
}

Color Shader::shadeDiffuse(const Scene &scene, const Hit &hit, const Vec3 &N) {
  Color result(0.f, 0.f, 0.f);

  const Material &material = *hit.material;
  Vec3 P = hit.positionWorldSpace;
  const PhongParams &phongParams = material.phong();
  Color surfaceColor = material.color();

  // if material is texured use texture instead of color
  if (material.isTextured()) {
    const Texture &texture = scene.textureManager().getTexture(material.textureName());
    surfaceColor = texture.sampleColor(hit.u, hit.v);
  }

  for (const auto &eachLightPtr : scene.lights()) {
    const Light &light = *eachLightPtr;

    // area light (soft shadows)
    if (light.type() == LightType::AREA) {
      const auto &area = static_cast<const AreaLight &>(light);

      const int samples = 16;
      float ndotlSum = 0.0f;

      for (int i = 0; i < samples; i++) {
        Vec3 lightPoint = area.samplePoint(rand01(), rand01());

        Vec3 L = lightPoint - P;
        float dist = L.length();
        if (dist <= 1e-6f)
          continue;
        L = L/dist;

        Ray shadowRay;
        float maxDist = 0.f;
        if (!ShadowTester::castShadowRayToSpecificPoint(lightPoint, P, N, shadowRay, maxDist))
          continue;

        if (ShadowTester::occluded(scene, shadowRay, maxDist))
          continue;

        ndotlSum += std::max(0.0f, dot(N, L));
      }

      float ndotlAvg = (samples > 0) ? (ndotlSum / (float)samples) : 0.0f;
      result += phongParams.kDiffuse * light.color() * ndotlAvg * surfaceColor;
      continue;
    }

    // point/parallel/spot light (hard shadows)
    Vec3 L;
    if (!calculateDirectionFromPointToLight(light, P, L))
      continue;

    // SPOT: angular falloff (0..1)
    float spotAtt = 1.0f;
    if (light.type() == LightType::SPOT) {
      const auto &spot = static_cast<const SpotLight &>(light);
      Vec3 dirLightToPoint = (P - spot.position()).normalized(); // from light -> point
      spotAtt = spot.spotAttenuation(dirLightToPoint);
      if (spotAtt <= 0.0f)
        continue; // outside cone
    }

    if (ShadowTester::isInShadow(scene, P, N, light))
      continue;

    float ndotl = std::max(0.0f, dot(N, L));
    result += phongParams.kDiffuse * light.color() * (ndotl * spotAtt) * surfaceColor;
  }
  return result;
}

Color Shader::shadeSpecular(const Scene &scene, const Material &material, const Vec3 &P, const Vec3 &N, const Vec3 &V) {
  Color result(0.f, 0.f, 0.f);

  const PhongParams &phongParams = material.phong();
  if (phongParams.kSpecular <= 0.f)
    return result;

  for (const auto &eachLightPtr : scene.lights()) {
    const Light &light = *eachLightPtr;

    // area light (soft shadows)
    if (light.type() == LightType::AREA) {
      const auto &area = static_cast<const AreaLight &>(light);

      const int samples = 16;
      float specSum = 0.0f;

      for (int i = 0; i < samples; i++) {
        Vec3 lightPoint = area.samplePoint(rand01(), rand01());

        Vec3 L = lightPoint - P;
        float dist = L.length();
        if (dist <= 1e-6f)
          continue;
        L = L/dist;

        Ray shadowRay;
        float maxDist = 0.f;
        if (!ShadowTester::castShadowRayToSpecificPoint(lightPoint, P, N, shadowRay, maxDist))
          continue;

        if (ShadowTester::occluded(scene, shadowRay, maxDist))
          continue;

        Vec3 R = reflectVec(-L, N).normalized();
        float rdotv = std::max(0.0f, dot(R, V));
        specSum += std::pow(rdotv, phongParams.exponentShininess);
      }

      float specAvg = (samples > 0) ? (specSum / (float)samples) : 0.0f;
      result += phongParams.kSpecular * light.color() * specAvg;
      continue;
    }

    // point/parallel/spot light (hard shadows)
    Vec3 L;
    if (!calculateDirectionFromPointToLight(light, P, L))
      continue;

    // angular falloff (0..1)
    float spotAtt = 1.0f;
    if (light.type() == LightType::SPOT) {
      const auto &spot = static_cast<const SpotLight &>(light);
      Vec3 dirLightToPoint = (P - spot.position()).normalized();
      spotAtt = spot.spotAttenuation(dirLightToPoint);
      if (spotAtt <= 0.0f)
        continue;
    }

    if (ShadowTester::isInShadow(scene, P, N, light))
      continue;

    Vec3 R = reflectVec(-L, N).normalized();
    float rdotv = std::max(0.0f, dot(R, V));
    float specular = std::pow(rdotv, phongParams.exponentShininess);

    result += phongParams.kSpecular * light.color() * (specular * spotAtt);
  }

  return result;
}

bool Shader::calculateDirectionFromPointToLight(const Light &light, const Vec3 &P, Vec3 &outL) {
  switch (light.type()) {
  case LightType::POINT: {
    const auto &pointLight = static_cast<const PointLight &>(light);
    outL = (pointLight.position() - P).normalized();
    return true;
  }
  case LightType::PARALLEL: {
    const auto &parallelLight = static_cast<const ParallelLight &>(light);
    // incoming light direction (-direction)
    outL = (-parallelLight.direction()).normalized();
    return true;
  }
  case LightType::SPOT: {
    const auto &spot = static_cast<const SpotLight &>(light);
    outL = (spot.position() - P).normalized();
    return true;
  }
  default:
    return false; // unimplemented spot light
  }
}

Vec3 Shader::reflectVec(const Vec3 &minusL, const Vec3 &N) {
  return minusL - 2.0f * dot(minusL, N) * N;
}

Vec3 Shader::applyBumpMapping(const Scene &scene, const Hit &hit, const Vec3 &N) {
  const Material &material = *hit.material;

  const TextureManager &textureManager = scene.textureManager();
  const Texture &heightMapTexture = textureManager.getTexture(material.bumpMapName());

  const float u = hit.u;
  const float v = hit.v;

  // offset for getting neightbour texel height
  const float uOffset = 1e-3f;
  const float vOffset = 1e-3f;

  const float heightLeft = heightMapTexture.sampleHeight(u - uOffset, v);
  const float heightRight = heightMapTexture.sampleHeight(u + uOffset, v);
  const float heightDown = heightMapTexture.sampleHeight(u, v - vOffset);
  const float heightUp = heightMapTexture.sampleHeight(u, v + vOffset);

  // differences (gradient)
  float uHeightGradient = (heightRight - heightLeft) * 0.5f;
  float vHeightGradient = (heightUp - heightDown) * 0.5f;

  const float strength = material.bumpStrength();
  uHeightGradient *= strength;
  vHeightGradient *= strength;

  // pick a helper vector not parallel to N
  Vec3 helper = (std::abs(N.y) < 0.999f) ? Vec3(0.f, 1.f, 0.f) : Vec3(1.f, 0.f, 0.f);

  Vec3 T = cross(helper, N).normalized();
  Vec3 B = cross(N, T).normalized();

  // perturb the normal in tangent directions
  Vec3 bumpedN = (N + uHeightGradient * T + vHeightGradient * B).normalized();

  // keep bumped normal on the same hemisphere as original
  if (dot(bumpedN, N) < 0.0f)
    bumpedN = -bumpedN;

  return bumpedN;
}

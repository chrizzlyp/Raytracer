#include "render/shader.h"
#include "render/shadow_tester.h"
#include "scene/surfaces/components/texture.h"
#include "textures/texture_manager.h"

#include <algorithm>
#include <cmath>

Color Shader::shadePhong(const Scene &scene, const Hit &hit) {
  const Material &materialSurface = *hit.material;

  const Vec3 P = hit.positionWorldSpace; // hitpoint on the surface
  const Vec3 N = hit.normalWorldSpace;   // normal vector on the hitpoint
  const Vec3 V = (scene.camera().position() - P).normalized();

  Color resultColor(0.f, 0.f, 0.f);

  resultColor += shadeAmbient(scene, hit);
  resultColor += shadeDiffuse(scene, hit);
  resultColor += shadeSpecular(scene, materialSurface, P, N, V);

  return resultColor;
}

Color Shader::shadeAmbient(const Scene &scene, const Hit &hit) {
  Color result(0.f, 0.f, 0.f);

  if (!scene.ambientLight().has_value())
    return result; // no ambient light :(

  const Material &mat = *hit.material;
  const PhongParams &phongParams = mat.phong();
  Color surfaceColor = mat.color();
  const Color &lightColor = scene.ambientLight()->color();

  // if material is textured use texture instead of color
  if (mat.isTextured()) {
    const Texture &texture = scene.textureManager().getTexture(mat.textureName());
    surfaceColor = texture.sampleColor(hit.u, hit.v);
  }

  result += phongParams.kAmbient * lightColor * surfaceColor;
  return result;
}

Color Shader::shadeDiffuse(const Scene &scene, const Hit& hit) {
  Color result(0.f, 0.f, 0.f);

  const Material& mat = *hit.material;
  Vec3 P = hit.positionWorldSpace;
  Vec3 N = hit.normalWorldSpace;
  const PhongParams &phongParams = mat.phong();
  Color surfaceColor = mat.color();

  // if material is texured use texture instead of color
  if (mat.isTextured()) {
    const Texture &texture = scene.textureManager().getTexture(mat.textureName());
    surfaceColor = texture.sampleColor(hit.u, hit.v);
  }

  for (const auto &eachLightPtr : scene.lights()) {
    const Light &light = *eachLightPtr;

    if (ShadowTester::isInShadow(scene, P, N, light))
      continue; // skip if in shadow

    Vec3 L;
    if (!calculateDirectionFromPointToLight(light, P, L))
      continue; // skip unimplemented spot light

    float ndotl = std::max(0.0f, dot(N, L));
    result += phongParams.kDiffuse * light.color() * ndotl * surfaceColor;
  }

  return result;
}

Color Shader::shadeSpecular(const Scene &scene, const Material &mat, const Vec3 &P, const Vec3 &N, const Vec3 &V) {
  Color result(0.f, 0.f, 0.f);

  const PhongParams &phongParams = mat.phong();
  if (phongParams.kSpecular <= 0.f)
    return result;

  for (const auto &eachLightPtr : scene.lights()) {
    const Light &light = *eachLightPtr;

    if (ShadowTester::isInShadow(scene, P, N, light))
      continue; // skip if in shadow

    Vec3 L;
    if (!calculateDirectionFromPointToLight(light, P, L))
      continue; // skip unimplemented spot light

    // R = reflect(-L, N) -> thats why again -L
    Vec3 minusL = -L;
    Vec3 R = reflectVec(minusL, N).normalized();

    float rdotv = std::max(0.0f, dot(R, V));
    float specular = std::pow(rdotv, phongParams.exponentShininess);

    result += phongParams.kSpecular * light.color() * specular;
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
  default:
    return false; // unimplemented spot light
  }
}

Vec3 Shader::reflectVec(const Vec3 &minusL, const Vec3 &N) {
  return minusL - 2.0f * dot(minusL, N) * N;
}
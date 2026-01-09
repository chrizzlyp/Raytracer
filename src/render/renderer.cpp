#include "render/renderer.h"
#include "render/intersections/hit.h"
#include "render/material_response.h"
#include "render/ray_generator.h"
#include "render/shader.h"

#include <algorithm>
#include <stdexcept>

static constexpr float EPS = 1e-2f;

bool Renderer::renderToPNG(const Scene &scene, RenderMode mode) const {
  Framebuffer framebuffer = this->render(scene, mode);
  return this->writer.writePNG(framebuffer, scene.outputFileName());
}

Framebuffer Renderer::render(const Scene &scene, RenderMode mode) const {
  switch (mode) {
  case RenderMode::DebugRayDirection:
    return renderDebugRayDirection(scene);
  case RenderMode::Normal:
    return renderNormal(scene);
  case RenderMode::Realistic:
    return renderRealistic(scene);
  default:
    throw std::runtime_error("Renderer::render(): unsupported RenderMode");
  }
}

Framebuffer Renderer::renderDebugRayDirection(const Scene &scene) const {
  const Camera &camera = scene.camera();
  const int widthPx = camera.resHorizontal();
  const int heightPx = camera.resVertical();

  Framebuffer framebuffer(widthPx, heightPx);
  RayGenerator rayGenerator(camera);

  // prevent floats > 1.0
  auto saturate = [](float x) { return std::clamp(x, 0.0f, 1.0f); };

  for (int y = 0; y < heightPx; ++y) {
    for (int x = 0; x < widthPx; ++x) {
      Ray primaryRay = rayGenerator.makePrimaryRay(x, y);
      Vec3 primaryRayDirection = primaryRay.direction;

      // convert ray[-1,+1] -> color = [0,1]
      float r = saturate((primaryRayDirection.x + 1.0f) / 2.0f);
      float g = saturate((primaryRayDirection.y + 1.0f) / 2.0f);
      float b = saturate((primaryRayDirection.z + 1.0f) / 2.0f);

      framebuffer.setPixel(x, y, Color(r, g, b));
    }
  }

  return framebuffer;
}

Framebuffer Renderer::renderNormal(const Scene &scene) const {
  const Camera &camera = scene.camera();
  const int widthPx = camera.resHorizontal();
  const int heightPx = camera.resVertical();

  Framebuffer framebuffer(widthPx, heightPx);
  RayGenerator rayGenerator(camera);

  for (int y = 0; y < heightPx; ++y) {
    for (int x = 0; x < widthPx; ++x) {
      Ray ray = rayGenerator.makePrimaryRay(x, y);

      Hit hit;
      bool anyHit = false;

      for (const auto &surface : scene.surfaces()) {
        if (surface->intersect(ray, hit)) {
          anyHit = true;
        }
      }

      if (!anyHit) {
        framebuffer.setPixel(x, y, scene.backgroundColor());
        continue;
      }

      // Normal [-1,1] -> Color [0,1]
      Vec3 hitpointNormal = hit.normalWorldSpace.normalized();
      Color color((hitpointNormal.x + 1.0f) * 0.5f, (hitpointNormal.y + 1.0f) * 0.5f, (hitpointNormal.z + 1.0f) * 0.5f);

      framebuffer.setPixel(x, y, color);
    }
  }

  return framebuffer;
}

Framebuffer Renderer::renderRealistic(const Scene &scene) const {
  const Camera &camera = scene.camera();
  const int widthPx = camera.resHorizontal();
  const int heightPx = camera.resVertical();

  Framebuffer framebuffer(widthPx, heightPx);
  RayGenerator rayGenerator(camera);

  auto saturate = [](float x) {
    return std::clamp(x, 0.0f, 1.0f);
  };

  auto saturateColor = [&](const Color &c) {
    return Color(saturate(c.x), saturate(c.y), saturate(c.z));
  };

  for (int y = 0; y < heightPx; ++y) {
    for (int x = 0; x < widthPx; ++x) {
      Ray ray = rayGenerator.makePrimaryRay(x, y);
      Color resultColor = trace(scene, ray, 4, 1.0f);
      framebuffer.setPixel(x, y, saturateColor(resultColor));
    }
  }

  return framebuffer;
}

Color Renderer::trace(const Scene &scene, const Ray &ray, int depth, float currentIor) const {
  if (depth <= 0)
    return Color(0.f, 0.f, 0.f); // stop tracing could end in endless loop

  Hit hit;
  if (!findClosestHit(scene, ray, hit))
    return scene.backgroundColor();

  const Material &material = *hit.material;

  Vec3 P = hit.positionWorldSpace;
  Vec3 N = hit.normalWorldSpace.normalized();
  Vec3 I = ray.direction.normalized();

  // local illumination
  Color localIllumination = Shader::shadePhong(scene, hit);

  // global illumination
  float kReflectance = material.reflectance();
  float kTransmittance = material.transmittance();

  // if the material is not reflective and not transmissive, local shading is final
  if (kReflectance <= 0.f && kTransmittance <= 0.f)
    return localIllumination;

  Color reflected(0.f, 0.f, 0.f);
  Color refracted(0.f, 0.f, 0.f);

  // reflection
  if (kReflectance > 0.f) {
    Vec3 reflectedDirection = MaterialResponse::reflect(I, N);
    Ray reflectedRay = makeRayFromSurface(P, N, reflectedDirection);
    reflected = trace(scene, reflectedRay, depth - 1, currentIor);
  }

  // refraction
  if (kTransmittance > 0.f) {
    // next medium after crossing the surface
    float etaI, etaT;
    computeRefractionIors(material, I, N, currentIor, etaI, etaT);

    Vec3 refractedDirection;
    if (MaterialResponse::refract(I, N, etaI, etaT, refractedDirection)){
      Ray refractedRay = makeRayFromSurface(P, N, refractedDirection);

      refracted = trace(scene, refractedRay, depth - 1, etaT);
    } else {
      // reflection
      Vec3 reflectedDirection = MaterialResponse::reflect(I, N);
      Ray reflectedRay = makeRayFromSurface(P, N, reflectedDirection);
      reflected = trace(scene, reflectedRay, depth - 1, currentIor);

      if (kReflectance <= 0.f)
        kReflectance = 1.0f;
      kTransmittance = 0.f;
    }
  }

  float base = std::max(0.0f, 1.0f - kReflectance - kTransmittance);
  return localIllumination * base + reflected * kReflectance + refracted * kTransmittance;
}

bool Renderer::findClosestHit(const Scene &scene, const Ray &ray, Hit &outHit) const {

  bool anyHit = false;
  for (const auto &surface : scene.surfaces()) {
    if (surface->intersect(ray, outHit))
      anyHit = true;
  }
  return anyHit;
}

Ray Renderer::makeRayFromSurface(const Vec3 &P, const Vec3 &N, const Vec3 &dir) const {
  Vec3 d = dir.normalized();
  Vec3 origin = (dot(d, N) >= 0.0f) ? (P + N * EPS) : (P - N * EPS);
  return Ray{origin, d};
}

void Renderer::computeRefractionIors(const Material &material, const Vec3 &I, const Vec3 &N, float currentIor, float &etaI, float &etaT) const {

  etaI = currentIor;

  // if dot(I,N) < 0: Ray from outside and goes inside
  // if dot(I,N) > 0: Ray is inside and goes outside
  if (dot(I, N) < 0.0f) {
    etaT = material.ior();
  } else {
    etaT = 1.0f; // air
  }
}
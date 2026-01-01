#include "render/renderer.h"
#include "render/ray_generator.h"
#include <algorithm>
#include <stdexcept>
#include "render/intersections/hit.h"

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

Framebuffer Renderer::renderNormal(const Scene& scene) const {
  const Camera& camera = scene.camera();
  const int widthPx  = camera.resHorizontal();
  const int heightPx = camera.resVertical();

  Framebuffer framebuffer(widthPx, heightPx);
  RayGenerator rayGenerator(camera);

  for (int y = 0; y < heightPx; ++y) {
    for (int x = 0; x < widthPx; ++x) {
      Ray ray = rayGenerator.makePrimaryRay(x, y);

      Hit hit;           
      bool anyHit = false;

      for (const auto& surface : scene.surfaces()) {
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
      Color color((hitpointNormal.x + 1.0f) * 0.5f, (hitpointNormal.y + 1.0f) * 0.5f,  (hitpointNormal.z + 1.0f) * 0.5f);

      framebuffer.setPixel(x, y, color);
    }
  }

  return framebuffer;
}

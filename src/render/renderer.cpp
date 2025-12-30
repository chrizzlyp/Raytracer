#include "render/renderer.h"
#include "render/ray_generator.h"
#include <algorithm>
#include <stdexcept>

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

Framebuffer Renderer::renderNormal(const Scene &scene) const {
  // Platzhalter: später Intersection + Normal
  const Camera &camera = scene.camera();
  Framebuffer framebuffer(camera.resHorizontal(), camera.resVertical());
  return framebuffer;
}

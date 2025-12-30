#ifndef RENDERER_H
#define RENDERER_H

#include "image-writer/image8.h"
#include "image-writer/image_writer.h"
#include "math/color.h"
#include "render/framebuffer.h"
#include "scene/scene.h"

enum class RenderMode {
  DebugRayDirection,
  Normal
};

class Renderer {
public:
  Renderer() = default;
  bool renderToPNG(const Scene &scene, RenderMode mode) const;

private:
  Framebuffer render(const Scene &scene, RenderMode mode) const;
  Framebuffer renderDebugRayDirection(const Scene &scene) const;
  Framebuffer renderNormal(const Scene &scene) const;

private:
  ImageWriter writer;
};

#endif

#ifndef RENDERER_H
#define RENDERER_H

#include "image-writer/image8.h"
#include "image-writer/image_writer.h"
#include "math/color.h"
#include "render/framebuffer.h"
#include "scene/scene.h"
#include "render/intersections/bvh/scene_bvh.h"

enum class RenderMode {
  DebugRayDirection,
  Normal,
  Realistic
};

class Renderer {
public:
  Renderer() = default;
  bool renderToPNG(const Scene &scene, RenderMode mode) const;

private:
  Framebuffer render(const Scene &scene, RenderMode mode) const;
  Framebuffer renderDebugRayDirection(const Scene &scene) const;
  Framebuffer renderNormal(const Scene &scene) const;
  Framebuffer renderRealistic(const Scene &scene) const;
  Color trace(const Scene &scene, const Ray &ray, int depth, float currentIor) const;

  void computeRefractionIors(const Material &mat, const Vec3 &I, const Vec3 &N, float currentIor, float &etaI, float &etaT) const;
  Ray makeRayFromSurface(const Vec3& P, const Vec3& N, const Vec3& dir) const;
  bool findClosestHit(const Scene& scene, const Ray& ray, Hit& outHit) const;

private:
  ImageWriter writer;
  mutable SceneBVH bvh;
  bool useBVH = false;
};

#endif

#include "render/ray_generator.h"
#include "math/angle.h"

RayGenerator::RayGenerator(const Camera &cam) {
  this->imageWidthPx = cam.resHorizontal();
  this->imageHeightPx = cam.resVertical();

  // viewplane position based on camera
  this->planeOrigin = cam.position();
  this->planeForward = (cam.lookat() - cam.position()).normalized();
  this->planeRight = cross(this->planeForward, cam.up()).normalized();
  this->planeUp = cross(this->planeRight, this->planeForward);

  float fovx = degToRad(cam.horizontalFovHalfAngle());
  const float aspect = static_cast<float>(this->imageWidthPx) / static_cast<float>(this->imageHeightPx);

  // half viewplane size ( with z=|1| )
  this->tan_fovx = std::tan(fovx);
  this->tan_fovy = this->tan_fovx / aspect;

  // precompute constant steps on the viewplane:
  this->dx = (2.0f / static_cast<float>(this->imageWidthPx)) * this->tan_fovx;
  this->dy = (2.0f / static_cast<float>(this->imageHeightPx)) * this->tan_fovy;

  this->x0 = ((1.0f / static_cast<float>(this->imageWidthPx)) - 1.0f) * this->tan_fovx;
  this->y0 = ((1.0f / static_cast<float>(this->imageHeightPx)) - 1.0f) * this->tan_fovy;
}

Ray RayGenerator::makePrimaryRay(int u, int v) const {

  // Get viewplane coordinates using cached increments
  const float x = this->x0 + static_cast<float>(u) * this->dx;
  const float y = this->y0 + static_cast<float>((this->imageHeightPx - 1) - v) * this->dy;

  // build the ray direction by moving from the camera forward direction to the pixel on the viewplane
  Vec3 rayDirection = (this->planeRight * x + this->planeUp * y + this->planeForward).normalized();
  return Ray{this->planeOrigin, rayDirection};
}

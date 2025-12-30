#ifndef RAY_GENERATOR_H
#define RAY_GENERATOR_H

#include "math/ray.h"
#include "math/vec3.h"
#include "scene/camera.h"

class RayGenerator {
public:
  explicit RayGenerator(const Camera &cam);

  // (u,v): (0,0) = top-left, Ray goes through pixel center
  Ray makePrimaryRay(int u, int v) const;

private:
  int imageWidthPx = 0;
  int imageHeightPx = 0;

  float tan_fovx = 0.0f; // image plane half wight at |z| = 1 and given fov_x -> GK = tan(fov_x)/1
  float tan_fovy = 0.0f; // image plane half hight at |z| = 1 and given fov_y -> GK = tan(fov_y)/1

  Vec3 planeOrigin;
  Vec3 planeForward;
  Vec3 planeRight;
  Vec3 planeUp;

  // viewplane stepping
  float x0 = 0.0f; // viewplane x at u = 0 (pixel center)
  float y0 = 0.0f; // viewplane y at v = 0 (pixel center)
  float dx = 0.0f; // delta x when u increases by 1
  float dy = 0.0f; // delta y when v increases by 1

};

#endif // RAY_GENERATOR_H

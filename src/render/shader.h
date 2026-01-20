#ifndef SHADER_H
#define SHADER_H

#include "math/color.h"
#include "render/intersections/hit.h"
#include "scene/camera.h"
#include "scene/lights/utils/lights.h"
#include "scene/scene.h"

class Shader {
public:
  // Ambient + Lambert diffuse (Point + Parallel)
  static Color shadePhong(const Scene &scene, const Hit &hit);

private:
  static Color shadeAmbient(const Scene &scene, const Hit &hit);
  static Color shadeDiffuse(const Scene &scene, const Hit &hit, const Vec3& N);
  static Color shadeSpecular(const Scene &scene, const Material &mat, const Vec3 &P, const Vec3 &N, const Vec3 &V);

  static Vec3 applyBumpMapping(const Scene &scene, const Hit &hit, const Vec3 &geometricNormal);

  static bool calculateDirectionFromPointToLight(const Light &light, const Vec3 &P, Vec3 &outL);

  static Vec3 reflectVec(const Vec3 &I, const Vec3 &N);
};

#endif

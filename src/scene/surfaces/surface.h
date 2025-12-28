#ifndef SURFACE_H
#define SURFACE_H

#include "scene/surfaces/components/material.h"
#include "scene/surfaces/components/transform.h"

enum class SurfaceType {
  SPHERE,
  MESH
};

class Surface {
public:
  virtual ~Surface() = default;
  virtual SurfaceType type() const = 0;
  void setMaterial(Material material) {
    material_ = std::move(material);
  }
  void setTransform(const Transform &transform) {
    transform_ = transform;
  }
  const Material &material() const {
    return material_;
  }
  const Transform &transform() const {
    return transform_;
  }

protected:
  Material material_;
  Transform transform_;
};

#endif
#ifndef SURFACE_H
#define SURFACE_H

#include "scene/surfaces/material.h"
#include "scene/surfaces/transform.h"

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

protected:
  Material material_;
  Transform transform_;
};

#endif
#ifndef SURFACE_IO_H
#define SURFACE_IO_H

#include <ostream>
#include <stdexcept>

#include "scene/surfaces/mesh.h"
#include "scene/surfaces/sphere.h"
#include "scene/surfaces/surface.h"

inline std::ostream &operator<<(std::ostream &os, const Surface &s) {
  switch (s.type()) {
  case SurfaceType::SPHERE:
    return os << static_cast<const Sphere &>(s);
  case SurfaceType::MESH:
    return os << static_cast<const Mesh &>(s);
  default:
    throw std::runtime_error("Unknown SurfaceType");
  }
}

#endif
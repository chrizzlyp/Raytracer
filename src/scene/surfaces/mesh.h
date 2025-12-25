#ifndef MESH_H
#define MESH_H

#include "math/vec3.h"
#include "scene/surfaces/surface.h"
#include <ostream>
#include <vector>

struct TrianglePrimitive {
  Vec3 v0, v1, v2;
  Vec3 n0, n1, n2;
  Vec3 uv0, uv1, uv2;
};

class Mesh : public Surface {
public:
  SurfaceType type() const override {
    return SurfaceType::MESH;
  }
  void setTrianglePrimitives(std::vector<TrianglePrimitive> trianglePrimitives) {
    trianglePrimitives_ = std::move(trianglePrimitives);
  }

  const std::vector<TrianglePrimitive>& triangles() const {
  return trianglePrimitives_;
}

private:
  std::vector<TrianglePrimitive> trianglePrimitives_;
};

inline std::ostream &operator<<(std::ostream &os, const Mesh &m) {
  os << "Mesh{triangles=" << m.triangles().size() << "}";
  return os;
}

#endif
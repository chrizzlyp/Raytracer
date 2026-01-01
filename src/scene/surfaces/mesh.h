#ifndef MESH_H
#define MESH_H

#include "math/vec3.h"
#include "scene/surfaces/surface.h"
#include <ostream>
#include <vector>
#include "render/intersections/hit.h"

struct TrianglePrimitive {
  Vec3 v0, v1, v2;
  Vec3 n0, n1, n2;
  Vec3 uv0, uv1, uv2;
};

inline std::ostream &operator<<(std::ostream &os, const TrianglePrimitive &t) {
  os << "TrianglePrimitive{"
     << "v0=" << t.v0 << ", v1=" << t.v1 << ", v2=" << t.v2
     << ", n0=" << t.n0 << ", n1=" << t.n1 << ", n2=" << t.n2
     << ", uv0=" << t.uv0 << ", uv1=" << t.uv1 << ", uv2=" << t.uv2
     << "}";
  return os;
}

class Mesh : public Surface {
public:
  SurfaceType type() const override {
    return SurfaceType::MESH;
  }
  void setTrianglePrimitives(std::vector<TrianglePrimitive> trianglePrimitives) {
    trianglePrimitives_ = std::move(trianglePrimitives);
  }

  const std::vector<TrianglePrimitive> &triangles() const {
    return trianglePrimitives_;
  }

   bool intersect(const Ray& rayWorld, Hit& hit) const override;

private:
  std::vector<TrianglePrimitive> trianglePrimitives_;
};

inline std::ostream &operator<<(std::ostream &os, const Mesh &m) {
  os << "Mesh{triangles=" << m.triangles().size();

  const auto &trianglePrimitives = m.triangles();
  constexpr std::size_t maxPrint = 3;

  if (!trianglePrimitives.empty()) {
    os << ", Triangle primitives=[";
    for (std::size_t i = 0; i < trianglePrimitives.size() && i < maxPrint; ++i) {
      if (i)
        os << ", ";
      os << trianglePrimitives[i];
    }
    if (trianglePrimitives.size() > maxPrint)
      os << ", ...";
    os << "]";
  }

  os << ", material=" << m.material() << ", transform=\n" << m.transform() << "}";
  return os;
}

#endif
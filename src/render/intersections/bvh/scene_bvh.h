#ifndef SCENE_BVH_H
#define SCENE_BVH_H

#include <vector>

#include "math/ray.h"
#include "math/vec3.h"
#include "render/intersections/hit.h"
#include "scene/surfaces/surface.h"

class Scene; 

struct SurfaceReference {
  const Surface *surface = nullptr;
  AABB boundingVolume;
  Vec3 boundingVolumeCenter;
};

struct BVHNode {
  AABB boundingBox;
  int left = -1;
  int right = -1;

  int indexFirstSurfaceInside = 0;
  int countSurfacesInside = 0;

  bool isLeaf() const {
    return countSurfacesInside > 0;
  }
};

class SceneBVH {
public:
  void build(const Scene &scene);
  bool intersect(const Ray& ray, Hit& hit) const;
  bool occluded(const Ray& ray, float maxDistanceToLight) const;

private:
  void buildSurfaceReferences(const Scene &scene);
  int buildBvhNode(int begin, int end);

private:
  static constexpr int LEAF_SIZE = 4;

  std::vector<SurfaceReference> surfaceReferences;
  std::vector<int> surfaceReferenceIndices;
  std::vector<BVHNode> bvhNodes;
};

#endif
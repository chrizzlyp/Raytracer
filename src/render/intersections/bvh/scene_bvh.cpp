#include "render/intersections/bvh/scene_bvh.h"
#include "render/intersections/bvh/aabb.h"
#include "render/intersections/bvh/sah_splitter.h"
#include "scene/scene.h"

#include <algorithm>
#include <cassert>

static constexpr float EPS = 1e-4f;

void SceneBVH::build(const Scene &scene) {
  buildSurfaceReferences(scene);

  surfaceReferenceIndices.clear();
  bvhNodes.clear();

  surfaceReferenceIndices.resize(surfaceReferences.size());
  for (int i = 0; i < (int)surfaceReferenceIndices.size(); ++i)
    surfaceReferenceIndices[i] = i;

  if (!surfaceReferenceIndices.empty())
    buildBvhNode(0, surfaceReferenceIndices.size());
}

void SceneBVH::buildSurfaceReferences(const Scene &scene) {
  surfaceReferences.clear();
  surfaceReferences.reserve(scene.surfaces().size());

  for (const auto &eachSurface : scene.surfaces()) {
    SurfaceReference reference;
    reference.surface = eachSurface.get();
    reference.boundingVolume = eachSurface->getWorldBoundingVolume();
    reference.boundingVolumeCenter = (reference.boundingVolume.min() + reference.boundingVolume.max()) * 0.5f;

    surfaceReferences.push_back(reference);
  }
}

int SceneBVH::buildBvhNode(int begin, int end) {
  assert(begin < end);

  AABB nodeBoundingVolume;   // includes child bounding volumes
  AABB centerBoundingVolume; // box arround all centeres

  // build bounding boxes for this bvhNode
  for (int i = begin; i < end; ++i) {
    int referenceIndex = surfaceReferenceIndices[i];
    const SurfaceReference &reference = surfaceReferences[referenceIndex];

    nodeBoundingVolume.includeBox(reference.boundingVolume);
    centerBoundingVolume.includePoint(reference.boundingVolumeCenter);
  }

  // create bvhNode and set its bounding box containing all surfaces in range
  BVHNode bvhNode;
  bvhNode.boundingBox = nodeBoundingVolume;

  // amount of surfaces in range
  const int count = end - begin;

  // check if less enough surfaces so its already a leaf
  if (count <= LEAF_SIZE) {
    bvhNode.indexFirstSurfaceInside = begin;
    bvhNode.countSurfacesInside = count;

    // if yes save those bounding volumes
    int nodeIndex = bvhNodes.size();
    bvhNodes.push_back(bvhNode);
    return nodeIndex;
  }

  // SAH split
  SAHSplitter splitter(surfaceReferences, surfaceReferenceIndices, 12);
  SAHSplit split = splitter.findBestSplit(begin, end, centerBoundingVolume);

  // if SAH found no split -> stop here (make leaf)
  if (split.splitAxis == -1) {
    bvhNode.indexFirstSurfaceInside = begin;
    bvhNode.countSurfacesInside = count;
    int nodeIndex = (int)bvhNodes.size();
    bvhNodes.push_back(bvhNode);
    return nodeIndex;
  }

  // rearranges the index range into left/right groups based on each surface'currentSurface bounding-box center along the split axis,
  // and returns the split position (iterator) where the right group begins.
  auto midIterator = std::partition(surfaceReferenceIndices.begin() + begin, surfaceReferenceIndices.begin() + end, [&](int idx) {
    return surfaceReferences[idx].boundingVolumeCenter[split.splitAxis] < split.splitPositionOnAxis;
  });
  int midIndex = (int)(midIterator - surfaceReferenceIndices.begin());

  // right or left group is empty -> stop here (make leaf)
  if (midIndex == begin || midIndex == end) {
    bvhNode.indexFirstSurfaceInside = begin;
    bvhNode.countSurfacesInside = count;
    int nodeIndex = bvhNodes.size();
    bvhNodes.push_back(bvhNode);
    return nodeIndex;
  }

  // store parent in bounding volume hicharay
  const int nodeIndex = bvhNodes.size();
  bvhNodes.push_back(bvhNode);

  // build children recursivly
  const int leftIndex = buildBvhNode(begin, midIndex);
  const int rightIndex = buildBvhNode(midIndex, end);

  // link the children to its parent
  bvhNodes[nodeIndex].left = leftIndex;
  bvhNodes[nodeIndex].right = rightIndex;

  return nodeIndex;
}

bool SceneBVH::intersect(const Ray& ray, Hit& hit) const {
  if (bvhNodes.empty()) return false;

  bool anyHit = false;

  float minDistanceAlongRay = 0.0f;
  float maxDistanceAlongRay = hit.distanceClosestIntersection; // current best distance

  int bvhNodeStack[64];
  int stackTop = 0; // index of the next free stack slot
  bvhNodeStack[stackTop++] = 0; // push root bvhNode

  while (stackTop > 0) {
    int nodeIndex = bvhNodeStack[--stackTop];
    const BVHNode& bvhNode = bvhNodes[nodeIndex];

    // AABB test: if ray misses this bvhNode, skip whole subtree
    if (!bvhNode.boundingBox.intersect(ray, minDistanceAlongRay, maxDistanceAlongRay))
      continue;

    if (bvhNode.isLeaf()) {
      // Test only the surfaces inside this leaf
      for (int i = 0; i < bvhNode.countSurfacesInside; ++i) {
        int referenceIndex = surfaceReferenceIndices[bvhNode.indexFirstSurfaceInside + i];
        const Surface* currentSurface = surfaceReferences[referenceIndex].surface;

        // tempHit starts with current best distance so the surface can early-reject
        Hit tempHit = hit;

        // check if the surface intersects with the ray and update if its the closest intersection
        if (currentSurface->intersect(ray, tempHit) && tempHit.distanceClosestIntersection < hit.distanceClosestIntersection) {
          hit = tempHit;
          maxDistanceAlongRay = hit.distanceClosestIntersection; // tighten max distance
          anyHit = true;
        }
      }
    } else {
      // its a inner bvhNode -> push childs on top of the stack
      bvhNodeStack[stackTop++] = bvhNode.left;
      bvhNodeStack[stackTop++] = bvhNode.right;
    }
  }

  return anyHit;
}

bool SceneBVH::occluded(const Ray& ray, float maxDistanceToLight) const {
  if (bvhNodes.empty()) return false;

  float minRayDistance = 0.0f;
  float maxRayDistance = maxDistanceToLight;

  int bvhNodeStack[64];
  int stackTop = 0;
  bvhNodeStack[stackTop++] = 0;

  while (stackTop > 0) {
    int nodeIndex = bvhNodeStack[--stackTop];
    const BVHNode& bvhNode = bvhNodes[nodeIndex];

    if (!bvhNode.boundingBox.intersect(ray, minRayDistance, maxRayDistance ))
      continue;

    if (bvhNode.isLeaf()) {
      for (int i = 0; i < bvhNode.countSurfacesInside; ++i) {
        int referenceIndex = surfaceReferenceIndices[bvhNode.indexFirstSurfaceInside + i];
        const Surface* currentSurface = surfaceReferences[referenceIndex].surface;

        Hit tempHit; 
        tempHit.distanceClosestIntersection = maxRayDistance; // only check until distance to light

        if (currentSurface->intersect(ray, tempHit) && tempHit.distanceClosestIntersection > EPS && tempHit.distanceClosestIntersection < maxRayDistance - EPS) {
          return true; // stop here its already in shadow
        }
      }
    } else {
      // its a inner bvhNode -> push childs on top of the stack
      bvhNodeStack[stackTop++] = bvhNode.left;
      bvhNodeStack[stackTop++] = bvhNode.right;
    }
  }

  return false;
}

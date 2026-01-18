#ifndef SAH_SPLITTER_H
#define SAH_SPLITTER_H

#include <algorithm>
#include <limits>
#include <vector>

#include "render/intersections/bvh/scene_bvh.h"
#include "render/intersections/bvh/aabb.h"
#include "math/vec3.h"

struct SAHSplit {
  int splitAxis = -1; // -1 = no split found
  float splitPositionOnAxis = 0.0f;
  float splitCost = std::numeric_limits<float>::infinity(); // smaller = better
};

class SAHSplitter {
public:
  explicit SAHSplitter(const std::vector<SurfaceReference> &refs, const std::vector<int> &indices, int bins = 12) : surfaceReferences(refs), surfaceReferenceIndices(indices), BINS(bins) {}

  /*
  For each axis, the algorithm looks at how far apart the centers of the surface bounding volumes are, splits this range into bins,
  and places each surface into a bin based on where its center lies within that range.

  For each possible split between two adjacent bins, the surfaces are divided into a left and a right group. All surfaces on each side of the split are
  accumulated into a single bounding volume, and the number of contained surfaces is counted.
  
  A split is better if both sides are small and do not contain many surfaces.
  */
  SAHSplit findBestSplit(int begin, int end, const AABB &centroidBoundingVolume) const {
    SAHSplit bestSplit;

    // check per axis
    for (int splitAxis = 0; splitAxis < 3; ++splitAxis) {
      const float minCentroidPosOnAxis = centroidBoundingVolume.min()[splitAxis];
      const float maxCentroidPosOnAxis = centroidBoundingVolume.max()[splitAxis];
      const float centroidExtendOnAxis = maxCentroidPosOnAxis - minCentroidPosOnAxis; // biggest extend surface centeres per axis
      if (centroidExtendOnAxis <= 1e-8f)
        continue; // surface centeres are to close together

      // bins
      std::vector<AABB> boundingVolumeBins(BINS);
      std::vector<int> binSurfaceCounts(BINS, 0);

      // fill the bounding volume bins with the surfaces
      for (int i = begin; i < end; ++i) {
        int eachIndex = surfaceReferenceIndices[i];
        const Vec3 &eachSurfaceBoundingVolumeCenter = surfaceReferences[eachIndex].boundingVolumeCenter;

        // calculate bin index for the surfaces bounding volume
        int binIndex = int(((eachSurfaceBoundingVolumeCenter[splitAxis] - minCentroidPosOnAxis) / centroidExtendOnAxis) * float(BINS));

        // prevents edge cases
        if (binIndex < 0)
          binIndex = 0;
        if (binIndex >= BINS)
          binIndex = BINS - 1;

        // the bounding volume in the bin will contain the bounding volume of the surface
        binSurfaceCounts[binIndex]++;
        boundingVolumeBins[binIndex].includeBox(surfaceReferences[eachIndex].boundingVolume);
      }

      // splitting bins

      // prefix left
      // leftBounds[0] = Bin0
      // leftBounds[1] = Bin0 + Bin1
      // leftBounds[2] = Bin0 + Bin1 + Bin2
      // leftBounds[3] = Bin0 + Bin1 + Bin2 + Bin3
      std::vector<AABB> leftBounds(BINS);
      std::vector<int> leftCount(BINS, 0);
      {
        AABB accumulator;
        int count = 0;
        for (int binIndex = 0; binIndex < BINS; ++binIndex) {
          if (binSurfaceCounts[binIndex] > 0)
            accumulator.includeBox(boundingVolumeBins[binIndex]);
          count += binSurfaceCounts[binIndex];
          leftBounds[binIndex] = accumulator;
          leftCount[binIndex] = count;
        }
      }

      // suffix right
      // rightBounds[3] = Bin3
      // rightBounds[2] = Bin2 + Bin3
      // rightBounds[1] = Bin1 + Bin2 + Bin3
      // rightBounds[0] = Bin0 + Bin1 + Bin2 + Bin3
      std::vector<AABB> rightBounds(BINS);
      std::vector<int> rightCount(BINS, 0);
      {
        AABB accumulator;
        int count = 0;
        for (int binIndex = BINS - 1; binIndex >= 0; --binIndex) {
          if (binSurfaceCounts[binIndex] > 0)
            accumulator.includeBox(boundingVolumeBins[binIndex]);
          count += binSurfaceCounts[binIndex];
          rightBounds[binIndex] = accumulator;
          rightCount[binIndex] = count;
        }
      }

      // their are n bins but only n-1 splits between
      for (int binIndex = 0; binIndex < BINS - 1; ++binIndex) {
        const int amountLeftSurfaces = leftCount[binIndex];       // left: 0 ... binIndex
        const int amountRightSurfaces = rightCount[binIndex + 1]; // right: binIndex+1 ... BINS-1
        if (amountLeftSurfaces == 0 || amountRightSurfaces == 0)
          continue;

        const AABB &leftBoundingVolume = leftBounds[binIndex];
        const AABB &rightBoundingVolume = rightBounds[binIndex + 1];
        if (!leftBoundingVolume.isValid() || !rightBoundingVolume.isValid())
          continue; // bounding volume is not valid skip

        const float currentCost = leftBoundingVolume.surfaceArea() * float(amountLeftSurfaces) + rightBoundingVolume.surfaceArea() * float(amountRightSurfaces);
        if (currentCost < bestSplit.splitCost) {
          bestSplit.splitCost = currentCost;
          bestSplit.splitAxis = splitAxis;
          bestSplit.splitPositionOnAxis = minCentroidPosOnAxis + centroidExtendOnAxis * float(binIndex + 1) / float(BINS);
        }
      }
    }

    return bestSplit;
  }

private:
  const std::vector<SurfaceReference> &surfaceReferences;
  const std::vector<int> &surfaceReferenceIndices;
  int BINS;
};

#endif
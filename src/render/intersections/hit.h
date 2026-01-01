#ifndef HIT_H
#define HIT_H

struct Hit {
  float distanceClosestIntersection = 1e30f;     
  Vec3  positionWorldSpace;      
  Vec3  normalWorldSpace;        
  const Material* material = nullptr;
};

#endif
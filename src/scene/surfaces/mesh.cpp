#include "scene/surfaces/mesh.h"

// o + t*d = v0 + u*e1 + v*e2 (t = hitdistance)
// Returns true if the ray hits the triangle in front of the ray origin. (Möller-Trumbore)
// Updates the rays hit distance in object space and u, v
static bool intersectTriangleMT(const Ray &ray, const TrianglePrimitive &trianglePrimitiv, float &hitDistanceObjSpace, float &u, float &v) {

  // to treat "very close" and "almost parallel" hits as no hit
  const float eps = 1e-7f;

  // triangle edges from first vertex (v0)
  Vec3 edge1 = trianglePrimitiv.v1 - trianglePrimitiv.v0;
  Vec3 edge2 = trianglePrimitiv.v2 - trianglePrimitiv.v0;

  // determinant = edge1 * (raydirection x edge2)
  // Build a vector perpendicular to ray.direction and edge2.
  Vec3 perpendicularRayDirTrinagleEdge = cross(ray.direction, edge2);
  float determinant = dot(edge1, perpendicularRayDirTrinagleEdge);

  // determinant is proportional to the volume (near 0 means the ray is parallel to the triangle plane or triangle area is ~0).
  // backface culling only positiv determinat values (remove fabs)
  if (std::fabs(determinant) < eps) {
    return false;
  }

  float invDet = 1.0f / determinant;
  Vec3 distanceTrinagleVertexToRayOrigin = ray.origin - trianglePrimitiv.v0;

  // It tells how far the hit point lies along edge1 starting from v0. (weight for vertex v1).
  u = dot(distanceTrinagleVertexToRayOrigin, perpendicularRayDirTrinagleEdge) * invDet;
  // If u is outside [0,1], the intersection point lies outside the triangle
  if (u < 0.0f || u > 1.0f)
    return false;

  Vec3 qvec = cross(distanceTrinagleVertexToRayOrigin, edge1);

  // weight for vertex v2, together with u it determines whether the intersection lies inside the triangle.
  v = dot(ray.direction, qvec) * invDet;

  // If v < 0 -> outside the triangle.
  // If u + v > 1 -> outside across the edge between v1 and v2.
  if (v < 0.0f || (u + v) > 1.0f)
    return false;

  // The hit point is: P = ray.origin + t * ray.direction
  hitDistanceObjSpace = dot(edge2, qvec) * invDet;
  return hitDistanceObjSpace > eps;
}

bool Mesh::intersect(const Ray &rayWorldSpace, Hit &hit) const {
  // transform ray into object space of this mesh
  Ray rayObjectSpace = transform_.toObjectRay(rayWorldSpace);

  bool anyHit = false;

  for (const TrianglePrimitive &eachTriangle : this->trianglePrimitives_) {
    float hitDistanceObjSpace = 0.0f;
    float u = 0.0f; // interpolation weight vertex 1
    float v = 0.0f; // interpolation weight vertex 2

    // intersect in object space
    if (!intersectTriangleMT(rayObjectSpace, eachTriangle, hitDistanceObjSpace, u, v))
      continue;

    // hitpoint in object space
    Vec3 hitpointObj = rayObjectSpace.origin + hitDistanceObjSpace * rayObjectSpace.direction;
    
    // normal in object space
    float w = 1.0f - u - v; //interpolation weight vertex 0
    Vec3 normalObjSpace = (w * eachTriangle.n0 + u * eachTriangle.n1 + v * eachTriangle.n2).normalized();

    // transform hitpoint + normal back to world space
    Vec3 hitpointWorld = transform_.applyPoint(hitpointObj);
    Vec3 normalWorld = transform_.applyNormal(normalObjSpace).normalized();

    // calculate distance of intersection in world space
    float distanceIntersection = (hitpointWorld - rayWorldSpace.origin).length(); 

    if (distanceIntersection < hit.distanceClosestIntersection) {
      hit.distanceClosestIntersection = distanceIntersection;
      hit.positionWorldSpace = hitpointWorld;
      hit.normalWorldSpace = normalWorld;
      hit.material = &material_;
      // texture mapping
      hit.u = w * eachTriangle.uv0.x + u * eachTriangle.uv1.x + v * eachTriangle.uv2.x; 
      hit.v = w * eachTriangle.uv0.y + u * eachTriangle.uv1.y + v * eachTriangle.uv2.y;

      anyHit = true;
    }
  }

  return anyHit;
}
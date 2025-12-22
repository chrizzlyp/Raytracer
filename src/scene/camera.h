#ifndef CAMERA_H
#define CAMERA_H

#include "math/vec3.h"
#include <sstream>
#include <stdexcept>

class Camera {
public:
  // getter
  const Vec3 &position() const {
    return position_;
  }

  const Vec3 &lookat() const {
    return lookat_;
  }

  const Vec3 &up() const {
    return up_;
  }

  float horizontalFovHalfAngle() const {
    return horizontal_fov_half_angle_;
  }

  int resHorizontal() const {
    return res_horizontal_;
  }

  int resVertical() const {
    return res_vertical_;
  }

  int maxBounces() const {
    return max_bounces_;
  }

  // setter
  void setPosition(const Vec3 &v) {
    position_ = v;
  }

  void setLookat(const Vec3 &v) {
    lookat_ = v;
  }

  void setUp(const Vec3 &v) {
    up_ = v;
  }

  void setHorizontalFovHalfAngle(float deg) {
    if (deg <= 0.f || deg >= 180.f)
      throw std::invalid_argument("Camera FOV half-angle must be in (0, 180)");
    horizontal_fov_half_angle_ = deg;
  }

  void setResolution(int w, int h) {
    if (w <= 0 || h <= 0)
      throw std::invalid_argument("Camera resolution must be positive");
    res_horizontal_ = w;
    res_vertical_ = h;
  }

  void setMaxBounces(int n) {
    if (n < 0)
      throw std::invalid_argument("max_bounces must be >= 0");
    max_bounces_ = n;
  }

private:
  Vec3 position_{};
  Vec3 lookat_{};
  Vec3 up_{};
  float horizontal_fov_half_angle_ = 0.f;
  int res_horizontal_ = 0;
  int res_vertical_ = 0;
  int max_bounces_ = 0;
};

inline std::ostream &operator<<(std::ostream &os, const Camera &c) {
  os << "Camera{pos=" << c.position()
     << ", lookat=" << c.lookat()
     << ", up=" << c.up()
     << ", fov_half=" << c.horizontalFovHalfAngle()
     << ", res=" << c.resHorizontal() << "x" << c.resVertical()
     << ", max_bounces=" << c.maxBounces()
     << "}";
  return os;
}

#endif // CAMERA_H

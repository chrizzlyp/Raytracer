#include "scene/lights/utils/lights.h"
#include <ostream>

// Stream operators are implemented in this .cpp file to keep the header lightweight

std::ostream &operator<<(std::ostream &os, const AmbientLight &l) {
  os << "AmbientLight{color=" << l.color() << "}";
  return os;
}

std::ostream &operator<<(std::ostream &os, const PointLight &l) {
  os << "PointLight{color=" << l.color()
     << ", position=" << l.position()
     << "}";
  return os;
}

std::ostream &operator<<(std::ostream &os, const ParallelLight &l) {
  os << "ParallelLight{color=" << l.color()
     << ", direction=" << l.direction()
     << "}";
  return os;
}

std::ostream &operator<<(std::ostream &os, const SpotLight &l) {
  os << "SpotLight{color=" << l.color()
     << ", position=" << l.position()
     << ", direction=" << l.direction()
     << ", alpha1=" << l.alpha1()
     << ", alpha2=" << l.alpha2()
     << "}";
  return os;
}

// Polymorpher Printer: arbeitet über type() und castet gezielt
std::ostream &operator<<(std::ostream &os, const Light &l) {
  switch (l.type()) {
    case LightType::AMBIENT:
      return os << static_cast<const AmbientLight&>(l);
    case LightType::POINT:
      return os << static_cast<const PointLight&>(l);
    case LightType::PARALLEL:
      return os << static_cast<const ParallelLight&>(l);
    case LightType::SPOT:
      return os << static_cast<const SpotLight&>(l);
    default:
      os << "Light{color=" << l.color() << "}";
      return os;
  }
}
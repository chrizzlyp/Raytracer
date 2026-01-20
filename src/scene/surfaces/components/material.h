#ifndef MATERIAL_H
#define MATERIAL_H

#include "math/color.h"
#include <optional>
#include <ostream>
#include <string>

struct PhongParams {
  float kAmbient = 0.f;
  float kDiffuse = 0.f;
  float kSpecular = 0.f;
  float exponentShininess = 1.f;
};

enum class MaterialType {
  SOLID,
  TEXTURED
};

class Material {
public:
  Material() = default;

  void setType(MaterialType t) {
    type_ = t;
  }

  MaterialType type() const {
    return type_;
  }

  bool isTextured() const {
    return type_ == MaterialType::TEXTURED;
  }

  bool isBumped() const {
    return !bumpMapName_.empty();
  }

  void setColor(const Color &c) {
    color_ = c;
  }

  const Color &color() const {
    return color_;
  }

  // For textured materials: store the referenced filename (loading can be done elsewhere).
  void setTextureName(std::string name) {
    textureName_ = std::move(name);
  }

  const std::string &textureName() const {
    return textureName_;
  }

  void setPhong(const PhongParams &p) {
    phong_ = p;
  }

  const PhongParams &phong() const {
    return phong_;
  }

  void setReflectance(float r) {
    reflectance_ = r;
  }

  void setTransmittance(float t) {
    transmittance_ = t;
  }

  void setIor(float ior) {
    ior_ = ior;
  }

  float reflectance() const {
    return reflectance_;
  }

  float transmittance() const {
    return transmittance_;
  }

  float ior() const {
    return ior_;
  }

  void setBumpMapName(std::string name) {
    bumpMapName_ = std::move(name);
  }

  const std::string& bumpMapName() const {
    return bumpMapName_;
  }

  void setBumpStrength(float s) {
    bumpStrength_ = s;
  }

  float bumpStrength() const {
    return bumpStrength_;
  }

private:
  MaterialType type_ = MaterialType::SOLID;

  Color color_{1.f, 1.f, 1.f};
  std::string textureName_{};

  PhongParams phong_{};

  float reflectance_ = 0.f;
  float transmittance_ = 0.f;
  float ior_ = 1.f;

  std::string bumpMapName_;
  float bumpStrength_ = 1.0f;
};

inline std::ostream &operator<<(std::ostream &os, const PhongParams &p) {
  os << "PhongParams{ ka=" << p.kAmbient
     << ", kd=" << p.kDiffuse
     << ", ks=" << p.kSpecular
     << ", exponent=" << p.exponentShininess
     << " }";
  return os;
}

inline std::ostream &operator<<(std::ostream &os, const Material &m) {
  os << "Material{ type=" << (m.isTextured() ? "TEXTURED" : "SOLID");

  if (m.isTextured()) {
    os << ", texture=\"" << m.textureName() << "\"";
  } else {
    os << m.color();
  }

  os << ", " << m.phong()
     << ", reflectance=" << m.reflectance()
     << ", transmittance=" << m.transmittance()
     << ", ior=" << m.ior()
     << " }";
  return os;
}

#endif

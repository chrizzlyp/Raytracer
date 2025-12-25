#ifndef MATERIAL_H
#define MATERIAL_H

#include "math/color.h"
#include <optional>
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

  void setType(MaterialType t) { type_ = t; }
  MaterialType type() const { return type_; }
  bool isTextured() const { return type_ == MaterialType::TEXTURED; }

  void setColor(const Color &c) { color_ = c; }
  const Color &color() const { return color_; }

  // For textured materials: store the referenced filename (loading can be done elsewhere).
  void setTextureName(std::string name) { textureName_ = std::move(name); }
  const std::string &textureName() const { return textureName_; }

  void setPhong(const PhongParams &p) { phong_ = p; }
  const PhongParams &phong() const { return phong_; }

  void setReflectance(float r) { reflectance_ = r; }
  void setTransmittance(float t) { transmittance_ = t; }
  void setIor(float ior) { ior_ = ior; }

  float reflectance() const { return reflectance_; }
  float transmittance() const { return transmittance_; }
  float ior() const { return ior_; }

private:
  MaterialType type_ = MaterialType::SOLID;

  Color color_{1.f, 1.f, 1.f}; // <color r g b>
  std::string textureName_{};  // <texture name="..."> (nur wenn textured)

  PhongParams phong_{}; // <phong ka kd ks exponent>

  float reflectance_ = 0.f;   // <reflectance r="...">
  float transmittance_ = 0.f; // <transmittance t="...">
  float ior_ = 1.f;           // <refraction iof="...">
};

#endif

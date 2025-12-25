#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "math/color.h"
#include "scene/camera.h"
#include "scene/lights/utils/lights.h"
#include "scene/surfaces/surface.h"
#include "scene/surfaces/surface_io.h"

class Scene {
public:
  // getter
  const std::string &outputFileName() const {
    return outputFileName_;
  }

  const Color &backgroundColor() const {
    return backgroundColor_;
  }

  const Camera &camera() const {
    return camera_;
  }

  const std::optional<AmbientLight> &ambientLight() const {
    return ambient_;
  }

  const std::vector<std::unique_ptr<Light>> &lights() const {
    return lights_;
  }

  const std::vector<std::unique_ptr<Surface>> &surfaces() const {
    return surfaces_;
  }

  // setter
  void setOutputFileName(std::string path) {
    outputFileName_ = std::move(path);
  }

  void setBackgroundColor(const Color &c) {
    backgroundColor_ = c;
  }

  Camera &cameraMutable() {
    return camera_;
  }

  void setAmbientLight(const AmbientLight &a) {
    ambient_ = a;
  }

  void addLight(std::unique_ptr<Light> l) {
    lights_.push_back(std::move(l));
  }

  void addSurface(std::unique_ptr<Surface> s) {
    surfaces_.push_back(std::move(s));
  }

private:
  std::string outputFileName_;
  Color backgroundColor_{0.f, 0.f, 0.f};
  Camera camera_{};
  std::optional<AmbientLight> ambient_;
  std::vector<std::unique_ptr<Light>> lights_;
  std::vector<std::unique_ptr<Surface>> surfaces_;
};

inline std::ostream &operator<<(std::ostream &os, const Scene &s) {
  os << "Scene{output file name=" << s.outputFileName()
     << ", background=" << s.backgroundColor()
     << ", camera=" << s.camera();

  os << ", ambient=";
  if (s.ambientLight())
    os << *s.ambientLight();
  else
    os << "none";

  os << ", lights=[";
  for (size_t i = 0; i < s.lights().size(); ++i) {
    if (i)
      os << ", ";
    os << *s.lights()[i]; // nutzt operator<<(Light&)
  }

  os << ", surfaces=[";
  for (size_t i = 0; i < s.surfaces().size(); ++i) {
    if (i)
      os << ", ";
    os << *s.surfaces()[i]; 
  }
  os << "]}";

  return os;
}

#endif // SCENE_H

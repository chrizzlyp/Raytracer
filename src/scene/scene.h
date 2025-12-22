#ifndef SCENE_H
#define SCENE_H

#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>

#include "scene/camera.h"
#include "math/color.h"

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

  // setter
  void setOutputFileName(std::string path) {
    if (path.empty())
      throw std::invalid_argument("Scene outout file name must not be empty");
    outputFileName_ = std::move(path);
  }

  void setBackgroundColor(const Color &c) {
    backgroundColor_ = c;
  }

  Camera &cameraMutable() {
    return camera_;
  }

private:
  std::string outputFileName_;
  Color backgroundColor_{0.f, 0.f, 0.f};
  Camera camera_{};
};

// Debug-Ausgabe
inline std::ostream &operator<<(std::ostream &os, const Scene &s) {
  os << "Scene{outout file name=" << s.outputFileName()
     << ", background=" << s.backgroundColor()
     << ", " << s.camera()
     << "}";
  return os;
}

#endif // SCENE_H

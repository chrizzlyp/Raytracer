#ifndef TEXTURE_H
#define TEXTURE_H

#include "math/color.h"
#include <string>
#include <vector>

class Texture {
public:
  bool loadPNG(const std::string &filename);
  Color sampleColor(float u, float v) const;
  int getWidth() const {
    return width;
  }
  int getHeight() const {
    return height;
  }
  bool isLoaded() const {
    return width > 0 && height > 0 && !rgba.empty();
  }

private:
  int width = 0;
  int height = 0;
  std::vector<unsigned char> rgba;
};

#endif
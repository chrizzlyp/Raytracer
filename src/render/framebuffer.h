#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <algorithm>
#include <vector>
#include "math/color.h" 

class Framebuffer {
public:
  Framebuffer() {
    widthPx = 0;
    heightPx = 0;
  }

  Framebuffer(int w, int h) {
    resize(w, h);
  }

  void resize(int w, int h) {
    widthPx = w;
    heightPx = h;
    this->pixels.assign(widthPx * heightPx, Color{0.f, 0.f, 0.f});
  }

  int width() const {
    return widthPx;
  }

  int height() const {
    return heightPx;
  }

  // row major: first all x values with y = 0 ...
  void setPixel(int x, int y, const Color &c) {
    this->pixels[y * widthPx + x] = c;
  }

  const std::vector<Color> &getPixels() const {
    return this->pixels;
  }

private:
  int widthPx;
  int heightPx;
  std::vector<Color> pixels;
};

#endif
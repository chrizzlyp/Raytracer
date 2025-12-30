#ifndef IMAGE8_H
#define IMAGE8_H

#include <cstdint>   
#include <vector>    

struct RGB8 {
  uint8_t r, g, b;
};

class Image8 {
public:
  Image8(int widthPx, int heightPx) : widthPx(widthPx), heightPx(heightPx), pixels(widthPx * heightPx) {}

  int width() const {
    return this->widthPx;
  }

  int height() const {
    return this->heightPx;
  }

  void setPixel(int x, int y, RGB8 color8) {
    this->pixels[y * widthPx + x] = color8;
  }

  const std::vector<RGB8> &getPixels() const {
    return this->pixels;
  }

private:
  int widthPx;
  int heightPx;
  std::vector<RGB8> pixels;
};

#endif
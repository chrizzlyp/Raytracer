#include "scene/surfaces/components/texture.h"
#include <algorithm>
#include <cmath>

// PNG loading via stb_image
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool Texture::loadPNG(const std::string &filename) {
  int width = 0;
  int height = 0;
  int channels = 0;

  // force RGBA output (4 channels)
  unsigned char *imageData = stbi_load(filename.c_str(), &width, &height, &channels, 4);
  if (!imageData) {
    this->width = 0;
    this->height = 0;
    this->rgba.clear();
    return false;
  }

  this->width = width;
  this->height = height;

  // copy into rgba vector
  this->rgba.assign(imageData, imageData + (this->width * this->height * 4));

  stbi_image_free(imageData);
  return true;
}

Color Texture::sampleColor(float u, float v) const {
  if (!isLoaded())
    return Color(1.f, 0.f, 1.f);
  return sampleBilinear(u, v);
}

float Texture::sampleHeight(float u, float v) const {
  Color c = sampleColor(u, v);
  return c.x;
}

float Texture::wrapTo01(float x) {
  x = x - std::floor(x);
  if (x < 0.0f)
    x += 1.0f;
  return x;
}

int Texture::wrapi(int i, int n) {
  i %= n;
  if (i < 0)
    i += n;
  return i;
}

Color Texture::fetchTexel(int x, int y) const {
  // repeat wrap
  x = wrapi(x, this->width);
  y = wrapi(y, this->height);

  int index = (y * this->width + x) * 4;
  float r = this->rgba[index + 0] / 255.0f;
  float g = this->rgba[index + 1] / 255.0f;
  float b = this->rgba[index + 2] / 255.0f;
  return Color(r, g, b);
}

Color Texture::sampleBilinear(float u, float v) const {
  // wrap UV to [0,1)
  u = wrapTo01(u);
  v = wrapTo01(v);

  // -0.5 shifts so sampling aligns with texel centers
  float x = u * this->width - 0.5f;
  float y = v * this->height - 0.5f;

  // neighborhood boardes around (x,y)
  int x0 = (int)std::floor(x); // left
  int y0 = (int)std::floor(y); // down
  int x1 = x0 + 1;             // right
  int y1 = y0 + 1;             // up

  float tx = x - (float)x0; // 0..1, how far to the right in interval [x0,x1]
  float ty = y - (float)y0; // 0..1, how far up in interval [y0,y1]

  Color texelBottomLeft = fetchTexel(x0, y0);
  Color texelBottomRight = fetchTexel(x1, y0);
  Color texelTopLeft = fetchTexel(x0, y1);
  Color texelTopRight = fetchTexel(x1, y1);

  auto lerpColor = [](const Color &left, const Color &right, float t) {
    return left * (1.0f - t) + right * t;
  };

  Color bottomRowInterpolated = lerpColor(texelBottomLeft, texelBottomRight, tx);
  Color topRowInterpolated = lerpColor(texelTopLeft, texelTopRight, tx);

  // interpolate along y between bottom and top
  Color bilinearFilteredColor = lerpColor(bottomRowInterpolated, topRowInterpolated, ty);

  return bilinearFilteredColor;
}
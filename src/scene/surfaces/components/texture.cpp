#include "scene/surfaces/components/texture.h"  
#include <algorithm>
#include <cmath>

// PNG loading via stb_image
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 

bool Texture::loadPNG(const std::string& filename) {
  int width = 0;
  int height = 0;
  int channels = 0;

  // force RGBA output (4 channels)
  unsigned char* imageData = stbi_load(filename.c_str(), &width, &height, &channels, 4);
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

// returns the pixel color of a texture on position uv
Color Texture::sampleColor(float u, float v) const {
  // if not loaded, return "debug pink"
  if (this->width <= 0 || this->height <= 0 || this->rgba.empty()) {
    return Color(1.f, 0.f, 1.f);
  }

  // wrapping (u,v can be outside [0,1]) and makes uv values 1.2 valid values e.g. 0.2
  auto wrap = [](float x) {
    x = x - std::floor(x); // makes it [0,1]
    if (x < 0.0f) x += 1.0f;  // handle edge case
    return x;
  };

  u = wrap(u);
  v = wrap(v);

  // convert UV to pixel coordinates
  // UV conventions use v=0 at bottom and images use y=0 at top
  
  float flip_x = u * float(this->width - 1);
  float flip_y = v * float(this->height - 1);

  int x = int(std::round(flip_x));
  int y = int(std::round(flip_y));

  x = std::clamp(x, 0, this->width - 1);
  y = std::clamp(y, 0, this->height - 1);

  int index = (y * this->width + x) * 4;
  float r = this->rgba[index + 0] / 255.0f;
  float g = this->rgba[index + 1] / 255.0f;
  float b = this->rgba[index + 2] / 255.0f;
  // alpha: rgba_[index+3] 

  return Color(r, g, b);
}

#include "render/image-writer/image_writer.h"

#include <algorithm>
#include <cstdint>

// igonore unimplemented warnings for lib image writer
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#pragma GCC diagnostic pop

// float [0..1] -> uint8 [0..255]
static uint8_t toByte(float x) {
  x = std::clamp(x, 0.0f, 1.0f);
  return static_cast<uint8_t>(x * 255.0f + 0.5f);
}

Image8 ImageWriter::toImage8(const Framebuffer &framebuffer) const{
  Image8 image(framebuffer.width(), framebuffer.height());
  const auto &pixels = framebuffer.getPixels();

  const int weidthPx = framebuffer.width();
  const int heightPx = framebuffer.height();

  for (int y = 0; y < heightPx; ++y) {
    for (int x = 0; x < weidthPx; ++x) {
      const Color color = pixels[static_cast<size_t>(y) * static_cast<size_t>(weidthPx) + static_cast<size_t>(x)];
      image.setPixel(x, y, RGB8{toByte(color.x), toByte(color.y), toByte(color.z)});
    }
  }
  return image;
}

bool ImageWriter::writePNG(const Image8 &image, const std::string &path) const{
  const int weidthPx = image.width();
  const int heightPx = image.height();
  const int channels = 3;
  const int strideBytes = weidthPx * channels;

  const int ok = stbi_write_png(path.c_str(), weidthPx, heightPx, channels, image.getPixels().data(), strideBytes);

  return ok != 0;
}

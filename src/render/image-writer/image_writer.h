#ifndef IMAGE_WRITER_H
#define IMAGE_WRITER_H

#include "render/image-writer/image8.h"
#include "render/framebuffer.h"
#include <string>

class ImageWriter {
public:
  Image8 toImage8(const Framebuffer &fb) const;
  bool writePNG(const Image8 &img, const std::string &path) const;
  bool writePNG(const Framebuffer &fb, const std::string &path) const{
    return writePNG(toImage8(fb), path);
  }
};

#endif
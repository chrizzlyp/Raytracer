#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H
#include <filesystem>

#include "scene/surfaces/components/texture.h"
#include <string>
#include <unordered_map>

class TextureManager {
public:
  const Texture& getTexture(const std::string& textureName) const;
  void setBaseDir(std::string baseDir);

private:
  std::string baseDir = (std::filesystem::current_path().parent_path() / "assets/textures/").string();
  mutable std::unordered_map<std::string, Texture> cache;
};

#endif

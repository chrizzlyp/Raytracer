#include "textures/texture_manager.h"
#include "scene/surfaces/components/texture.h"
#include <utility>

#include <iostream>

const Texture &TextureManager::getTexture(const std::string &textureName) const {
  auto it = this->cache.find(textureName);
  if (it != this->cache.end())
    return it->second;

  Texture texture;
  std::string path = this->baseDir + textureName;

  const bool ok = texture.loadPNG(path);
  if (!ok) {
    std::cerr << "[TextureManager] FAILED to load texture: '" << textureName << "'\n  baseDir: '" << this->baseDir << "'\n  path:    '" << path << "'\n";
  } else {
    std::cerr << "[TextureManager] Loaded texture: '" << textureName << "' (" << texture.getWidth() << "x" << texture.getHeight() << ")\n";
  }

  auto [newIt, _] = this->cache.emplace(textureName, std::move(texture));
  return newIt->second;
}

void TextureManager::setBaseDir(std::string baseDir) {
  this->baseDir = std::move(baseDir);
}

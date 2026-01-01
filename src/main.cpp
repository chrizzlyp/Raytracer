#include <iostream>
#include <string>

#include "parser/xml-parser/scene_parser.h"
#include "render/renderer.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <scene.xml>\n";
    return 1;
  }

  Scene scene;
  std::string error;
  SceneParser parser;

  if (!parser.loadSceneFromXMLFile(argv[1], scene, error)) {
    std::cerr << "Parse error: " << error << "\n";
    return 2;
  }

  std::cout << scene << "\n";
  std::cout << "Parsed OK!\n";

  Renderer renderer;

  try {
    const bool ok = renderer.renderToPNG(scene, RenderMode::Normal);
    if (!ok) {
      std::cerr << "Failed to write PNG.\n";
      return 3;
    }
    std::cout << "Wrote debug image: " << scene.outputFileName() << "\n";
  } catch (const std::exception &e) {
    std::cerr << "Render error: " << e.what() << "\n";
    return 4;
  }

  return 0;
}

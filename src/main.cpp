#include <iostream>
#include <string>
#include <chrono>

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
    auto start = std::chrono::high_resolution_clock::now();

    const bool ok = renderer.renderToPNG(scene, RenderMode::Realistic);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> seconds = end - start;
    std::cout << "Render time: " << seconds.count() << " s\n";

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

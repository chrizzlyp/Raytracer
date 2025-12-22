#include <iostream>
#include <string>

#include "parser/scene_parser.h"

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

  return 0;
}

#ifndef SCENE_PARSER_H
#define SCENE_PARSER_H

#include <string>

#include "scene/scene.h"
#include "tinyxml2.h"

// Parses a Scene from the XML format defined by the assignment
class SceneParser {
public:
  // Loads and parses a scene XML file at 'path' into 'outScene'.
  bool loadSceneFromXMLFile(const std::string &path, Scene &outScene, std::string &outError) const;

private:
  bool parseBasics(const tinyxml2::XMLElement *sceneEl, Scene &outScene, std::string &outError) const;
  bool parseCamera(const tinyxml2::XMLElement *sceneEl, Camera &outCamera, std::string &outError) const;
  bool parseLights(const tinyxml2::XMLElement *sceneEl, Scene &outScene, std::string &outError) const;
  bool parseAmbientLight(const tinyxml2::XMLElement *el, Scene &outScene, std::string &outError) const;
  bool parsePointLight(const tinyxml2::XMLElement *el, Scene &outScene, std::string &outError) const;
  bool parseParallelLight(const tinyxml2::XMLElement *el, Scene &outScene, std::string &outError) const;
  bool parseSpotLight(const tinyxml2::XMLElement *el, Scene &outScene, std::string &outError) const;
};

#endif
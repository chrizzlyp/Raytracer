#include "parser/scene_parser.h"
#include "parser/xml_parser_utils.h"
#include "scene/lights/utils/lights.h"
#include "scene/scene.h"

#include <cstring>
#include <memory>
#include <sstream>

// Top-level entry: load XML file, locate <scene>, then delegate to parse steps.
bool SceneParser::loadSceneFromXMLFile(const std::string &path, Scene &outScene, std::string &outError) const {
  // Load XML document from disk
  tinyxml2::XMLDocument doc;
  tinyxml2::XMLError err = doc.LoadFile(path.c_str());
  if (err != tinyxml2::XML_SUCCESS) {
    std::ostringstream oss;
    oss << "XML load failed: " << doc.ErrorStr();
    outError = oss.str();
    return false;
  }

  // Find <scene> root element
  const tinyxml2::XMLElement *xmlElement = doc.FirstChildElement("scene");
  if (!xmlElement) {
    outError = "Missing <scene> root element.";
    return false;
  }

  // Parse scene sections
  if (!parseBasics(xmlElement, outScene, outError))
    return false;
  if (!parseCamera(xmlElement, outScene.cameraMutable(), outError))
    return false;
  if (!parseLights(xmlElement, outScene, outError))
    return false;

  return true;
}

// Parse attributes directly on <scene> ... </scene>
bool SceneParser::parseBasics(const tinyxml2::XMLElement *xmlElement, Scene &outScene, std::string &outError) const {
  // set output file name
  const char *outputFileName = xmlElement->Attribute("output_file");
  if (!outputFileName) {
    outError = "Missing attribute output_file on <scene>.";
    return false;
  }
  outScene.setOutputFileName(outputFileName);

  // set background color
  const tinyxml2::XMLElement *bgEl = xmlutils::getRequiredChild(xmlElement, "background_color", outError, "scene");
  if (!bgEl)
    return false;

  Color background_color{};
  if (!xmlutils::readFloatAttribute(bgEl, "r", background_color.x) ||
      !xmlutils::readFloatAttribute(bgEl, "g", background_color.y) ||
      !xmlutils::readFloatAttribute(bgEl, "b", background_color.z)) {
    outError = "background_color must have r,g,b float attributes.";
    return false;
  }
  outScene.setBackgroundColor(background_color);
  return true;
}

// Parse <camera> ... </camera> section
bool SceneParser::parseCamera(const tinyxml2::XMLElement *xmlElement, Camera &outCamera, std::string &outError) const {
  // Locate <camera> under <scene>
  const tinyxml2::XMLElement *camEl = xmlutils::getRequiredChild(xmlElement, "camera", outError, "scene");
  if (!camEl)
    return false;

  Vec3 pos{}, look{}, up{};
  // Read <position x= y= z=>
  const tinyxml2::XMLElement *posEl = xmlutils::getRequiredChild(camEl, "position", outError, "camera");
  if (!posEl || !xmlutils::readVec3Attributes(posEl, pos)) {
    outError = "Missing/invalid <position x= y= z=> inside <camera>.";
    return false;
  }
  // Read <lookat x= y= z=>
  const tinyxml2::XMLElement *lookEl = xmlutils::getRequiredChild(camEl, "lookat", outError, "camera");
  if (!lookEl || !xmlutils::readVec3Attributes(lookEl, look)) {
    outError = "Missing/invalid <lookat x= y= z=> inside <camera>.";
    return false;
  }
  // Read <up x= y= z=>
  const tinyxml2::XMLElement *upEl = xmlutils::getRequiredChild(camEl, "up", outError, "camera");
  if (!upEl || !xmlutils::readVec3Attributes(upEl, up)) {
    outError = "Missing/invalid <up x= y= z=> inside <camera>.";
    return false;
  }

  // Read <horizontal_fov angle=>
  float fovHalf = 0.f;
  const tinyxml2::XMLElement *fovEl = xmlutils::getRequiredChild(camEl, "horizontal_fov", outError, "camera");
  if (!fovEl || !xmlutils::readFloatAttribute(fovEl, "angle", fovHalf)) {
    outError = "Missing/invalid <horizontal_fov angle=> inside <camera>.";
    return false;
  }

  // Read <resolution horizontal= vertical=>
  int w = 0, h = 0;
  const tinyxml2::XMLElement *resEl = xmlutils::getRequiredChild(camEl, "resolution", outError, "camera");
  if (!resEl || !xmlutils::readIntAttribute(resEl, "horizontal", w) || !xmlutils::readIntAttribute(resEl, "vertical", h)) {
    outError = "Missing/invalid <resolution horizontal= vertical=> inside <camera>.";
    return false;
  }

  // Read <max_bounces n=>
  int mb = 0;
  const tinyxml2::XMLElement *mbEl = xmlutils::getRequiredChild(camEl, "max_bounces", outError, "camera");
  if (!mbEl || !xmlutils::readIntAttribute(mbEl, "n", mb)) {
    outError = "Missing/invalid <max_bounces n=> inside <camera>.";
    return false;
  }

  try {
    outCamera.setPosition(pos);
    outCamera.setLookat(look);
    outCamera.setUp(up);
    outCamera.setHorizontalFovHalfAngle(fovHalf);
    outCamera.setResolution(w, h);
    outCamera.setMaxBounces(mb);
  } catch (const std::exception &e) {
    outError = std::string("Camera values invalid: ") + e.what();
    return false;
  }

  return true;
}

bool SceneParser::parseLights(const tinyxml2::XMLElement *sceneEl, Scene &outScene, std::string &outError) const {
  // <lights> optional
  const tinyxml2::XMLElement *lightsEl = sceneEl->FirstChildElement("lights");
  if (!lightsEl)
    return true;

  for (const tinyxml2::XMLElement *el = lightsEl->FirstChildElement();
       el != nullptr; el = el->NextSiblingElement()) {
    const char *name = el->Name();
    if (!name)
      continue;

    if (std::strcmp(name, "ambient_light") == 0) {
      if (!parseAmbientLight(el, outScene, outError))
        return false;
    } else if (std::strcmp(name, "point_light") == 0) {
      if (!parsePointLight(el, outScene, outError))
        return false;
    } else if (std::strcmp(name, "parallel_light") == 0) {
      if (!parseParallelLight(el, outScene, outError))
        return false;
    } else if (std::strcmp(name, "spot_light") == 0) {
      // optional (wenn du es noch nicht machst: return false mit Message)
      if (!parseSpotLight(el, outScene, outError))
        return false;
    } else {
      outError = std::string("Unknown light type <") + name + "> inside <lights>.";
      return false;
    }
  }

  return true;
}


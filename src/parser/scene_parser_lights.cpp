#include "parser/scene_parser.h"
#include "parser/xml_parser_utils.h"
#include "scene/lights/utils/lights.h"

#include <cstring>
#include <memory>

namespace {
// reads a mandatory <color r= g= b=> child
bool readColorChild(const tinyxml2::XMLElement *parent, Color &out, std::string &outError, const char *ctx) {
  const tinyxml2::XMLElement *cEl = xmlutils::getRequiredChild(parent, "color", outError, ctx);
  if (!cEl)
    return false;

  if (!xmlutils::readFloatAttribute(cEl, "r", out.x) || !xmlutils::readFloatAttribute(cEl, "g", out.y) || !xmlutils::readFloatAttribute(cEl, "b", out.z)) {
    outError = std::string(ctx) + ": <color> must have r,g,b float attributes.";
    return false;
  }
  return true;
}
} // namespace

// Parse <ambient_light> (the scene allows at only one ambient light) and creates and adds the light to the scene
bool SceneParser::parseAmbientLight(const tinyxml2::XMLElement *el, Scene &outScene, std::string &outError) const {
  if (outScene.ambientLight().has_value()) {
    outError = "Scene can have at most one <ambient_light>.";
    return false;
  }

  AmbientLight a;
  Color c{};
  if (!readColorChild(el, c, outError, "ambient_light"))
    return false;
  a.setColor(c);

  outScene.setAmbientLight(a);
  return true;
}

// Parse <point_light> reads mandatory <color> and <position> and creates and adds the light to the scene
bool SceneParser::parsePointLight(const tinyxml2::XMLElement *el, Scene &outScene, std::string &outError) const {
  auto l = std::make_unique<PointLight>();

  Color c{};
  if (!readColorChild(el, c, outError, "point_light"))
    return false;
  l->setColor(c);

  Vec3 pos{};
  const tinyxml2::XMLElement *pEl = xmlutils::getRequiredChild(el, "position", outError, "point_light");
  if (!pEl || !xmlutils::readVec3Attributes(pEl, pos)) {
    outError = "point_light: Missing/invalid <position x= y= z=>.";
    return false;
  }
  l->setPosition(pos);

  outScene.addLight(std::move(l));
  return true;
}

// Parse <parallel_light> reads mandatory <color> and <direction> and creates and adds the light to the scene (not normalized!)
bool SceneParser::parseParallelLight(const tinyxml2::XMLElement *el, Scene &outScene, std::string &outError) const {
  auto l = std::make_unique<ParallelLight>();

  Color c{};
  if (!readColorChild(el, c, outError, "parallel_light"))
    return false;
  l->setColor(c);

  Vec3 dir{};
  const tinyxml2::XMLElement *dEl = xmlutils::getRequiredChild(el, "direction", outError, "parallel_light");
  if (!dEl || !xmlutils::readVec3Attributes(dEl, dir)) {
    outError = "parallel_light: Missing/invalid <direction x= y= z=>.";
    return false;
  }
  l->setDirection(dir); // normalize inside setter

  outScene.addLight(std::move(l));
  return true;
}

// Parse <spot_light> not implemented yet
bool SceneParser::parseSpotLight(const tinyxml2::XMLElement *, Scene &, std::string &outError) const {
  outError = "<spot_light> not supported yet.";
  return false;
}

#include "parser/xml-parser/scene_parser.h"
#include "parser/xml-parser/utils/xml_parser_utils.h"
#include "scene/lights/utils/lights.h"

#include <cstring>
#include <memory>

namespace {

static float degToRad(float deg) {
  return deg * (3.14159265358979323846f / 180.0f);
}

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

// Parse <spot_light>
bool SceneParser::parseSpotLight(const tinyxml2::XMLElement *el, Scene &outScene, std::string &outError) const {
  auto l = std::make_unique<SpotLight>();

  // color
  Color c{};
  if (!readColorChild(el, c, outError, "spot_light"))
    return false;
  l->setColor(c);

  // position
  Vec3 pos{};
  const tinyxml2::XMLElement *pEl = xmlutils::getRequiredChild(el, "position", outError, "spot_light");
  if (!pEl || !xmlutils::readVec3Attributes(pEl, pos)) {
    outError = "spot_light: Missing/invalid <position x= y= z=>.";
    return false;
  }
  l->setPosition(pos);

  // direction
  Vec3 dir{};
  const tinyxml2::XMLElement *dEl = xmlutils::getRequiredChild(el, "direction", outError, "spot_light");
  if (!dEl || !xmlutils::readVec3Attributes(dEl, dir)) {
    outError = "spot_light: Missing/invalid <direction x= y= z=>.";
    return false;
  }
  l->setDirection(dir);

  // falloff (alpha1/alpha2)
  const tinyxml2::XMLElement *fEl = xmlutils::getRequiredChild(el, "falloff", outError, "spot_light");
  if (!fEl) return false;

  float a1 = 0.f, a2 = 0.f;
  if (!xmlutils::readFloatAttribute(fEl, "alpha1", a1) ||
      !xmlutils::readFloatAttribute(fEl, "alpha2", a2)) {
    outError = "spot_light: <falloff> must have alpha1 and alpha2 attributes.";
    return false;
  }

  // interpret as degrees in XML -> convert to radians for math
  l->setFalloff(degToRad(a1), degToRad(a2));

  outScene.addLight(std::move(l));
  return true;
}

bool SceneParser::parseAreaLight(const tinyxml2::XMLElement *el, Scene &outScene, std::string &outError) const {
  auto l = std::make_unique<AreaLight>();

  // color
  Color c{};
  if (!readColorChild(el, c, outError, "area_light"))
    return false;
  l->setColor(c);

  // center
  Vec3 center{};
  const tinyxml2::XMLElement *cEl = xmlutils::getRequiredChild(el, "center", outError, "area_light");
  if (!cEl || !xmlutils::readVec3Attributes(cEl, center)) {
    outError = "area_light: Missing/invalid <center x= y= z=>.";
    return false;
  }
  l->setCenter(center);

  // half_width_vec
  Vec3 hw{};
  const tinyxml2::XMLElement *uEl = xmlutils::getRequiredChild(el, "half_width_vec", outError, "area_light");
  if (!uEl || !xmlutils::readVec3Attributes(uEl, hw)) {
    outError = "area_light: Missing/invalid <half_width_vec x= y= z=>.";
    return false;
  }
  l->setHalfWidthVec(hw);

  // half_height_vec
  Vec3 hh{};
  const tinyxml2::XMLElement *vEl = xmlutils::getRequiredChild(el, "half_height_vec", outError, "area_light");
  if (!vEl || !xmlutils::readVec3Attributes(vEl, hh)) {
    outError = "area_light: Missing/invalid <half_height_vec x= y= z=>.";
    return false;
  }
  l->setHalfHeightVec(hh);

  outScene.addLight(std::move(l));
  return true;
}

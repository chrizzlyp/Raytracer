#include "parser/scene_parser.h"

#include "parser/xml_parser_utils.h"
#include "parser/obj-parser/object_parser.h"

#include "scene/surfaces/sphere.h"
#include "scene/surfaces/mesh.h"

#include <cstring>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <vector>

static std::vector<TrianglePrimitive> buildTrianglesFromObj(const ObjMeshData& data) {
  std::vector<TrianglePrimitive> tris;

  if (data.position.size() % 9 != 0)
    throw std::runtime_error("OBJ expanded position array must be multiple of 9 floats (triangle).");

  const bool hasNormals = !data.normal.empty();
  const bool hasUVs     = !data.texcoord.empty();

  if (hasNormals && data.normal.size() != data.position.size())
    throw std::runtime_error("OBJ normal array size mismatch.");

  const size_t vertexCount = data.position.size() / 3;
  if (hasUVs && data.texcoord.size() != vertexCount * 2)
    throw std::runtime_error("OBJ texcoord array size mismatch.");

  tris.reserve(data.position.size() / 9);

  size_t uvIdx = 0;
  for (size_t i = 0; i < data.position.size(); i += 9) {
    TrianglePrimitive t;

    t.v0 = Vec3{data.position[i+0], data.position[i+1], data.position[i+2]};
    t.v1 = Vec3{data.position[i+3], data.position[i+4], data.position[i+5]};
    t.v2 = Vec3{data.position[i+6], data.position[i+7], data.position[i+8]};

    if (hasNormals) {
      t.n0 = Vec3{data.normal[i+0], data.normal[i+1], data.normal[i+2]};
      t.n1 = Vec3{data.normal[i+3], data.normal[i+4], data.normal[i+5]};
      t.n2 = Vec3{data.normal[i+6], data.normal[i+7], data.normal[i+8]};
    } else {
      // Fallback (besser wäre Face-Normal berechnen, falls du Vec Ops hast)
      t.n0 = t.n1 = t.n2 = Vec3{0,1,0};
    }

    if (hasUVs) {
      t.uv0 = Vec3{data.texcoord[uvIdx+0], data.texcoord[uvIdx+1], 0};
      t.uv1 = Vec3{data.texcoord[uvIdx+2], data.texcoord[uvIdx+3], 0};
      t.uv2 = Vec3{data.texcoord[uvIdx+4], data.texcoord[uvIdx+5], 0};
      uvIdx += 6;
    } else {
      t.uv0 = t.uv1 = t.uv2 = Vec3{0,0,0};
    }

    tris.push_back(t);
  }

  return tris;
}

bool SceneParser::parseSphere(const tinyxml2::XMLElement *sphereEl, Scene &outScene, std::string &outError) const {
  float radius = 1.0f;
  if (!xmlutils::readFloatAttribute(sphereEl, "radius", radius)) {
    outError = "<sphere> must have float attribute radius.";
    return false;
  }

  const tinyxml2::XMLElement *posEl =
      xmlutils::getRequiredChild(sphereEl, "position", outError, "sphere");
  if (!posEl) return false;

  Vec3 center{};
  if (!xmlutils::readVec3Attributes(posEl, center)) {
    outError = "<sphere><position> must have x,y,z float attributes.";
    return false;
  }

  auto s = std::make_unique<Sphere>();
  s->setRadius(radius);
  s->setCenterPosition(center);

  // TODO: Material/Transform (wenn du es später willst)

  outScene.addSurface(std::move(s));
  return true;
}

bool SceneParser::parseMesh(const tinyxml2::XMLElement *meshEl, Scene &outScene, std::string &outError) const {
  const char *nameAttr = meshEl ? meshEl->Attribute("name") : nullptr;
  if (!nameAttr || std::string(nameAttr).empty()) {
    outError = "<mesh> is missing attribute 'name'.";
    return false;
  }

  try {
    // Schutz gegen Pfad-Traversal: nur Dateiname verwenden
    std::filesystem::path fileName = std::filesystem::path(nameAttr).filename();
    std::filesystem::path objPath  = std::filesystem::path("../assets/objects") / fileName;

    std::string objText = xmlutils::readTextFileOrThrow(objPath);
    ObjMeshData data = parseObj(objText);

    std::vector<TrianglePrimitive> tris = buildTrianglesFromObj(data);

    auto m = std::make_unique<Mesh>();
    m->setTrianglePrimitives(std::move(tris));

    // TODO: Material/Transform (wenn du es später willst)

    outScene.addSurface(std::move(m));
    return true;
  } catch (const std::exception &e) {
    outError = std::string("Mesh load/parse failed: ") + e.what();
    return false;
  }
}

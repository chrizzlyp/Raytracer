#include "parser/xml-parser/scene_parser.h"

#include "parser/obj-parser/object_parser.h"
#include "parser/xml-parser/utils/xml_parser_utils.h"

#include "scene/surfaces/mesh.h"
#include "scene/surfaces/sphere.h"

#include <cstring>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <vector>

static std::vector<TrianglePrimitive> buildTrianglesFromObj(const ObjMeshData &data) {
  std::vector<TrianglePrimitive> tris;

  // 3 vertices per primitiv (3 values per vertex)
  if (data.position.size() % 9 != 0)
    throw std::runtime_error("OBJ expanded position array must be multiple of 9 floats (triangle).");

  const bool hasNormals = !data.normal.empty();
  const bool hasUVs = !data.texcoord.empty();

  // each vertex neads a normal
  if (hasNormals && data.normal.size() != data.position.size())
    throw std::runtime_error("OBJ normal array size mismatch.");

  // if a primitiv has textures each vertex neads a texture
  const size_t vertexCount = data.position.size() / 3;
  if (hasUVs && data.texcoord.size() != vertexCount * 2)
    throw std::runtime_error("OBJ texcoord array size mismatch.");

  tris.reserve(data.position.size() / 9);

  size_t uvIdx = 0;
  for (size_t i = 0; i < data.position.size(); i += 9) {
    TrianglePrimitive t;

    // positions
    t.v0 = Vec3{data.position[i + 0], data.position[i + 1], data.position[i + 2]};
    t.v1 = Vec3{data.position[i + 3], data.position[i + 4], data.position[i + 5]};
    t.v2 = Vec3{data.position[i + 6], data.position[i + 7], data.position[i + 8]};

    // normals
    if (hasNormals) {
      t.n0 = Vec3{data.normal[i + 0], data.normal[i + 1], data.normal[i + 2]};
      t.n1 = Vec3{data.normal[i + 3], data.normal[i + 4], data.normal[i + 5]};
      t.n2 = Vec3{data.normal[i + 6], data.normal[i + 7], data.normal[i + 8]};
    } else {
      // Fallback normal if OBJ has none
      t.n0 = t.n1 = t.n2 = Vec3{0, 1, 0};
    }

    // textures
    if (hasUVs) {
      t.uv0 = Vec3{data.texcoord[uvIdx + 0], data.texcoord[uvIdx + 1], 0};
      t.uv1 = Vec3{data.texcoord[uvIdx + 2], data.texcoord[uvIdx + 3], 0};
      t.uv2 = Vec3{data.texcoord[uvIdx + 4], data.texcoord[uvIdx + 5], 0};
      uvIdx += 6;
    } else {
      t.uv0 = t.uv1 = t.uv2 = Vec3{0, 0, 0};
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

  // Locates the <position> 
  const tinyxml2::XMLElement *posEl = xmlutils::getRequiredChild(sphereEl, "position", outError, "sphere");
  if (!posEl)
    return false;

  // Read center position (x, y, z attributes)
  Vec3 center{};
  if (!xmlutils::readVec3Attributes(posEl, center)) {
    outError = "<sphere><position> must have x,y,z float attributes.";
    return false;
  }

  auto s = std::make_unique<Sphere>();
  s->setRadius(radius);
  s->setCenterPosition(center);

  // parses and adds the <material> to the scene
  Material material;
  if (!parseMaterial(sphereEl, material, outError))
    return false;
  s->setMaterial(material);

  // parses and adds the <transformation> to the scene
  Transform transform;
  if (!parseTransforms(sphereEl, transform, outError))
    return false;
  s->setTransform(transform);

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
    std::filesystem::path fileName = std::filesystem::path(nameAttr).filename();
    std::filesystem::path objPath = std::filesystem::path("../assets/objects") / fileName;

    // Load OBJ file contents and parse into mesh data
    std::string objText = xmlutils::readTextFileOrThrow(objPath);
    ObjMeshData data = parseObj(objText);

    // Convert parsed OBJ data into scenes triangle primitives
    std::vector<TrianglePrimitive> tris = buildTrianglesFromObj(data);

    auto m = std::make_unique<Mesh>();
    m->setTrianglePrimitives(std::move(tris));

    // parses and adds the <material> to the scene
    Material material;
    if (!parseMaterial(meshEl, material, outError))
      return false;
    m->setMaterial(material);

    // parses and adds the <transformation> to the scene
    Transform transform;
    if (!parseTransforms(meshEl, transform, outError))
      return false;
    m->setTransform(transform);

    outScene.addSurface(std::move(m));
    return true;
  } catch (const std::exception &e) {
    outError = std::string("Mesh load/parse failed: ") + e.what();
    return false;
  }
}

// Parse the <material> section of the surface object
bool SceneParser::parseMaterial(const tinyxml2::XMLElement *parentEl, Material &outMaterial, std::string &outError) const {
  if (!parentEl) {
    outError = "parseMaterial: parent element is null.";
    return false;
  }

  // A surface can have either a solid OR a textured material 
  const tinyxml2::XMLElement *solidEl = parentEl->FirstChildElement("material_solid");
  const tinyxml2::XMLElement *texturedEl = parentEl->FirstChildElement("material_textured");

  // No material specified
  if (!solidEl && !texturedEl) {
    return true;
  }

  // Both material types specified at once
  if (solidEl && texturedEl) {
    outError = "Object has both <material_solid> and <material_textured>. Only one is allowed.";
    return false;
  }

  // Select the active material
  const tinyxml2::XMLElement *materialEl = solidEl ? solidEl : texturedEl;
  outMaterial.setType(solidEl ? MaterialType::SOLID : MaterialType::TEXTURED);

  // Solid material requires color
  if (solidEl) {
    const tinyxml2::XMLElement *colorEl = materialEl->FirstChildElement("color");
    if (!colorEl) {
      outError = "Missing <color> inside <material_solid>.";
      return false;
    }

    float r = 0.f, g = 0.f, b = 0.f;
    if (colorEl->QueryFloatAttribute("r", &r) != tinyxml2::XML_SUCCESS ||
        colorEl->QueryFloatAttribute("g", &g) != tinyxml2::XML_SUCCESS ||
        colorEl->QueryFloatAttribute("b", &b) != tinyxml2::XML_SUCCESS) {
      outError = "<color> must have float attributes r, g, b.";
      return false;
    }

    outMaterial.setColor(Color{r, g, b});
  }

  // Textured material requires a name of a texture (png)
  if (texturedEl) {
    const tinyxml2::XMLElement *texEl = materialEl->FirstChildElement("texture");
    if (!texEl) {
      outError = "Missing <texture> inside <material_textured>.";
      return false;
    }

    const char *texName = texEl->Attribute("name");
    if (!texName || std::string(texName).empty()) {
      outError = "<texture> must have a non-empty name attribute.";
      return false;
    }

    outMaterial.setTextureName(texName);
  }

  // <phong> shading
  {
    const tinyxml2::XMLElement *phongEl = materialEl->FirstChildElement("phong");
    if (!phongEl) {
      outError = "Missing <phong> inside material.";
      return false;
    }

    PhongParams phong{};
    if (phongEl->QueryFloatAttribute("ka", &phong.kAmbient) != tinyxml2::XML_SUCCESS ||
        phongEl->QueryFloatAttribute("kd", &phong.kDiffuse) != tinyxml2::XML_SUCCESS ||
        phongEl->QueryFloatAttribute("ks", &phong.kSpecular) != tinyxml2::XML_SUCCESS ||
        phongEl->QueryFloatAttribute("exponent", &phong.exponentShininess) != tinyxml2::XML_SUCCESS) {
      outError = "<phong> must have float attributes ka, kd, ks, exponent.";
      return false;
    }

    outMaterial.setPhong(phong);
  }

  // <reflectance r="float"/>
  {
    const tinyxml2::XMLElement *reflEl = materialEl->FirstChildElement("reflectance");
    if (reflEl) {
      float reflectance = outMaterial.reflectance();
      if (reflEl->QueryFloatAttribute("r", &reflectance) != tinyxml2::XML_SUCCESS) {
        outError = "<reflectance> must have float attribute r.";
        return false;
      }
      outMaterial.setReflectance(reflectance);
    }
  }

  // <transmittance t="float"/>
  {
    const tinyxml2::XMLElement *transEl = materialEl->FirstChildElement("transmittance");
    if (transEl) {
      float transmittance = outMaterial.transmittance();
      if (transEl->QueryFloatAttribute("t", &transmittance) != tinyxml2::XML_SUCCESS) {
        outError = "<transmittance> must have float attribute t.";
        return false;
      }
      outMaterial.setTransmittance(transmittance);
    }
  }

  // <refraction iof="float"/>
  {
    const tinyxml2::XMLElement *refrEl = materialEl->FirstChildElement("refraction");
    if (refrEl) {
      float ior = outMaterial.ior();
      if (refrEl->QueryFloatAttribute("iof", &ior) != tinyxml2::XML_SUCCESS) {
        outError = "<refraction> must have float attribute iof.";
        return false;
      }
      outMaterial.setIor(ior);
    }
  }

  return true;
}

// Parse the <transform> block of a surface object
bool SceneParser::parseTransforms(const tinyxml2::XMLElement *parentEl, Transform &outTransform, std::string &outError) const {
  if (!parentEl) {
    outError = "parseTransforms: parent element is null.";
    return false;
  }

  // locate <transform> section in <surface>
  const tinyxml2::XMLElement *transformsEl = parentEl->FirstChildElement("transform");

  // <transform> not exisiting
  if (!transformsEl) {
    outTransform = Transform{};
    return true;
  }

  // Starts with identity matrix
  outTransform = Transform{};

  // Iterate over all transformation operations (<translate>, <scale>, <rotateX>, ...)
  for (const tinyxml2::XMLElement *opEl = transformsEl->FirstChildElement();
       opEl != nullptr;
       opEl = opEl->NextSiblingElement()) {
    const char *opName = opEl->Name();
    if (!opName)
      continue;

    // <translate x="" y="" z=""/>
    if (std::strcmp(opName, "translate") == 0) {
      float x = 0.f, y = 0.f, z = 0.f;
      if (opEl->QueryFloatAttribute("x", &x) != tinyxml2::XML_SUCCESS ||
          opEl->QueryFloatAttribute("y", &y) != tinyxml2::XML_SUCCESS ||
          opEl->QueryFloatAttribute("z", &z) != tinyxml2::XML_SUCCESS) {
        outError = "<translate> must have float attributes x, y, z.";
        return false;
      }
      outTransform.translate(Vec3{x, y, z});
    
    // <scale x="" y="" z=""/>
    } else if (std::strcmp(opName, "scale") == 0) {
      float x = 1.f, y = 1.f, z = 1.f;
      if (opEl->QueryFloatAttribute("x", &x) != tinyxml2::XML_SUCCESS ||
          opEl->QueryFloatAttribute("y", &y) != tinyxml2::XML_SUCCESS ||
          opEl->QueryFloatAttribute("z", &z) != tinyxml2::XML_SUCCESS) {
        outError = "<scale> must have float attributes x, y, z.";
        return false;
      }
      outTransform.scale(Vec3{x, y, z});

    // <rotateX theta=""/>, <rotateY .../>, <rotateZ .../> 
    } else if (std::strcmp(opName, "rotateX") == 0) {
      float theta = 0.f;
      if (opEl->QueryFloatAttribute("theta", &theta) != tinyxml2::XML_SUCCESS) {
        outError = "<rotateX> must have float attribute theta.";
        return false;
      }
      outTransform.rotateX(theta);
    } else if (std::strcmp(opName, "rotateY") == 0) {
      float theta = 0.f;
      if (opEl->QueryFloatAttribute("theta", &theta) != tinyxml2::XML_SUCCESS) {
        outError = "<rotateY> must have float attribute theta.";
        return false;
      }
      outTransform.rotateY(theta);
    } else if (std::strcmp(opName, "rotateZ") == 0) {
      float theta = 0.f;
      if (opEl->QueryFloatAttribute("theta", &theta) != tinyxml2::XML_SUCCESS) {
        outError = "<rotateZ> must have float attribute theta.";
        return false;
      }
      outTransform.rotateZ(theta);
    } else {
      outError = std::string("Unknown transform operation <") + opName + "> inside <transforms>.";
      return false;
    }
  }

  return true;
}

#include "xml_parser_utils.h"

namespace xmlutils {

// Returns true on success, false if element is null or attribute is missing/invalid.
bool readFloatAttribute(const tinyxml2::XMLElement *element, const char *name, float &out) {
  if (!element)
    return false;
  return element->QueryFloatAttribute(name, &out) == tinyxml2::XML_SUCCESS;
}

// Returns true on success, false if element is null or attribute is missing/invalid.
bool readIntAttribute(const tinyxml2::XMLElement *element, const char *name, int &out) {
  if (!element)
    return false;
  return element->QueryIntAttribute(name, &out) == tinyxml2::XML_SUCCESS;
}
// Returns true on success, false if any attribute is missing/invalid.
bool readVec3Attributes(const tinyxml2::XMLElement *element, Vec3 &out) {
  if (!element)
    return false;
  return element->QueryFloatAttribute("x", &out.x) == tinyxml2::XML_SUCCESS &&
         element->QueryFloatAttribute("y", &out.y) == tinyxml2::XML_SUCCESS &&
         element->QueryFloatAttribute("z", &out.z) == tinyxml2::XML_SUCCESS;
}

// Finds a required child element by name.
const tinyxml2::XMLElement *getRequiredChild(const tinyxml2::XMLElement *parent, const char *childName, std::string &outError, const char *contextName) {
  if (!parent) {
    outError = "Internal error: parent element is null.";
    return nullptr;
  }

  const tinyxml2::XMLElement *child = parent->FirstChildElement(childName);
  if (!child) {
    if (contextName) {
      outError = std::string("Missing <") + childName + "> inside <" + contextName + ">.";
    } else {
      outError = std::string("Missing <") + childName + ">.";
    }
    return nullptr;
  }
  return child;
}

} // namespace xmlutils

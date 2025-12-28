#ifndef XML_PARSER_UTILS_H
#define XML_PARSER_UTILS_H

#include "scene/scene.h"
#include "tinyxml2.h"
#include <string>
#include <filesystem>  
#include <fstream>
#include <sstream>

namespace xmlutils {

// Tries to read a float attribute 'name' from 'element' into 'out'
bool readFloatAttribute(const tinyxml2::XMLElement *element, const char *name, float &out);
// Tries to read an int attribute 'name' from 'element' into 'out'
bool readIntAttribute(const tinyxml2::XMLElement *element, const char *name, int &out);
// Reads x/y/z float attributes into a Vec3
bool readVec3Attributes(const tinyxml2::XMLElement *element, Vec3 &out);
// Finds a required child element <childName> under 'parent'
const tinyxml2::XMLElement *getRequiredChild(const tinyxml2::XMLElement *parent, const char *childName, std::string &outError, const char *contextName);

std::string readTextFileOrThrow(const std::filesystem::path& p);
} // namespace xmlutils

#endif
#ifndef OBJECT_PARSER_H
#define OBJECT_PARSER_H

#include <string>
#include <vector>

struct ObjMeshData {
    std::vector<float> position;  // x,y,z pro Vertex
    std::vector<float> texcoord;  // u,v pro Vertex
    std::vector<float> normal;    // x,y,z pro Vertex
};

// Parst einen OBJ-Text und gibt "expanded" Triangle-Arrays zurück (kein Index-Buffer),
// analog zu deinem JS: position/texcoord/normal sind flache float-arrays.
ObjMeshData parseObj(const std::string& text);

#endif
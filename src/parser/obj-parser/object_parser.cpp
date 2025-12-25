#include "parser/obj-parser/object_parser.h"

#include <sstream>
#include <iostream>
#include <unordered_map>
#include <functional>
#include <cctype>

namespace {

static inline std::string trim(const std::string& s) {
    size_t b = 0;
    while (b < s.size() && std::isspace(static_cast<unsigned char>(s[b]))) ++b;
    size_t e = s.size();
    while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) --e;
    return s.substr(b, e - b);
}

static inline bool starts_with(const std::string& s, const char* prefix) {
    for (size_t i = 0; prefix[i]; ++i) {
        if (i >= s.size() || s[i] != prefix[i]) return false;
    }
    return true;
}

static inline std::vector<std::string> split_ws(const std::string& s) {
    std::istringstream iss(s);
    std::vector<std::string> out;
    std::string tok;
    while (iss >> tok) out.push_back(tok);
    return out;
}

static inline std::vector<std::string> split_char(const std::string& s, char delim) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : s) {
        if (c == delim) {
            out.push_back(cur);
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    out.push_back(cur);
    return out;
}

static inline bool to_float(const std::string& s, float& out) {
    if (s.empty()) return false;
    try {
        size_t idx = 0;
        out = std::stof(s, &idx);
        return idx == s.size();
    } catch (...) {
        return false;
    }
}

static inline bool to_int(const std::string& s, int& out) {
    if (s.empty()) return false;
    try {
        size_t idx = 0;
        out = std::stoi(s, &idx);
        return idx == s.size();
    } catch (...) {
        return false;
    }
}

} // namespace

ObjMeshData parseObj(const std::string& text) {
    // Repos mit Dummy an Index 0 (wie in deinem JS)
    std::vector<std::vector<float>> vertexPositionRepo = { {0.f, 0.f, 0.f} };
    std::vector<std::vector<float>> normalVectorRepo   = { {0.f, 0.f, 0.f} };
    std::vector<std::vector<float>> textureCoordRepo   = { {0.f, 0.f} };

    ObjMeshData out;

    auto parseIndex = [&](const std::string& indexString, int repoLength) -> int {
        // JS: Number("") -> 0 => meaning "empty"
        if (indexString.empty()) return 0;

        int idx = 0;
        if (!to_int(indexString, idx)) {
            std::cerr << "The obj index is not an integer: '" << indexString << "'\n";
            return 0;
        }

        if (idx == 0) {
            return 0; // empty (0 is not existing)
        }

        if (idx > 0) {
            if (idx >= repoLength) {
                std::cerr << "The obj index is out of range: " << idx
                          << " with " << repoLength << " element in found\n";
                return 0;
            }
            return idx;
        } else {
            // Dein JS: negative => invalid
            std::cerr << "The obj index: " << idx << " is an unvalid index\n";
            return 0;
        }
    };

    auto addVertex = [&](const std::string& token) {
        // "2//1" -> ["2", "", "1"]
        const auto comps = split_char(token, '/');

        const std::string vString  = comps.size() > 0 ? comps[0] : "";
        const std::string vtString = comps.size() > 1 ? comps[1] : "";
        const std::string vnString = comps.size() > 2 ? comps[2] : "";

        const int vIndex  = parseIndex(vString,  static_cast<int>(vertexPositionRepo.size()));
        const int vtIndex = parseIndex(vtString, static_cast<int>(textureCoordRepo.size()));
        const int vnIndex = parseIndex(vnString, static_cast<int>(normalVectorRepo.size()));

        if (vIndex == 0) {
            std::cerr << "Face: (f " << token << ") is using a undefined vertex position (v)!\n";
            return;
        }

        // Position (immer 3 floats pushen; falls nur 2D -> z=0)
        const auto& v = vertexPositionRepo[vIndex];
        out.position.push_back(v.size() > 0 ? v[0] : 0.f);
        out.position.push_back(v.size() > 1 ? v[1] : 0.f);
        out.position.push_back(v.size() > 2 ? v[2] : 0.f);

        // Texcoord (oder placeholder 0,0)
        if (vtIndex != 0) {
            const auto& vt = textureCoordRepo[vtIndex];
            out.texcoord.push_back(vt.size() > 0 ? vt[0] : 0.f);
            out.texcoord.push_back(vt.size() > 1 ? vt[1] : 0.f);
        } else {
            out.texcoord.push_back(0.f);
            out.texcoord.push_back(0.f);
        }

        // Normal (oder placeholder 0,0,0)
        if (vnIndex != 0) {
            const auto& vn = normalVectorRepo[vnIndex];
            out.normal.push_back(vn.size() > 0 ? vn[0] : 0.f);
            out.normal.push_back(vn.size() > 1 ? vn[1] : 0.f);
            out.normal.push_back(vn.size() > 2 ? vn[2] : 0.f);
        } else {
            out.normal.push_back(0.f);
            out.normal.push_back(0.f);
            out.normal.push_back(0.f);
        }
    };

    auto handleVertexPosition = [&](const std::vector<std::string>& parts) {
        std::vector<float> nums;
        nums.reserve(parts.size());
        for (const auto& p : parts) {
            float f = 0.f;
            if (!to_float(p, f)) {
                std::cerr << "Invalid float in vertex position: v " << p << "\n";
                return;
            }
            nums.push_back(f);
        }
        if (nums.size() < 2 || nums.size() > 3) {
            std::cerr << "In the obj file is a invalid vertex position: v (count="
                      << nums.size() << ")\n";
        }
        vertexPositionRepo.push_back(std::move(nums));
    };

    auto handleNormalVector = [&](const std::vector<std::string>& parts) {
        std::vector<float> nums;
        nums.reserve(parts.size());
        for (const auto& p : parts) {
            float f = 0.f;
            if (!to_float(p, f)) {
                std::cerr << "Invalid float in normal vector: vn " << p << "\n";
                return;
            }
            nums.push_back(f);
        }
        if (nums.size() < 2 || nums.size() > 3) {
            std::cerr << "In the obj file has invalid normal vectors: vn (count="
                      << nums.size() << ")\n";
        }
        normalVectorRepo.push_back(std::move(nums));
    };

    auto handleTextureCoord = [&](const std::vector<std::string>& parts) {
        std::vector<float> nums;
        nums.reserve(parts.size());
        for (const auto& p : parts) {
            float f = 0.f;
            if (!to_float(p, f)) {
                std::cerr << "Invalid float in texture coord: vt " << p << "\n";
                return;
            }
            nums.push_back(f);
        }
        if (nums.size() != 2) {
            std::cerr << "The obj file is having invalid texture coordinates: vt (count="
                      << nums.size() << ")\n";
        }
        textureCoordRepo.push_back(std::move(nums));
    };

    auto handleFace = [&](const std::vector<std::string>& parts) {
        if (parts.size() != 3) {
            std::cerr << "The obj file is having a face unable to be mapped to a triangle containing "
                      << parts.size() << " vertecis\n";
            return;
        }
        addVertex(parts[0]);
        addVertex(parts[1]);
        addVertex(parts[2]);
    };

    std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> keywords = {
        {"v",  handleVertexPosition},
        {"vn", handleNormalVector},
        {"vt", handleTextureCoord},
        {"f",  handleFace},
    };

    std::istringstream stream(text);
    std::string line;
    int lineNo = 0;

    while (std::getline(stream, line)) {
        ++lineNo;
        const std::string lineText = trim(line);
        if (lineText.empty() || starts_with(lineText, "#")) continue;

        // keyword + rest
        size_t firstSpace = lineText.find_first_of(" \t\r");
        std::string keyword, rest;
        if (firstSpace == std::string::npos) {
            keyword = lineText;
            rest = "";
        } else {
            keyword = lineText.substr(0, firstSpace);
            rest = trim(lineText.substr(firstSpace));
        }

        auto it = keywords.find(keyword);
        if (it == keywords.end()) {
            std::cerr << "Undefined keyword: " << keyword << " at line " << lineNo << "\n";
            continue;
        }

        const auto parts = rest.empty() ? std::vector<std::string>{} : split_ws(rest);
        it->second(parts);
    }

    return out;
}

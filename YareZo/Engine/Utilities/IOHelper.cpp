//
// Created by Drew on 11/27/2019.
//
#include "Utilities/IOHelper.h"
#include "Core/Yzh.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <tinyobjloader/tiny_obj_loader.h>
#include <unordered_map>

namespace std {
    template<> struct hash<Yarezo::Vertex> {
        size_t operator()(Yarezo::Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

namespace Yarezo::Utilities {

    std::vector<std::string> readFile(const std::string& filename) {
        std::ifstream file(filename);

        if (!file.is_open()) {
            YZ_ERROR("File '" + filename + "' was unable to open.");
            throw std::runtime_error("File '" + filename + "' was unable to open.");
        }

        // captures lines not separated by whitespace
        std::vector<std::string> myLines;
        std::copy(std::istream_iterator<std::string>(file),
                  std::istream_iterator<std::string>(),
                  std::back_inserter(myLines));

        return myLines;
    }

	void loadModel(const std::string& modelFilePath, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelFilePath.c_str())) {
            YZ_ERROR(warn + err);
        }

       std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex = {};

                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };

                vertex.color = { 1.0f, 1.0f, 1.0f };

                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]);
            }
        }
	}

}

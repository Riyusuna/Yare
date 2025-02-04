#ifndef YARE_DATASTRUCTURES_H
#define YARE_DATASTRUCTURES_H

#include <glm/glm.hpp>

namespace Yare {
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec3 normal;
        glm::vec2 uv;

        bool operator==(const Vertex& other) const {
            return pos == other.pos && uv == other.uv && normal == other.normal && color == other.normal;
        }
    };

    struct UniformBufferObject {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    struct UboDataDynamic {
        glm::mat4* model = nullptr;
    };

    struct UniformVS {
        glm::mat4 view;
        glm::mat4 projection;
    };
}  // namespace Yare

#endif

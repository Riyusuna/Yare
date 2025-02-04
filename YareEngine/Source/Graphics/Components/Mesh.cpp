#include "Mesh.h"

#include "Utilities/IOHelper.h"

namespace Yare::Graphics {

    Mesh::Mesh(const std::string& meshFilePath) { loadMeshFromFile(meshFilePath); }

    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
        createBuffers(vertices, indices);
    }

    Mesh::~Mesh() {
        delete m_VertexBuffer;
        delete m_IndexBuffer;
    }

    void Mesh::loadMeshFromFile(const std::string& meshFilePath) {
        if (m_VertexBuffer || m_IndexBuffer) {
            throw std::runtime_error("Mesh already has buffers allocated.");
        }

        m_FilePath = meshFilePath;

        if (!meshFilePath.empty()) {
            std::vector<Vertex>   vertices;
            std::vector<uint32_t> indices;

            Utilities::loadMesh(meshFilePath, vertices, indices);
            createBuffers(vertices, indices);
        }
    }

    void Mesh::createBuffers(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
        // Vertex Buffers
        VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();

        m_VertexBuffer = new Buffer(BufferUsage::VERTEX, (size_t)bufferSize, vertices.data());

        // Index Buffers
        bufferSize = sizeof(indices[0]) * indices.size();

        m_IndexBuffer = new Buffer(BufferUsage::INDEX, (size_t)bufferSize, indices.data());
    }
}  // namespace Yare::Graphics

#ifndef DWARF_MESHDATA_H_
#define DWARF_MESHDATA_H_
#pragma once

#include <glm/gtx/hash.hpp>

#include "Tools.h"
#include "Material.h"

namespace Dwarf
{
    class Vertex
    {
    public:
        Vertex() {}

        Vertex(const glm::vec3 &pos, const glm::vec3 &normal)
            : pos(pos), normal(normal)
        {}

        Vertex(const glm::vec3 &pos, const glm::vec3 &normal, const glm::vec2 &uv)
            : pos(pos), normal(normal), uv(uv)
        {}

        static vk::VertexInputBindingDescription getBindingDescription()
        {
            return (vk::VertexInputBindingDescription(0, sizeof(Vertex), vk::VertexInputRate::eVertex));
        }

        static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions()
        {
            std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions =
            {
                vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos)),
                vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal)),
                vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, uv))
            };
            return (attributeDescriptions);
        }

        bool operator==(const Vertex &rhs) const
        {
            return (pos == rhs.pos && normal == rhs.normal && uv == rhs.uv);
        }

        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    struct MeshData
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        size_t materialID;
    };
}

namespace std
{
    template<> struct hash<Dwarf::Vertex>
    {
        size_t operator()(const Dwarf::Vertex &vertex) const
        {
            return (((hash<glm::vec3>()(vertex.pos) ^
                (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^
                (hash<glm::vec2>()(vertex.uv) << 1));
        }
    };
}

#endif // DWARF_MESHDATA_H_

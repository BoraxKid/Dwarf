#ifndef DWARF_MESH_H_
#define DWARF_MESH_H_
#pragma once

#include <unordered_map>
#include <chrono>

#include "Tools.h"
#include "Submesh.h"
#include "MaterialManager.h"
#include "Transformable.h"

namespace Dwarf
{
	class Renderer;

	struct UniformBufferObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	class Mesh : public Transformable
	{
	public:
        Mesh(const vk::Device &device, Dwarf::MaterialManager &materialManager, const std::string &meshFilename, const vk::DescriptorBufferInfo &lightBufferInfo);
		virtual ~Mesh();

		void loadFromFile(Dwarf::MaterialManager &materialManager, const std::string &filename);
        std::vector<IBuildable *> getBuildables();
        std::vector<Submesh> &getSubmeshes();

	private:
		const vk::Device &_device;
        const vk::DescriptorBufferInfo &_lightBufferInfo;
        std::vector<Submesh> _submeshes;
	};

    struct TmpSubmesh
    {
        Material::ID materialID;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

    class TmpMesh : public Transformable
    {
    public:
        TmpMesh();
        virtual ~TmpMesh();

    private:
        std::vector<Submesh> _submeshes;
    };
}

#endif // DWARF_MESH_H_

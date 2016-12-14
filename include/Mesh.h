#ifndef MESH_H_
#define MESH_H_
#pragma once

#include <unordered_map>
#include <chrono>

#include "Tools.h"
#include "Submesh.h"
#include "MaterialManager.h"

namespace Dwarf
{
	class Renderer;

	struct UniformBufferObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	class Mesh
	{
	public:
		Mesh(const vk::Device &device, Dwarf::MaterialManager &materialManager, const std::string &meshFilename);
		virtual ~Mesh();

		void loadFromFile(Dwarf::MaterialManager &materialManager, const std::string &filename);
        std::vector<IBuildable *> getBuildables();

	private:
		const vk::Device &_device;

        std::vector<Submesh> _submeshes;
	};
}

#endif // MESH_H_

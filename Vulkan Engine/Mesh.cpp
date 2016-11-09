#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace Dwarf
{
	Mesh::Mesh(const vk::Device &device, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, Dwarf::MaterialManager &materialManager, const std::string &meshFilename)
		: _device(device), _commandPool(commandPool), _graphicsQueue(graphicsQueue)
	{
		this->loadFromFile(materialManager, meshFilename);
	}

	Mesh::~Mesh()
	{
		this->_verticesPerMaterials.clear();
		this->_device.freeMemory(this->_buffersMemory, CUSTOM_ALLOCATOR);
		this->_device.destroyBuffer(this->_buffer, CUSTOM_ALLOCATOR);
	}

	void Mesh::loadFromFile(Dwarf::MaterialManager &materialManager, const std::string &filename)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string error;
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &error, filename.c_str(), "materials/"))
			Tools::exitOnError(error);
		std::vector<tinyobj::material_t>::iterator iter = materials.begin();
		std::vector<tinyobj::material_t>::iterator iter2 = materials.end();
		Material *material = nullptr;
		while (iter != iter2)
		{
            material = materialManager.createMaterial(iter->name);
			material->setAmbient(Color(iter->ambient[0], iter->ambient[1], iter->ambient[2]));
			material->setDiffuse(Color(iter->diffuse[0], iter->diffuse[1], iter->diffuse[2]));
			material->setSpecular(Color(iter->specular[0], iter->specular[1], iter->specular[2]));
			material->setTransmittance(Color(iter->transmittance[0], iter->transmittance[1], iter->transmittance[2]));
			material->setEmission(Color(iter->emission[0], iter->emission[1], iter->emission[2]));
			material->setShininess(iter->shininess);
			material->setIor(iter->ior);
			material->setDissolve(iter->dissolve);
			material->setIllum(iter->illum);
			material->setRoughness(iter->roughness);
			material->setMetallic(iter->metallic);
			material->setSheen(iter->sheen);
			material->setClearcoatThickness(iter->clearcoat_thickness);
			material->setClearcoatRoughness(iter->clearcoat_roughness);
			material->setAnisotropy(iter->anisotropy);
			material->setAnisotropyRotation(iter->anisotropy_rotation);
			if (!iter->ambient_texname.empty())
				material->createAmbientTexture(iter->ambient_texname);
			if (!iter->diffuse_texname.empty())
				material->createDiffuseTexture(iter->diffuse_texname);
			if (!iter->specular_texname.empty())
				material->createSpecularTexture(iter->specular_texname);
			if (!iter->specular_highlight_texname.empty())
				material->createSpecularHighlightTexture(iter->specular_highlight_texname);
			if (!iter->bump_texname.empty())
				material->createBumpTexture(iter->bump_texname);
			if (!iter->displacement_texname.empty())
				material->createDisplacementTexture(iter->displacement_texname);
			if (!iter->alpha_texname.empty())
				material->createAlphaTexture(iter->alpha_texname);
			if (!iter->roughness_texname.empty())
				material->createRoughnessTexture(iter->roughness_texname);
			if (!iter->metallic_texname.empty())
				material->createMetallicTexture(iter->metallic_texname);
			if (!iter->sheen_texname.empty())
				material->createSheenTexture(iter->sheen_texname);
			if (!iter->emissive_texname.empty())
				material->createEmissiveTexture(iter->emissive_texname);
			if (!iter->normal_texname.empty())
				material->createNormalTexture(iter->normal_texname);
			this->_verticesPerMaterials[material] = std::vector<Vertex>();
			material = nullptr;
			++iter;
		}
		std::vector<tinyobj::shape_t>::const_iterator iterShapes = shapes.begin();
		std::vector<tinyobj::shape_t>::const_iterator iterShapes2 = shapes.end();
		std::vector<tinyobj::index_t>::const_iterator iterIndices;
		std::vector<tinyobj::index_t>::const_iterator iterIndices2;
		std::unordered_map<Vertex, int> uniqueVertices;
		Vertex vertex;
		this->_vertices.clear();
		this->_indices.clear();

		while (iterShapes != iterShapes2)
		{
			iterIndices = iterShapes->mesh.indices.begin();
			iterIndices2 = iterShapes->mesh.indices.end();
            iterShapes->mesh.material_ids[1];
			while (iterIndices != iterIndices2)
			{
				vertex = Vertex();
                
				vertex.pos = glm::vec3(attrib.vertices.at(3 * iterIndices->vertex_index + 0), attrib.vertices.at(3 * iterIndices->vertex_index + 1), attrib.vertices.at(3 * iterIndices->vertex_index + 2));
				if (!attrib.texcoords.empty())
				vertex.uv = glm::vec2(attrib.texcoords.at(2 * iterIndices->texcoord_index + 0), 1.0f - attrib.texcoords.at(2 * iterIndices->texcoord_index + 1));
				if (uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<int>(this->_vertices.size());
					this->_vertices.push_back(vertex);
				}
				this->_indices.push_back(uniqueVertices.at(vertex));
				++iterIndices;
			}
			++iterShapes;
		}
	}

	void Mesh::createBuffers(vk::PhysicalDeviceMemoryProperties memProperties)
	{
		vk::DeviceSize vertexBufferSize = sizeof(Vertex) * this->_vertices.size();
		vk::DeviceSize indexBufferSize = sizeof(uint32_t) * this->_indices.size();
		vk::DeviceSize uniformBufferSize = sizeof(UniformBufferObject);
		vk::BufferCreateInfo bufferInfo(vk::BufferCreateFlags(), vertexBufferSize, vk::BufferUsageFlagBits::eVertexBuffer);
		vk::Buffer vertexBuffer = this->_device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);
		bufferInfo.setSize(indexBufferSize);
		bufferInfo.setUsage(vk::BufferUsageFlagBits::eIndexBuffer);
		vk::Buffer indexBuffer = this->_device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);
		bufferInfo.setSize(uniformBufferSize);
		bufferInfo.setUsage(vk::BufferUsageFlagBits::eUniformBuffer);
		vk::Buffer uniformBuffer = this->_device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);

		vk::MemoryRequirements memRequirements[3];
		memRequirements[0] = this->_device.getBufferMemoryRequirements(vertexBuffer);
		memRequirements[1] = this->_device.getBufferMemoryRequirements(indexBuffer);
		memRequirements[2] = this->_device.getBufferMemoryRequirements(uniformBuffer);
		this->_vertexBufferOffset = 0;
		this->_indexBufferOffset = memRequirements[0].size + (memRequirements[1].alignment - (memRequirements[0].size % memRequirements[1].alignment));
		this->_uniformBufferOffset = (this->_indexBufferOffset + memRequirements[1].size) + (memRequirements[2].size - ((this->_indexBufferOffset + memRequirements[1].size) % memRequirements[2].alignment));
		bufferInfo.setSize(this->_uniformBufferOffset + memRequirements[2].size);
		bufferInfo.setUsage(vk::BufferUsageFlagBits::eTransferSrc);
		vk::Buffer stagingBuffer = this->_device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);
		uint32_t memTypes = memRequirements[0].memoryTypeBits & memRequirements[1].memoryTypeBits & memRequirements[2].memoryTypeBits;
		vk::MemoryAllocateInfo allocInfo(bufferInfo.size, Tools::getMemoryType(memProperties, memTypes, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
		vk::DeviceMemory stagingBufferMemory = this->_device.allocateMemory(allocInfo, CUSTOM_ALLOCATOR);
		this->_device.bindBufferMemory(stagingBuffer, stagingBufferMemory, 0);
		void *data = this->_device.mapMemory(stagingBufferMemory, this->_vertexBufferOffset, memRequirements[0].size);
		memcpy(data, this->_vertices.data(), static_cast<size_t>(vertexBufferSize));
		this->_device.unmapMemory(stagingBufferMemory);
		data = this->_device.mapMemory(stagingBufferMemory, this->_indexBufferOffset, memRequirements[1].size);
		memcpy(data, this->_indices.data(), static_cast<size_t>(indexBufferSize));
		this->_device.unmapMemory(stagingBufferMemory);
		bufferInfo = vk::BufferCreateInfo(vk::BufferCreateFlags(), this->_uniformBufferOffset + uniformBufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eUniformBuffer);
		this->_buffer = this->_device.createBuffer(bufferInfo, CUSTOM_ALLOCATOR);
		memRequirements[0] = this->_device.getBufferMemoryRequirements(this->_buffer);
		allocInfo = vk::MemoryAllocateInfo(memRequirements[0].size, Tools::getMemoryType(memProperties, memRequirements[0].memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal));
		if (this->_buffersMemory != (vk::DeviceMemory)VK_NULL_HANDLE)
			this->_device.freeMemory(this->_buffersMemory, CUSTOM_ALLOCATOR);
		this->_buffersMemory = this->_device.allocateMemory(allocInfo, CUSTOM_ALLOCATOR);
		this->_device.bindBufferMemory(this->_buffer, this->_buffersMemory, 0);
		vk::CommandBuffer commandBuffer = Tools::beginSingleTimeCommands(this->_device, this->_commandPool);
		commandBuffer.copyBuffer(stagingBuffer, this->_buffer, vk::BufferCopy(0, 0, this->_uniformBufferOffset + uniformBufferSize));
		Tools::endSingleTimeCommands(this->_device, this->_graphicsQueue, this->_commandPool, commandBuffer);

		this->_device.freeMemory(stagingBufferMemory, CUSTOM_ALLOCATOR);
		this->_device.destroyBuffer(stagingBuffer, CUSTOM_ALLOCATOR);
		this->_device.destroyBuffer(uniformBuffer, CUSTOM_ALLOCATOR);
		this->_device.destroyBuffer(indexBuffer, CUSTOM_ALLOCATOR);
		this->_device.destroyBuffer(vertexBuffer, CUSTOM_ALLOCATOR);
	}

	vk::Buffer Mesh::getBuffer() const
	{
		return (this->_buffer);
	}

	size_t Mesh::getIndicesCount() const
	{
		return (this->_indices.size());
	}

	vk::DeviceSize Mesh::getVertexBufferOffset() const
	{
		return (this->_vertexBufferOffset);
	}

	vk::DeviceSize Mesh::getIndexBufferOffset() const
	{
		return (this->_indexBufferOffset);
	}

	vk::DeviceSize Mesh::getUniformBufferOffset() const
	{
		return (this->_uniformBufferOffset);
	}
}

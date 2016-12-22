#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace Dwarf
{
	Mesh::Mesh(const vk::Device &device, Dwarf::MaterialManager &materialManager, const std::string &meshFilename)
		: _device(device)
	{
		this->loadFromFile(materialManager, meshFilename);
	}

	Mesh::~Mesh()
	{
        std::vector<Submesh>::iterator iter = this->_submeshes.begin();
        std::vector<Submesh>::iterator iterEnd = this->_submeshes.end();
        while (iter != iterEnd)
        {
            if (iter->getVerticesCount() != 0 && iter->getIndicesCount() != 0)
                iter->cleanup(this->_device);
            ++iter;
        }
	}

	void Mesh::loadFromFile(Dwarf::MaterialManager &materialManager, const std::string &filename)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string error;
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &error, filename.c_str(), "resources/materials/"))
			Tools::exitOnError(error);
		std::vector<tinyobj::material_t>::iterator iter = materials.begin();
		std::vector<tinyobj::material_t>::iterator iter2 = materials.end();
		Material *material = nullptr;
        this->_submeshes.push_back(Submesh(materialManager.getMaterial("default")));
		while (iter != iter2)
		{
            material = materialManager.createMaterial(iter->name, !iter->diffuse_texname.empty());
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
			/*if (!iter->ambient_texname.empty())
				material->createAmbientTexture(iter->ambient_texname);*/
			if (!iter->diffuse_texname.empty())
				material->createDiffuseTexture(iter->diffuse_texname);
			/*if (!iter->specular_texname.empty())
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
				material->createNormalTexture(iter->normal_texname);*/
            this->_submeshes.push_back(Submesh(material));
			material = nullptr;
			++iter;
		}

		Vertex vertex;
        size_t s = 0;
        size_t indexOffset;
        size_t f;
        size_t fv;
        size_t v;
        int materialID;
        tinyobj::index_t i;
        std::vector<std::unordered_map<Vertex, int>> perMaterialUniqueVertices(this->_submeshes.size());
        std::vector<std::vector<uint32_t>> submeshIndices(this->_submeshes.size());
        std::vector<std::vector<Vertex>> submeshVertices(this->_submeshes.size());
        while (s < shapes.size())
        {
            indexOffset = 0;
            f = 0;
            while (f < shapes.at(s).mesh.num_face_vertices.size())
            {
                materialID = shapes.at(s).mesh.material_ids.at(f) + 1; // + 1 because the material ID at 0 is the default one
                fv = shapes.at(s).mesh.num_face_vertices.at(f);
                v = 0;
                while (v < fv)
                {
                    i = shapes.at(s).mesh.indices.at(indexOffset + v);
                    vertex = Vertex();
                    vertex.pos = glm::vec3(attrib.vertices.at(3 * i.vertex_index + 0), attrib.vertices.at(3 * i.vertex_index + 1), attrib.vertices.at(3 * i.vertex_index + 2));
                    if (!attrib.normals.empty() && attrib.normals.size() >= 3 * i.normal_index + 2)
                        vertex.normal = glm::vec3(attrib.normals.at(3 * i.normal_index + 0), attrib.normals.at(3 * i.normal_index + 1), attrib.normals.at(3 * i.normal_index + 2));
                    if (!attrib.texcoords.empty())
                        vertex.uv = glm::vec2(attrib.texcoords.at(2 * i.texcoord_index + 0), 1.0f - attrib.texcoords.at(2 * i.texcoord_index + 1));
                    if (perMaterialUniqueVertices.at(materialID).count(vertex) == 0)
                    {
                        perMaterialUniqueVertices.at(materialID)[vertex] = static_cast<int>(submeshVertices.at(materialID).size());
                        submeshVertices.at(materialID).push_back(vertex);
                    }
                    submeshIndices.at(materialID).push_back(perMaterialUniqueVertices.at(materialID).at(vertex));
                    ++v;
                }
                indexOffset += fv;
                ++f;
            }
            ++s;
        }
        s = 0;
        while (s < this->_submeshes.size())
        {
            this->_submeshes.at(s).setVertices(submeshVertices.at(s));
            this->_submeshes.at(s).setIndices(submeshIndices.at(s));
            ++s;
        }
	}

    std::vector<IBuildable *> Mesh::getBuildables()
    {
        std::vector<IBuildable *> buildables;
        std::vector<Submesh>::iterator iter = this->_submeshes.begin();
        std::vector<Submesh>::iterator iterEnd = this->_submeshes.end();
        while (iter != iterEnd)
        {
            if (iter->getVerticesCount() != 0 && iter->getIndicesCount() != 0)
                buildables.push_back(&(*iter));
            ++iter;
        }
        return (buildables);
    }
}

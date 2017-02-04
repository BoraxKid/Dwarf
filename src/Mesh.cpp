#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace Dwarf
{
	Mesh::Mesh(const vk::Device &device, Dwarf::MaterialManager &materialManager, const std::string &meshFilename, const vk::DescriptorBufferInfo &lightBufferInfo)
		: _device(device), _lightBufferInfo(lightBufferInfo)
	{
		this->loadFromFile(materialManager, meshFilename);
	}

	Mesh::~Mesh()
	{
        for (const auto &submesh : this->_submeshes)
        {
            if (submesh.getVerticesCount() != 0 && submesh.getIndicesCount() != 0)
                submesh.cleanup(this->_device);
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
		Material *tmpMaterial = nullptr;
        this->_submeshes.push_back(Submesh(materialManager.getMaterial("default"), this->_transformationMatrix, this->_lightBufferInfo));
		for (const auto &material : materials)
		{
            tmpMaterial = materialManager.createMaterial(material.name, !material.diffuse_texname.empty());
            LOG(INFO) << "MATERIAL -> " << material.name;
            tmpMaterial->setAmbient(Color(material.ambient[0], material.ambient[1], material.ambient[2]));
            LOG(INFO) << "MESH -> Ambient (" << material.ambient[0] << ", " << material.ambient[1] << ", " << material.ambient[2] << ")";
            tmpMaterial->setDiffuse(Color(material.diffuse[0], material.diffuse[1], material.diffuse[2]));
            LOG(INFO) << "MESH -> Diffuse (" << material.diffuse[0] << ", " << material.diffuse[1] << ", " << material.diffuse[2] << ")";
            tmpMaterial->setSpecular(Color(material.specular[0], material.specular[1], material.specular[2]));
            LOG(INFO) << "MESH -> Specular (" << material.specular[0] << ", " << material.specular[1] << ", " << material.specular[2] << ")";
            tmpMaterial->setTransmittance(Color(material.transmittance[0], material.transmittance[1], material.transmittance[2]));
            LOG(INFO) << "MESH -> Transmittance (" << material.transmittance[0] << ", " << material.transmittance[1] << ", " << material.transmittance[2] << ")";
            tmpMaterial->setEmission(Color(material.emission[0], material.emission[1], material.emission[2]));
            LOG(INFO) << "MESH -> Emission (" << material.emission[0] << ", " << material.emission[1] << ", " << material.emission[2] << ")";
            tmpMaterial->setShininess(material.shininess);
            tmpMaterial->setIor(material.ior);
            tmpMaterial->setDissolve(material.dissolve);
            tmpMaterial->setIllum(material.illum);
            tmpMaterial->setRoughness(material.roughness);
            tmpMaterial->setMetallic(material.metallic);
            tmpMaterial->setSheen(material.sheen);
            tmpMaterial->setClearcoatThickness(material.clearcoat_thickness);
            tmpMaterial->setClearcoatRoughness(material.clearcoat_roughness);
            tmpMaterial->setAnisotropy(material.anisotropy);
            tmpMaterial->setAnisotropyRotation(material.anisotropy_rotation);
			/*if (!material.ambient_texname.empty())
				tmpMaterial->createAmbientTexture(material.ambient_texname);*/
			if (!material.diffuse_texname.empty())
                tmpMaterial->createDiffuseTexture(material.diffuse_texname);
			/*if (!material.specular_texname.empty())
				tmpMaterial->createSpecularTexture(material.specular_texname);
			if (!material.specular_highlight_texname.empty())
				tmpMaterial->createSpecularHighlightTexture(material.specular_highlight_texname);
			if (!material.bump_texname.empty())
				tmpMaterial->createBumpTexture(material.bump_texname);
			if (!material.displacement_texname.empty())
				tmpMaterial->createDisplacementTexture(material.displacement_texname);
			if (!material.alpha_texname.empty())
				tmpMaterial->createAlphaTexture(material.alpha_texname);
			if (!material.roughness_texname.empty())
				tmpMaterial->createRoughnessTexture(material.roughness_texname);
			if (!material.metallic_texname.empty())
				tmpMaterial->createMetallicTexture(material.metallic_texname);
			if (!material.sheen_texname.empty())
				tmpMaterial->createSheenTexture(material.sheen_texname);
			if (!material.emissive_texname.empty())
				tmpMaterial->createEmissiveTexture(material.emissive_texname);
			if (!material.normal_texname.empty())
				tmpMaterial->createNormalTexture(material.normal_texname);*/
            this->_submeshes.push_back(Submesh(tmpMaterial, this->_transformationMatrix, this->_lightBufferInfo));
            tmpMaterial = nullptr;
		}

		Vertex vertex;
        size_t s = 0;
        size_t indexOffset;
        size_t f;
        size_t fv;
        size_t v;
        int materialID;
        tinyobj::index_t i;
        std::vector<std::unordered_map<Vertex, size_t>> perMaterialUniqueVertices(this->_submeshes.size());
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
                        perMaterialUniqueVertices.at(materialID)[vertex] = submeshVertices.at(materialID).size();
                        submeshVertices.at(materialID).push_back(vertex);
                    }
                    submeshIndices.at(materialID).push_back(static_cast<uint32_t>(perMaterialUniqueVertices.at(materialID).at(vertex)));
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
            LOG(INFO) << "TinyOBJLoader: vertices number(" << submeshVertices.at(s).size() << ") with indices number (" << submeshIndices.at(s).size() << ")";
            this->_submeshes.at(s).setVertices(submeshVertices.at(s));
            this->_submeshes.at(s).setIndices(submeshIndices.at(s));
            ++s;
        }
	}

    std::vector<IBuildable *> Mesh::getBuildables()
    {
        std::vector<IBuildable *> buildables;
        for (auto &submesh : this->_submeshes)
        {
            if (submesh.getVerticesCount() != 0 && submesh.getIndicesCount() != 0)
                buildables.push_back(&submesh);
        }
        return (buildables);
    }

    std::vector<Submesh> &Mesh::getSubmeshes()
    {
        return (this->_submeshes);
    }

    TmpMesh::TmpMesh()
    {
    }

    TmpMesh::~TmpMesh()
    {
    }
}

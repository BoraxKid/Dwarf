#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace Dwarf
{
	Mesh::Mesh(const vk::Device &device, Dwarf::MaterialManager &materialManager, const std::string &meshFilename)
		: _device(device), _position(0, 0, 0), _rotation(0, 0, 0), _scale(1, 1, 1)
	{
		this->loadFromFile(materialManager, meshFilename);
        this->updateTransformationMatrix();
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
        this->_submeshes.push_back(Submesh(materialManager.getMaterial("default"), this->_transformationMatrix));
		for (const auto &material : materials)
		{
            tmpMaterial = materialManager.createMaterial(material.name, !material.diffuse_texname.empty());
            tmpMaterial->setAmbient(Color(material.ambient[0], material.ambient[1], material.ambient[2]));
            tmpMaterial->setDiffuse(Color(material.diffuse[0], material.diffuse[1], material.diffuse[2]));
            tmpMaterial->setSpecular(Color(material.specular[0], material.specular[1], material.specular[2]));
            tmpMaterial->setTransmittance(Color(material.transmittance[0], material.transmittance[1], material.transmittance[2]));
            tmpMaterial->setEmission(Color(material.emission[0], material.emission[1], material.emission[2]));
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
            this->_submeshes.push_back(Submesh(tmpMaterial, this->_transformationMatrix));
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

    void Mesh::updateTransformationMatrix()
    {
        this->_preciseTransformationMatrix = this->_scaleMatrix * this->_rotationMatrix * this->_positionMatrix;
        this->_transformationMatrix = this->_preciseTransformationMatrix;
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

    void Mesh::move(double x, double y, double z)
    {
        this->move(glm::dvec3(x, y, z));
    }

    void Mesh::move(glm::dvec3 movement)
    {
        this->setPosition(this->_position + movement);
    }

    void Mesh::setPosition(double x, double y, double z)
    {
        this->setPosition(glm::dvec3(x, y, z));
    }

    void Mesh::setPosition(glm::dvec3 position)
    {
        this->_position = position;
        this->_positionMatrix = glm::translate(glm::dmat4(), this->_position);
        this->updateTransformationMatrix();
    }

    void Mesh::scale(double x, double y, double z)
    {
        this->scale(glm::dvec3(x, y, z));
    }

    void Mesh::scale(glm::dvec3 scale)
    {
        this->setScale(this->_scale + scale);
    }

    void Mesh::setScale(double x, double y, double z)
    {
        this->setScale(glm::dvec3(x, y, z));
    }

    void Mesh::setScale(glm::dvec3 scale)
    {
        this->_scale = scale;
        this->_scaleMatrix = glm::scale(glm::dmat4(), this->_scale);
        this->updateTransformationMatrix();
    }

    void Mesh::rotate(double x, double y, double z)
    {
        this->rotate(glm::dvec3(x, y, z));
    }

    void Mesh::rotate(glm::dvec3 rotation)
    {
        this->setRotation(this->_rotation + rotation);
    }

    void Mesh::setRotation(double x, double y, double z)
    {
        this->setRotation(glm::dvec3(x, y, z));
    }

    void Mesh::setRotation(glm::dvec3 rotation)
    {
        this->_rotation = rotation;
        this->_rotationMatrix = glm::rotate(glm::dmat4(), glm::radians(this->_rotation.x), glm::dvec3(1.0, 0.0, 0.0));
        this->_rotationMatrix = glm::rotate(this->_rotationMatrix, glm::radians(this->_rotation.y), glm::dvec3(0.0, 1.0, 0.0));
        this->_rotationMatrix = glm::rotate(this->_rotationMatrix, glm::radians(this->_rotation.z), glm::dvec3(0.0, 0.0, 1.0));
        this->updateTransformationMatrix();
    }
}

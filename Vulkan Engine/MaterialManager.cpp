#include "MaterialManager.h"

namespace Dwarf
{
	MaterialManager::MaterialManager()
	{
	}

	MaterialManager::~MaterialManager()
	{
	}

	bool MaterialManager::exist(const std::string &materialName) const
	{
		return (this->_materials.find(materialName) != this->_materials.end());
	}

	void MaterialManager::addMaterial(Material *material)
	{
		if (!material || this->exist(material->getName()))
			return; // TODO: add error handling
		this->_materials[material->getName()] = material;
	}

	bool MaterialManager::isSame(const std::string &leftMaterialName, Material *rightMaterial) const
	{
		if (!rightMaterial || this->_materials.find(leftMaterialName) == this->_materials.end())
			return (false);
		return (*this->_materials.at(leftMaterialName) == *rightMaterial);
	}

}

#ifndef MATERIALMANAGER_H_
#define MATERIALMANAGER_H_
#pragma once

#include <map>

#include "Material.h"

namespace Dwarf
{
	class MaterialManager
	{
	public:
		MaterialManager();
		virtual ~MaterialManager();
		bool exist(const std::string &materialName) const;
		void addMaterial(Material *material);

	private:
		bool isSame(const std::string &leftMaterialName, Material *rightMaterial) const;

		std::map<const std::string, Material *> _materials;
	};
}

#endif // MATERIALMANAGER_H_

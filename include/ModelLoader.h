#ifndef DWARF_MODELLOADER_H_
#define DWARF_MODELLOADER_H_
#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>
#include <string>
#include <unordered_map>

#include "Tools.h"
#include "MeshData.h"
#include "ModelData.h"
#include "MaterialManager.h"

namespace Dwarf
{
    class ModelLoader
    {
    public:
        ModelLoader();
        virtual ~ModelLoader();
        bool loadModel(const std::string &fileName);

    private:
        void loadMesh(const aiScene *scene, const std::string &meshName);
        Material::ID loadMaterial(const aiMaterial *material);

        Assimp::Importer _importer;
        std::map<const std::string, ModelData> _modelDatas;
    };
}

#endif // DWARF_MODELLOADER_H_

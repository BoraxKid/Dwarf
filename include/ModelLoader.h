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
#include "TextureManager.h"

namespace Dwarf
{
    class ModelLoader
    {
    public:
        ModelLoader(std::shared_ptr<MaterialManager> materialManager, std::vector<ModelData> &modelDatas);
        virtual ~ModelLoader();

        size_t loadModel(const std::string &fileName);

    private:
        void loadMesh(const aiScene *scene, const std::string &meshName);

        std::shared_ptr<MaterialManager> _materialManager;
        Assimp::Importer _importer;
        std::vector<ModelData> &_modelDatas;
    };
}

#endif // DWARF_MODELLOADER_H_

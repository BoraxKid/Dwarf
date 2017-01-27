#ifndef DWARF_MODELLOADER_H_
#define DWARF_MODELLOADER_H_
#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>

#include "Tools.h"
#include "Mesh.h"

namespace Dwarf
{
    class ModelLoader
    {
    public:
        ModelLoader();
        virtual ~ModelLoader();
        bool loadModel(const std::string &fileName);
    private:
        struct RawMeshData
        {
            uint32_t indicesNumber;
            uint32_t materialIndex;
            uint32_t vertexBase;
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;
        };
        Assimp::Importer _importer;
        std::vector<std::string> _meshes;
    };
}

#endif // DWARF_MODELLOADER_H_

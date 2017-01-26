#ifndef DWARF_MODELLOADER_H_
#define DWARF_MODELLOADER_H_
#pragma once

#include <assimp/Importer.hpp>

namespace Dwarf
{
    class ModelLoader
    {
    public:
        ModelLoader();
        virtual ~ModelLoader();

    private:
        Assimp::Importer _importer;
    };
}

#endif // DWARF_MODELLOADER_H_

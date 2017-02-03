#ifndef DWARF_MODELDATA_H_
#define DWARF_MODELDATA_H_
#pragma once

#include "MeshData.h"

namespace Dwarf
{
    struct ModelData
    {
        std::vector<MeshData> meshes;
    };
}

#endif // DWARF_MODELDATA_H_

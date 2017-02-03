#ifndef DWARF_MODEL_H_
#define DWARF_MODEL_H_
#pragma once

#include "Transformable.h"

namespace Dwarf
{
    class Model : public Transformable
    {
    public:
        Model(size_t modelDataID);
        virtual ~Model();

    private:
        size_t _modelDataID;
    };
}

#endif // DWARF_MODEL_H_

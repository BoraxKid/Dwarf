#ifndef DWARF_SCENEMANAGER_H_
#define DWARF_SCENEMANAGER_H_
#pragma once

#include "ModelManager.h"

namespace Dwarf
{
    class SceneManager
    {
    public:
        SceneManager();
        virtual ~SceneManager();

    private:
        ModelManager _modelManager;
    };
}

#endif // DWARF_SCENEMANAGER_H_

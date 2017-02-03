#ifndef DWARF_SCENEMANAGER_H_
#define DWARF_SCENEMANAGER_H_
#pragma once

#include "ModelLoader.h"

namespace Dwarf
{
    class SceneManager
    {
    public:
        SceneManager();
        virtual ~SceneManager();

    private:
        ModelLoader _modelLoader;
    };
}

#endif // DWARF_SCENEMANAGER_H_

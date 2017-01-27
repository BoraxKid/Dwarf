#include "ModelLoader.h"

namespace Dwarf
{
    ModelLoader::ModelLoader()
    {
    }

    ModelLoader::~ModelLoader()
    {
    }

    bool ModelLoader::loadModel(const std::string &fileName)
    {
        const aiScene *scene = this->_importer.ReadFile(fileName, aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);
        if (scene)
        {
            const aiMesh *mesh = scene->mMeshes[0];
            LOG(INFO) << "File \"" << fileName << "\" has " << scene->mNumMeshes << " meshes";
            return (true);
        }
        else
            LOG(ERROR) << "Model \"" << fileName << "\": " << this->_importer.GetErrorString();
        return (false);
    }
}

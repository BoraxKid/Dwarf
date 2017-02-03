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
            this->loadMesh(scene, fileName);
            LOG(INFO) << "File \"" << fileName << "\" has " << scene->mNumMeshes << " meshes";
            return (true);
        }
        else
            LOG(ERROR) << "Model \"" << fileName << "\": " << this->_importer.GetErrorString();
        return (false);
    }

    void ModelLoader::loadMesh(const aiScene *scene, const std::string &meshName)
    {
        uint32_t i = 0;
        uint32_t j;
        uint32_t verticesNum = 0;
        std::vector<MeshData> meshDatas(scene->mNumMeshes);
        const aiMesh *mesh;
        std::unordered_map<Vertex, size_t> uniqueVertices;
        std::vector<Material::ID> materialIDs(scene->mNumMaterials);

        while (i < scene->mNumMaterials)
        {
            materialIDs[i] = this->loadMaterial(scene->mMaterials[i]);
            ++i;
        }
        i = 0;
        this->_modelDatas[meshName] = ModelData();
        while (i < scene->mNumMeshes)
        {
            mesh = scene->mMeshes[i];
            meshDatas.at(i).materialID = materialIDs.at(mesh->mMaterialIndex);
            verticesNum += mesh->mNumVertices;
            j = 0;
            aiVector3D zero3D(0.0f, 0.0f, 0.0f);
            aiVector3D *pos;
            aiVector3D *normal;
            aiVector3D *textureCoord;
            Vertex vertex;
            uniqueVertices.clear();
            while (j < mesh->mNumVertices)
            {
                pos = &(mesh->mVertices[j]);
                normal = &(mesh->mNormals[j]);
                if (mesh->HasTextureCoords(0))
                {
                    textureCoord = &(mesh->mTextureCoords[0][j]);
                    vertex = Vertex(glm::vec3(pos->x, pos->y, pos->z), glm::vec3(normal->x, normal->y, normal->z), glm::vec2(textureCoord->x, textureCoord->y));
                }
                else
                    vertex = Vertex(glm::vec3(pos->x, pos->y, pos->z), glm::vec3(normal->x, normal->y, normal->z));
                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = meshDatas.at(i).vertices.size();
                    meshDatas.at(i).vertices.push_back(vertex);
                }
                ++j;
            }
            j = 0;
            while (j < mesh->mNumFaces)
            {
                const aiFace &face = mesh->mFaces[j];
                if (face.mNumIndices != 3)
                {
                    ++j;
                    continue;
                }
                meshDatas.at(i).indices.push_back(face.mIndices[0]);
                meshDatas.at(i).indices.push_back(face.mIndices[1]);
                meshDatas.at(i).indices.push_back(face.mIndices[2]);
                ++j;
            }
            LOG(INFO) << "assimp: vertices number(" << meshDatas.at(i).vertices.size() << ") with indices number (" << meshDatas.at(i).indices.size() << ")";
            this->_modelDatas.at(meshName).meshes.clear();
            this->_modelDatas.at(meshName).meshes.insert(this->_modelDatas.at(meshName).meshes.begin(), meshDatas.begin(), meshDatas.end());
            ++i;
        }
    }

    Material::ID ModelLoader::loadMaterial(const aiMaterial *material)
    {
        aiString name;
        material->Get(AI_MATKEY_NAME, name);
        LOG(INFO) << "    [name] = \"" << name.C_Str() << "\"";

        return Material::ID();
    }
}

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
        std::vector<RawMeshData> meshData(scene->mNumMeshes);
        const aiMesh *mesh;
        std::unordered_map<Vertex, size_t> uniqueVertices;
        const aiMaterial *material;
        const aiMaterialProperty *property;

        while (i < scene->mNumMaterials)
        {
            material = scene->mMaterials[i];
            LOG(INFO) << "Material #" << i;
            aiString name;
            material->Get(AI_MATKEY_NAME, name);
            LOG(INFO) << "    [name] = \"" << name.C_Str() << "\"";
            ++i;
        }
        i = 0;
        while (i < scene->mNumMeshes)
        {
            mesh = scene->mMeshes[i];
            meshData.at(i).vertexBase = verticesNum;
            meshData.at(i).materialIndex = mesh->mMaterialIndex;
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
                    uniqueVertices[vertex] = meshData.at(i).vertices.size();
                    meshData.at(i).vertices.push_back(vertex);
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
                meshData.at(i).indices.push_back(face.mIndices[0]);
                meshData.at(i).indices.push_back(face.mIndices[1]);
                meshData.at(i).indices.push_back(face.mIndices[2]);
                ++j;
            }
            LOG(INFO) << "assimp: vertices number(" << meshData.at(i).vertices.size() << ") with indices number (" << meshData.at(i).indices.size() << ")";
            ++i;
        }
    }
}

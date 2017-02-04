#include "ModelLoader.h"

namespace Dwarf
{
    ModelLoader::ModelLoader(MaterialManager &materialManager)
        : _materialManager(materialManager)
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
        LOG(INFO) << "[name] = \"" << name.C_Str() << "\"";
        Material *newMaterial = this->_materialManager.createMaterial(name.C_Str(), true);
        aiColor3D color;
        material->Get(AI_MATKEY_COLOR_AMBIENT, color); // Ka
        newMaterial->setAmbient(Color(color.r, color.g, color.b));
        LOG(INFO) << "Ka = " << color.r << " " << color.g << " " << color.b;
        material->Get(AI_MATKEY_COLOR_DIFFUSE, color); // Kd
        newMaterial->setDiffuse(Color(color.r, color.g, color.b));
        LOG(INFO) << "Kd = " << color.r << " " << color.g << " " << color.b;
        material->Get(AI_MATKEY_COLOR_SPECULAR, color); // Ks
        newMaterial->setSpecular(Color(color.r, color.g, color.b));
        LOG(INFO) << "Ks = " << color.r << " " << color.g << " " << color.b;
        material->Get(AI_MATKEY_COLOR_EMISSIVE, color); // Ke
        newMaterial->setEmission(Color(color.r, color.g, color.b));
        LOG(INFO) << "Ke = " << color.r << " " << color.g << " " << color.b;
        material->Get(AI_MATKEY_COLOR_TRANSPARENT, color); // Tf
        aiColor3D color2;
        material->Get(AI_MATKEY_COLOR_REFLECTIVE, color2);
        if (color != color2)
            LOG(WARNING) << "In material " << name.C_Str() << ": Transparent color and reflective color differ. Transparent(" << color.r << "," << color.g << "," << color.b << ") & Reflective(" << color2.r << "," << color2.g << "," << color2.b << ")";
        newMaterial->setTransmittance(Color(color.r, color.g, color.b));
        LOG(INFO) << "Tf = " << color.r << " " << color.g << " " << color.b;
        int illum;
        material->Get(AI_MATKEY_SHADING_MODEL, illum); // illum ?
        LOG(INFO) << "illum = " << illum;
        float value;
        material->Get(AI_MATKEY_SHININESS, value); // Ns
        LOG(INFO) << "Ns = " << value;
        material->Get(AI_MATKEY_REFRACTI, value); // Ni
        LOG(INFO) << "Ni = " << value;
        material->Get(AI_MATKEY_OPACITY, value); // d ?
        LOG(INFO) << "d = " << value;
        return Material::ID();
    }
}

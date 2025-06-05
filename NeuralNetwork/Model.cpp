#include "Model.h"
//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>
//#include <stdexcept>
//#include <memory>
//#include "Material.h"
//
//void Model::AddMesh(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) {
//    meshes.push_back(mesh);
//    materials.push_back(material);
//}
//
//void Model::LoadFromFile(const std::string& path) {
//    Assimp::Importer importer;
//    const aiScene* scene = importer.ReadFile(path,
//        aiProcess_Triangulate |
//        aiProcess_GenSmoothNormals |
//        aiProcess_CalcTangentSpace |
//        aiProcess_JoinIdenticalVertices);
//
//    if (!scene || !scene->HasMeshes()) {
//        throw std::runtime_error("Failed to load model: " + path + " " + importer.GetErrorString());
//    }
//
//    meshes.clear();
//    materials.clear();
//
//    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
//        const aiMesh* mesh = scene->mMeshes[m];
//        std::vector<Vertex> vertices;
//        std::vector<uint32_t> indices;
//
//        vertices.reserve(mesh->mNumVertices);
//        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
//            Vertex v;
//            v.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
//            v.normal = mesh->HasNormals() ? DirectX::XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z) : DirectX::XMFLOAT3(0,0,0);
//            if (mesh->HasTextureCoords(0))
//                v.texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
//            else
//                v.texcoord = { 0, 0 };
//            if (mesh->HasTangentsAndBitangents())
//                v.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
//            else
//                v.tangent = { 0, 0, 0 };
//            vertices.push_back(v);
//        }
//
//        indices.reserve(mesh->mNumFaces * 3);
//        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
//            const aiFace& face = mesh->mFaces[i];
//            for (unsigned int j = 0; j < face.mNumIndices; ++j)
//                indices.push_back(face.mIndices[j]);
//        }
//
//        auto meshPtr = std::make_shared<Mesh>(vertices, indices, std::string(mesh->mName.C_Str()));
//
//        // Create a default material for this mesh (replace shader paths as needed)
//        auto materialPtr = std::make_shared<Material>(L"Shaders/PbrVS.cso", L"Shaders/PbrPS.cso");
//
//        AddMesh(meshPtr, materialPtr);
//    }
//}
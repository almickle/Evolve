//#pragma once
//#include <vector>
//#include <memory>
//#include <string>
//#include "Mesh.h"
//#include "Material.h"
//
//class Model {
//public:
//    // Constructor that loads from file
//    Model(const std::string& path) { LoadFromFile(path); }
//
//    // Add a mesh with its associated material
//    void AddMesh(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
//    void LoadFromFile(const std::string& path);
//    // Getters
//    size_t GetMeshCount() const { return meshes.size(); }
//    std::shared_ptr<Mesh> GetMesh(size_t index) const { return meshes.at(index); }
//    std::shared_ptr<Material> GetMaterial(size_t index) const { return materials.at(index); }
//    const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const { return meshes; }
//    const std::vector<std::shared_ptr<Material>>& GetMaterials() const { return materials; }
//
//private:
//    std::vector<std::shared_ptr<Mesh>> meshes;
//    std::vector<std::shared_ptr<Material>> materials;
//};
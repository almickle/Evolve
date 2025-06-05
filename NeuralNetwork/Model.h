#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Material.h"
#include "Mesh.h"
#include "Types.h"

class Model {
public:
	Model( const std::string& name ) : name( name ) {}
	~Model() = default;
public:
	void LoadFromFile( const std::string& path );
	void AddMesh( std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material );
public:
	size_t GetMeshCount() const { return meshes.size(); }
	std::shared_ptr<Mesh> GetMesh( uint index ) const { return meshes.at( index ); }
	std::shared_ptr<Material> GetMaterial( size_t index ) const { return materials.at( index ); }
	const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const { return meshes; }
	const std::vector<std::shared_ptr<Material>>& GetMaterials() const { return materials; }
private:
	std::string name;
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<Material>> materials;
};
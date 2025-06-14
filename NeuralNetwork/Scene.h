#pragma once
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "Actor.h"
#include "Asset.h"
#include "Camera.h"
#include "Light.h"
#include "Types.h"

class Scene :
	public Asset {
public:
	Scene( const std::string& name = "Mesh" )
		: Asset( AssetType::Mesh, name )
	{
	}
	~Scene() = default;
public:
	void AddLight( std::unique_ptr<Light> light ) { lights.push_back( std::move( light ) ); };
	void AddCamera( std::unique_ptr<Camera> camera ) { cameras.push_back( std::move( camera ) ); };
public:
	const std::vector<std::unique_ptr<Light>>& GetLights() const { return lights; };
	const std::vector<std::unique_ptr<Camera>>& GetCameras() const { return cameras; };
	const std::vector<std::unique_ptr<Actor>>& GetDynamicActors() const { return dynamicActors; };
	const std::vector<std::unique_ptr<Actor>>& GetStaticActors() const { return staticActors; };
public:
	Camera* GetActiveCamera() const { return activeCamera; };
	void SetActiveCamera( const uint& index ) { activeCamera = cameras[index].get(); }
private:
	Camera* activeCamera = nullptr;
	std::vector<std::unique_ptr<Light>> lights{};
	std::vector<std::unique_ptr<Camera>> cameras{};
	std::vector<std::unique_ptr<Actor>> dynamicActors{ 1024 };
	std::vector<std::unique_ptr<Actor>> staticActors{ 1024 };
};
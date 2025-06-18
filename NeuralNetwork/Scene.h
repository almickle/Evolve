#pragma once
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "Actor.h"
#include "Asset.h"
#include "Camera.h"
#include "InstanceManager.h"
#include "JsonSerializer.h"
#include "Light.h"
#include "SystemManager.h"
#include "Types.h"

class Scene :
	public Asset {
public:
	Scene( const std::string& name = "Scene" )
		: Asset( AssetType::Scene, name ),
		dynamicInstanceManager( std::make_unique<InstanceManager>() ),
		staticInstanceManager( std::make_unique<InstanceManager>() )
	{
	}
	~Scene() = default;
public:
	void Load( SystemManager* systemManager, JsonSerializer& serializer ) override;
	std::string Serialize( JsonSerializer& serializer ) const override;
	void Deserialize( JsonSerializer& serializer ) override;
public:
	void AddLight( std::unique_ptr<Light> light ) { lights.push_back( std::move( light ) ); }
	void AddCamera( std::unique_ptr<Camera> camera ) { cameras.push_back( std::move( camera ) ); }
public:
	const std::vector<std::unique_ptr<Light>>& GetLights() const { return lights; }
	uint GetLightCount() const { return lights.size(); }
	const std::vector<std::unique_ptr<Camera>>& GetCameras() const { return cameras; }
	const std::vector<std::unique_ptr<Actor>>& GetStaticActors() const { return staticActors; }
	const std::vector<std::unique_ptr<Actor>>& GetDynamicActors() const { return dynamicActors; }
	InstanceManager* GetStaticInstanceManager() const { return staticInstanceManager.get(); }
	InstanceManager* GetDynamicInstanceManager() const { return dynamicInstanceManager.get(); }
	ResourceID GetSceneBuffer() const { return sceneConstantBuffer; }
public:
	Camera* GetActiveCamera() const { return activeCamera; }
	void SetActiveCamera( const uint& index ) { activeCamera = cameras[index].get(); }
private:
	Camera* activeCamera = nullptr;
	std::vector<std::unique_ptr<Light>> lights{};
	std::vector<std::unique_ptr<Camera>> cameras{};
	std::vector<std::unique_ptr<Actor>> staticActors{ 1024 };
	std::vector<std::unique_ptr<Actor>> dynamicActors{ 1024 };
	std::unique_ptr<InstanceManager> staticInstanceManager;
	std::unique_ptr<InstanceManager> dynamicInstanceManager;
private:
	ResourceID sceneConstantBuffer;
	ResourceID staticStructuredBuffer;
	ResourceID dynamicStructuredBuffer;
};
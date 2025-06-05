//#pragma once
//#include <vector>
//#include <memory>
//#include <string>
//#include "InstanceManager.h" // Include for InstanceManager
//#include "StructuredBuffer.h" // Include for StructuredBuffer
//
//// Forward declarations for your object types
//class Light;
//class Camera;
//class Actor;
//
//class Scene {
//public:
//    Scene();
//    void AddLight(std::shared_ptr<Light> light);
//    void AddCamera(std::shared_ptr<Camera> camera);
//    void SetLandscape(std::shared_ptr<Actor> landscape) { this->landscape = landscape; }
//    std::shared_ptr<Actor> AddActorByModel(std::shared_ptr<Model> model, const InstanceData& data = { DirectX::XMMatrixIdentity() });
//public:
//    const std::vector<std::shared_ptr<Light>>& GetLights() const;
//    const std::vector<std::shared_ptr<Camera>>& GetCameras() const;
//    const std::vector<std::shared_ptr<Actor>>& GetActors() const;
//    const std::shared_ptr<Actor>& GetLandscape() const { return landscape; }
//    const std::shared_ptr<Camera>& GetActiveCamera() const { return activeCamera; };
//    void SetActiveCamera(std::shared_ptr<Camera> camera) { activeCamera = camera; }
//    const InstanceManager& GetInstanceManager() const { return instanceManager; }
//    StructuredBuffer& GetInstanceBuffer() { return instanceBuffer; }
//    const StructuredBuffer& GetInstanceBuffer() const { return instanceBuffer; }
//private:
//    std::shared_ptr<Camera> activeCamera = nullptr;
//    std::vector<std::shared_ptr<Light>> lights = {};
//    std::vector<std::shared_ptr<Camera>> cameras = {};
//    std::vector<std::shared_ptr<Actor>> actors = {};
//    std::shared_ptr<Actor> landscape;
//private:
//    InstanceManager instanceManager;
//    StructuredBuffer instanceBuffer;
//};
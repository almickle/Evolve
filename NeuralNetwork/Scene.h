#pragma once
#include <vector>
#include <memory>
#include <string>

// Forward declarations for your object types
class Light;
class Camera;
class Actor;

class Scene {
public:
    Scene();
    // Add methods to manage scene objects
    void AddLight(std::shared_ptr<Light> light);
    void AddCamera(std::shared_ptr<Camera> camera);
    void AddActor(std::shared_ptr<Actor> actor);

    const std::vector<std::shared_ptr<Light>>& GetLights() const;
    const std::vector<std::shared_ptr<Camera>>& GetCameras() const;
    const std::vector<std::shared_ptr<Actor>>& GetActors() const;
    const std::shared_ptr<Camera>& GetActiveCamera() const { return activeCamera; };
	void SetActiveCamera(std::shared_ptr<Camera> camera) { activeCamera = camera; }

    // Optionally, methods to remove or find objects by name/id

private:
    std::shared_ptr<Camera> activeCamera = nullptr;
    std::vector<std::shared_ptr<Light>> lights = {};
    std::vector<std::shared_ptr<Camera>> cameras = {};
    std::vector<std::shared_ptr<Actor>> actors = {};
};
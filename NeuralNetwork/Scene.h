#pragma once
#include <vector>
#include <memory>
#include <string>

// Forward declarations for your object types
class Mesh;
class Light;
class Camera;

class Scene {
public:
    Scene();
    // Add methods to manage scene objects
    void AddMesh(std::shared_ptr<Mesh> mesh);
    void AddLight(std::shared_ptr<Light> light);
    void AddCamera(std::shared_ptr<Camera> camera);

    const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const;
    const std::vector<std::shared_ptr<Light>>& GetLights() const;
    const std::vector<std::shared_ptr<Camera>>& GetCameras() const;

    // Optionally, methods to remove or find objects by name/id

private:
    std::vector<std::shared_ptr<Mesh>> meshes = {};
    std::vector<std::shared_ptr<Light>> lights = {};
    std::vector<std::shared_ptr<Camera>> cameras = {};
};
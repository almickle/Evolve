#include "Scene.h"
#include "Mesh.h"
#include "Light.h"
#include "Camera.h"

Scene::Scene() {
    // Constructor implementation can be empty or initialize members if needed
	AddCamera(std::make_shared<Camera>());
	AddLight(std::make_shared<Light>());
	AddMesh(std::make_shared<Mesh>("C:\\source\\assets\\primitives\\sphere.fbx"));
}

void Scene::AddMesh(std::shared_ptr<Mesh> mesh) {
    meshes.push_back(mesh);
}

void Scene::AddLight(std::shared_ptr<Light> light) {
    lights.push_back(light);
}

void Scene::AddCamera(std::shared_ptr<Camera> camera) {
    cameras.push_back(camera);
}

const std::vector<std::shared_ptr<Mesh>>& Scene::GetMeshes() const {
    return meshes;
}

const std::vector<std::shared_ptr<Light>>& Scene::GetLights() const {
    return lights;
}

const std::vector<std::shared_ptr<Camera>>& Scene::GetCameras() const {
    return cameras;
}
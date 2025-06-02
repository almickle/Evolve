#include "Scene.h"
#include "Mesh.h"
#include "Light.h"
#include "Camera.h"
#include "Actor.h"

Scene::Scene() {
    // Add a default camera
    AddCamera(std::make_shared<Camera>());
	SetActiveCamera(cameras[0]);
    GetActiveCamera()->SetPosition({0.0f, 2.0f, -10.0f});
    GetActiveCamera()->SetTarget({ 0.0f, 0.0f, 0.0f });
    GetActiveCamera()->SetUp({ 0.0f, 1.0f, 0.0f });

	// Add a default light
    AddLight(std::make_shared<Light>());
    lights[0]->SetDirection({-0.5f, -1.0f, 0.5f}); // Normalized in your code
    lights[0]->SetColor({ 1.0f, 1.0f, 1.0f });
    lights[0]->SetIntensity(1.0f);

	// Add a default actor with a primitive mesh
    auto mesh = std::make_shared<Mesh>("C:/source/assets/primitives/sphere.fbx");
    auto actor = std::make_shared<Actor>(mesh);
    AddActor(actor);
    actor->SetPosition({ 0.0f, 0.0f, 0.0f });
    actor->SetScale({ 1.0f, 1.0f, 1.0f });
    actor->SetRotation({ 0.0f, 0.0f, 0.0f, 1.0f }); // Identity quaternion
}

void Scene::AddLight(std::shared_ptr<Light> light) {
    lights.push_back(light);
}

void Scene::AddCamera(std::shared_ptr<Camera> camera) {
    cameras.push_back(camera);
}

void Scene::AddActor(std::shared_ptr<Actor> actor) {
    actors.push_back(std::move(actor));
}

const std::vector<std::shared_ptr<Light>>& Scene::GetLights() const {
    return lights;
}

const std::vector<std::shared_ptr<Camera>>& Scene::GetCameras() const {
    return cameras;
}

const std::vector<std::shared_ptr<Actor>>& Scene::GetActors() const {
    return actors;
}
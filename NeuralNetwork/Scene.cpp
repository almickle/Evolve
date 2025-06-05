//#include "Scene.h"
//#include "Mesh.h"
//#include "Light.h"
//#include "FPCamera.h"
//#include "Actor.h"
//#include "Landscape.h" // Add this include at the top
//#include "Model.h"
//
//Scene::Scene() {
//    // Add a default camera
//	auto camera = std::make_shared<FPCamera>();
//	camera->SetSpeed(20.0f);
//    camera->SetPosition({ 0.0f, -10.0f, 0.0f });
//	camera->SetForwardVec({ 1.0f, 1.0f, -0.5f });
//    camera->SetFarZ(5000.0f);
//    AddCamera(camera);
//	SetActiveCamera(camera);
//
//	// Add a default light
//    AddLight(std::make_shared<Light>());
//    lights[0]->SetPosition({10.0f, 10.0f, 10.0f}); 
//    lights[0]->SetColor({ 1.0f, 1.0f, 1.0f });
//    lights[0]->SetIntensity(1.0f);
//
//    //// Add a default actor with a primitive mesh
//    //auto model = std::make_shared<Model>("C:/source/assets/primitives/sphere.fbx");
//    //AddActorByModel(model);
//
//    // --- Add a landscape from a heightmap image ---
// //   auto landscapeMesh = std::make_shared<Landscape>("C:/source/assets/heightmaps/SFHeightMap.dds", 1.0f, 40.0f);
// //   auto landscapeActor = std::make_shared<Actor>(landscapeMesh);
// //   landscapeActor->SetPosition({ 0.0f, 0.0f, 0.0f });
// //   landscapeActor->SetScale({ 1.0f, 1.0f, 1.0f });
// //   landscapeActor->SetRotation({ 0.0f, 0.0f, 0.0f, 1.0f });
// //   SetLandscape(landscapeActor);
//}
//
//void Scene::AddLight(std::shared_ptr<Light> light) {
//    lights.push_back(light);
//}
//
//void Scene::AddCamera(std::shared_ptr<Camera> camera) {
//    cameras.push_back(camera);
//}
//
//std::shared_ptr<Actor> Scene::AddActorByModel(std::shared_ptr<Model> model, const InstanceData& data ) {
//    // Register model if not already registered
//    ModelID modelId = instanceManager.RegisterModel(model);
//    // Add instance and get its ID
//    InstanceID instanceId = instanceManager.AddInstance(modelId, data);
//    // Create actor with these IDs
//    auto actor = std::make_shared<Actor>(modelId, instanceId);
//    actors.push_back(actor);
//    return actor;
//}
//
//const std::vector<std::shared_ptr<Light>>& Scene::GetLights() const {
//    return lights;
//}
//
//const std::vector<std::shared_ptr<Camera>>& Scene::GetCameras() const {
//    return cameras;
//}
//
//const std::vector<std::shared_ptr<Actor>>& Scene::GetActors() const {
//    return actors;
//}
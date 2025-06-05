#include "SceneInitPass.h"
//#include "Model.h"
//#include "Texture.h"
//#include "InstanceManager.h"
//#include "Actor.h"
//#include <thread>
//
//void SceneInitPass::Execute(Renderer& renderer) {
//    // Start async loading only once
//    if (!running) {
//        running = true;
//        loadFuture = std::async(std::launch::async, [this, &renderer]() { AsyncLoad(renderer); });
//    }
//    // Optionally poll or block until ready
//    if (!ready.load(std::memory_order_acquire)) {
//        // Could display a loading screen or skip rendering
//        return;
//    }
//}
//
//void SceneInitPass::AsyncLoad(Renderer& renderer) {
//    auto model = std::make_shared<Model>("C:/source/assets/primitives/sphere.fbx");
//    scene->AddActorByModel(model);
//    InitInstanceBuffers(renderer);
//
//    renderer.GetUploadManager()->Flush();
//
//    ready.store(true, std::memory_order_release);
//}
//
//void SceneInitPass::InitInstanceBuffers(Renderer& renderer) {
//    std::vector<InstanceData> instanceDataCPU;
//    const auto& actors = scene->GetActors();
//    instanceDataCPU.resize(actors.size());
//    for (size_t i = 0; i < actors.size(); ++i) {
//        instanceDataCPU[i] = actors[i]->GetInstanceData();
//    }
//	scene->GetInstanceBuffer().CreateResource(renderer, static_cast<uint>(instanceDataCPU.size()), sizeof(InstanceData));
//    scene->GetInstanceBuffer().Upload(renderer, instanceDataCPU.data(), static_cast<uint>(instanceDataCPU.size()));
//}
//
//void SceneInitPass::Shutdown() {
//    // Wait for async load to finish if still running
//    if (loadFuture.valid()) loadFuture.wait();
//}
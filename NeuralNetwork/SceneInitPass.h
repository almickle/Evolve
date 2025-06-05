//#pragma once
//#include <future>
//#include <atomic>
//
//#include "GraphPass.h"
//#include "StructuredBuffer.h"
//#include "Scene.h"
//#include "Renderer.h"
//
//class SceneInitPass : public GraphPass {
//public:
//    SceneInitPass(Scene* scene, Renderer& renderer)
//        : GraphPass("SceneInitPass"), scene(scene), renderer(&renderer) {
//    }
//    void Execute(Renderer& renderer) override;
//    void AsyncLoad(Renderer& renderer);
//    void Shutdown() override;
//public:
//    bool IsFinished() const override { return ready.load(); }
//	bool IsRunning() const { return running; }
//public:
//    void InitInstanceBuffers(Renderer& renderer);
//private:
//    bool running = false;
//    Scene* scene;
//    Renderer* renderer;
//    std::future<void> loadFuture;
//    std::atomic<bool> ready{ false };
//};
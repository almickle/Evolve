#pragma once
#include "RenderPass.h"

class UploadPass : public GraphPass {
public:
    UploadPass() : GraphPass("UploadPass", RenderPassType::Asynchronous) {}

    void Execute(Renderer& renderer) override {
        // Let the UploadManager process all pending uploads
        if (renderer.GetUploadManager()) {
            //renderer.GetUploadManager()->Upload(renderer);
        }
    }

    void Shutdown() override {}
};
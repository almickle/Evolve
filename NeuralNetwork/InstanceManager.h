//#pragma once
//#include <unordered_map>
//#include <map>
//#include <vector>
//#include <memory>
//#include "Model.h"
//#include <DirectXMath.h>
//
//using ModelID = size_t;
//using InstanceID = size_t;
//
//struct InstanceData {
//    DirectX::XMMATRIX transform;
//    // Add more per-instance data as needed
//};
//
//class Scene;
//
//class InstanceManager {
//public:
//    ModelID RegisterModel(std::shared_ptr<Model> model);
//    InstanceID AddInstance(ModelID modelId, const InstanceData& data);
//    void RemoveInstance(ModelID modelId, InstanceID instanceId);
//
//    std::shared_ptr<Model> GetModel(ModelID modelId) const;
//    const InstanceData& GetInstanceData(ModelID modelId, InstanceID instanceId) const;
//    const std::vector<InstanceData> GetAllInstanceData(ModelID modelId) const;
//
//    void UpdateInstanceData(const Scene& scene);
//
//    // Returns a const reference to the internal models map
//    const std::unordered_map<ModelID, std::shared_ptr<Model>>& GetModels() const;
//
//private:
//    std::unordered_map<ModelID, std::shared_ptr<Model>> models;
//    std::unordered_map<ModelID, std::map<InstanceID, InstanceData>> instanceData;
//    std::unordered_map<ModelID, std::vector<InstanceID>> freeInstanceIds;
//    std::unordered_map<ModelID, InstanceID> nextInstanceId;
//    ModelID nextModelId = 0;
//};
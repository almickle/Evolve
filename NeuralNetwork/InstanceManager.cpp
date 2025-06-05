//#include "InstanceManager.h"
//#include "Scene.h"
//#include "Actor.h"
//#include <stdexcept>
//#include <limits>
//
//namespace {
//    constexpr InstanceID INVALID_INSTANCE_ID = std::numeric_limits<InstanceID>::max();
//}
//
//ModelID InstanceManager::RegisterModel(std::shared_ptr<Model> model) {
//    // Check if model already exists
//    for (const auto& [id, m] : models) {
//        if (m == model) return id;
//    }
//    ModelID id = nextModelId++;
//    models[id] = std::move(model);
//    instanceData[id] = {};
//    nextInstanceId[id] = 0;
//    freeInstanceIds[id] = {};
//    return id;
//}
//
//InstanceID InstanceManager::AddInstance(ModelID modelId, const InstanceData& data) {
//    auto& map = instanceData.at(modelId); // throws if not found
//    auto& freelist = freeInstanceIds[modelId];
//    InstanceID id;
//    if (!freelist.empty()) {
//        id = freelist.back();
//        freelist.pop_back();
//    } else {
//        id = nextInstanceId[modelId]++;
//    }
//    map[id] = data;
//    return id;
//}
//
//void InstanceManager::RemoveInstance(ModelID modelId, InstanceID instanceId) {
//    auto& map = instanceData.at(modelId);
//    auto it = map.find(instanceId);
//    if (it == map.end())
//        throw std::out_of_range("InstanceID not found.");
//    map.erase(it);
//    freeInstanceIds[modelId].push_back(instanceId);
//}
//
//std::shared_ptr<Model> InstanceManager::GetModel(ModelID modelId) const {
//    auto it = models.find(modelId);
//    if (it == models.end())
//        throw std::runtime_error("ModelID not found in InstanceManager.");
//    return it->second;
//}
//
//const InstanceData& InstanceManager::GetInstanceData(ModelID modelId, InstanceID instanceId) const {
//    const auto& map = instanceData.at(modelId);
//    auto it = map.find(instanceId);
//    if (it == map.end())
//        throw std::out_of_range("InstanceID not found.");
//    return it->second;
//}
//
//const std::vector<InstanceData> InstanceManager::GetAllInstanceData(ModelID modelId) const {
//    const auto& map = instanceData.at(modelId);
//    std::vector<InstanceData> result;
//    result.reserve(map.size());
//    for (const auto& [id, data] : map) {
//        result.push_back(data);
//    }
//    return result;
//}
//
//void InstanceManager::UpdateInstanceData(const Scene& scene) {
//    // For each actor in the scene, update the instance data in the manager
//    for (const auto& actor : scene.GetActors()) {
//        if (!actor) continue;
//        ModelID modelId = actor->GetModelID();
//        InstanceID instanceId = actor->GetInstanceID();
//        InstanceData data = actor->GetInstanceData();
//        // Only update if the instance exists
//        auto modelIt = instanceData.find(modelId);
//        if (modelIt != instanceData.end()) {
//            auto& map = modelIt->second;
//            auto instIt = map.find(instanceId);
//            if (instIt != map.end()) {
//                instIt->second = data;
//            }
//        }
//    }
//}
//
//const std::unordered_map<ModelID, std::shared_ptr<Model>>& InstanceManager::GetModels() const {
//    return models;
//}
//

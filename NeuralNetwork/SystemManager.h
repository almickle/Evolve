#pragma once
#include <memory>
#include <unordered_map>
#include <Windows.h>
#include "System.h"

class ThreadManager;
class JsonSerializer;
class FileIOManager;
class AssetManager;
class NodeLibrary;
class ImportManager;
class Renderer;
class UploadManager;
class GpuResourceManager;
class DescriptorHeapManager;
class ImGuiLayer;
class Window;
class InputSystem;

struct AppContext {
	const wchar_t* title;
	HINSTANCE hInstance;
	int nCmdShow;
};

enum class SystemID {
	ThreadManager,
	JsonSerializer,
	FileIOManager,
	AssetManager,
	NodeLibrary,
	ImportManager,
	Renderer,
	UploadManager,
	GpuResourceManager,
	DescriptorHeapManager,
	ImGuiLayer,
	Window
};

class SystemManager
{
public:
	SystemManager();
	~SystemManager();
public:
	void Init( const AppContext& appContext );
public:
	ImGuiLayer* GetUILayer() { return uiLayer.get(); }
	Window* GetWindow() { return window.get(); }
public:
	ThreadManager* GetThreadManager() { return threadManager.get(); }
	JsonSerializer* GetSerializer() { return serializer.get(); }
	FileIOManager* GetFileManager() { return fileManager.get(); }
	InputSystem* GetInputSystem() { return inputSystem.get(); }
public:
	AssetManager* GetAssetManager() { return assetManager.get(); }
	NodeLibrary* GetNodeLibrary() { return nodeLibrary.get(); }
	ImportManager* GetImportManager() { return importManager.get(); }
public:
	Renderer* GetRenderer() { return renderer.get(); }
	UploadManager* GetUploadManager() { return uploadManager.get(); }
	GpuResourceManager* GetResourceManager() { return resourceManager.get(); }
	DescriptorHeapManager* GetSrvHeapManager() { return srvHeapManager.get(); }
private:
	void RegisterSystem( SystemID id, System* systemPtr )
	{
		systemMap[id] = systemPtr;
	}
	void* GetSystemByID( SystemID id ) const
	{
		auto it = systemMap.find( id );
		return (it != systemMap.end()) ? it->second : nullptr;
	}
private:
	std::unique_ptr<ImGuiLayer> uiLayer;
	std::unique_ptr<Window> window;
	std::unique_ptr<InputSystem> inputSystem;
private:
	std::unique_ptr<ThreadManager> threadManager;
	std::unique_ptr<JsonSerializer> serializer;
	std::unique_ptr<FileIOManager> fileManager;
private:
	std::unique_ptr<AssetManager> assetManager;
	std::unique_ptr<ImportManager> importManager;
	std::unique_ptr<NodeLibrary> nodeLibrary;
private:
	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<GpuResourceManager> resourceManager;
	std::unique_ptr<UploadManager> uploadManager;
	std::unique_ptr<DescriptorHeapManager> srvHeapManager;
private:
	std::unordered_map<SystemID, System*> systemMap;
};


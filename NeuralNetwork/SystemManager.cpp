#include <d3d12.h>
#include <memory>
#include "AssetManager.h"
#include "DescriptorHeapManager.h"
#include "FileIOManager.h"
#include "GpuResourceManager.h"
#include "ImGuiLayer.h"
#include "ImportManager.h"
#include "JsonSerializer.h"
#include "NodeLibrary.h"
#include "Renderer.h"
#include "SystemManager.h"
#include "ThreadManager.h"
#include "UploadManager.h"
#include "Window.h"

SystemManager::SystemManager()
{

}
SystemManager::~SystemManager()
{

}

void SystemManager::Init( const AppContext& appContext )
{
	window = std::make_unique<Window>();
	window->Init( appContext.title, appContext.hInstance, appContext.nCmdShow );
	renderer = std::make_unique<Renderer>( *this );
	renderer->Init();
	threadManager = std::make_unique<ThreadManager>();
	threadManager->Init();
	uploadManager = std::make_unique<UploadManager>( *this );
	uploadManager->Init();
	srvHeapManager = std::make_unique<DescriptorHeapManager>( *this );
	srvHeapManager->Init( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1024, true );
	uiLayer = std::make_unique<ImGuiLayer>( *this );
	uiLayer->Init();
	fileManager = std::make_unique<FileIOManager>( *this );
	nodeLibrary = std::make_unique<NodeLibrary>();
	resourceManager = std::make_unique<GpuResourceManager>( *this );
	importManager = std::make_unique<ImportManager>( *this );
	assetManager = std::make_unique<AssetManager>( *this );
	assetManager->Init();
}
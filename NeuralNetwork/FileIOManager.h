#pragma once
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <string>
#include "System.h"
#include "SystemManager.h"

class ThreadManager;

struct FileTask {
	std::function<void( const std::string& )> taskFunc;
	std::function<void()> onComplete;
	std::string path;
};

class FileIOManager : public System {
public:
	FileIOManager( SystemManager& systemManager )
		: threadManager( systemManager.GetThreadManager() )
	{
	}
	~FileIOManager() = default;
public:
	void Enqueue( FileTask task );
	std::future<void> InsertFence();
private:
	std::mutex queueMutex;
	std::queue<FileTask> taskQueue;
private:
	ThreadManager* threadManager;
};
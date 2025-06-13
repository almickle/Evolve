#pragma once
#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>
#include "System.h"
#include "SystemManager.h"
#include "Types.h"

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
	std::queue<FileTask> taskQueue;
	std::atomic<uint> outstandingTasks{ 0 };
	std::vector<std::shared_ptr<std::promise<void>>> pendingFences;
	std::mutex fenceMutex;
	std::mutex queueMutex;
private:
	ThreadManager* threadManager;
};
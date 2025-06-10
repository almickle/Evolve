#pragma once
#include <functional>
#include <mutex>
#include <queue>
#include "ThreadManager.h"

struct FileTask {
	std::function<void()> taskFunc;
	std::function<void()> onComplete;
};

class FileIOManager {
public:
	FileIOManager( ThreadManager* threadManager );
	~FileIOManager() = default;
public:
	void Enqueue( FileTask task );
private:
	ThreadManager* threadManager = nullptr;
	std::mutex queueMutex;
	std::queue<FileTask> taskQueue;
};
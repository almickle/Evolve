#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include "Types.h"

class ThreadManager {
public:
	ThreadManager( uint threadCount = std::thread::hardware_concurrency() )
		: threadCount( threadCount ? threadCount : 1 )
	{
	}
	~ThreadManager();
public:
	void Init();
	void Launch( std::function<void()> func );
	void JoinAll();
public:
	uint GetThreadCount() const { return threadCount; }
private:
	void WorkerLoop();
private:
	uint threadCount;
	std::vector<std::thread> threads;
	std::queue<std::function<void()>> tasks;
	std::mutex tasksMutex;
	std::condition_variable tasksCv;
	std::atomic<bool> running{ true };
	std::atomic<uint> activeTasks{ 0 };
};
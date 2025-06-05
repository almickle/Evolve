#pragma once
#include <atomic>
#include <functional>
#include <thread>
#include <vector>

class ThreadManager {
public:
	ThreadManager();
	~ThreadManager();

	// Returns the number of hardware threads available
	unsigned int GetThreadCount() const;

	// Launch a worker thread with a given function
	void Launch( std::function<void()> func );

	// Wait for all threads to finish
	void JoinAll();

private:
	unsigned int threadCount;
	std::vector<std::thread> threads;
	std::atomic<bool> running{ true };
};
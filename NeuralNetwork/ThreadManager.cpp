#include <functional>
#include <thread>
#include <utility>
#include "ThreadManager.h"

ThreadManager::ThreadManager()
{
	threadCount = std::thread::hardware_concurrency();
	if( threadCount == 0 ) threadCount = 1; // Fallback to 1 if detection fails
}

ThreadManager::~ThreadManager()
{
	running = false;
	JoinAll();
}

unsigned int ThreadManager::GetThreadCount() const
{
	return threadCount;
}

void ThreadManager::Launch( std::function<void()> func )
{
	threads.emplace_back( std::move( func ) );
}

void ThreadManager::JoinAll()
{
	for( auto& t : threads ) {
		if( t.joinable() ) t.join();
	}
	threads.clear();
}
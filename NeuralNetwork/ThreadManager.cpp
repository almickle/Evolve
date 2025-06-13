#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include "ThreadManager.h"
#include "Types.h"

ThreadManager::~ThreadManager()
{
	{
		std::lock_guard<std::mutex> lock( tasksMutex );
		running = false;
	}
	tasksCv.notify_all();
	for( auto& t : threads ) {
		if( t.joinable() ) t.join();
	}
}

void ThreadManager::Init()
{
	for( uint i = 0; i < threadCount; ++i ) {
		threads.emplace_back( &ThreadManager::WorkerLoop, this );
	}
}

void ThreadManager::Launch( std::function<void()> func )
{
	{
		std::lock_guard<std::mutex> lock( tasksMutex );
		tasks.push( std::move( func ) );
		++activeTasks;
	}
	tasksCv.notify_one();
}

void ThreadManager::JoinAll()
{
	while( activeTasks > 0 ) {
		std::this_thread::yield();
	}
}

void ThreadManager::WorkerLoop()
{
	while( true ) {
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> lock( tasksMutex );
			tasksCv.wait( lock, [this] { return !tasks.empty() || !running; } );
			if( !running && tasks.empty() ) break;
			task = std::move( tasks.front() );
			tasks.pop();
		}
		task();
		--activeTasks;
	}
}
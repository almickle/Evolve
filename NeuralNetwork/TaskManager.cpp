#include <atomic>
#include <future>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>
#include "TaskManager.h"
#include "ThreadManager.h"

void TaskManager::Enqueue( Task task )
{
	{
		std::lock_guard<std::mutex> lock( queueMutex );
		taskQueue.push( std::move( task ) );
		++outstandingTasks;
	}
	threadManager->Launch( [this] {
		Task localTask;
		{
			std::lock_guard<std::mutex> lock( queueMutex );
			if( taskQueue.empty() ) return;
			localTask = std::move( taskQueue.front() );
			taskQueue.pop();
		}
		if( localTask.taskFunc ) localTask.taskFunc( localTask.path );
		if( localTask.onComplete ) localTask.onComplete();

		// Decrement outstanding tasks and check for fences
		if( --outstandingTasks == 0 ) {
			std::lock_guard<std::mutex> fenceLock( fenceMutex );
			for( auto& fence : pendingFences ) {
				fence->set_value();
			}
			pendingFences.clear();
		}
						   } );
}

std::future<void> TaskManager::InsertFence()
{
	auto promise = std::make_shared<std::promise<void>>();
	std::future<void> fut = promise->get_future();

	if( outstandingTasks == 0 ) {
		promise->set_value();
	}
	else {
		std::lock_guard<std::mutex> lock( fenceMutex );
		pendingFences.push_back( promise );
	}
	return fut;
}
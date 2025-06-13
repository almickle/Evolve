#include <future>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include "FileIOManager.h"
#include "ThreadManager.h"

void FileIOManager::Enqueue( FileTask task )
{
	{
		std::lock_guard<std::mutex> lock( queueMutex );
		taskQueue.push( std::move( task ) );
	}
	// Immediately launch the task on the thread pool
	threadManager->Launch( [this] {
		FileTask localTask;
		{
			std::lock_guard<std::mutex> lock( queueMutex );
			if( taskQueue.empty() ) return;
			localTask = std::move( taskQueue.front() );
			taskQueue.pop();
		}
		if( localTask.taskFunc ) localTask.taskFunc( localTask.path );
		if( localTask.onComplete ) localTask.onComplete();
						   } );
}

std::future<void> FileIOManager::InsertFence()
{
	auto promise = std::make_shared<std::promise<void>>();
	std::future<void> fut = promise->get_future();

	FileTask fenceTask;
	fenceTask.taskFunc = [promise]( const std::string& ) {
		promise->set_value();
		};
	fenceTask.onComplete = nullptr;
	fenceTask.path = "";

	Enqueue( std::move( fenceTask ) );
	return fut;
}
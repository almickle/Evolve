#include <mutex>
#include <utility>
#include "FileIOManager.h"
#include "ThreadManager.h"

FileIOManager::FileIOManager( ThreadManager* threadManager )
	: threadManager( threadManager )
{
}

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
		if( localTask.taskFunc ) localTask.taskFunc();
		if( localTask.onComplete ) localTask.onComplete();
						   } );
}
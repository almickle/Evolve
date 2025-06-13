#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <vector>
#include "ExecutionGraph.h"
#include "GraphPass.h"
#include "InitializationGraph.h"
#include "SystemManager.h"
#include "ThreadManager.h"

void InitializationGraph::ExecuteSync( SystemManager& systemManager )
{
	std::unordered_map<GraphPass*, int> indegree;
	std::unordered_map<GraphPass*, std::vector<GraphPass*>> adj;
	std::queue<GraphPass*> ready;

	for( auto& pass : passes ) indegree[pass.get()] = 0;
	for( auto& pass : passes ) {
		for( auto* dep : pass->GetDependencies() ) {
			adj[dep].push_back( pass.get() );
			indegree[pass.get()]++;
		}
	}
	for( auto& [node, deg] : indegree ) if( deg == 0 ) ready.push( node );

	std::unordered_map<GraphPass*, bool> executed;
	for( auto& pass : passes ) executed[pass.get()] = false;

	while( !ready.empty() ) {
		GraphPass* pass = ready.front();
		ready.pop();

		if( pass->IsReady() ) {
			pass->Init( systemManager );
			executed[pass] = true;

			for( GraphPass* neighbor : adj[pass] ) {
				bool allDepsExecuted = true;
				for( auto* dep : neighbor->GetDependencies() ) {
					if( !executed[dep] ) { allDepsExecuted = false; break; }
				}
				if( allDepsExecuted && --indegree[neighbor] == 0 ) {
					ready.push( neighbor );
				}
			}
		}
	}
}

void InitializationGraph::ExecuteAsync( SystemManager& systemManager )
{
	//ThreadManager& threadManager = systemManager.GetThreadManager();
	//if( !threadManager ) {
	//	ExecuteSync( renderer );
	//	return;
	//}

	// Build dependency graph
	std::unordered_map<GraphPass*, int> refCount;
	std::unordered_map<GraphPass*, std::vector<GraphPass*>> dependents;
	for( auto& pass : passes ) refCount[pass.get()] = 0;
	for( auto& pass : passes ) {
		for( auto* dep : pass->GetDependencies() ) {
			dependents[dep].push_back( pass.get() );
			refCount[pass.get()]++;
		}
	}

	// Thread-safe work queue
	std::queue<GraphPass*> workQueue;
	std::mutex queueMutex;
	std::condition_variable cv;

	// Track finished passes
	std::atomic<size_t> finishedCount = 0;
	const size_t totalPasses = passes.size();

	// Enqueue passes with zero dependencies
	for( auto& pass : passes ) {
		if( refCount[pass.get()] == 0 ) {
			workQueue.push( pass.get() );
		}
	}

	// Worker function
	auto worker = [&]() {
		while( true ) {
			GraphPass* pass = nullptr;
			{
				std::unique_lock lock( queueMutex );
				cv.wait( lock, [&] { return !workQueue.empty() || finishedCount == totalPasses; } );
				if( finishedCount == totalPasses ) break;
				pass = workQueue.front();
				workQueue.pop();
			}
			pass->Init( systemManager );
			finishedCount++;

			if( finishedCount == totalPasses ) {
				std::lock_guard lock( queueMutex );
				cv.notify_all();
			}

			// Update dependents
			for( GraphPass* dependent : dependents[pass] ) {
				if( --refCount[dependent] == 0 ) {
					std::lock_guard lock( queueMutex );
					workQueue.push( dependent );
					cv.notify_all();
				}
			}
		}
		};

	// Launch worker threads
	unsigned int numThreads = std::min( systemManager.GetThreadManager()->GetThreadCount(), static_cast<unsigned int>(totalPasses) );
	for( unsigned int i = 0; i < numThreads; ++i ) {
		systemManager.GetThreadManager()->Launch( worker );
	}

	// Notify workers in case work was enqueued before threads started
	cv.notify_all();

	// Wait for all threads to finish
	systemManager.GetThreadManager()->JoinAll();
}
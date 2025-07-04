#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <d3d12.h>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>
#include "ExecutionGraph.h"
#include "GraphPass.h"
#include "SystemManager.h"
#include "ThreadManager.h"
#include "Types.h"

ExecutionGraph* ExecutionGraph::AddPass( std::unique_ptr<GraphPass> pass )
{
	passes.push_back( std::move( pass ) );

	return this;
}

void ExecutionGraph::ExecuteSync( SystemManager& systemManager, const AssetID& sceneID )
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
			pass->Execute( systemManager, sceneID );
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

void ExecutionGraph::ExecuteAsync( SystemManager& systemManager, const AssetID& sceneID )
{
	ThreadManager* threadManager = systemManager.GetThreadManager();
	if( !threadManager ) {
		ExecuteSync( systemManager, sceneID );
		return;
	}

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
			pass->Execute( systemManager, sceneID );
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

std::vector<ID3D12CommandList*> ExecutionGraph::GetAllCommandLists( uint frameIndex ) const
{
	// Kahn's algorithm for topological sort
	std::unordered_map<const GraphPass*, int> indegree;
	std::unordered_map<const GraphPass*, std::vector<const GraphPass*>> adj;

	// Build indegree and adjacency list
	for( const auto& pass : passes ) indegree[pass.get()] = 0;
	for( const auto& pass : passes ) {
		for( const auto* dep : pass->GetDependencies() ) {
			adj[dep].push_back( pass.get() );
			indegree[pass.get()]++;
		}
	}

	// Queue for passes with no dependencies
	std::queue<const GraphPass*> ready;
	for( const auto& [node, deg] : indegree ) {
		if( deg == 0 ) ready.push( node );
	}

	std::vector<const GraphPass*> sorted;
	while( !ready.empty() ) {
		const GraphPass* pass = ready.front();
		ready.pop();
		sorted.push_back( pass );

		for( const GraphPass* neighbor : adj[pass] ) {
			if( --indegree[neighbor] == 0 ) {
				ready.push( neighbor );
			}
		}
	}

	// Gather command lists in topological order
	std::vector<ID3D12CommandList*> allCmdLists;
	allCmdLists.reserve( sorted.size() );
	for( const GraphPass* pass : sorted ) {
		allCmdLists.push_back( pass->GetCurrentCommandList( frameIndex ) );
	}
	return allCmdLists;
}
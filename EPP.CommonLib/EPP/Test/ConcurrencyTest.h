#pragma once
#include "..\Time\TimeDuration.h"
#include "..\Locks\ManualEvent.h"
#include <thread>
#include <condition_variable>
#include <intrin.h>

namespace EPP::Test
{
	template<typename TConcurrencyAction, typename TTestProvider>
	__declspec(noinline) void SyncConcurrencyTest(EPP::Time::TimeDuration duration, size_t numThreads, TTestProvider && testPovider, TConcurrencyAction && action)
	{
		std::vector<std::thread> runningThreads(numThreads);
		std::vector<uint32_t> runningThreadsState(numThreads);
		volatile uint32_t stopTest = 0;
		EPP::Locks::ManualEvent manualEvent;

		testPovider.BeginTest();
		for (size_t threadIndex = 0; threadIndex < numThreads; threadIndex++)
		{
			runningThreads[threadIndex] = std::thread( [&, threadIndex]() mutable
			{
				((volatile uint32_t&)runningThreadsState[threadIndex])++;
				manualEvent.Wait();
				((volatile uint32_t&)runningThreadsState[threadIndex])++;
				do 
				{
					action(threadIndex);
				} while (!stopTest);
			});
		}
		for (size_t threadIndex = 0; threadIndex < numThreads; threadIndex++)
		{
			while ((volatile uint32_t &)runningThreadsState[threadIndex] != 1);
		}
		manualEvent.Set();
		for (size_t threadIndex = 0; threadIndex < numThreads; threadIndex++)
		{
			while ((volatile uint32_t &)runningThreadsState[threadIndex] != 2);
		}
		testPovider.BeginMonitor();
		Sleep((DWORD) duration.ToMilliSeconds());
		testPovider.EndMonitor();

		stopTest = 1;
		_WriteBarrier();
		for (size_t i = 0; i < numThreads; i++)
		{
			runningThreads[i].join();
		}
		testPovider.EndTest();
	};
}
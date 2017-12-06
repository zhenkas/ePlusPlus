#include "stdafx.h"
#include "ThreadPool.h"
#include <atomic>
#include <atlutil.h>

namespace EPP::Threading
{

	void ThreadPool::Initialize(int numThreads)
	{
		m_completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

		m_threads.resize(numThreads);
		std::atomic<int> numThreadsStarted = 0;
		for (auto &thread : m_threads)
		{
			thread = std::thread([&]()
			{
				numThreadsStarted.fetch_add(1);
				ThreadFunc();
			});
		}
		while (numThreadsStarted.load() != numThreads) 
		{
			Sleep(0);
		}
	}

	void ThreadPool::ThreadFunc()
	{

	}

}
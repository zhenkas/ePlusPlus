#pragma once
#include <EPP\Threading\IThreadPool.h>
#include <windows.h>
#include <vector>
#include <thread>

namespace EPP::Threading
{
	using namespace EPP::Holders;

	struct ThreadPool : IThreadPool
	{
		void Initialize(int numThreads);
	protected:
		void ThreadFunc();
	protected:
		HANDLE m_completionPort;
		std::vector<std::thread> m_threads;
	};
}

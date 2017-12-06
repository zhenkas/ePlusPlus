#pragma once
#include "..\Time\DateTime.h"
#include <vector>
#include <winnt.h>

namespace EPP::Monitoring
{
	using namespace EPP::Time;
	struct ThreadCounters
	{
		ThreadCounters(size_t numThreads)
		{
			m_numThreads = numThreads;
			m_counters.resize(numThreads);
		}
		void BeginMonitor()
		{
			MemoryBarrier();
			m_effectiveCounters = m_counters;
			m_startTime = DateTime::Now();
		}
		void Inc(size_t threadIndex)
		{
			++m_counters[threadIndex];
		}
		void EndMonitor()
		{
			MemoryBarrier();
			std::vector<__int64> snapShot = m_counters;
			m_endTime = DateTime::Now();
			for (size_t i = 0; i < m_effectiveCounters.size(); ++i)
			{
				m_effectiveCounters[i] = snapShot[i] - m_effectiveCounters[i];
			}
		}
		DateTime m_startTime;
		DateTime m_endTime;
		std::vector<__int64> m_effectiveCounters;
		std::vector<__int64> m_counters;
		size_t m_numThreads;
	};

}
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
			m_effectiveCounters.resize(m_counters.size());
			MemoryBarrier();
			memcpy(m_effectiveCounters.data(), m_counters.data(), sizeof(uint64_t) * m_counters.size());
			m_startTime = DateTime::Now();
		}
		void Inc(size_t threadIndex)
		{
			++m_counters[threadIndex];
		}
		void EndMonitor()
		{
			std::vector<uint64_t> snapShot;
			snapShot.resize(m_counters.size());
			MemoryBarrier();
			memcpy(snapShot.data(), m_counters.data(), sizeof(uint64_t) * m_counters.size());
			m_endTime = DateTime::Now();
			for (size_t i = 0; i < m_effectiveCounters.size(); ++i)
			{
				m_effectiveCounters[i] = snapShot[i] - m_effectiveCounters[i];
			}
		}
		DateTime m_startTime;
		DateTime m_endTime;
		std::vector<uint64_t> m_effectiveCounters;
		std::vector<uint64_t> m_counters;
		size_t m_numThreads;
	};

}
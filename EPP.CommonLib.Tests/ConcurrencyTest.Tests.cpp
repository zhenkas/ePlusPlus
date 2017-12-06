#include "stdafx.h"
#include "CppUnitTest.h"
#include <EPP\Test\ConcurrencyTest.h>
#include "EPP\Time\DateTime.h"
#include <atlstr.h>
#include "EPP\Locks\ExclusiveSpinLock.h"
#include "EPP\Monitoring\ThreadCounters.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace EPP::Test;
using namespace EPP::Time;
using namespace EPP::Locks;
using namespace EPP::Monitoring;
using namespace std::chrono_literals;

namespace EPP::Tests
{
	TEST_CLASS(ConcurrencyTest)
	{
	public:

		TEST_METHOD(SyncConcurrencyTest_Basic)
		{
			struct TestProvider : ThreadCounters
			{
				TestProvider(size_t numThreads): ThreadCounters(numThreads)
				{
				}
				void BeginTest()
				{
					m_testStartTime = DateTime::Now();
				}
				void EndTest()
				{
					m_testEndTime = DateTime::Now();
				}
				DateTime m_testStartTime;
				DateTime m_testEndTime;
			};
			TestProvider testProvider(4);
			ExclusiveSpinLock lock;
			volatile uint32_t check_sum = 0;
			SyncConcurrencyTest(1000ms, testProvider.m_numThreads, testProvider, [&](size_t threadIndex)
			{
				testProvider.Inc(threadIndex);
				lock.ExclusiveLock();
				check_sum++;
				check_sum--;
				lock.ExclusiveRelease();
			});

			auto duration = testProvider.m_endTime - testProvider.m_startTime;
			if (check_sum != 0)
			{
				Assert::Fail(L"Expected check_sum 0");
			}
			if (duration.ToMilliSeconds() < 1000)
			{
				Assert::Fail(L"Expected duration more than 1000 ms");
			}
			CString msg = L"Counters: ";
			for (size_t i = 0; i < testProvider.m_numThreads; i++)
			{
				Assert::IsTrue(testProvider.m_effectiveCounters[i] <= testProvider.m_counters[i], L"Expected <= 0");
				msg.AppendFormat(L"%I64u(+%I64u) ", testProvider.m_effectiveCounters[i], testProvider.m_counters[i] - testProvider.m_effectiveCounters[i]);
			}
			Logger::WriteMessage(msg);
		}
	};
}
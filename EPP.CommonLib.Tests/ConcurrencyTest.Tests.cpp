#include "stdafx.h"
#include "CppUnitTest.h"
#include <EPP\Test\ConcurrencyTest.h>
#include "EPP\Time\DateTime.h"
#include <atlstr.h>
#include "EPP\Locks\ExclusiveSpinLock.h"
#include "EPP\Monitoring\ThreadCounters.h"

std::string string_format(const std::string fmt, ...) {
	int size = ((int)fmt.size()) * 2 + 50;   // Use a rubric appropriate for your code
	std::string str;
	va_list ap;
	while (1) {     // Maximum two passes on a POSIX system...
		str.resize(size);
		va_start(ap, fmt);
		int n = vsnprintf((char *)str.data(), size, fmt.c_str(), ap);
		va_end(ap);
		if (n > -1 && n < size) {  // Everything worked
			str.resize(n);
			return str;
		}
		if (n > -1)  // Needed size returned
			size = n + 1;   // For null char
		else
			size *= 2;      // Guess at a larger size (OS specific)
	}
	return str;
}

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
			TestProvider * testProviderPtr = new TestProvider(8);
			TestProvider & testProvider = *testProviderPtr;
			using TLock = ExclusiveSpinLock;
			TLock lock;
			volatile uint64_t check_sum = 0;
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
			uint64_t totalPerSec = 0;
			for (size_t i = 0; i < testProvider.m_numThreads; i++)
			{
				totalPerSec += testProvider.m_effectiveCounters[i];
				Assert::IsTrue(testProvider.m_effectiveCounters[i] <= testProvider.m_counters[i], L"Expected <= 0");
			}
			msg.AppendFormat(L"Total: %I64u/s ", totalPerSec);
			for (size_t i = 0; i < testProvider.m_numThreads; i++)
			{
				msg.AppendFormat(L"%I64u(+%I64u) ", testProvider.m_effectiveCounters[i], testProvider.m_counters[i] - testProvider.m_effectiveCounters[i]);
			}
			Logger::WriteMessage(msg);
		}
	};
}
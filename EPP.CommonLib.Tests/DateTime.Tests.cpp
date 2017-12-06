#include "stdafx.h"
#include "CppUnitTest.h"
#include <EPP\Time\DateTime.h>
#include <windows.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace EPP::Time;

namespace EPP::Tests
{
	TEST_CLASS(DateTime_Tests)
	{
	public:
		TEST_METHOD(DateTimeNow)
		{
			DateTime dt = DateTime::Now();			
		}
		TEST_METHOD(DateTimeDuration)
		{
			DateTime dt = DateTime::Now();
			Sleep(500);
			DateTime dt2 = DateTime::Now();
			TimeDuration tt = dt2 - dt;
			if (tt.ToMilliSeconds() - tt.ToMilliSeconds() % 100 != 500)
			{
				Assert::Fail(L"Expected duration 500 ms");
			}
		}
	};
}
#include "stdafx.h"
#include "CppUnitTest.h"
#include <EPP\Holders\SafeObject.h>
#include <EPP\Monitoring\PerfMon.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace EPP;
using namespace EPP::Monitoring;

struct S1 : ISafeObject
{

};

namespace EPP::Tests
{		
	TEST_CLASS(SafeObjectTests)
	{
	public:
		
		TEST_METHOD(DefaultConstructor)
		{

			PerfMon perfMon("sdfsd");
			SafeObject<S1> s1;
			Assert::IsTrue(s1.ptr == nullptr, L"Should be null", LINE_INFO());
			Assert::IsTrue(s1.safePtr.use_count() == 0, L"use_count should 0", LINE_INFO());
		}

	};
}
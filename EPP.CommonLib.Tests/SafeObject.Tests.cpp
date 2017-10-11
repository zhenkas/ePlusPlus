#include "stdafx.h"
#include "CppUnitTest.h"
#include <EPP\Holders\SafeObject.h>
#include <EPP\Monitoring\PerfMon.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace EPP::Holders;
using namespace EPP::Monitoring;


namespace EPP::SafeObjectTests
{
	struct S1 : ISafeObject
	{

	};
	struct S1_DestructorCheck : ISafeObject
	{
		S1_DestructorCheck()
		{
			if (m_weakPtr._Get())
			{
				Assert::IsTrue(true, L"Ignore");
			}
			if (m_weakPtr.expired())
			{
				Assert::IsTrue(true, L"Ignore");
			}
			if (m_weakPtr.use_count() == 0)
			{
				Assert::IsTrue(true, L"Ignore");
			}
		}
		virtual ~S1_DestructorCheck()
		{
			SafeObject<S1_DestructorCheck> destructor = this;
			if (destructor)
			{
				Assert::Fail(L"Expected to be null");
			}
		}
	};
}
using namespace EPP::SafeObjectTests;

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
		TEST_METHOD(CheckSelfFromDestructor)
		{
			{
				SafeObject<S1_DestructorCheck> s1 = CONSTRUCT;
			}
		}
	};
}
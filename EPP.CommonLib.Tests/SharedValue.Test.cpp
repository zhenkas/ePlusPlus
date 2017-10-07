#include "stdafx.h"
#include "CppUnitTest.h"
#include <EPP\Holders\SharedValue.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace EPP::Holders;

namespace EPP::ShareValueTests
{
	struct S1 : ISafeObject
	{
		int value;
	};
}
using namespace EPP::ShareValueTests;

namespace EPP::Tests
{
	TEST_CLASS(SafeObjectTests)
	{
	public:

		TEST_METHOD(SharedValue_int)
		{
			SharedValue<int> s1 = CONSTRUCT;
			s1->value = 3;

			SharedValue<int> s2 = s1;
			Assert::IsTrue(s1->value == s2->value, L"Should equal", LINE_INFO());
		}
		TEST_METHOD(SharedValue_SafeObject)
		{
			SharedValue<S1> s1 = CONSTRUCT;
			s1->value = 3;

			SharedValue<S1> s2 = s1;
			Assert::IsTrue(s1->value == s2->value, L"Should equal", LINE_INFO());
		}
	};
}
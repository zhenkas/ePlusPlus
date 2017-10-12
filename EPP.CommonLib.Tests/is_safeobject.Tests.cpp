#include "stdafx.h"
#include "CppUnitTest.h"
#include <EPP\Holders\SharedValue.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace EPP::Holders;
using namespace EPP::Templates;

namespace EPP::IsSafeObjectTests
{
	struct S1 : ISafeObject
	{
		int value;
	};
}
using namespace EPP::IsSafeObjectTests;

namespace EPP::Tests
{
	TEST_CLASS(is_safeobject_tests)
	{
	public:

		TEST_METHOD(TestOnSafeObject)
		{
			static_assert(is_safeobject<SafeObject<S1>>::value, "Expected to be safe object");
			static_assert(is_safeobject<SafeObject<S1>&>::value, "Expected to be safe object");
			static_assert(is_safeobject<SafeObject<S1>&&>::value, "Expected to be safe object");
			static_assert(is_safeobject<const SafeObject<S1>>::value, "Expected to be safe object");
			static_assert(is_safeobject<const SafeObject<S1>&>::value, "Expected to be safe object");
			static_assert(is_safeobject<const SafeObject<S1>&&>::value, "Expected to be safe object");
		}
		TEST_METHOD(TestNotSafeObject)
		{
			static_assert(!is_safeobject<S1>::value, "Expected to be NOT safe object");
			static_assert(!is_safeobject<S1&>::value, "Expected to be NOT safe object");
			static_assert(!is_safeobject<S1&&>::value, "Expected to be NOT safe object");
			static_assert(!is_safeobject<const S1>::value, "Expected to NOT be safe object");
			static_assert(!is_safeobject<const S1&>::value, "Expected to NOT be safe object");
			static_assert(!is_safeobject<const S1&&>::value, "Expected to NOT be safe object");

			static_assert(!is_safeobject<int>::value, "Expected to be NOT safe object");
		}
	};
}
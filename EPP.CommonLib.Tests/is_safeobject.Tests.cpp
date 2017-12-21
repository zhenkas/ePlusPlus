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
			static_assert(is_safeobject_v<SafeObject<S1>>, "Expected to be safe object");
			static_assert(is_safeobject_v<SafeObject<S1>&>, "Expected to be safe object");
			static_assert(is_safeobject_v<SafeObject<S1>&&>, "Expected to be safe object");
			static_assert(is_safeobject_v<const SafeObject<S1>>, "Expected to be safe object");
			static_assert(is_safeobject_v<const SafeObject<S1>&>, "Expected to be safe object");
			static_assert(is_safeobject_v<const SafeObject<S1>&&>, "Expected to be safe object");
		}
		TEST_METHOD(TestNotSafeObject)
		{
			static_assert(!is_safeobject_v<S1>, "Expected to be NOT safe object");
			static_assert(!is_safeobject_v<S1&>, "Expected to be NOT safe object");
			static_assert(!is_safeobject_v<S1&&>, "Expected to be NOT safe object");
			static_assert(!is_safeobject_v<const S1>, "Expected to NOT be safe object");
			static_assert(!is_safeobject_v<const S1&>, "Expected to NOT be safe object");
			static_assert(!is_safeobject_v<const S1&&>, "Expected to NOT be safe object");

			static_assert(!is_safeobject_v<int>, "Expected to be NOT safe object");
		}
	};
}
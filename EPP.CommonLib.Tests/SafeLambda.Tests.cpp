#include "stdafx.h"
#include "CppUnitTest.h"
#include <EPP\Templates\t_lambda_resolver.h>
#include <EPP\Holders\SafeLambda.h>
#include <windows.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace EPP::Templates;
using namespace EPP::Holders;

namespace EPP::Tests
{
	TEST_CLASS(SafeLambda_Tests)
	{
	public:
		TEST_METHOD(SafeLambda_Empty)
		{
			AutoSafeLambda<void(void)> autoLambda1 = [](){};
			AutoSafeLambda<void(int)> autoLambda2 = [](int) {};
			AutoSafeLambda<int(void)> autoLambda3 = []() -> int { return 0; };
		}

		TEST_METHOD(SafeLambda_Creator)
		{
			bool lambda1_called = false;
			bool lambda2_called = false;
			bool lambda3_called = false;

			auto lambda1 = CreateSafeLambda([&]() 
			{
				lambda1_called = true;
			});
			auto lambda2 = CreateSafeLambda([&](int) {
				lambda2_called = true;
			});
			auto lambda3 = CreateSafeLambda([&]() -> int { 
				lambda3_called = true;
				return 22;  
			});

			lambda1->CallFunction();
			lambda2->CallFunction(0);
			int lambda3_result = lambda3->CallFunction();

			Assert::IsTrue(lambda1_called, L"Expected lambda1 called");
			Assert::IsTrue(lambda2_called, L"Expected lambda2 called");
			Assert::IsTrue(lambda3_called, L"Expected lambda3 called");
			Assert::AreEqual(22, lambda3_result, L"Expected lambda3 result 22");
		}
	};
}

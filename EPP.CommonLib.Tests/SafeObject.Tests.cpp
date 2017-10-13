#include "stdafx.h"
#include "CppUnitTest.h"
#include <EPP\Holders\SafeObject.h>
#include <EPP\Monitoring\PerfMon.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace EPP::Holders;
using namespace EPP::Monitoring;


namespace EPP::SafeObjectTests
{
	const int g_defaultVal1 = 1;
	const int g_defaultVal2 = 2;
	const int g_defaultVal3 = 3;
	const int g_defaultVal4 = 4;
	const int g_paramVal1 = 10;
	const int g_paramVal2 = 11;
	const int g_paramVal3 = 12;
	const int g_paramVal4 = 13;

	struct Default1 : ISafeObject
	{
		int i1 = g_defaultVal1;
	};

	struct Default2 : Default1
	{
		int i2 = g_defaultVal2;
	};

	struct Default3 : Default1
	{
		int i3 = g_defaultVal3;
	};

	struct DefaultVirtual1 : virtual ISafeObject
	{
		int i1 = g_defaultVal1;
	};

	struct DefaultVirtual2 : virtual DefaultVirtual1
	{
		int i2 = g_defaultVal2;
	};

	struct DefaultVirtual3 : virtual DefaultVirtual1
	{
		int i3 = g_defaultVal3;
	};

	struct DefaultVirtual4 : virtual DefaultVirtual2, virtual DefaultVirtual3
	{
		int i4 = g_defaultVal4;
	};

	struct Forward1 : Default1
	{
		Forward1(int ii1) 
		{
			i1 = ii1;
		}
	};

	struct Forward2 : Default2
	{
		Forward2(int ii1, int ii2) 
		{ 
			i1 = ii1;
			i2 = ii2;
		}
	};

	struct ForwardVirtual1 : virtual DefaultVirtual1
	{
		ForwardVirtual1(int ii1) 
		{
			i1 = ii1;
		}
	};

	struct ForwardVirtual2 : virtual DefaultVirtual2
	{
		ForwardVirtual2(int ii1, int ii2)
		{
			i1 = ii1;
			i2 = ii2;
		}
	};
	struct ForwardVirtual3 : virtual DefaultVirtual3
	{
		ForwardVirtual3(int ii1, int ii3)
		{
			i1 = ii1;
			i3 = ii3;
		}
	};

	struct ForwardVirtual4 : ForwardVirtual2, ForwardVirtual3, virtual DefaultVirtual4
	{
		ForwardVirtual4(int ii1, int ii2, int ii3, int ii4) : ForwardVirtual2(ii1, ii2), ForwardVirtual3(ii1, ii3)
		{
			i4 = ii4;
		}
	};

	struct S1 : ISafeObject
	{

	};
	struct DestructorCheck : ISafeObject
	{
		DestructorCheck()
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
		virtual ~DestructorCheck()
		{
			SafeObject<DestructorCheck> destructor = this;
			if (destructor)
			{
				Assert::Fail(L"Expected to be null");
			}
		}
	};

	template<typename SO>
	void CheckNull(SO && s1, const __LineInfo* pLineInfo)
	{
		Assert::IsTrue(s1.ptr == nullptr, L"Should be null", pLineInfo);
		Assert::IsTrue(s1.safePtr.use_count() == 0, L"use_count should be 0", pLineInfo);
	}
	template<typename SO>
	void CheckNotNull(SO && s1, const __LineInfo* pLineInfo)
	{
		Assert::IsTrue(s1.ptr != nullptr, L"Should be not null", pLineInfo);
		Assert::IsTrue(s1->m_weakPtr._Get() == (ISafeObject *) s1, L"Weak pointer is invalid", pLineInfo);
		Assert::IsTrue(s1->m_weakPtr._Get() == s1.safePtr._Get(), L"Weak and Safe pointers should be equal", pLineInfo);
		Assert::IsTrue(s1.safePtr.use_count() != 0, L"use_count should not be 0", pLineInfo);
	}
	template<typename SO>
	void CheckMoved(SO && s1, const __LineInfo* pLineInfo)
	{
		Assert::IsTrue(s1.ptr != nullptr, L"Should be not null", pLineInfo);
		Assert::IsTrue(s1->m_weakPtr._Get() == (ISafeObject *)s1, L"Weak pointer is invalid", pLineInfo);
		Assert::IsTrue(s1->m_weakPtr.use_count() != 0, L"Weak pointer use_count() is 0", pLineInfo);
		Assert::IsTrue(s1.safePtr.use_count() == 0, L"use_count should be 0", pLineInfo);
	}
	template<typename SO>
	void CheckDefaultVal1(SO && s1, const __LineInfo* pLineInfo)
	{
		Assert::IsTrue(s1.i1 == g_defaultVal1, L"Invalid i1", pLineInfo);
	}
	template<typename SO>
	void CheckDefaultVal2(SO && s1, const __LineInfo* pLineInfo)
	{
		Assert::IsTrue(s1.i2 == g_defaultVal2, L"Invalid i2", pLineInfo);
	}
	template<typename SO>
	void CheckDefaultVal3(SO && s1, const __LineInfo* pLineInfo)
	{
		Assert::IsTrue(s1.i3 == g_defaultVal3, L"Invalid i3", pLineInfo);
	}
	template<typename SO>
	void CheckDefaultVal4(SO && s1, const __LineInfo* pLineInfo)
	{
		Assert::IsTrue(s1.i4 == g_defaultVal4, L"Invalid i4", pLineInfo);
	}
	template<typename SO>
	void CheckParamVal1(SO && s1, const __LineInfo* pLineInfo)
	{
		Assert::IsTrue(s1.i1 == g_paramVal1, L"Invalid i1", pLineInfo);
	}
	template<typename SO>
	void CheckParamVal2(SO && s1, const __LineInfo* pLineInfo)
	{
		Assert::IsTrue(s1.i2 == g_paramVal2, L"Invalid i2", pLineInfo);
	}
	template<typename SO>
	void CheckParamVal3(SO && s1, const __LineInfo* pLineInfo)
	{
		Assert::IsTrue(s1.i3 == g_paramVal3, L"Invalid i3", pLineInfo);
	}
	template<typename SO>
	void CheckParamtVal4(SO && s1, const __LineInfo* pLineInfo)
	{
		Assert::IsTrue(s1.i4 == g_paramVal4, L"Invalid i4", pLineInfo);
	}


	template<typename SO1, typename SO2>
	void CheckIsSame(SO1 && s1, SO2 && s2, const __LineInfo* pLineInfo)
	{
		Assert::IsTrue(s1->m_weakPtr._Get() == s2->m_weakPtr._Get(), L"Expected to be same object", pLineInfo);
	}
}
using namespace EPP::SafeObjectTests;


namespace EPP::Tests
{		
	TEST_CLASS(SafeObjectTests)
	{
	public:
		
		TEST_METHOD(DefaultConstructor)
		{

			SafeObject<Default1> s1;
			CheckNull(s1, LINE_INFO());
		}
		TEST_METHOD(ReleaseInstance)
		{

			SafeObject<Default1> s1 = CONSTRUCT;
			CheckNotNull(s1, LINE_INFO());
			s1.ReleaseInstance();
			CheckNull(s1, LINE_INFO());
		}
		TEST_METHOD(ConstructWithTempObject)
		{

			SafeObject<Default1> s1 = CONSTRUCT;
			SafeObject<Default1> s2 = std::move(s1);

			CheckMoved(s1, LINE_INFO());
			CheckNotNull(s2, LINE_INFO());
		}
		TEST_METHOD(AssignToNullptr)
		{

			SafeObject<Default1> s1 = CONSTRUCT;
			CheckNotNull(s1, LINE_INFO());
			s1 = nullptr;
			CheckNull(s1, LINE_INFO());
		}
		TEST_METHOD(ConstructWithConstruct1)
		{
			{
				SafeObject<Default1> s1 = CONSTRUCT;
				CheckNotNull(s1, LINE_INFO());
				CheckDefaultVal1(s1.StaticCast<Default1>(), LINE_INFO());
			}
			{
				SafeObject<Default2> s2 = CONSTRUCT;
				CheckNotNull(s2, LINE_INFO());
				CheckDefaultVal1(s2.StaticCast<Default1>(), LINE_INFO());
				CheckDefaultVal1(s2.StaticCast<Default2>(), LINE_INFO());
				CheckDefaultVal2(s2.StaticCast<Default2>(), LINE_INFO());
			}
		}
		TEST_METHOD(ConstructWithConstruct2)
		{
			{
				SafeObject<DefaultVirtual1> s1 = CONSTRUCT;
				CheckNotNull(s1, LINE_INFO());
				CheckDefaultVal1(*s1, LINE_INFO());
			}
			{
				SafeObject<DefaultVirtual2> s2 = CONSTRUCT;
				CheckNotNull(s2, LINE_INFO());
				CheckDefaultVal1(s2.StaticCast<DefaultVirtual1>(), LINE_INFO());
				CheckDefaultVal1(s2.StaticCast<DefaultVirtual2>(), LINE_INFO());
				CheckDefaultVal2(s2.StaticCast<DefaultVirtual2>(), LINE_INFO());
			}
			{
				SafeObject<DefaultVirtual3> s3 = CONSTRUCT;
				CheckNotNull(s3, LINE_INFO());
				CheckDefaultVal1(s3.StaticCast<DefaultVirtual1>(), LINE_INFO());
				CheckDefaultVal1(s3.StaticCast<DefaultVirtual3>(), LINE_INFO());
				CheckDefaultVal3(s3.StaticCast<DefaultVirtual3>(), LINE_INFO());
			}
			{
				SafeObject<DefaultVirtual4> s4 = CONSTRUCT;
				CheckNotNull(s4, LINE_INFO());
				CheckDefaultVal1(s4.StaticCast<DefaultVirtual1>(), LINE_INFO());
				CheckDefaultVal1(s4.StaticCast<DefaultVirtual2>(), LINE_INFO());
				CheckDefaultVal1(s4.StaticCast<DefaultVirtual3>(), LINE_INFO());
				CheckDefaultVal1(s4.StaticCast<DefaultVirtual4>(), LINE_INFO());
				CheckDefaultVal2(s4.StaticCast<DefaultVirtual2>(), LINE_INFO());
				CheckDefaultVal2(s4.StaticCast<DefaultVirtual4>(), LINE_INFO());
				CheckDefaultVal3(s4.StaticCast<DefaultVirtual3>(), LINE_INFO());
				CheckDefaultVal3(s4.StaticCast<DefaultVirtual4>(), LINE_INFO());
				CheckDefaultVal4(s4.StaticCast<DefaultVirtual4>(), LINE_INFO());
			}
		}

		TEST_METHOD(ConstructWithParams1)
		{
			{
				SafeObject<Forward1> s1 = { CONSTRUCT, g_paramVal1 };
				CheckNotNull(s1, LINE_INFO());
				CheckParamVal1(*s1, LINE_INFO());
			}

			{
				SafeObject<Forward2> s2 = { CONSTRUCT, g_paramVal1, g_paramVal2 };
				CheckNotNull(s2, LINE_INFO());
				CheckParamVal1(*s2, LINE_INFO());
				CheckParamVal2(*s2, LINE_INFO());
			}
		}

		TEST_METHOD(ConstructWithParams2)
		{
			{
				SafeObject<ForwardVirtual1> s1 = { CONSTRUCT, g_paramVal1 };
				CheckNotNull(s1, LINE_INFO());
				CheckParamVal1(*s1, LINE_INFO());
			}
			{
				SafeObject<ForwardVirtual2> s2 = { CONSTRUCT, g_paramVal1, g_paramVal2 };
				CheckNotNull(s2, LINE_INFO());
				CheckParamVal1(s2.StaticCast<DefaultVirtual1>(), LINE_INFO());
				CheckParamVal1(s2.StaticCast<ForwardVirtual2>(), LINE_INFO());
				CheckParamVal2(s2.StaticCast<ForwardVirtual2>(), LINE_INFO());
			}
			{
				SafeObject<ForwardVirtual3> s3 = { CONSTRUCT, g_paramVal1, g_paramVal3 };
				CheckNotNull(s3, LINE_INFO());
				CheckParamVal1(s3.StaticCast<DefaultVirtual1>(), LINE_INFO());
				CheckParamVal1(s3.StaticCast<ForwardVirtual3>(), LINE_INFO());
				CheckParamVal3(s3.StaticCast<ForwardVirtual3>(), LINE_INFO());
			}
			{
				SafeObject<ForwardVirtual4> s4 = { CONSTRUCT, g_paramVal1, g_paramVal2, g_paramVal3, g_paramVal4 };
				CheckNotNull(s4, LINE_INFO());
				CheckParamVal1(s4.StaticCast<DefaultVirtual1>(), LINE_INFO());
				CheckParamVal1(s4.StaticCast<ForwardVirtual2>(), LINE_INFO());
				CheckParamVal1(s4.StaticCast<ForwardVirtual3>(), LINE_INFO());
				CheckParamVal1(s4.StaticCast<ForwardVirtual4>(), LINE_INFO());
				CheckParamVal2(s4.StaticCast<ForwardVirtual2>(), LINE_INFO());
				CheckParamVal2(s4.StaticCast<ForwardVirtual4>(), LINE_INFO());
				CheckParamVal3(s4.StaticCast<ForwardVirtual3>(), LINE_INFO());
				CheckParamVal3(s4.StaticCast<ForwardVirtual4>(), LINE_INFO());
			}
		}

		TEST_METHOD(ShareSelfFromDestructor)
		{
			{
				SafeObject<DestructorCheck> s1 = CONSTRUCT;
			}
		}
		TEST_METHOD(Upcast)
		{
			SafeObject<Default2> s2 = CONSTRUCT;
			SafeObject<Default1> s1 = s2;

			CheckNotNull(s2, LINE_INFO());
			CheckNotNull(s1, LINE_INFO());
			CheckIsSame(s1, s2, LINE_INFO());
			CheckDefaultVal1(s1.StaticCast<Default1>(), LINE_INFO());
			CheckDefaultVal1(s1.StaticCast<Default2>(), LINE_INFO());
			CheckDefaultVal2(s1.StaticCast<Default2>(), LINE_INFO());
		}

		TEST_METHOD(CPPCast)
		{
			SafeObject<Default2> s2 = CONSTRUCT;
			SafeObject<Default1> s1 = s2;

			CheckNotNull(s2, LINE_INFO());
			CheckNotNull(s1, LINE_INFO());
			CheckIsSame(s1, s2, LINE_INFO());
			CheckDefaultVal1((Default1 &) s1, LINE_INFO());
			CheckDefaultVal1((Default2 &) *s1, LINE_INFO());
			CheckDefaultVal2((Default2 &) *s1, LINE_INFO());

			CheckDefaultVal1(*(SafeObject<Default1>)s2, LINE_INFO());

			CheckDefaultVal1(*(const SafeObject<Default2> &)s2, LINE_INFO());

		}

		TEST_METHOD(DownCast)
		{
			SafeObject<Default1> s1 = SafeObject<Default2>(CONSTRUCT);
			SafeObject<Default2> s2 = s1.StaticCast<Default2>();

			CheckNotNull(s2, LINE_INFO());
			CheckNotNull(s1, LINE_INFO());
			CheckIsSame(s1, s2, LINE_INFO());
		}

		TEST_METHOD(UpcastVirtual)
		{
			SafeObject<DefaultVirtual1> s1;
			SafeObject<DefaultVirtual2> s2;
			SafeObject<DefaultVirtual3> s3;
			SafeObject<DefaultVirtual4> s4 = CONSTRUCT;

			s1 = s4;
			s2 = s4;
			s3 = s4;
			CheckIsSame(s1, s4, LINE_INFO());
			CheckIsSame(s2, s4, LINE_INFO());
			CheckIsSame(s3, s4, LINE_INFO());

			CheckDefaultVal1( s1.StaticCast<DefaultVirtual1>(), LINE_INFO());
			CheckDefaultVal1(*s1.DynamicCast<DefaultVirtual2>(), LINE_INFO());
			CheckDefaultVal1(*s1.DynamicCast<DefaultVirtual3>(), LINE_INFO());
			CheckDefaultVal1(*s1.DynamicCast<DefaultVirtual4>(), LINE_INFO());
			CheckDefaultVal2(*s1.DynamicCast<DefaultVirtual2>(), LINE_INFO());
			CheckDefaultVal2(*s1.DynamicCast<DefaultVirtual4>(), LINE_INFO());
			CheckDefaultVal3(*s1.DynamicCast<DefaultVirtual3>(), LINE_INFO());
			CheckDefaultVal3(*s1.DynamicCast<DefaultVirtual4>(), LINE_INFO());
			CheckDefaultVal4(*s1.DynamicCast<DefaultVirtual4>(), LINE_INFO());

			CheckDefaultVal1(s2.StaticCast <DefaultVirtual1>(), LINE_INFO());
			CheckDefaultVal1(s2.StaticCast<DefaultVirtual2>(), LINE_INFO());
			CheckDefaultVal1(*s2.DynamicCast<DefaultVirtual3>(), LINE_INFO());
			CheckDefaultVal1(*s2.DynamicCast<DefaultVirtual4>(), LINE_INFO());
			CheckDefaultVal2(s2.StaticCast<DefaultVirtual2>(), LINE_INFO());
			CheckDefaultVal2(*s2.DynamicCast<DefaultVirtual4>(), LINE_INFO());
			CheckDefaultVal3(*s2.DynamicCast<DefaultVirtual3>(), LINE_INFO());
			CheckDefaultVal3(*s2.DynamicCast<DefaultVirtual4>(), LINE_INFO());
			CheckDefaultVal4(*s2.DynamicCast<DefaultVirtual4>(), LINE_INFO());

			CheckDefaultVal1(s3.StaticCast <DefaultVirtual1>(), LINE_INFO());
			CheckDefaultVal1(*s3.DynamicCast<DefaultVirtual2>(), LINE_INFO());
			CheckDefaultVal1(s3.StaticCast<DefaultVirtual3>(), LINE_INFO());
			CheckDefaultVal1(*s3.DynamicCast<DefaultVirtual4>(), LINE_INFO());
			CheckDefaultVal2(*s3.DynamicCast<DefaultVirtual2>(), LINE_INFO());
			CheckDefaultVal2(*s3.DynamicCast<DefaultVirtual4>(), LINE_INFO());
			CheckDefaultVal3(s3.StaticCast<DefaultVirtual3>(), LINE_INFO());
			CheckDefaultVal3(*s3.DynamicCast<DefaultVirtual4>(), LINE_INFO());
			CheckDefaultVal4(*s3.DynamicCast<DefaultVirtual4>(), LINE_INFO());

			CheckDefaultVal1(s4.StaticCast<DefaultVirtual1>(), LINE_INFO());
			CheckDefaultVal1(s4.StaticCast<DefaultVirtual2>(), LINE_INFO());
			CheckDefaultVal1(s4.StaticCast<DefaultVirtual3>(), LINE_INFO());
			CheckDefaultVal1(s4.StaticCast<DefaultVirtual4>(), LINE_INFO());
			CheckDefaultVal2(s4.StaticCast<DefaultVirtual2>(), LINE_INFO());
			CheckDefaultVal2(s4.StaticCast<DefaultVirtual4>(), LINE_INFO());
			CheckDefaultVal3(s4.StaticCast<DefaultVirtual3>(), LINE_INFO());
			CheckDefaultVal3(s4.StaticCast<DefaultVirtual4>(), LINE_INFO());
			CheckDefaultVal4(s4.StaticCast<DefaultVirtual4>(), LINE_INFO());

		}

		TEST_METHOD(CPPCastVirtual)
		{
			SafeObject<DefaultVirtual1> s1;
			SafeObject<DefaultVirtual2> s2;
			SafeObject<DefaultVirtual3> s3;
			SafeObject<DefaultVirtual4> s4 = CONSTRUCT;

			s1 = s4;
			s2 = s4;
			s3 = s4;
			CheckIsSame(s1, s4, LINE_INFO());
			CheckIsSame(s2, s4, LINE_INFO());
			CheckIsSame(s3, s4, LINE_INFO());

			CheckDefaultVal1((DefaultVirtual1 &) s1, LINE_INFO());
			CheckDefaultVal1(*s1.DynamicCast<DefaultVirtual2>(), LINE_INFO());
			CheckDefaultVal1(*s1.DynamicCast<DefaultVirtual3>(), LINE_INFO());
			CheckDefaultVal1(*s1.DynamicCast<DefaultVirtual4>(), LINE_INFO());
			CheckDefaultVal2(*s1.DynamicCast<DefaultVirtual2>(), LINE_INFO());
			CheckDefaultVal2(*s1.DynamicCast<DefaultVirtual4>(), LINE_INFO());
			CheckDefaultVal3(*s1.DynamicCast<DefaultVirtual3>(), LINE_INFO());
			CheckDefaultVal3(*s1.DynamicCast<DefaultVirtual4>(), LINE_INFO());
			CheckDefaultVal4(*s1.DynamicCast<DefaultVirtual4>(), LINE_INFO());

			CheckDefaultVal1((DefaultVirtual1 &) s2, LINE_INFO());
			CheckDefaultVal1((DefaultVirtual2 &) s2, LINE_INFO());
			CheckDefaultVal1(*s2.DynamicCast<DefaultVirtual3>(), LINE_INFO());
			CheckDefaultVal1(*s2.DynamicCast<DefaultVirtual4>(), LINE_INFO());
			CheckDefaultVal2((DefaultVirtual2 &) s2, LINE_INFO());
			CheckDefaultVal2(*s2.DynamicCast<DefaultVirtual4>(), LINE_INFO());
			CheckDefaultVal3(*s2.DynamicCast<DefaultVirtual3>(), LINE_INFO());
			CheckDefaultVal3(*s2.DynamicCast<DefaultVirtual4>(), LINE_INFO());
			CheckDefaultVal4(*s2.DynamicCast<DefaultVirtual4>(), LINE_INFO());

			CheckDefaultVal1((DefaultVirtual1 &) s3, LINE_INFO());
			CheckDefaultVal1(*s3.DynamicCast<DefaultVirtual2>(), LINE_INFO());
			CheckDefaultVal1((DefaultVirtual3 &) s3, LINE_INFO());
			CheckDefaultVal1(*s3.DynamicCast<DefaultVirtual4>(), LINE_INFO());
			CheckDefaultVal2(*s3.DynamicCast<DefaultVirtual2>(), LINE_INFO());
			CheckDefaultVal2(*s3.DynamicCast<DefaultVirtual4>(), LINE_INFO());
			CheckDefaultVal3((DefaultVirtual3 &)s3, LINE_INFO());
			CheckDefaultVal3(*s3.DynamicCast<DefaultVirtual4>(), LINE_INFO());
			CheckDefaultVal4(*s3.DynamicCast<DefaultVirtual4>(), LINE_INFO());

			CheckDefaultVal1((DefaultVirtual1 &) s4, LINE_INFO());
			CheckDefaultVal1((DefaultVirtual2 &) s4, LINE_INFO());
			CheckDefaultVal1((DefaultVirtual3 &) s4, LINE_INFO());
			CheckDefaultVal1((DefaultVirtual4 &) s4, LINE_INFO());
			CheckDefaultVal2((DefaultVirtual2 &) s4, LINE_INFO());
			CheckDefaultVal2((DefaultVirtual4 &) s4, LINE_INFO());
			CheckDefaultVal3((DefaultVirtual3 &) s4, LINE_INFO());
			CheckDefaultVal3((DefaultVirtual4 &) s4, LINE_INFO());
			CheckDefaultVal4((DefaultVirtual4 &) s4, LINE_INFO());

		}

		TEST_METHOD(DynamicCast)
		{
			SafeObject<DefaultVirtual2> s2;
			SafeObject<DefaultVirtual3> s3;
			SafeObject<DefaultVirtual4> s4 = CONSTRUCT;

			s2 = s4;
			s3 = s2.DynamicCast<DefaultVirtual3>();
			CheckIsSame(s2, s4, LINE_INFO());
			CheckIsSame(s3, s4, LINE_INFO());
		}
		TEST_METHOD(InvalidDynamicCast)
		{
			SafeObject<DefaultVirtual2> s2 = CONSTRUCT;
			SafeObject<DefaultVirtual3> s3;

			s3 = s2.DynamicCast<DefaultVirtual3>();
			CheckNull(s3, LINE_INFO());
		}
		TEST_METHOD(CastToBool)
		{
			SafeObject<Default1> s1 = CONSTRUCT;
			SafeObject<Default1> s2;
			if (s1)
			{

			}
			if (!s1)
			{
				Assert::Fail(L"expected to be true", LINE_INFO());
			}
			if (s2)
			{
				Assert::Fail(L"expected to be false", LINE_INFO());
			}
		}
		TEST_METHOD(CompareNullptr)
		{
			SafeObject<Default1> s1 = CONSTRUCT;
			SafeObject<Default1> s2;
			if (s1 != nullptr) //checks compilation only
			{

			}
			if (s1 == nullptr)
			{
				Assert::Fail(L"expected to be true", LINE_INFO());
			}
			if (s2 != nullptr)
			{
				Assert::Fail(L"expected to be false", LINE_INFO());
			}
		}
	};
}
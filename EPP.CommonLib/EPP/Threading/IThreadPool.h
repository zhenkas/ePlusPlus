#pragma once

#include <EPP\Holders\SafeObject.h>
#include <EPP\Holders\SafeLambda.h>


namespace EPP::Threading
{
	using namespace EPP::Holders;

	struct IThreadPool : ISafeObject
	{
		enum EPosition
		{
			request,
			response,
			first,
		};
		virtual void EnqueueFunction(EPosition position, AutoSafeLambda<void(void)> safeLambda) = 0;
	};
}
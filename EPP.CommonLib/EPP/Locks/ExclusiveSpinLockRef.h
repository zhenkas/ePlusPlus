#pragma once
#include "Backoff.h"

namespace EPP::Locks
{
	struct ExclusiveSpinLockRef
	{
		static inline void ExclusiveLock(volatile long & lockState)
		{
			if (!_bittest((long *)&lockState,0) && InterlockedBitTestAndSet(&lockState, 0) == 0)
			{
				return;
			}
			_TryLockLoop(lockState);
		}
		static __declspec(noinline) void _TryLockLoop(volatile long & lockState)
		{
			uint32_t ctx;
			ctx = (uint32_t)(size_t) &ctx;
			while (true)
			{
				ctx = BackoffExp(ctx);
				if (InterlockedBitTestAndSet(&lockState, 0) == 0)
					break;
				SwitchToThread();
				if (!_bittest((long *) &lockState, 0) && InterlockedBitTestAndSet(&lockState, 0) == 0)
					break;
			};
		}
		static inline void ExclusiveRelease(volatile long & lockState)
		{
			_bittestandreset((long*)&lockState, 0);
			_ReadWriteBarrier();
		}
	};
}
#pragma once
#include "Backoff.h"

namespace EPP::Locks
{
	struct ExclusiveSpinLock
	{
		ExclusiveSpinLock() : m_lockState(0){}
		ExclusiveSpinLock(const ExclusiveSpinLock &) = delete;
		ExclusiveSpinLock(ExclusiveSpinLock &&) = delete;

		inline void ExclusiveLock()
		{
			if (m_lockState == 0 && InterlockedBitTestAndSet(&m_lockState, 0) == 0)
			{
				return;
			}
			_TryLockLoop();
		}
		__declspec(noinline) void _TryLockLoop()
		{
			uint32_t ctx;
			ctx = (uint32_t)(size_t) &ctx;
			while (true)
			{
				ctx = BackoffExp(ctx);
				if (InterlockedBitTestAndSet(&m_lockState, 0) == 0)
					break;
				SwitchToThread();
				if (m_lockState == 0 && InterlockedBitTestAndSet(&m_lockState, 0) == 0)
					break;
			};
		}
		inline void ExclusiveRelease()
		{
			m_lockState = 0;
			_ReadWriteBarrier();
		}
		volatile long m_lockState;
	};
}
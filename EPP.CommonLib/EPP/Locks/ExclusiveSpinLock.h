#pragma once
#include "Backoff.h"
#include "ExclusiveSpinLockRef.h"

namespace EPP::Locks
{
	struct ExclusiveSpinLock
	{
		ExclusiveSpinLock() : m_lockState(0){}
		ExclusiveSpinLock(const ExclusiveSpinLock &) = delete;
		ExclusiveSpinLock(ExclusiveSpinLock &&) = delete;
		ExclusiveSpinLock & operator=(const ExclusiveSpinLock &) = delete;
		ExclusiveSpinLock & operator=(ExclusiveSpinLock &&) = delete;

		inline void ExclusiveLock()
		{
			ExclusiveSpinLockRef::ExclusiveLock(m_lockState);
		}
		inline void ExclusiveRelease()
		{
			ExclusiveSpinLockRef::ExclusiveRelease(m_lockState);
		}
		volatile long m_lockState;
	};
}
#pragma once
#include <windows.h>

namespace EPP::Locks
{
	struct ManualEvent
	{
		ManualEvent(bool isSignaled = false) : m_event(CreateEvent(NULL, TRUE, isSignaled, NULL))
		{}
		~ManualEvent()
		{
			CloseHandle(m_event);
		}
		bool Wait(DWORD timeInMilliseconds = INFINITE)
		{
			return WaitForSingleObject(m_event, timeInMilliseconds) == WAIT_OBJECT_0;
		}
		void Set()
		{
			SetEvent(m_event);
		}
		void Reset()
		{
			ResetEvent(m_event);
		}
	protected:
		HANDLE m_event;
	};
}

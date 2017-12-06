#pragma once
#include <windows.h>


namespace EPP::Locks
{
	struct AutoEvent
	{
		AutoEvent(bool isSignaled) : m_event(CreateEvent(NULL, FALSE, isSignaled, NULL))
		{}
		~AutoEvent()
		{
			CloseHandle(m_event);
		}
		bool Wait(DWORD timeInMilliseconds = INFINITE)
		{
			WaitForSingleObject(m_event, timeInMilliseconds);
		}
		void Set()
		{
			SetEvent(m_event);
		}
	protected:
		HANDLE m_event;
	};
}
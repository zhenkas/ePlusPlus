#pragma once
#include <stdint.h>
#include <chrono>
#include "TimeDuration.h"

namespace EPP::Time
{
	struct DateTime
	{
		template<class _Clock, class _Duration>
		inline DateTime(std::chrono::time_point<_Clock, _Duration> timePoint):m_time(timePoint)	{}
		DateTime() {}
	public:
		static DateTime Now()
		{
			return DateTime(std::chrono::steady_clock::now());
		}
	public:
		TimeDuration operator-(DateTime dt)
		{
			return TimeDuration(m_time - dt.m_time);
		}
	protected:
		std::chrono::steady_clock::time_point m_time;
	};
}

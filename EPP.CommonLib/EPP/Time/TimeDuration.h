#pragma once
#include <chrono>

namespace EPP::Time
{
	struct TimeDuration
	{
		template<class _Rep, class _Period>
		inline TimeDuration(std::chrono::duration<_Rep, _Period> timeDuration) :m_duration(timeDuration) {}
		TimeDuration() {}
	public:
		long long ToMilliSeconds()
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(m_duration).count();
		}
		long long ToSeconds()
		{
			return std::chrono::duration_cast<std::chrono::seconds>(m_duration).count();
		}
		long long ToMinutes()
		{
			return std::chrono::duration_cast<std::chrono::minutes>(m_duration).count();
		}
		long long ToHours()
		{
			return std::chrono::duration_cast<std::chrono::hours>(m_duration).count();
		}
		auto GetChrono()
		{
			return m_duration;
		}
		template<class _Rep, class _Period>
		operator std::chrono::duration<_Rep, _Period>() { return m_duration;  }
	public:
		static TimeDuration FromMilliSeconds(long long milliSeconds)
		{
			return TimeDuration(std::chrono::milliseconds(milliSeconds));
		}
		static TimeDuration FromSeconds(long long seconds)
		{
			return TimeDuration(std::chrono::seconds(seconds));
		}
		static TimeDuration FromMinutes(long long minutes)
		{
			return TimeDuration(std::chrono::minutes(minutes));
		}
		static TimeDuration FromHours(long long hours)
		{
			return TimeDuration(std::chrono::hours(hours));
		}
		
	protected:
		std::chrono::steady_clock::duration m_duration;
	};
}


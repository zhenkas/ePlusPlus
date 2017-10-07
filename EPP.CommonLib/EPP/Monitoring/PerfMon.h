#pragma once
#include <string>

namespace EPP::Monitoring
{

	class PerfMon
	{
	public:
		PerfMon(std::string && name);
		~PerfMon();
		void CalcAndPrint();
		uint32_t Elapsed();
	protected:
		uint32_t m_ticks;
		std::string m_name;
		bool m_isPrinted;
	};

}


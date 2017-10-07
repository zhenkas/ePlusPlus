#include "stdafx.h"
#include "PerfMon.h"

namespace EPP::Monitoring
{

	PerfMon::PerfMon(std::string && name)
	{
		m_name = std::move(name);
		m_ticks = GetTickCount();
		m_isPrinted = false;
	}


	PerfMon::~PerfMon()
	{
		CalcAndPrint();
	}

	void PerfMon::CalcAndPrint()
	{
		if (!m_isPrinted) {
			printf("%s = %d milliseconds\n", m_name.c_str(), Elapsed());
			m_isPrinted = true;
		}
	}

	uint32_t PerfMon::Elapsed()
	{
		return GetTickCount() - m_ticks;
	}
}

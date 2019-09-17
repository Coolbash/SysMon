
#include "framework.h"
#include "sensor_mem.h"

//---------------------------------------------------------------
void CSensorMemory::read()
{
	MEMORYSTATUSEX mem = { sizeof(mem) };
	if (GlobalMemoryStatusEx(&mem))
	{
		m_total		= mem.ullTotalPhys;
		m_available	= mem.ullAvailPhys;
		m_used		= m_total - m_available;
		m_value		= double(m_used) / m_total;
		m_valid		= true;
	}else
		m_valid=false;
}
//---------------------------------------------------------------
//---------------------------------------------------------------

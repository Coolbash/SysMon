
#include "framework.h"
#include "sensor_CPU.h"

//---------------------------------------------------------------
LPCTSTR	CSensorCPU::name()
{
	return _T("CPU utilization");
}
//---------------------------------------------------------------
CSensorCPU::~CSensorCPU()
{
	if (m_pCPU_idle)
		delete(m_pCPU_idle), m_pCPU_idle = nullptr;

	if (m_pCPU_idle_prev)
		delete(m_pCPU_idle_prev), m_pCPU_idle_prev = nullptr;

	if (m_pCPU_utilization)
		delete(m_pCPU_utilization),	m_pCPU_utilization = nullptr;

	m_CPU_idle_size = 0;
}
//---------------------------------------------------------------
const bool CSensorCPU::init()
{
	QueryIdleProcessorCycleTime(&m_CPU_idle_size, nullptr);
	if (m_CPU_idle_size)
	{
		m_nCPU_cores = m_CPU_idle_size / sizeof ULONG64;
		m_pCPU_idle = reinterpret_cast<PULONG64>(new char[m_CPU_idle_size]);
		m_pCPU_idle_prev = reinterpret_cast<PULONG64>(new char[m_CPU_idle_size]);
		m_pCPU_utilization = new double[m_nCPU_cores];
		if (m_pCPU_idle && m_pCPU_idle_prev && m_pCPU_utilization)
		{
			m_CPU_ticks = __rdtsc();
			return QueryIdleProcessorCycleTime(&m_CPU_idle_size, m_pCPU_idle_prev);
		}
	}
	return false;
}
//---------------------------------------------------------------
void CSensorCPU::read()
{
	ULONG64 CPU_ticks_cur = __rdtsc();
	if (QueryIdleProcessorCycleTime(&m_CPU_idle_size, m_pCPU_idle))
	{
		const LONG64 CPU_ticks_since = CPU_ticks_cur - m_CPU_ticks;
		m_CPU_ticks = CPU_ticks_cur;
		for (int core = 0; core < m_nCPU_cores; core++)
		{
			const LONG64 idle_dif = m_pCPU_idle[core] - m_pCPU_idle_prev[core];
			m_pCPU_utilization[core] = double(CPU_ticks_since - idle_dif) / CPU_ticks_since;
		}
		memcpy(m_pCPU_idle_prev, m_pCPU_idle, m_CPU_idle_size);

		double result = 0;
			for (int core = 0; core < m_nCPU_cores; core++)
				result += m_pCPU_utilization[core];
		m_value = result / m_nCPU_cores;

		m_valid = true;
	}
	else
		m_valid=false;
}
//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------

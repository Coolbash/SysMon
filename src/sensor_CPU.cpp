#include "framework.h"
#include "sensor_CPU.h"

//---------------------------------------------------------------
const bool CSensorCPU::init()
{
	QueryIdleProcessorCycleTime(&m_CPU_idle_size, nullptr);
	if (m_CPU_idle_size)
	{
		const auto nCores = m_CPU_idle_size / sizeof ULONG64;
		m_cores.reserve(nCores);
		if(m_cores.capacity() >= nCores)
		{
			m_cores.resize(nCores);
			if(m_pCPU_idle = std::make_unique<ULONG64[]>(m_cores.size()))
			{
				m_CPU_ticks = __rdtsc();
				if (QueryIdleProcessorCycleTime(&m_CPU_idle_size, m_pCPU_idle.get()))
				{
					ULONG64* pCur{ m_pCPU_idle.get() };
					for (auto& core : m_cores)
						core.m_idle_prev = *(pCur++);
					return true;
				}
			}
		}
	}
	return false;
}
//---------------------------------------------------------------
void CSensorCPU::read()
{
	ULONG64 CPU_ticks_cur = __rdtsc();
	if (QueryIdleProcessorCycleTime(&m_CPU_idle_size, m_pCPU_idle.get()))
	{
		const auto CPU_ticks_since{ CPU_ticks_cur - m_CPU_ticks };
		m_CPU_ticks = CPU_ticks_cur;
		double result{ 0 };
		ULONG64* pCur{ m_pCPU_idle.get() };
		for(auto &core: m_cores)
		{
			const auto cur = *pCur++;
			const auto idle_dif = cur - core.m_idle_prev;
			core.m_idle_prev = cur;
			result += core.m_utilization = double(CPU_ticks_since - idle_dif) / CPU_ticks_since;
		}
		m_value = result / m_cores.size();
		m_valid = true;
	}
	else
		m_valid=false;
}
//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------

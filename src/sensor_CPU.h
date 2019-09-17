#pragma once
#include "sensor.h"
#include <vector>
#include <memory>
//---------------------------------------------------------------
class CViewCPU;
class CSensorCPU : public CSensor
{
public:
	using viewer_type = CViewCPU;
	struct core_t
	{
		ULONG64 m_idle_prev = 0;
		double	m_utilization =0;
	};
	using cores_t = std::vector<core_t>;

	CSensorCPU():CSensor{ _T("CPU utilization") }{};
	const bool		init();
	virtual void	read() override;
	const cores_t	data() const { return m_cores; }
	const int		cores() const { return m_cores.size(); };	///< returns CPU cores quantity

private:
	cores_t		m_cores;
	std::unique_ptr<ULONG64[]> m_pCPU_idle;
	ULONG		m_CPU_idle_size = 0;	
	ULONG64		m_CPU_ticks = 0;		///< CPU tick counter in time of last read
};

//---------------------------------------------------------------

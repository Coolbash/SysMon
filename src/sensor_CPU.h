#pragma once
#include "sensor.h"
//---------------------------------------------------------------
class CSensorCPU : public CSensor
{
public:
	~CSensorCPU();
	const bool		init();
	virtual void	read() override;
	const int		cores() const { return m_nCPU_cores; };	///return CPU cores quantity
	const double*	value_by_cores() const { return m_pCPU_utilization; };///returns CPU utilization for each core
	virtual LPCTSTR	name() override;			///name of the sensor (for notification)

private:
	int			m_nCPU_cores = 0;		///quantity of CPU cores
	ULONG64*	m_pCPU_idle = nullptr;
	ULONG		m_CPU_idle_size = 0;	
	ULONG64		m_CPU_ticks = 0;		///CPU tick counter in time of last read
	ULONG64*	m_pCPU_idle_prev = nullptr;
	double*		m_pCPU_utilization = nullptr;

};

//---------------------------------------------------------------

#pragma once
#include "sensor.h"

class CViewMem;
//---------------------------------------------------------------
class CSensorMemory : public CSensor
{
public:
	using viewer_type = CViewMem;
	CSensorMemory():CSensor{ _T("memory utilization")}{}
	virtual void	read() override;
	const ULONG64	total() const { return m_total; };	///total ammount of physical memory in GiBytes
	const ULONG64	used() const { return m_used; }; ///ammount of used physical memory in GiBytes
	const ULONG64	available() const { return m_available; }; ///ammount of available physical memory in GiBytes
protected:
	ULONG64	m_total=0;
	ULONG64	m_used=0;
	ULONG64	m_available=0;
};
//---------------------------------------------------------------


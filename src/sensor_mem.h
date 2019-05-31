#pragma once
#include "sensor.h"

//---------------------------------------------------------------
class CSensorMemory : public CSensor
{
public:
	virtual void	read() override;
	const ULONG64	total() const { return m_total; };	///total ammount of physical memory in GiBytes
	const ULONG64	used() const { return m_used; }; ///ammount of used physical memory in GiBytes
	const ULONG64	available() const { return m_available; }; ///ammount of available physical memory in GiBytes
	virtual LPCTSTR	name() override;			///name of the sensor (for notification)
protected:
	ULONG64	m_total;
	ULONG64	m_used;
	ULONG64	m_available;
};
//---------------------------------------------------------------


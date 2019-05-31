#pragma once
#include "sensor_mem.h"
#include <atlstr.h>
//---------------------------------------------------------------
class CSensorDisk : public CSensorMemory
{
public:
	virtual void	read() override;
	const bool		init(CString szDir);
	LPCTSTR			dir() const { return m_szDir.GetString(); };
	virtual LPCTSTR	name() override;			///name of the sensor (for notification)
private:
	CString			m_szDir;
	CString			m_name;
};
//---------------------------------------------------------------

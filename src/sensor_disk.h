#pragma once
#include "sensor_mem.h"
//---------------------------------------------------------------
class CViewDisk;
class CSensorDisk : public CSensorMemory
{
public:
	using viewer_type = CViewDisk;
	virtual void	read() override;
	const bool		init(tstring szDir);
	const tstring	dir() const { return m_szDir; };
private:
	tstring			m_szDir;
};
//---------------------------------------------------------------

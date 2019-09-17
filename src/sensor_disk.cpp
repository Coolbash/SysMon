
#include "framework.h"
#include "sensor_disk.h"
//---------------------------------------------------------------
//---------------------------------------------------------------
const bool CSensorDisk::init(tstring szDir)
{
	m_szDir = szDir;
	m_name = _T("Disk usage for \"") + szDir + TCHAR('"');
	read();
	return m_valid;
}

//---------------------------------------------------------------
void CSensorDisk::read()
{
	ULARGE_INTEGER FreeBytesAvailableToCaller;
	ULARGE_INTEGER TotalNumberOfBytes;
	ULARGE_INTEGER TotalNumberOfFreeBytes;
	if (GetDiskFreeSpaceEx(m_szDir.c_str(), &FreeBytesAvailableToCaller, &TotalNumberOfBytes, &TotalNumberOfFreeBytes))
	{
		m_total		= TotalNumberOfBytes.QuadPart;
		m_used		= (TotalNumberOfBytes.QuadPart - TotalNumberOfFreeBytes.QuadPart);
		m_available	= FreeBytesAvailableToCaller.QuadPart;
		m_value		= double(m_used) / m_total;
		m_valid		= true;
	}else
		m_valid = false;
}
//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------

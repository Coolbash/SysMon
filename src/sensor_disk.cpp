
#include "framework.h"
#include "sensor_disk.h"
//---------------------------------------------------------------
LPCTSTR	CSensorDisk::name()
{
	return m_name.GetString();
}
//---------------------------------------------------------------
//---------------------------------------------------------------
const bool CSensorDisk::init(CString szDir)
{
	m_szDir = szDir;
	m_name.Format(_T("Disk usage for \"%s\""),szDir);
	read();
	return m_valid;
}

//---------------------------------------------------------------
void CSensorDisk::read()
{
	ULARGE_INTEGER FreeBytesAvailableToCaller;
	ULARGE_INTEGER TotalNumberOfBytes;
	ULARGE_INTEGER TotalNumberOfFreeBytes;
	if (GetDiskFreeSpaceEx(m_szDir, &FreeBytesAvailableToCaller, &TotalNumberOfBytes, &TotalNumberOfFreeBytes))
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

#pragma once
#include "sensor_CPU.h"
#include "sensor_disk.h"
#include "sensor_network.h"
#include "SysMonView.h"
#include <atlstr.h>
#include <thread>
#include <vector>

//---------------------------------------------------------------
class	CiniReader;
//---------------------------------------------------------------
class CsensorManager
{
public:
	~CsensorManager();
	bool	init(CclientViewer* viewer);
	void	done();

private:
	CclientViewer*				m_pViewer=nullptr;
	bool						m_bRun = false;
	std::vector<CSensor*>		m_sensors;
	std::thread					m_thread_worker;	//a background working thread
	std::thread					m_thread_message;
	CString						m_szNotification;
	DWORD						m_inteval=1000;

	void						read_ini_section(CiniReader& reader);
	bool						add_sensor_CPU(CiniReader& reader);
	bool						add_sensor_mem(CiniReader& reader);
	bool						add_sensor_disk(CiniReader& reader);
	bool						add_sensor_network(CiniReader& reader);
	bool						init_default();
	bool						parse_ini();
	void						thread_worker();
	void						thread_message();
	void						show_notification();
	void						close_notification();
};
//---------------------------------------------------------------
//---------------------------------------------------------------


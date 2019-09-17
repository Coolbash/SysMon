#pragma once
#include "sensor_CPU.h"
#include "sensor_disk.h"
#include "sensor_network.h"
#include "SysMonView.h"
#include <thread>
#include <vector>
#include <memory>

//---------------------------------------------------------------
class	CiniReader;
//---------------------------------------------------------------
class CsensorManager	///class that incapsulates and checks all sensors adn shows notifications
{
public:
	~CsensorManager();
	bool	init(CclientViewer* viewer);	///reads config file and creates all sensors
	void	done();	///stops all threads

private:
	CclientViewer*				m_pViewer=nullptr;		///a viewer to draw sensor data
	bool						m_bRun = false;
	std::vector<std::unique_ptr<CSensor>>		m_sensors;
	std::thread					m_thread_worker;	///a background working thread that checks all sensors and shows notifications
	std::thread					m_thread_message;	///different thread for notification window
	tstring						m_szNotification;	///user notification text
	DWORD						m_inteval_ms=1000;	///interval of sensor checking in milliseconds

	void						read_ini_section(CiniReader& reader);
	bool						add_sensor_CPU(CiniReader& reader);
	bool						add_sensor_mem(CiniReader& reader);
	bool						add_sensor_disk(CiniReader& reader);
	bool						add_sensor_network(CiniReader& reader);
	bool						init_default();	///initializes default sensors set if there is no config file
	bool						parse_ini();	///reads config file
	void						thread_worker();///a background working thread that checks all sensors and shows notifications
	void						thread_message();///different thread for notification window
	void						show_notification();
	void						close_notification();
};
//---------------------------------------------------------------
//---------------------------------------------------------------


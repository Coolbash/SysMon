#include "framework.h"
#include "sensorManager.h"
#include <string>
#include <fstream>
#include <algorithm>

//---------------------------------------------------------------
class CiniReader : public std::ifstream		///small utility class for reading ini-file liny-by-line
{
	std::string line;	///current line
public:
	std::string name;	///name of the current line
	std::string value;  ///value of the current line
	bool section = false; ///current line is a section. 
	//--------------------------
	CiniReader(LPCSTR name) :std::ifstream(name) {};//constructor
	//--------------------------
	bool get_next_line()
	{
		while (std::getline(*this, line))
		{
			line.erase(std::remove_if(line.begin(), line.end(), isspace),line.end());
			if (line.empty() || line[0] == ';' || line[0] == '#') 
				continue;
			if (line[0] == '[' && line[line.size() - 1] == ']')
				name = std::string(line.begin() + 1, line.end() - 1), section = true;
			else
			{
				auto delimiterPos = line.find("=");
				name = line.substr(0, delimiterPos);
				value = line.substr(delimiterPos + 1);
				section = false;
			}
			return true;
		}
		return false;
	}
};
//---------------------------------------------------------------
LPCSTR ini_interval			= "interval_ms";
LPCSTR ini_sensor_heigh		= "sensor_heigh_px";
LPCSTR ini_sensor_weight	= "sensor_weight_px";
LPCSTR ini_threshold_warning= "threshold_warning";
LPCSTR ini_threshold_error	= "threshold_error";
LPCSTR ini_directory		= "directory";
LPCSTR ini_CPU				= "CPU";
LPCSTR ini_memory			= "memory";
LPCSTR ini_disk				= "disk";
LPCSTR ini_network			= "network";

//---------------------------------------------------------------

bool CsensorManager::parse_ini()
{
	CiniReader reader("SysMon.ini");
	while (reader.get_next_line())
	{
		while (reader.section)
			read_ini_section(reader);

		if (reader.name == ini_interval)
			m_inteval_ms = std::stoi(reader.value);
		else
			if (reader.name == ini_sensor_heigh)
				m_pViewer->m_sensor_cy = std::stoi(reader.value);
			else 
				if (reader.name == ini_sensor_weight)
					m_pViewer->m_sensor_cx = std::stoi(reader.value);
	}
	return m_sensors.size();
}
//---------------------------------------------------------------
void	CsensorManager::read_ini_section(CiniReader& reader)
{
	if (reader.name == ini_CPU)
		add_sensor_CPU(reader);
	else
		if (reader.name == ini_memory)
			add_sensor_mem(reader);
		else
			if (reader.name == ini_disk)
				add_sensor_disk(reader);
			else
				if (reader.name == ini_network)
					add_sensor_network(reader);
				else
				{
					//unknown ini_section
					CString szMsg;
					szMsg.Format(_T("Unknown section \"%S\"in config file"), reader.name.c_str());
					MessageBox(NULL, szMsg.GetString(), _T("Error"), MB_OK | MB_ICONSTOP);
					while (reader.get_next_line() && !reader.section);
				}

}
//---------------------------------------------------------------
bool	CsensorManager::add_sensor_CPU(CiniReader& reader)
{
	if (auto pSensor{ std::make_unique<CSensorCPU>() })
	{
		if (pSensor->init())
		{
			m_pViewer->add_sensor(pSensor.get());
			//reading sensor settings from config
			while (reader.get_next_line() && !reader.section)
			{
				if (reader.name == ini_threshold_error)
					pSensor->m_threshold_error = std::stoi(reader.value) / 100.;
				else
					if (reader.name == ini_threshold_warning)
						pSensor->m_threshold_warning = std::stoi(reader.value) / 100.;
			}
			m_sensors.push_back(std::move(pSensor));
			return true;
		};
	};
	return false;
}
//---------------------------------------------------------------
bool	CsensorManager::add_sensor_mem(CiniReader& reader)
{
	if (auto pSensor{ std::make_unique<CSensorMemory>() })
	{
		//reading sensor settings from config
		while (reader.get_next_line() && !reader.section)
		{
			if (reader.name == ini_threshold_error)
				pSensor->m_threshold_error = std::stoi(reader.value) / 100.;
			else
				if (reader.name == ini_threshold_warning)
					pSensor->m_threshold_warning = std::stoi(reader.value) / 100.;
		}
		m_pViewer->add_sensor(pSensor.get());
		m_sensors.push_back(std::move(pSensor));
		return true;
	};
	return false;
}
//---------------------------------------------------------------
bool	CsensorManager::add_sensor_disk(CiniReader& reader)
{
	if (auto pSensor{ std::make_unique <CSensorDisk>() })
	{
		m_pViewer->add_sensor(pSensor.get());
		while (reader.get_next_line() && !reader.section)
		{
			if (reader.name == ini_threshold_error)
				pSensor->m_threshold_error = std::stoi(reader.value) / 100.;
			else
				if (reader.name == ini_threshold_warning)
					pSensor->m_threshold_warning = std::stoi(reader.value) / 100.;
				else
					if (reader.name == ini_directory)
						pSensor->init(CString(reader.value.c_str()));
		}
		m_sensors.push_back(std::move(pSensor));
		return true;
	};
	return false;
}
//---------------------------------------------------------------
bool	CsensorManager::add_sensor_network(CiniReader& reader)
{
	if (auto pSensor{ std::make_unique <CSensorNetwork>() })
	{
		if (pSensor->init())
		{
			m_pViewer->add_sensor(pSensor.get());
			//reading sensor settings from config
			while (reader.get_next_line() && !reader.section)
			{
				if (reader.name == ini_threshold_error)
					pSensor->m_threshold_error = std::stoi(reader.value) / 100.;
				else
					if (reader.name == ini_threshold_warning)
						pSensor->m_threshold_warning = std::stoi(reader.value) / 100.;
				//it would be nice to read adapter LUID from config file
			}
			m_sensors.push_back(move(pSensor));
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------
bool CsensorManager::init(CclientViewer* viewer)
{
	m_pViewer = viewer;

	if(parse_ini() || init_default())
	{
		m_bRun = true;
		m_thread_worker = std::thread(&CsensorManager::thread_worker, this);
		return m_thread_worker.joinable();
	}else
		return false;
}
//---------------------------------------------------------------
bool CsensorManager::init_default()
{
	if (auto pSensor{ std::make_unique<CSensorCPU>() })//CPU 
		if (pSensor->init())
		{
			m_pViewer->add_sensor(pSensor.get());
			m_sensors.push_back(std::move(pSensor));
		};
	
	if (auto pSensor{ std::make_unique<CSensorMemory>() })//memory 
	{
		m_pViewer->add_sensor(pSensor.get());
		m_sensors.push_back(std::move(pSensor));
	};

	if (auto pSensor{ std::make_unique<CSensorDisk>() })//disk
		if (pSensor->init(_T("c:\\")))
		{
			m_pViewer->add_sensor(pSensor.get());
			m_sensors.push_back(std::move(pSensor));
		}

	if (auto pSensor{ std::make_unique<CSensorNetwork>() })//network
		if (pSensor->init())
		{
			m_pViewer->add_sensor(pSensor.get());
			m_sensors.push_back(std::move(pSensor));
		}
	return true;
}
//---------------------------------------------------------------
CsensorManager::~CsensorManager()
{
	done();
}
//---------------------------------------------------------------

void CsensorManager::done()
{
	m_bRun = false;
	if (m_thread_worker.joinable())
		m_thread_worker.join();
	close_notification();
}
//---------------------------------------------------------------
void	CsensorManager::thread_worker()
{
	while (m_bRun)
	{
		bool threshold_state_changed = false;
		for (auto& sensor : m_sensors)//read all sensors
		{
			sensor->read();
			threshold_state_changed |= sensor->check_threshold();
		}

		if (threshold_state_changed)
			show_notification();

		if (m_pViewer)
			m_pViewer->update();
		Sleep(m_inteval_ms);
	}
}

//---------------------------------------------------------------
LPCTSTR		szNotificationCaption = _T("threshold exceeded");
//---------------------------------------------------------------
void CsensorManager::thread_message()
{
	bool confirmed = false;
	if (MessageBox(NULL, m_szNotification.GetString(), szNotificationCaption, MB_OKCANCEL | MB_ICONINFORMATION) == IDOK)
	{
		for (auto& sensor : m_sensors)//read all sensors
		{
			switch (sensor->threshold_state())
			{
			case threshold_error:
			case threshold_warning: 
				sensor->threshold_confirm();
			}
		}
	}
}
//---------------------------------------------------------------
void	CsensorManager::close_notification()
{
	HWND hwnd_msg = ::FindWindow(0, szNotificationCaption);
	if (hwnd_msg)
		::SendMessage(hwnd_msg, WM_CLOSE, 0, 0);
	if (m_thread_message.joinable())
		m_thread_message.join();
}
//---------------------------------------------------------------
void	CsensorManager::show_notification()
{
	close_notification();//closing notification if it was shown
	m_szNotification.Empty();
	for (auto& sensor : m_sensors)//read all sensors
	{
		switch (sensor->threshold_state())
		{
		case threshold_error	: 
			if(!sensor->is_threshold_confirmed()) 
				m_szNotification.AppendFormat(_T("sensor %s has exceeded error level\n"), sensor->name()); 
			break;
		case threshold_warning	: 
			if (!sensor->is_threshold_confirmed())
				m_szNotification.AppendFormat(_T("sensor %s has exceeded warning level\n"), sensor->name());
		}
	}
	if(!m_szNotification.IsEmpty())
		m_thread_message = std::thread(&CsensorManager::thread_message, this);
}
//---------------------------------------------------------------


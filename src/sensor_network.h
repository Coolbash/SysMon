#pragma once
#include "sensor.h"
#include <atlstr.h>
//---------------------------------------------------------------
class CSensorNetwork : public CSensor
{
public:
	const bool		init();
	void			init(ULONG64 network_adapter_luid) { m_network_adapter_luid = network_adapter_luid; init(); };
	virtual void	read() override;
	virtual LPCTSTR	name() override;			///name of the sensor (for notification)
	const double	max_bytes_transmit_in_period()	const { return (m_transmit_link_speed_bitPsec >> 3) * m_period_sec; };
	const double	max_bytes_receive_in_period()	const { return (m_receive_link_speed_bitPsec >> 3) * m_period_sec; };
	const double	utilization_transmit()			const { return m_transmit_in_period_byte / max_bytes_transmit_in_period(); };
	const double	utilization_receive()			const { return m_receive_in_period_byte / max_bytes_receive_in_period(); };
	const bool		is_link_simmetrical()			const { return m_transmit_link_speed_bitPsec == m_receive_link_speed_bitPsec; };
	const ULONG64	link_speed_transmit_bitPsec()	const { return  m_transmit_link_speed_bitPsec;	};
	const ULONG64	link_speed_receive_bitPsec()	const { return  m_receive_link_speed_bitPsec;	};
	const ULONG64	sent_since_byte()				const { return  m_transmit_in_period_byte;};
	const ULONG64	received_since_byte()			const { return  m_receive_in_period_byte;};
private:
	double			m_period_sec = 0;						///seconds bitween the last and the prevous readings.
	ULONG64			m_transmit_link_speed_bitPsec = 0;
	ULONG64			m_receive_link_speed_bitPsec = 0;
	LONG64			m_transmit_in_period_byte = 0;
	LONG64			m_receive_in_period_byte = 0;

	ULONG64			m_network_adapter_luid = 0;
	ULONG64			m_network_received = 0;
	ULONG64			m_network_transmitted = 0;
	LONGLONG		m_network_time_pctick = 0;	//time of last measure of network utilization in performance counter ticks
	double			m_counter_frequency = 0;	//Performance counter frequency
	CString			m_name;
};
//---------------------------------------------------------------
//---------------------------------------------------------------



#include "framework.h"
#include "sensor_network.h"
#include <algorithm>

//network traffic includes and libraries:
//#include <winsock2.h>
#include <ws2tcpip.h>
#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")


//---------------------------------------------------------------
LPCTSTR	CSensorNetwork::name()
{
	return _T("Network utilization");
}
//---------------------------------------------------------------
const bool CSensorNetwork::init()
{
	LARGE_INTEGER perf_frequency;
	if (QueryPerformanceFrequency(&perf_frequency))
		m_counter_frequency = double(perf_frequency.QuadPart);
	else return false;
	if (!QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&m_network_time_pctick)))
		return false;

	if(m_network_adapter_luid==0)
	{//enumerating network adapters and find the first hardware-type.
		PMIB_IF_TABLE2 table;
		if (NO_ERROR == GetIfTable2Ex(MibIfTableNormal, &table))
		{
			for (ULONG i = 0; i < table->NumEntries; i++)
			{
				MIB_IF_ROW2& ifRow = table->Table[i];
				if (ifRow.InterfaceAndOperStatusFlags.HardwareInterface &&
					ifRow.MediaConnectState == MediaConnectStateConnected &&
					ifRow.ReceiveLinkSpeed || ifRow.TransmitLinkSpeed)
					{
						m_network_adapter_luid	= ifRow.InterfaceLuid.Value;
						m_transmit_link_speed_bitPsec = ifRow.TransmitLinkSpeed;
						m_receive_link_speed_bitPsec	= ifRow.ReceiveLinkSpeed;
						m_network_received		= ifRow.InOctets;
						m_network_transmitted	= ifRow.OutOctets;
						break;
					}
			}
			FreeMibTable(table);
		}else return false;
	}else 
	{
		MIB_IF_ROW2 ifRow = { m_network_adapter_luid };
		if (NO_ERROR == GetIfEntry2(&ifRow))
		{
			m_network_adapter_luid = ifRow.InterfaceLuid.Value;
			m_transmit_link_speed_bitPsec = ifRow.TransmitLinkSpeed;
			m_receive_link_speed_bitPsec = ifRow.ReceiveLinkSpeed;
			m_network_received = ifRow.InOctets;
			m_network_transmitted = ifRow.OutOctets;
		}else
			return false;
	}

	return true;
}
//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------
void CSensorNetwork::read()
{
	MIB_IF_ROW2 ifRow = { m_network_adapter_luid };
	LARGE_INTEGER perf_counter;
	if (QueryPerformanceCounter(&perf_counter) && NO_ERROR == GetIfEntry2(&ifRow))
	{
		m_period_sec = (perf_counter.QuadPart - m_network_time_pctick) / m_counter_frequency;
		m_network_time_pctick = perf_counter.QuadPart;

		m_receive_in_period_byte	= ifRow.InOctets - m_network_received;
		m_network_received			= ifRow.InOctets;
		m_transmit_in_period_byte	= ifRow.OutOctets - m_network_transmitted;
		m_network_transmitted		= ifRow.OutOctets;
		m_value = std::max<>(utilization_transmit() , utilization_receive()); 
		m_valid = true;
	}else 
		m_valid = false;
}
//---------------------------------------------------------------
//---------------------------------------------------------------

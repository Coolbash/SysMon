
#include "framework.h"
#include "sensor_network.h"
#include <algorithm>

#include <ws2tcpip.h>
#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")

//---------------------------------------------------------------
const bool CSensorNetwork::init()
{
	//-----------------------
	auto init_from_ifRow = [this](MIB_IF_ROW2& ifRow)
	{ 
		m_network_adapter_luid = ifRow.InterfaceLuid.Value;
		m_transmit_link_speed_bitPsec = ifRow.TransmitLinkSpeed;
		m_receive_link_speed_bitPsec = ifRow.ReceiveLinkSpeed;
		m_network_received = ifRow.InOctets;
		m_network_transmitted = ifRow.OutOctets;
		tostringstream s;
		s << _T("Network utilization of adapter\n") << ifRow.Alias << _T("\n");
		m_name = s.str();
	};
	//-----------------------
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
					ifRow.ReceiveLinkSpeed && 
					ifRow.InOctets)

				{
					init_from_ifRow(ifRow);
					break;
				}
			}
			FreeMibTable(table);
		}else return false;
	}else 
	{
		MIB_IF_ROW2 ifRow = { m_network_adapter_luid };
		if (NO_ERROR == GetIfEntry2(&ifRow))
			init_from_ifRow(ifRow);
		else
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

#include "framework.h"
#include "SysMonView.h"
//---------------------------------------------------------------
const LONG margin = 5;
const LONG bar_weight = 10;
//---------------------------------------------------------------
tstring btots(LONG64 b)	///converts bytes to string with kb-Mb-Gb-Tb
{
	const LONG64 k = 1024;
	const LONG64 M = k*k;
	const LONG64 G = k*M;
	const LONG64 T = k*G;
	tostringstream s;
	if ( b <= k )
		s << b << _T(" b");
	else
		if ( b <= M )
			s << (b + k / 2)/k << _T(" kb");
		else
			if ( b <= G )
				s << (b + M / 2) / M << _T(" Mb");
			else
				if ( b <= T )
					s << (b + G / 2) / G << _T(" Gb");
				else
					s << (b + T / 2) / T << _T(" Tb");
	return s.str();
}
//---------------------------------------------------------------
void CBar::init(LONG left, LONG top, LONG cx, LONG cy, COLORREF color)
{
	done();//in case of sequentional initialization
	m_rect.left = left;
	m_rect.top = top;
	m_rect.right = left+cx;
	m_rect.bottom = top+cy;
	m_brush = CreateSolidBrush(color);
}
//---------------------------------------------------------------
void CBar::draw(HDC hdc, double value)	//background brush and outline pen must be selected in the hdc.
{
	Rectangle(hdc, m_rect.left, m_rect.top, m_rect.right, m_rect.bottom); //outline rectangle.
	RECT r;
	r.left		= m_rect.left  + 1;
	r.right		= m_rect.right - 1;
	r.bottom	= m_rect.bottom - 1;
	r.top		= r.bottom - LONG((m_rect.bottom - m_rect.top - 2) * value+.5);	//value rectangle
	FillRect(hdc, &r, m_brush);
}

//---------------------------------------------------------------
void CBar::done()
{
	if (m_brush)
		DeleteObject(m_brush);
}
//---------------------------------------------------------------
void CsensorViewer::setRect(LONG left, LONG top, LONG cx, LONG cy)
{ 
	m_rect.left		= left; 
	m_rect.top		= top; 
	m_rect.right	= left + cx; 
	m_rect.bottom	= top + cy; 

	m_rect_text.left	= m_rect.left	+ margin;
	m_rect_text.top		= m_rect.top	+ margin;
	m_rect_text.right	= m_rect.right	- margin;
	m_rect_text.bottom	= m_rect.bottom	- margin;
};
//---------------------------------------------------------------
bool CViewCPU::init()
{
	if (m_rect.right != 0)
	{
		LONG x = m_rect.left+margin;
		m_bars.resize(m_sensor.cores());
		for (auto& bar : m_bars)
		{
			bar.init(x+1, m_rect.top + margin, bar_weight, m_rect.bottom - m_rect.top - 2*margin, RGB(150, 255, 150));
			x = bar.m_rect.right;
		}
		m_rect_text.left = x + margin;
		return true;
	};
	return false;
}

//---------------------------------------------------------------
void CViewCPU::draw(HDC hdc)
{
	{
		auto bar = m_bars.begin();
		for (auto &core : m_sensor.data())
			(bar++)->draw(hdc, core.m_utilization);
	}

	tostringstream	s;
	s << _T("CPU ") << m_sensor.value_percents() << _T("%%");
	DrawText(hdc, s.str().c_str(), -1, &m_rect_text, DT_LEFT );
}
//---------------------------------------------------------------
bool CViewMem::init()
{
	if (m_rect.right != 0)
	{
		m_bar.init(m_rect.left + margin, m_rect.top + margin, bar_weight, m_rect.bottom - m_rect.top - 2 * margin, RGB(255, 150, 150));
		m_rect_text.left += bar_weight + margin;
		return true;
	}
	return false;
}

//---------------------------------------------------------------
void CViewMem::draw(HDC hdc)
{
	m_bar.draw(hdc, m_sensor.value());
	tstring	s{_T("Memory:\navailable ") + btots(m_sensor.available()) + _T(" of ") + btots(m_sensor.total()) };
	DrawText(hdc, s.c_str(), -1, &m_rect_text, DT_LEFT);
}
//---------------------------------------------------------------
bool CViewDisk::init()
{
	if (m_rect.right != 0)
	{
		m_bar.init(m_rect.left + margin, m_rect.top + margin, bar_weight, m_rect.bottom - m_rect.top - 2 * margin, RGB(150, 150, 255));
		m_rect_text.left += bar_weight + margin;
		return true;
	}
	return false;
}

//---------------------------------------------------------------
void CViewDisk::draw(HDC hdc)
{
	m_bar.draw(hdc, m_sensor.value());
	tstring s{ _T("Directory \"") + m_sensor.dir() + _T("\nused ") + btots(m_sensor.used()) +_T(" of ") + btots(m_sensor.total()) + _T("\navailable " )+ btots(m_sensor.available()) };
	DrawText(hdc, s.c_str(), -1, &m_rect_text, DT_LEFT);
}
//---------------------------------------------------------------

bool CViewNetwork::init()
{
	if (m_rect.right != 0)
	{
		m_bar_sent.		init(m_rect.left + margin, m_rect.top + margin, bar_weight, m_rect.bottom - m_rect.top - 2 * margin, RGB(255, 200, 150));
		m_bar_received.	init(m_bar_sent.m_rect.right + 1, m_rect.top + margin, bar_weight, m_rect.bottom - m_rect.top - 2 * margin, RGB(200, 255, 150));
		m_rect_text.left = m_bar_received.m_rect.right + margin;
		return true;
	}
	return false;
}

//---------------------------------------------------------------
void CViewNetwork::draw(HDC hdc)
{
	m_bar_sent.draw(hdc, m_sensor.utilization_transmit());
	m_bar_received.draw(hdc, m_sensor.utilization_receive());
	tstring szLinkSpeed;
	if (m_sensor.is_link_simmetrical())
		szLinkSpeed = btots(m_sensor.link_speed_receive_bitPsec()) + _T("it");
	else
		szLinkSpeed =_T("transmit ") + btots(m_sensor.link_speed_transmit_bitPsec()) +_T("it, receive ") + btots(m_sensor.link_speed_receive_bitPsec()) +_T("it");
	
	tstring s{_T("Network (") + szLinkSpeed +_T("):\nsent: ") + btots(m_sensor.sent_since_byte()) +_T("\nreceived: ") + btots(m_sensor.received_since_byte()) +_T("")};
	DrawText(hdc, s.c_str(), -1, &m_rect_text, DT_LEFT);
}
//---------------------------------------------------------------


//---------------------------------------------------------------
void CclientViewer::done()
{
	std::unique_lock lck{m_mutex};
	m_sensor_veiwers.clear();
}

//---------------------------------------------------------------
bool CclientViewer::init(HWND hWnd)
{
	std::unique_lock lck{ m_mutex };
	m_hWnd = hWnd;

	//positioning of elements
	int	cy = 0;
	for (auto& viewer : m_sensor_veiwers)
	{
		viewer->setRect(0, cy, m_sensor_cx, m_sensor_cy);
		cy += m_sensor_cy + 1;
		viewer->init();
	}

	RECT rect_window,rect_client;
	if(!GetWindowRect(hWnd, &rect_window)) return false;
	if(!GetClientRect(hWnd, &rect_client)) return false;
	int dif_cx = (rect_window.right - rect_window.left) - (rect_client.right - rect_client.left);
	int dif_cy = (rect_window.bottom - rect_window.top) - (rect_client.bottom- rect_client.top);
	SetWindowPos(hWnd, HWND_TOP, 0, 0, m_sensor_cx+dif_cx, cy+dif_cy, SWP_NOMOVE);

	return true;
}
//---------------------------------------------------------------
void CclientViewer::draw(HDC hdc)
{
	std::unique_lock lck{ m_mutex }; //might be called from different threads
	//erase background
	SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

	//draw all sensors
	for (auto& viewer : m_sensor_veiwers)
		viewer->draw(hdc);
}
//---------------------------------------------------------------
void CclientViewer::update()
{
	if (m_hWnd)
	{
		HDC hdc = GetDC(m_hWnd);
		if (hdc)
		{
			draw(hdc);
			ReleaseDC(m_hWnd, hdc);
		}
	}
	
}
//---------------------------------------------------------------


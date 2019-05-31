#include "framework.h"
#include "SysMonView.h"
#include <atlstr.h>
//---------------------------------------------------------------
#define LOCK(mutex)		std::unique_lock<decltype(mutex)> lck(mutex,std::defer_lock); lck.lock();
const LONG margin = 5;
const LONG bar_weight = 10;
//---------------------------------------------------------------
CString btos(LONG64 b)	///converts bytes to string with kb-Mb-Gb-Tb
{
	const LONG64 k = 1024;
	const LONG64 M = k*k;
	const LONG64 G = k*M;
	const LONG64 T = k*G;
	CString s;
	if (b <= k)
		s.Format(_T("%lld b"), b);
	else
		if (b <= M)
			s.Format(_T("%lld kb"), (b + k / 2)/k);
		else
			if (b <= G)
				s.Format(_T("%lld Mb"), (b + M / 2) / M);
			else
				if (b <= T)
					s.Format(_T("%lld Gb"), (b + G / 2) / G);
				else
					s.Format(_T("%lld Tb"), (b + T / 2) / T);
return s;
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
	for (int i = 0; i < m_sensor.cores(); i++)
		m_bars[i].draw(hdc, m_sensor.value_by_cores()[i]);
	CString		s;
	s.Format(_T("CPU %d%%"), m_sensor.value_percents());
	DrawText(hdc, s.GetString(), -1, &m_rect_text, DT_LEFT );
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
	CString		s;
	s.Format(_T("Memory:\navailable %s of %s"), 
		btos(m_sensor.available()).GetString(), 
		btos(m_sensor.total()).GetString());
	DrawText(hdc, s.GetString(), -1, &m_rect_text, DT_LEFT);
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
	CString		s;
	s.Format(_T("Directory \"%s\"\nused %s of %s\navailable %s"), 
		m_sensor.dir(), 
		btos(m_sensor.used()).GetString(), 
		btos(m_sensor.total()).GetString(), 
		btos(m_sensor.available()).GetString());
	DrawText(hdc, s.GetString(), -1, &m_rect_text, DT_LEFT);
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
	CString szLinkSpeed, s;
	if (m_sensor.is_link_simmetrical())
		szLinkSpeed.Format(_T("%sit"), btos(m_sensor.link_speed_receive_bitPsec()).GetString());
	else
		szLinkSpeed.Format(_T("transmit %sit, receive %sit"),
			btos(m_sensor.link_speed_transmit_bitPsec()).GetString(), 
			btos(m_sensor.link_speed_receive_bitPsec()).GetString());
	s.Format(_T("Network (%s):\nsent: %s\nreceived: %s"), 
		szLinkSpeed.GetString(), 
		btos(m_sensor.sent_since_byte()).GetString(), 
		btos(m_sensor.received_since_byte()).GetString());

	DrawText(hdc, s.GetString(), -1, &m_rect_text, DT_LEFT);
}
//---------------------------------------------------------------


//---------------------------------------------------------------
void CclientViewer::done()
{
	LOCK(m_mutex);
	for (auto& viewer : m_sensor_veiwers)
		delete viewer, viewer=nullptr;
	m_sensor_veiwers.clear();
}

//---------------------------------------------------------------
bool CclientViewer::init(HWND hWnd)
{
	LOCK(m_mutex);
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
	LOCK(m_mutex);	//might be called from different threads
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


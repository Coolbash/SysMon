#pragma once
#include <thread>
#include <memory>
#include <vector>
#include <mutex>
#include "sensor_CPU.h"
#include "sensor_disk.h"
#include "sensor_network.h"

//---------------------------------------------------------------
class CBar	///< simple vertical bar for showing percentage
{
public:
	~CBar() { done(); };
	void draw(HDC hdc, double value);	///< value must be floating point (0..1). Not percents. Background brush and outline pen must be selected in the DC.
	void init(LONG left, LONG top, LONG cx, LONG cy, COLORREF color);

	RECT		m_rect = { 0 };
private:
	HBRUSH		m_brush = NULL;
	void		done();
};
//---------------------------------------------------------------
class CsensorViewer	///< abstract parent for all sensor viewers
{
public:
	virtual			~CsensorViewer()=default;	///< destructor must be virtual for proper destructing all objects
	virtual void	draw(HDC hdc)=0;
	virtual bool	init() = 0;
	void			setRect(LONG left, LONG top, LONG cx, LONG cy);

protected:
	RECT		m_rect = { 0 };	///< a rect for whole viewer
	RECT		m_rect_text = { 0 }; ///< a rect for text
};
//---------------------------------------------------------------
class CViewCPU : public CsensorViewer	///< viewer for CPU-sensor
{
public:
	CViewCPU(const CSensorCPU& sensor) : m_sensor(sensor) {};	///sensor must be set in constructor
	virtual bool init()override;
	virtual void draw(HDC hdc)override;
private:
	const CSensorCPU& m_sensor;
	std::vector<CBar>		m_bars;
};
//---------------------------------------------------------------
class CViewMem : public CsensorViewer	///memory-sensor viewer
{
public:
	CViewMem(const CSensorMemory& sensor) : m_sensor(sensor) {};	///sensor must be set in constructor
	virtual bool init()override;
	virtual void draw(HDC hdc)override;

private:
	const CSensorMemory& m_sensor;
	CBar			m_bar;
};
//---------------------------------------------------------------
class CViewDisk : public CsensorViewer
{
public:
	CViewDisk(const CSensorDisk& sensor) : m_sensor(sensor) {};	///sensor must be set in constructor
	virtual bool init()override;
	virtual void draw(HDC hdc)override;

private:
	const CSensorDisk& m_sensor;
	CBar			m_bar;
};
//---------------------------------------------------------------
class CViewNetwork : public CsensorViewer
{
public:
	CViewNetwork(const CSensorNetwork& sensor) : m_sensor(sensor) {};	///sensor must be set in constructor
	virtual bool init()override;
	virtual void draw(HDC hdc)override;

private:
	const CSensorNetwork& m_sensor;
	CBar			m_bar_sent;
	CBar			m_bar_received;
};
//---------------------------------------------------------------
class CclientViewer	///class for incapsulating all sensor-viewers in client area
{
public:
	~CclientViewer() { done(); };
	bool	init(HWND hWnd);
	void	draw(HDC hdc);
	void	update();
	void	done();


//void	add_sensor(CSensorCPU* pSensor)
//{
//	m_sensor_veiwers.push_back(std::make_unique<CViewCPU>(*pSensor));
//};

//template<typename TSensor, typename TViewer>
//void	add_sensor(TSensor* pSensor)
//{
//	m_sensor_veiwers.push_back(std::make_unique<TViewer>(*pSensor));
//};



#define ADD_SENSOR(sensor_t, viewer_t) \
void	add_sensor(sensor_t* pSensor) \
{\
		m_sensor_veiwers.push_back(std::make_unique<viewer_t>(*pSensor));\
};
//---------------------------------------------------------------

	//void add_sensor(CSensorCPU)

	ADD_SENSOR(CSensorCPU, CViewCPU);
	ADD_SENSOR(CSensorMemory, CViewMem);
	ADD_SENSOR(CSensorDisk, CViewDisk);
	ADD_SENSOR(CSensorNetwork, CViewNetwork);

	int							m_sensor_cx = 200;
	int							m_sensor_cy = 50;

private:
	HWND						m_hWnd = NULL;				///a window to draw in
	bool						m_bRun = false;
	std::vector<std::unique_ptr<CsensorViewer>>	m_sensor_veiwers;
	std::mutex					m_mutex;
	

};
//---------------------------------------------------------------
//---------------------------------------------------------------


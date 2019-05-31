#pragma once

//---------------------------------------------------------------
enum threshold_state_t
{
	threshold_no,
	threshold_warning,
	threshold_error
};
//---------------------------------------------------------------
class CSensor		
{
public:
	virtual void			read() = 0;			///reads the actual system state and checks threholds
	virtual LPCTSTR			name() = 0;			///name of the sensor (for notification)
	const bool				is_valid() const { return m_valid; };
	const double			value() const { return m_value; };	///sensor value 
	const int				value_percents()const { return int(m_value * 100 + .5f); };	///sensor value in percents
	inline const bool		check_threshold();	///check if value exceeds any threshold and returns true if threshold state changes.
	const threshold_state_t threshold_state()const { return m_threshold_state; };
	void					set_thresholds(double warning, double error) { m_threshold_warning = warning; m_threshold_error = error; };
	void					threshold_confirm() { m_threshold_confirmed = true; }
	const bool				is_threshold_confirmed()const { return m_threshold_confirmed; };
	
	double					m_threshold_warning = 0;
	double					m_threshold_error = 0;

protected:
	double				m_value=0;
	bool				m_valid=false;
	bool				m_threshold_confirmed=false;
	threshold_state_t	m_threshold_state = threshold_no;
};
//---------------------------------------------------------------
const bool CSensor::check_threshold()
{
	threshold_state_t	threshold_state_new = threshold_no;
	if (m_value > m_threshold_error)
		threshold_state_new = threshold_error;
	else
		if (m_value > m_threshold_warning)
			threshold_state_new = threshold_warning;

	if (m_threshold_state == threshold_state_new)
		return false;
	else
	{
		m_threshold_confirmed = false;
		m_threshold_state = threshold_state_new;
		return true;
	}
}
//---------------------------------------------------------------


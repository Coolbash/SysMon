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
	const int				value_percents()const { return int(m_value * 100 + .5f); };	///sensor value in percents (from 0 to 100)
	inline const bool		check_threshold();	///check if value exceeds any threshold and returns true if threshold state changes.
	const threshold_state_t threshold_state()const { return m_threshold_state; };
	void					threshold_confirm() { m_threshold_confirmed = true; }
	const bool				is_threshold_confirmed()const { return m_threshold_confirmed; };
	
	double					m_threshold_warning = 0;///the level of warning threshold in fractional 0-to-1 format. works only in range 0< ... >=1 
	double					m_threshold_error = 0;///the level of error threshold in fractional 0-to-1 format. works only in range 0< ... >=1 

protected:
	double				m_value=0;	///the main value of sensor in fractional zero-to-1 format.
	bool				m_valid=false;	///true if m_value is valid.
	bool				m_threshold_confirmed=false;	///true if user confirmed threshold exceeding (there is no need to show it again till next threshold)
	threshold_state_t	m_threshold_state = threshold_no;
};
//---------------------------------------------------------------
const bool CSensor::check_threshold()
{
	threshold_state_t	threshold_state_new = threshold_no;
	if (m_threshold_error>0 && m_value > m_threshold_error)
		threshold_state_new = threshold_error;
	else
		if (m_threshold_warning>0 && m_value > m_threshold_warning)
			threshold_state_new = threshold_warning;

	if (m_threshold_state != threshold_state_new)
	{
		m_threshold_confirmed = false;
		m_threshold_state = threshold_state_new;
		return true;
	}
	return false;
}
//---------------------------------------------------------------


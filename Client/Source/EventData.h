#pragma once
#include "IEventData.h"

//////////////////////////////////////////////////////////////////////////
/// EXAMPLE EVENT DATA AND USED FOR TESTING
//////////////////////////////////////////////////////////////////////////
class TestEventData : public BaseEventData
{
private: 
	bool m_AssumingDirectControl;

public:
	static const IEventData::EventType sk_EventType;
	
	explicit TestEventData(bool p_AssumingControl) :
		m_AssumingDirectControl(p_AssumingControl)
	{
	}

	virtual const IEventData::EventType &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual IEventDataPtr copy(void) const override
	{
		return IEventDataPtr(new TestEventData(m_AssumingDirectControl));
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
		p_Out << m_AssumingDirectControl;
	}

	virtual const char *getName(void) const override
	{
		return "TestEvent";
	}

	/**
	* Used to get the event data. User defined function.
	*/
	bool directInterventionIsNecessary(void) const
	{
		return m_AssumingDirectControl;
	}
};

/**
* Unique identifier for event data type. This one is an example and for testing.
* E.g. IEventData::EventType UniqueEventDataName::sk_EventType(unique_hex);
*/
const IEventData::EventType TestEventData::sk_EventType(0x77dd2b3a);
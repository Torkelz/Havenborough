#pragma once
#include "IEventData.h"

#pragma region EXAMPLE READ THIS IF YOU DO NOT KNOW HOW TO CREATE AN EVENT
//////////////////////////////////////////////////////////////////////////
/// EXAMPLE EVENT DATA AND USED FOR TESTING
//////////////////////////////////////////////////////////////////////////
class TestEventData : public BaseEventData
{
private: 
	//Parameter for event, can be of any number
	bool m_AssumingDirectControl;

public:
	static const IEventData::Type sk_EventType;
	
	explicit TestEventData(bool p_AssumingControl) :
		m_AssumingDirectControl(p_AssumingControl)
	{
	}

	virtual const IEventData::Type &getEventType(void) const override
	{
		return sk_EventType;
	}

	virtual Ptr copy(void) const override
	{
		return Ptr(new TestEventData(m_AssumingDirectControl));
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
	* Can be of any number of functions.
	*/
	bool directInterventionIsNecessary(void) const
	{
		return m_AssumingDirectControl;
	}
};

/**
* Unique identifier for event data type. This one is an example and for testing.
* E.g. IEventData::Type UniqueEventDataName::sk_EventType(unique_hex);
*/
const IEventData::Type TestEventData::sk_EventType(0x77dd2b3a);
#pragma endregion

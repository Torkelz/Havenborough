#pragma once
#include "IEventData.h"

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

	bool directInterventionIsNecessary(void) const
	{
		return m_AssumingDirectControl;
	}
};

const IEventData::EventType TestEventData::sk_EventType(0x77dd2b3a);
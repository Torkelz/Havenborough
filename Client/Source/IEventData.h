#pragma once
#include <ostream>
#include <memory>

class IEventData
{
public:
	typedef std::shared_ptr<IEventData> IEventDataPtr;
	typedef unsigned long EventType;

	/**
	* Gets the type of event.
	* @return the type of event as unsigned long
	*/
	virtual const EventType &getEventType(void) const = 0;

	/**
	* Gets the time stamp of the event.
	* @return the time stamp
	*/
	virtual float getTimeStamp(void) const = 0;

	/**
	* Used for network. //TODO: Comment properly when implemented
	* @param p_Out the stream to be written to with the event data
	*/
	virtual void serialize(std::ostream &p_Out) const = 0;

	/**
	* Copies a shared pointer of event data.
	* @return deep copy of the pointer
	*/
	virtual IEventDataPtr copy(void) const = 0;

	/**
	* Gets the name of the event.
	* @return the name
	*/
	virtual const char *getName(void) const = 0;
};

class BaseEventData : public IEventData
{
private:
	const float m_TimeStamp;

public:
	explicit BaseEventData(const float p_TimeStamp = 0.0f) :
		m_TimeStamp(p_TimeStamp)
	{
	}
	
	virtual ~BaseEventData(void)
	{
	}

	virtual const EventType &getEventType(void) const = 0;

	float getTimeStamp(void) const
	{
		return m_TimeStamp;
	}

	virtual void serialize(std::ostream &p_Out) const override
	{
	}
};
#pragma once
#include "../Utilities/FastDelegate.h"
#include "IEventData.h"



class IEventManager
{
public:
	typedef fastdelegate::FastDelegate1<IEventData::IEventDataPtr> EventListenerDelegate;

	enum eConstants
	{
		kINFINITE = 0xffffffff
	};

	/**
	* Constructor.
	*/
	explicit IEventManager(void){}

	/**
	* Destructor.
	*/
	virtual ~IEventManager(void){}

	/**
	* Adds a function to be run when a specific event is triggered.
	*	Note, an exception is thrown if adding already existing combination of function and type.
	* @param p_EventDelegate 
	* @param p_Type the unique IEventData::EventType identifier to trigger the function
	*/
	virtual void addListener(const EventListenerDelegate &p_EventDelegate, const IEventData::EventType &p_Type) = 0;

	/**
	* Removes a function which is triggered on a specific event.
	* @param p_EventDelegate 
	* @param p_Type the unique IEventData::EventType identifier which trigger the function
	* @return true if function is removed, otherwise false
	*/
	virtual bool removeListener(const EventListenerDelegate &p_EventDelegate, const IEventData::EventType &p_Type) = 0;

	/**
	* 
	*/
	virtual bool triggerTriggerEvent(const IEventData::IEventDataPtr &p_Event) const = 0;

	/**
	* 
	*/
	virtual bool queueEvent(const IEventData::IEventDataPtr &p_Event) = 0;

	/**
	* 
	*/
	virtual bool abortEvent(const IEventData::EventType &p_Type, bool p_AllOfType = false) = 0;

	/**
	* 
	*/
	virtual bool tickUpdate(unsigned long p_MaxMS = kINFINITE) = 0;

protected:
	virtual unsigned long getTickCount(void) = 0;
};
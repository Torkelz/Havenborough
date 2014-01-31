#pragma once
#include "../3rd party/FastDelegate.h"
#include "IEventData.h"
#include <chrono>

/**
* Wrapper to a stored function pointer. The function to be bound MUST have return type void and 
* have solely IEventData::Ptr as parameter.
* E.g. to bind a member function: EventListenerDelegate variableName(&object, &className::memberFunction)
* E.g. to bind a static function: EventListenerDelegate variableName(&className::memberFunction)
*/
typedef fastdelegate::FastDelegate1<IEventData::Ptr> EventListenerDelegate;

class IEventManager
{
public:
	static const std::chrono::milliseconds m_MaxProcessTime;
	
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
	* @param p_EventDelegate (see comment for EventListenerDelegate in header)
	* @param p_Type the unique IEventData::Type identifier to trigger the function
	*/
	virtual void addListener(const EventListenerDelegate &p_EventDelegate, const IEventData::Type &p_Type) = 0;

	/**
	* Removes a function which is triggered on a specific event.
	* @param p_EventDelegate (see comment for EventListenerDelegate in header)
	* @param p_Type the unique IEventData::Type identifier which trigger the function
	* @return true if function is removed, otherwise false
	*/
	virtual bool removeListener(const EventListenerDelegate &p_EventDelegate, const IEventData::Type &p_Type) = 0;

	/**
	* Instantly trigger the functions linked to this event data
	* @param p_Event the data to be sent to the functions
	* @return true if the functions linked was processed, otherwise false
	*/
	virtual bool triggerTriggerEvent(const IEventData::Ptr &p_Event) const = 0;

	/**
	* Queue an event data to run when processEvents function is called
	*	Note, an exception is thrown if the queue goes out of bounds
	* @param p_Event the data to be sent to the functions
	* @return true if the event data was added to the queue, otherwise false
	*/
	virtual bool queueEvent(const IEventData::Ptr &p_Event) = 0;

	/**
	* Aborts an event data which have not yet been processed by processEvents function
	*	Note, an exception is thrown if the queue goes out of bounds
	* @param The event type to be aborted.
	* @param (true) Aborts all of the provided types, (false) aborts the first one in the queue
	* @return true if event type was successfully aborted
	*/
	virtual bool abortEvent(const IEventData::Type &p_Type, bool p_AllOfType = false) = 0;

	/**
	* Process all the events in the queue
	* @param p_MaxMS if provided sets a maximum time (ms) to process events
	* @return true if processed all events in queue, otherwise false
	*/
	virtual bool processEvents(std::chrono::milliseconds p_MaxMS = m_MaxProcessTime) = 0;
};
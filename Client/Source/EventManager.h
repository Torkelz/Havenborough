#pragma once
#include "IEventManager.h"
#include <list>
#include <map>

const unsigned int EVENTMANAGER_NUM_QUEUES = 2;

class EventManager : public IEventManager
{
private:
	typedef std::list<EventListenerDelegate> EventListenerList;
	typedef std::map<IEventData::Type, EventListenerList> EventListenerMap;
	typedef std::list<IEventData::Ptr> EventQueue;
	typedef std::chrono::high_resolution_clock Timer;

	EventListenerMap m_EventListeners;
	EventQueue m_Queues[EVENTMANAGER_NUM_QUEUES];
	int m_ActiveQueue;

public:
	explicit EventManager(void);
	virtual ~EventManager(void);

	virtual void addListener(const EventListenerDelegate &p_EventDelegate, const IEventData::Type &p_Type) override;

	virtual bool removeListener(const EventListenerDelegate &p_EventDelegate, const IEventData::Type &p_Type) override;

	virtual bool triggerTriggerEvent(const IEventData::Ptr &p_Event) const override;

	virtual bool queueEvent(const IEventData::Ptr &p_Event) override;

	virtual bool abortEvent(const IEventData::Type &p_Type, bool p_AllOfType = false) override;

	virtual bool processEvents(std::chrono::milliseconds p_MaxMS = m_MaxProcessTime) override;
};
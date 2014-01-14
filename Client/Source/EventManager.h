#pragma once
#include "IEventManager.h"
#include <list>
#include <map>

const unsigned int EVENTMANAGER_NUM_QUEUES = 2;

class EventManager : public IEventManager
{
	typedef std::list<EventListenerDelegate> EventListenerList;
	typedef std::map<IEventData::EventType, EventListenerList> EventListenerMap;
	typedef std::list<IEventData::IEventDataPtr> EventQueue;

private:
	EventListenerMap m_EventListeners;
	EventQueue m_Queues[EVENTMANAGER_NUM_QUEUES];
	int m_ActiveQueue;

	static EventManager *m_Instance;
public:
	explicit EventManager(void);
	virtual ~EventManager(void);

	static EventManager *getInstance(void);

	virtual void addListener(const EventListenerDelegate &p_EventDelegate, const IEventData::EventType &p_Type) override;

	virtual bool removeListener(const EventListenerDelegate &p_EventDelegate, const IEventData::EventType &p_Type) override;

	virtual bool triggerTriggerEvent(const IEventData::IEventDataPtr &p_Event) const override;

	virtual bool queueEvent(const IEventData::IEventDataPtr &p_Event) override;

	virtual bool abortEvent(const IEventData::EventType &p_Type, bool p_AllOfType = false) override;

	virtual bool tickUpdate(unsigned long p_MaxMS = kINFINITE) override;

	virtual unsigned long getTickCount(void) override;

private:
};
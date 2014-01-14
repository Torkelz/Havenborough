#include "EventManager.h"
#include "ClientExceptions.h"
#include "Logger.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

EventManager::EventManager() :
	IEventManager()
{
	m_ActiveQueue = 0;
}

EventManager::~EventManager(void)
{

}

void EventManager::addListener(const EventListenerDelegate &p_EventDelegate, const IEventData::EventType &p_Type)
{
	EventListenerList &eventListenerList = m_EventListeners[p_Type];
	for(auto it = eventListenerList.begin(); it != eventListenerList.end(); ++it)
	{
		if(p_EventDelegate == (*it))
		{
			throw EventException("Error when attempting to double-register a delegate", __LINE__, __FILE__);
		}
	}

	eventListenerList.push_back(p_EventDelegate);
}

bool EventManager::removeListener(const EventListenerDelegate &p_EventDelegate, const IEventData::EventType &p_Type)
{
	bool success = false;

	auto findIt = m_EventListeners.find(p_Type);

	if(findIt != m_EventListeners.end())
	{
		EventListenerList &listeners = findIt->second;

		for(auto it = listeners.begin(); it != listeners.end(); ++it)
		{
			if(p_EventDelegate == (*it))
			{
				listeners.erase(it);
				success = true;
				break;
			}
		}
	}

	return success;
}

bool EventManager::triggerTriggerEvent(const IEventData::IEventDataPtr &p_Event) const 
{
	bool processed = false;
	auto findIt = m_EventListeners.find(p_Event->getEventType());

	if(findIt != m_EventListeners.end())
	{
		const EventListenerList &eventListenerList = findIt->second;
		for(EventListenerList::const_iterator it = eventListenerList.begin(); it != eventListenerList.end(); ++it)
		{
			EventListenerDelegate listener = (*it);

			listener(p_Event);
			processed = true;
		}
	}

	return processed;
}

bool EventManager::queueEvent(const IEventData::IEventDataPtr &p_Event)
{
	if((m_ActiveQueue >= 0 && m_ActiveQueue < EVENTMANAGER_NUM_QUEUES) == false)
		throw EventException("Error queue is out of bounds.", __LINE__, __FILE__);

	auto findIt = m_EventListeners.find(p_Event->getEventType());
	if(findIt != m_EventListeners.end())
	{
		m_Queues[m_ActiveQueue].push_back(p_Event);
		return true;
	}

	return false;
}

bool EventManager::abortEvent(const IEventData::EventType &p_Type, bool p_AllOfType /*= false*/)
{
	if((m_ActiveQueue >= 0 && m_ActiveQueue < EVENTMANAGER_NUM_QUEUES) == false)
		throw EventException("Error queue is out of bounds.", __LINE__, __FILE__);

	bool success = false;
	EventListenerMap::iterator findIt = m_EventListeners.find(p_Type);
	if(findIt != m_EventListeners.end())
	{
		EventQueue &eventQueue = m_Queues[m_ActiveQueue];
		auto it = eventQueue.begin();

		while(it != eventQueue.end())
		{
			auto thisIt = it;
			++it;

			if((*thisIt)->getEventType() == p_Type)
			{
				eventQueue.erase(thisIt);
				success = true;

				if(!p_AllOfType)
				{
					break;
				}
			}
		}
	}

	return success;
}

bool EventManager::tickUpdate(unsigned long p_MaxMS /*= kINFINITE*/)
{
	unsigned long currMs = getTickCount();
	unsigned long maxMs = ((p_MaxMS == IEventManager::kINFINITE) ?
		(IEventManager::kINFINITE) : (currMs + p_MaxMS));

	int queueToProcess = m_ActiveQueue;
	m_ActiveQueue = (m_ActiveQueue + 1) % EVENTMANAGER_NUM_QUEUES;
	m_Queues[m_ActiveQueue].clear();

	while(!m_Queues[queueToProcess].empty())
	{
		IEventData::IEventDataPtr event = m_Queues[queueToProcess].front();
		m_Queues[queueToProcess].pop_front();

		const IEventData::EventType &eventType = event->getEventType();
		auto findIt = m_EventListeners.find(eventType);

		if(findIt != m_EventListeners.end())
		{
			const EventListenerList &eventListeners = findIt->second;
			for(auto it = eventListeners.begin(); it != eventListeners.end(); ++it)
			{
				EventListenerDelegate listener = (*it);
				listener(event);
			}
		}

		currMs = getTickCount();
		if(p_MaxMS != IEventManager::kINFINITE && currMs >= maxMs)
		{
			Logger::log(Logger::Level::WARNING, "Aborting event processing, time ran out.");
			break;
		}
	}

	bool queueFlushed = (m_Queues[queueToProcess].empty());
	if(!queueFlushed)
	{
		while(!m_Queues[queueToProcess].empty())
		{
			IEventData::IEventDataPtr event = m_Queues[queueToProcess].back();
			m_Queues[queueToProcess].pop_back();
			m_Queues[m_ActiveQueue].push_front(event);
		}
	}

	return queueFlushed;
}

unsigned long EventManager::getTickCount(void)
{
	unsigned __int64 currTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);

	return (long)currTimeStamp;
}
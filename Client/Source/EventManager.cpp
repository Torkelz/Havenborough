#include "EventManager.h"
#include "ClientExceptions.h"
#include "Logger.h"

const std::chrono::milliseconds IEventManager::m_MaxProcessTime(std::numeric_limits<long long>::max());

EventManager::EventManager() :
	IEventManager()
{
	m_ActiveQueue = 0;
}

EventManager::~EventManager(void)
{

}

void EventManager::addListener(const EventListenerDelegate &p_EventDelegate, const IEventData::Type &p_Type)
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

bool EventManager::removeListener(const EventListenerDelegate &p_EventDelegate, const IEventData::Type &p_Type)
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

bool EventManager::triggerTriggerEvent(const IEventData::Ptr &p_Event) const 
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

bool EventManager::queueEvent(const IEventData::Ptr &p_Event)
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

bool EventManager::abortEvent(const IEventData::Type &p_Type, bool p_AllOfType /*= false*/)
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

bool EventManager::processEvents(std::chrono::milliseconds p_MaxMS /*= m_MaxProcessTime*/)
{
	Timer::time_point currTime = Timer::now(); //getCurrentTime();
	Timer::time_point stopTime = currTime + p_MaxMS;
	
	int queueToProcess = m_ActiveQueue;
	m_ActiveQueue = (m_ActiveQueue + 1) % EVENTMANAGER_NUM_QUEUES;
	m_Queues[m_ActiveQueue].clear();

	while(!m_Queues[queueToProcess].empty())
	{
		IEventData::Ptr event = m_Queues[queueToProcess].front();
		m_Queues[queueToProcess].pop_front();

		const IEventData::Type &eventType = event->getEventType();
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

		currTime = Timer::now(); //getCurrentTime();
		if(p_MaxMS != m_MaxProcessTime && currTime >= stopTime)
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
			IEventData::Ptr event = m_Queues[queueToProcess].back();
			m_Queues[queueToProcess].pop_back();
			m_Queues[m_ActiveQueue].push_front(event);
		}
	}

	return queueFlushed;
}
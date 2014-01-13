#include "ClientExceptions.h"

#include <functional>
#include <utility>    
#include <map>
#include <vector>
#include <stdarg.h>

template <typename Event>
class Subject
{
public:
	/**
	* Register an event-key to an observer. E.g. to bind a function with parameters to a key use:
	*	registerObserver(key, std::bind(foo, param, (...))). Note, function to be bound must be void.
	*/
	template <typename Observer>
	void registerObserver(const Event &p_Event, Observer &&p_Observer)
	{
		m_Observers[p_Event].push_back(std::forward<Observer>(p_Observer));
	}

	/**
	* Register an event-key to an observer. E.g. to bind a function with parameters to a key use:
	*	registerObserver(key, std::bind(foo, param, (...))). Note, function to be bound must be void.
	*/
	template <typename Observer>
	void registerObserver(Event &&p_Event, Observer &&p_Observer)
	{
		m_Observers[std::move(p_Event)].push_back(std::forward<Observer>(p_Observer));
	}

	/**
	* Notify all mapped observers to a specified key.
	* @param p_Event the key to which all mapped observers will be called
	*/
	void notify(const Event &p_Event) const
	{
		if(m_Observers.count(p_Event) > 0)
		{
			for(const auto &obs : m_Observers.at(p_Event))
			{
				obs();
			}
		}
		else
		{
			throw EventException("No such event exists.", __LINE__, __FILE__);
		}
	}


private:
	std::map<Event, std::vector<std::function<void()>>> m_Observers;
};
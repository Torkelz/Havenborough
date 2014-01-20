#pragma once

#include <IConnectionController.h>

#include <functional>
#include <memory>

class User
{
public:
	enum class State
	{
		CHANGING_STATE,
		LOBBY,
		WAITING_FOR_GAME,
		IN_GAME,
	};

	typedef std::function<void()> listenerCallback_t;
	typedef std::shared_ptr<User> ptr;
	typedef std::weak_ptr<User> wPtr;

private:
	State m_State;
	listenerCallback_t m_PackageListener;
	IConnectionController* m_Connection;

public:
	explicit User(IConnectionController* p_Connection);

	IConnectionController* getConnection();
};

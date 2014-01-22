#include "User.h"

User::User(IConnectionController* p_Connection)
	:	m_Connection(p_Connection),
		m_State(State::LOBBY)
{
}

IConnectionController* User::getConnection() const
{
	return m_Connection;
}

User::State User::getState() const
{
	return m_State;
}

void User::setState(State p_NewState)
{
	m_State = p_NewState;
}

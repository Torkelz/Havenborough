#include "User.h"

User::User(IConnectionController* p_Connection)
	:	m_Connection(p_Connection),
		m_State(State::LOBBY)
{
}

IConnectionController* User::getConnection()
{
	return m_Connection;
}

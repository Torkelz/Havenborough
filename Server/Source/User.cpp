#include "User.h"

User::User(IConnectionController* p_Connection)
	:	m_Connection(p_Connection),
		m_State(State::LOBBY),
		m_Username("UnknownUser"),
		m_CharacterName("UnknownCharacter")
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

const std::string& User::getUsername() const
{
	return m_Username;
}

void User::setUsername(const std::string& p_Username)
{
	m_Username = p_Username;
}

void User::setCharacterName(const std::string& p_Username)
{
	m_CharacterName = p_Username;
}

const std::string& User::getCharacterName() const
{
	return m_CharacterName;
}

const std::string& User::getCharacterStyle() const
{
	return m_CharacterStyle;
}

void User::setCharacterStyle(const std::string& p_Style)
{
	m_CharacterStyle = p_Style;
}

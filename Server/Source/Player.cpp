#include "Player.h"

Player::Player(User::wPtr p_User)
	:	m_User(p_User)
{
}

User::wPtr Player::getUser() const
{
	return m_User;
}

void Player::releaseUser()
{
	m_User.reset();
}

Actor::wPtr Player::getActor() const
{
	return m_Actor;
}

void Player::setActor(Actor::wPtr p_Actor)
{
	m_Actor = p_Actor;
}

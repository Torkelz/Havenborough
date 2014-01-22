#include "Player.h"

Player::Player(User::wPtr p_User)
	:	m_User(p_User)
{
}

User::wPtr Player::getUser() const
{
	return m_User;
}

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

void Player::addCheckpoint(const std::weak_ptr<Actor> p_Checkpoint)
{
	m_CheckpointSystem.addCheckpoint(p_Checkpoint);
}

BodyHandle Player::getCurrentCheckpointBodyHandle(void)
{
	return m_CheckpointSystem.getCurrentCheckpointBodyHandle();
}

Actor::Id Player::reachedFinishLine(void)
{
	return m_CheckpointSystem.reachedFinishLine();
}

Vector3 Player::changeCheckpoint()
{
	return m_CheckpointSystem.changeCheckpoint();
}
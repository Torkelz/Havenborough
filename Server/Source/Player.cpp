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

bool Player::reachedFinishLine(void)
{
	return m_CheckpointSystem.reachedFinishLine();
}

void Player::changeCheckpoint()
{
	m_CheckpointSystem.changeCheckpoint();
}

Actor::ptr Player::getCurrentCheckpoint(void)
{
	return m_CheckpointSystem.getCurrentCheckpoint();
}

Vector3 Player::getCurrentCheckpointColor()
{
	return m_CheckpointSystem.getCurrentCheckpointColor();
}
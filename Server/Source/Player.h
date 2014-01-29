#pragma once

#include "User.h"
#include "../../Client/Utilities/Util.h"

class Player
{
public:
	struct Box
	{
		uint16_t actorId;

		Vector3 position;
		Vector3 velocity;
		Vector3 rotation;
		Vector3 rotationVelocity;
	};
	Box m_PlayerBox;

private:
	User::wPtr m_User;

public:
	explicit Player(User::wPtr p_User);

	User::wPtr getUser() const;
	void releaseUser();
};

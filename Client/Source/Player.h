#pragma once
#include <DirectXMath.h>
#include "IPhysics.h"
#include "../Utilities/Util.h"

using namespace DirectX;

class Player
{
private:
	XMFLOAT3 m_Position;
	XMFLOAT3 m_LookDirection;
	float m_ViewRotation[2];
	BodyHandle m_PlayerBody;
	IPhysics* m_Physics;
	float m_JumpTime;
	bool m_IsJumping;
	float m_JumpForceTime;
	float m_JumpForce;
	float maxSpeed;
	float accConstant;
	XMFLOAT4 m_PrevForce;
	float m_DirectionX;
	float m_DirectionZ;

public:
	Player(void);
	~Player(void);
	void initialize(IPhysics *p_Physics, XMFLOAT3 p_Position, XMFLOAT3 p_LookDirection);
	
	XMFLOAT3 getPosition(void) const;
	BodyHandle getBody() const;
	void setJump();
	void setDirectionX(float p_DirectionX);
	void setDirectionZ(float p_DirectionZ);

	void update(float dt);


private:
	void jump(float dt);
	void move();
};


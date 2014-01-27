#pragma once
#include "Actor.h"
#include "Components.h"

#include <vector>
#include <memory>

class CheckpointSystem
{
private:
	std::vector<std::weak_ptr<Actor>> m_Checkpoints;
	Vector3 m_CurrentColorTone;
	Vector3 m_FinishColorTone;
	
public:
	/**
	*
	*/
	CheckpointSystem();
	CheckpointSystem(Vector3 p_CurrentTone, Vector3 p_FinishTone);
	~CheckpointSystem(void);

	/**
	* Add a checkpoint to the list of checkpoints on the level
	*/
	void addCheckpoint(const std::weak_ptr<Actor> p_Checkpoint);
	
	/**
	* 
	*/
	BodyHandle getCurrentCheckpointBodyHandle(void);

	/**
	* 
	*/
	bool isFinishLine(void);

	/**
	*
	*/
	void changeCheckpoint(void);

	/**
	*
	*/
	std::weak_ptr<Actor> getCurrentActor(void);
};
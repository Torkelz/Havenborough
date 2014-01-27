#pragma once
#include "Actor.h"
#include "Components.h"

#include <vector>
#include <memory>

class CheckpointSystem
{
private:
	std::vector<std::weak_ptr<Actor>> m_Checkpoints;
	Vector3 m_DefaultColorTone;
	Vector3 m_CurrentColorTone;
	Vector3 m_FinishColorTone;
	
public:
	/**
	* Constructor
	* @param p_DefaultTone the tone which inactive checkpoints should be shaded in, RGB 0.0f to 1.0f, default is white
	* @param p_CurrentTone the tone which the current checkpoint should be shaded in, RGB 0.0f to 1.0f, default is blue
	* @param p_FinishTone the tone which the finish line should be shaded in, RGB 0.0f to 1.0f, default is red
	*/
	CheckpointSystem(Vector3 p_DefaultTone = Vector3(1.0f, 1.0f, 1.0f), Vector3 p_CurrentTone = Vector3(0.2f, 0.2f, 0.9f),
		Vector3 p_FinishTone = Vector3(0.95f, 0.1f, 0.1f));
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
	void changeCheckpoint(std::vector<Actor::ptr> &p_Objects);

	/**
	*
	*/
	std::weak_ptr<Actor> getCurrentActor(void);
};
#pragma once
#include "Actor.h"

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
	CheckpointSystem(Vector3 p_DefaultTone = Vector3(1.0f, 1.0f, 1.0f), Vector3 p_CurrentTone = Vector3(0.027f, 0.231f, 0.376f),
		Vector3 p_FinishTone = Vector3(0.95f, 0.1f, 0.1f));
	
	/**
	* Destructor'
	*/
	~CheckpointSystem(void);

	/**
	* Adds a checkpoint to the vector of checkpoints.
	* @param p_Checkpoint Actor pointer 
	*/
	void addCheckpoint(const std::weak_ptr<Actor> p_Checkpoint);
	
	/**
	* Gets the current checkpoint's body handle.
	* @return the BodyHandle
	*/
	BodyHandle getCurrentCheckpointBodyHandle(void);

	/**
     * Checks if the finish line been reached.
     * @return true if finish line been reached, false if not.
     */
    bool reachedFinishLine(void) const;

	/**
	* Checks if the finish line been reached.
	* @return true if finish line been reached, false if not.
	*/
	Actor::ptr getCurrentCheckpoint(void);

	/**
	 * Remove the last item in the vector of checkpoints and set the next one.
	 */
	void changeCheckpoint();

	/**
	* Returns the next checkpoint in the vector to the current checkpoint with corresponding tone to be shaded with.
	* @return a vector3 with color information in RGB values.
	*/
	Vector3 getCurrentCheckpointColor();

	/**
	 * Returns the number of checkpoints loaded from the level.
	 *
	 * @return an unsinged int size.
	 */
	unsigned int getNrOfCheckpoints();
};
#include "CheckpointSystem.h"

using std::vector;

CheckpointSystem::CheckpointSystem()
{

}

CheckpointSystem::CheckpointSystem(Vector3 p_CurrentTone, Vector3 p_FinishTone) :
	m_CurrentColorTone(p_CurrentTone),
	m_FinishColorTone(p_FinishTone)
{
}

CheckpointSystem::~CheckpointSystem(void)
{
}

void CheckpointSystem::addCheckpoint(const std::weak_ptr<Actor> p_Checkpoint)
{
	if(m_Checkpoints.empty())
	{
		std::weak_ptr<ModelInterface> mI =p_Checkpoint.lock()->getComponent<ModelInterface>(ModelInterface::m_ComponentId);

		mI.lock()->setColorTone(Vector3(1,0,0));
	}

	m_Checkpoints.push_back(p_Checkpoint);
}

BodyHandle CheckpointSystem::getCurrentCheckpointBodyHandle(void)
{
	return m_Checkpoints.back().lock()->getBodyHandles().back();
}

bool CheckpointSystem::isFinishLine(void)
{
	return m_Checkpoints.empty();
}

void CheckpointSystem::changeCheckpoint(void)
{
	m_Checkpoints.pop_back();

	if(m_Checkpoints.size() > 1)
	{
		std::weak_ptr<ModelInterface> mI = m_Checkpoints.back().lock()->getComponent<ModelInterface>(ModelInterface::m_ComponentId);

		mI.lock()->setColorTone(Vector3(0,0,1));
	}
}

std::weak_ptr<Actor> CheckpointSystem::getCurrentActor(void)
{
	return m_Checkpoints.back();
}
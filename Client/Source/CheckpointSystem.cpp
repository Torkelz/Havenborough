#include "CheckpointSystem.h"
#include "Components.h"

using std::vector;

CheckpointSystem::CheckpointSystem(Vector3 p_DefaultTone, Vector3 p_CurrentTone, Vector3 p_FinishTone) :
	m_DefaultColorTone(p_DefaultTone),
	m_CurrentColorTone(p_CurrentTone),
	m_FinishColorTone(p_FinishTone)
{
}

CheckpointSystem::~CheckpointSystem(void)
{
}

void CheckpointSystem::addCheckpoint(const std::weak_ptr<Actor> p_Checkpoint)
{
	std::weak_ptr<ModelInterface> mI = p_Checkpoint.lock()->getComponent<ModelInterface>(ModelInterface::m_ComponentId);

	if(m_Checkpoints.empty())
	{
		mI.lock()->setColorTone(m_FinishColorTone);
	}
	else if(m_Checkpoints.size() > 1)
	{
		mI.lock()->setColorTone(m_CurrentColorTone);
		mI = m_Checkpoints.back().lock()->getComponent<ModelInterface>(ModelInterface::m_ComponentId);
		mI.lock()->setColorTone(m_DefaultColorTone);
	}

	m_Checkpoints.push_back(p_Checkpoint);
}

BodyHandle CheckpointSystem::getCurrentCheckpointBodyHandle(void)
{
	return m_Checkpoints.back().lock()->getBodyHandles().back();
}

bool CheckpointSystem::reachedFinishLine(void)
{
	return m_Checkpoints.empty();
}

void CheckpointSystem::changeCheckpoint(vector<Actor::ptr> &p_Objects)
{
	for(int i = p_Objects.size() - 1; i >= 0; i--)
	{
		if(p_Objects.at(i) == m_Checkpoints.back().lock())
		{
			p_Objects.erase(p_Objects.begin() + i);
			break;
		}
	}

	m_Checkpoints.pop_back();

	if(m_Checkpoints.size() > 1)
	{
		m_Checkpoints.back().lock()->getComponent<ModelInterface>(ModelInterface::m_ComponentId).lock()->setColorTone(m_CurrentColorTone);
	}
}
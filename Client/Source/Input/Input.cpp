#include "Input.h"

void Input::init(InputTranslator::ptr p_Translator)
{
}

void Input::destroy()
{
	m_Translator->setRecordHandler(InputTranslator::recordFunc_t());
}

void Input::onFrame()
{
	m_FrameInputs.clear();
}

const InputState& Input::getCurrentState() const
{
	return m_InputState;
}

const std::vector<InputRecord>& Input::getFrameInputs() const
{
	return m_FrameInputs;
}

void Input::handleRecords(InputRecord p_Record)
{
	m_FrameInputs.push_back(p_Record);
}

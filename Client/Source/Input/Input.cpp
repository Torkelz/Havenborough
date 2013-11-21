#include "Input.h"

void Input::init(InputTranslator::ptr p_Translator)
{
	m_Translator = std::move(p_Translator);
	m_Translator->setRecordHandler(std::bind(&Input::handleRecords, this, std::placeholders::_1));
}

void Input::destroy()
{
	if (m_Translator)
	{
		m_Translator->destroy();
		m_Translator = nullptr;
	}
	m_FrameInputs.clear();
	m_InputState.clear();
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
	m_InputState.updateRecord(p_Record);
}

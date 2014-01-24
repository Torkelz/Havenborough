#include "Input.h"

#include "../Logger.h"

void Input::init(InputTranslator::ptr p_Translator)
{
	Logger::log(Logger::Level::INFO, "Initializing input");

	m_Translator = std::move(p_Translator);
	m_Translator->setRecordHandler(std::bind(&Input::handleRecords, this, std::placeholders::_1));
}

void Input::destroy()
{
	Logger::log(Logger::Level::INFO, "Shutting down input");

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
	Logger::log(Logger::Level::TRACE, "Input onFrame");

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
	Logger::log(Logger::Level::TRACE, "Input handling a record");

	p_Record.m_PrevValue = m_InputState.getValue(p_Record.m_Action);
	m_FrameInputs.push_back(p_Record);
	m_InputState.updateRecord(p_Record);
	
}

void Input::lockMouse(IEventData::Ptr p_Data)
{
	std::shared_ptr<MouseEventDataLock> data = std::static_pointer_cast<MouseEventDataLock>(p_Data);
	m_Translator->lockMouse(data->getLockState());
}

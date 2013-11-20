#include "InputTranslator.h"

void InputTranslator::init(Window* p_Window)
{
	m_Window = p_Window;
	m_Window->registerCallback(WM_INPUT, std::bind<bool>(&InputTranslator::handleRawInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void InputTranslator::destroy()
{
}

void InputTranslator::setRecordHandler(InputTranslator::recordFunc_t p_RecordHandler)
{
	m_RecordFunction = p_RecordHandler;
}

bool InputTranslator::handleRawInput(WPARAM p_WParam, LPARAM p_LParam, LRESULT& p_Result)
{
	return false;
}

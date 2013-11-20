#pragma once

#include "InputRecord.h"
#include "../Window.h"

#include <functional>
#include <memory>

class InputTranslator
{
public:
	typedef std::function<void(InputRecord)> recordFunc_t;

private:
	recordFunc_t	m_RecordFunction;
	Window*			m_Window;

public:
	typedef std::unique_ptr<InputTranslator> ptr;

	void init(Window* p_Window);
	void destroy();

	void setRecordHandler(recordFunc_t p_RecordHandler);
	bool handleRawInput(WPARAM p_WParam, LPARAM p_LParam, LRESULT& p_Result);
};

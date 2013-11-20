#pragma once

#include "../Window.h"
#include "InputRecord.h"
#include "InputState.h"
#include "InputTranslator.h"

#include <vector>

class Input
{
private:
	InputTranslator::ptr		m_Translator;

	InputState					m_InputState;
	std::vector<InputRecord>	m_FrameInputs;

public:
	void init(InputTranslator::ptr p_Translator);
	void destroy();

	void onFrame();
	const InputState& getCurrentState() const;
	const std::vector<InputRecord>& getFrameInputs() const;

private:
	void handleRecords(InputRecord p_Record);
};

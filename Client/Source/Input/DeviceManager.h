#pragma once

#include "Gamepad.h"

#include <map>

class DeviceManager
{
private:
	std::map<HANDLE, Gamepad> m_Gamepads;
	Gamepad::Translator::ptr m_GamepadTranslator;

public:
	explicit DeviceManager();

	void setRecordFunc(Gamepad::Translator::recordFunc_t p_RecordFunc);
	bool handleInput(RAWINPUT* p_RawInput);

	void addButtonMapping(USAGE p_ButtonUsage, const std::string& p_Action);
	void addAxisMapping(USAGE p_AxisUsage, bool p_PosDir, const std::string& p_Action);

private:
	Gamepad& getOrCreateGamepad(HANDLE p_Device);
};

#pragma once

#include "hid.h"

#include <map>
#include <vector>

class Gamepad
{
private:
	HANDLE m_Device;
	HIDP_CAPS m_Capabilities;
	HIDP_BUTTON_CAPS m_ButtonCaps;
	std::vector<HIDP_VALUE_CAPS> m_ValueCaps;

	std::map<USAGE, bool> m_CurrentButtonStates;
	std::map<USAGE, ULONG> m_CurrentAxisStates;

	std::vector<char> m_PrepBuffer;

public:
	explicit Gamepad(HANDLE p_Device);

	bool handleInput(RAWINPUT* p_RawInput);
	void checkButtons(const std::vector<USAGE>& p_PressedButtons);
	void checkAxis(USAGE p_Axis, ULONG p_Value);
};

#pragma once

#include "Gamepad.h"

#include <map>

class DeviceManager
{
private:
	std::map<HANDLE, Gamepad> m_Gamepads;

public:
	bool handleInput(RAWINPUT* p_RawInput);

private:
	Gamepad& getOrCreateGamepad(HANDLE p_Device);
};

#include "DeviceManager.h"

#include "../ClientExceptions.h"
#include <Logger.h>

#include <vector>

#include <hidsdi.h>

bool DeviceManager::handleInput(RAWINPUT* p_RawHID)
{
	Gamepad& gamepad = getOrCreateGamepad(p_RawHID->header.hDevice);

	return gamepad.handleInput(p_RawHID);
}

Gamepad& DeviceManager::getOrCreateGamepad(HANDLE p_Device)
{
	auto it = m_Gamepads.find(p_Device);
	if (it != m_Gamepads.end())
	{
		return it->second;
	}

	Logger::log(Logger::Level::INFO, "Detected new gamepad, initializing...");

	auto pos = m_Gamepads.emplace(p_Device, Gamepad(p_Device));

	Logger::log(Logger::Level::INFO, "Gamepad successfully initialized");

	return pos.first->second;
}

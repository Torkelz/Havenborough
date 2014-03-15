#include "DeviceManager.h"

#include "../ClientExceptions.h"
#include <Logger.h>

#include <vector>

#include <hidsdi.h>

DeviceManager::DeviceManager() :
	m_GamepadTranslator(new Gamepad::Translator)
{
}

void DeviceManager::setRecordFunc(Gamepad::Translator::recordFunc_t p_RecordFunc)
{
	m_GamepadTranslator->setRecordFunc(p_RecordFunc);
}

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

	auto pos = m_Gamepads.emplace(p_Device, Gamepad(p_Device, m_GamepadTranslator));

	Logger::log(Logger::Level::INFO, "Gamepad successfully initialized");

	return pos.first->second;
}

void DeviceManager::addButtonMapping(USAGE p_ButtonUsage, const std::string& p_Action)
{
	m_GamepadTranslator->addButtonMapping(p_ButtonUsage, p_Action);
}

void DeviceManager::addAxisMapping(USAGE p_AxisUsage, bool p_PosDir, const std::string& p_Action)
{
	m_GamepadTranslator->addAxisMapping(p_AxisUsage, p_PosDir, p_Action);
}

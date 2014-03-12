#include "Gamepad.h"

#include "../ClientExceptions.h"
#include <Logger.h>

Gamepad::Gamepad(HANDLE p_Device) :
	m_Device(p_Device)
{
	UINT bufferSize = 0;
	GetRawInputDeviceInfoA(m_Device,
		RIDI_PREPARSEDDATA, nullptr, &bufferSize);

	if (bufferSize == 0)
		return;

	m_PrepBuffer.resize(bufferSize);
	GetRawInputDeviceInfoA(m_Device,
		RIDI_PREPARSEDDATA, m_PrepBuffer.data(), &bufferSize);

	PHIDP_PREPARSED_DATA preparsedData = (PHIDP_PREPARSED_DATA)m_PrepBuffer.data();

	hid.HidP_GetCaps(preparsedData, &m_Capabilities);

	std::vector<HIDP_BUTTON_CAPS> buttonCaps(m_Capabilities.NumberInputButtonCaps);
	USHORT capsLength = m_Capabilities.NumberInputButtonCaps;
	hid.HidP_GetButtonCaps(HidP_Input, buttonCaps.data(), &capsLength, preparsedData);

	m_ButtonCaps.UsagePage = 0;
	for (const auto& bCaps : buttonCaps)
	{
		if (bCaps.UsagePage == 9)
		{
			m_ButtonCaps = bCaps;
			break;
		}
	}

	m_ValueCaps.resize(m_Capabilities.NumberInputValueCaps);
	capsLength = m_Capabilities.NumberInputValueCaps;
	hid.HidP_GetValueCaps(HidP_Input, m_ValueCaps.data(), &capsLength, preparsedData);
}

bool Gamepad::handleInput(RAWINPUT* p_RawHID)
{
	PHIDP_PREPARSED_DATA preparsedData = (PHIDP_PREPARSED_DATA)m_PrepBuffer.data();

	if (m_ButtonCaps.UsagePage == 9)
	{
		ULONG numButtons = m_ButtonCaps.Range.UsageMax - m_ButtonCaps.Range.UsageMin + 1;
		std::vector<USAGE> activeButtons(numButtons);
		NTSTATUS stat;
		if ((stat = hid.HidP_GetUsages(HidP_Input, m_ButtonCaps.UsagePage, 0, activeButtons.data(), &numButtons,
			preparsedData, (PCHAR)p_RawHID->data.hid.bRawData, p_RawHID->data.hid.dwSizeHid)) != HIDP_STATUS_SUCCESS)
		{
			throw ClientException("Getting HID usages failed: " + std::to_string(stat), __LINE__, __FILE__);
		}

		activeButtons.resize(numButtons);

		checkButtons(activeButtons);
	}

	for (const auto& vCaps : m_ValueCaps)
	{
		ULONG value = 0;
		NTSTATUS stat;
		if ((stat = hid.HidP_GetUsageValue(HidP_Input, vCaps.UsagePage, 0,
			vCaps.Range.UsageMin, &value, preparsedData,
			(PCHAR)p_RawHID->data.hid.bRawData, p_RawHID->data.hid.dwSizeHid)) != HIDP_STATUS_SUCCESS)
		{
			throw ClientException("Getting HID usage value failed: " + std::to_string(stat), __LINE__, __FILE__);
		}

		checkAxis(vCaps.Range.UsageMin, value);
	}

	return true;
}

void Gamepad::checkButtons(const std::vector<USAGE>& p_PressedButtons)
{
	for (auto& button : m_CurrentButtonStates)
	{
		if (button.second && std::find(
			p_PressedButtons.begin(),
			p_PressedButtons.end(),
			button.first) == p_PressedButtons.end())
		{
			Logger::log(Logger::Level::INFO, "Button " + std::to_string(button.first) + " released");
			button.second = false;
		}
	}

	for (const auto& button : p_PressedButtons)
	{
		auto insertRes = m_CurrentButtonStates.insert(std::make_pair(button, false));
		if (insertRes.second)
		{
			Logger::log(Logger::Level::INFO, "New button detected: " + std::to_string(button));
		}

		if (!insertRes.first->second)
		{
			Logger::log(Logger::Level::INFO, "Button " + std::to_string(button) + " pressed");
			insertRes.first->second = true;
		}
	}
}

void Gamepad::checkAxis(USAGE p_Axis, ULONG p_Value)
{
	auto insertRes = m_CurrentAxisStates.insert(std::make_pair(p_Axis, p_Value));
	if (insertRes.second)
	{
		Logger::log(Logger::Level::INFO, "New axis detected: " + std::to_string(p_Axis));
	}

	if (p_Value != insertRes.first->second)
	{
		Logger::log(Logger::Level::INFO, "Axis " + std::to_string(p_Axis) + " moved to " + std::to_string(p_Value));
		insertRes.first->second = p_Value;
	}
}

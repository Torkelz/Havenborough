#include "Gamepad.h"

#include "../ClientExceptions.h"
#include <Logger.h>

void Gamepad::Translator::setRecordFunc(recordFunc_t p_RecordFunc)
{
	m_RecordFunc = p_RecordFunc;
}

void Gamepad::Translator::addButtonMapping(USAGE p_ButtonUsage, const std::string& p_Action)
{
	m_ButtonBinds[p_ButtonUsage] = p_Action;
}

void Gamepad::Translator::addAxisMapping(USAGE p_AxisUsage, bool p_PosDir, const std::string& p_Action)
{
	m_AxesBinds[std::make_pair(p_AxisUsage, p_PosDir)] = p_Action;
}

void Gamepad::Translator::passButton(USAGE p_ButtonUsage, float p_Value, float p_PrevValue)
{
	auto it = m_ButtonBinds.find(p_ButtonUsage);
	if (it == m_ButtonBinds.end())
		return;

	InputRecord rec;
	rec.m_Action = it->second;
	rec.m_Value = p_Value;
	rec.m_PrevValue = p_PrevValue;

	m_RecordFunc(rec);
}

void Gamepad::Translator::passAxis(USAGE p_AxisUsage, bool p_PosDir, float p_Value, float p_PrevValue)
{
	auto it = m_AxesBinds.find(std::make_pair(p_AxisUsage, p_PosDir));
	if (it == m_AxesBinds.end())
		return;

	InputRecord rec;
	rec.m_Action = it->second;
	rec.m_Value = p_Value;
	rec.m_PrevValue = p_PrevValue;

	m_RecordFunc(rec);
}

Gamepad::Gamepad(HANDLE p_Device, Translator::ptr p_Translator) :
	m_Device(p_Device),
	m_Translator(p_Translator)
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

		checkAxis(vCaps, value);
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
			m_Translator->passButton(button.first, 0.f, 1.f);
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
			m_Translator->passButton(button, 1.f, 0.f);
			insertRes.first->second = true;
		}
	}
}

static float normalizeValue(const HIDP_VALUE_CAPS& p_Axis, ULONG p_Center, ULONG p_Value)
{
	if (p_Axis.LogicalMin == 0 && p_Axis.LogicalMax == -1)
	{
#undef max
		float val = (float)p_Value / (float)(2 << (p_Axis.BitSize - 2));
		return fabs(val - 1.f);
	}

	if (p_Value < p_Center)
	{
		const ULONG minRange = p_Center - p_Axis.LogicalMin;
		const ULONG distance = p_Center - p_Value;

		return (float)distance / (float)minRange;
	}
	else if (p_Value > p_Center)
	{
		const ULONG maxRange = p_Axis.LogicalMax - p_Center;
		const ULONG distance = p_Value - p_Center;

		return (float)distance / (float)maxRange;
	}
	else
	{
		return 0.f;
	}
}

void Gamepad::checkAxis(const HIDP_VALUE_CAPS& p_Axis, ULONG p_Value)
{
	const USAGE usage = p_Axis.Range.UsageMin;

	auto insertRes = m_CurrentAxisStates.insert(
		std::make_pair(usage, Axis(p_Value, p_Value)));
	if (insertRes.second)
	{
		Logger::log(Logger::Level::INFO, "New axis detected: " + std::to_string(usage));
	}

	Axis& axis = insertRes.first->second;

	const float prevValue = normalizeValue(p_Axis, axis.center, axis.value);
	float currentValue = normalizeValue(p_Axis, axis.center, p_Value);

	static const float deadZoneMin = 0.2f;
	static const float deadZoneMax = 0.5f;
	static const float deadZoneWidth = deadZoneMax - deadZoneMin;

	if (currentValue < deadZoneMax)
	{
		if (currentValue < deadZoneMin)
			currentValue = 0.f;
		else
			currentValue = (currentValue - deadZoneMin) * deadZoneMax / deadZoneWidth;
	}
	
	if (p_Value < axis.center)
	{
		m_Translator->passAxis(usage, false, currentValue, prevValue);
		m_Translator->passAxis(usage, true, 0.f, 0.f);
	}
	else if (p_Value > axis.center)
	{
		m_Translator->passAxis(usage, true, currentValue, prevValue);
		m_Translator->passAxis(usage, false, 0.f, 0.f);
	}
	else
	{
		m_Translator->passAxis(usage, true, 0.f, 0.f);
		m_Translator->passAxis(usage, false, 0.f, 0.f);
	}

	axis.value = p_Value;
}

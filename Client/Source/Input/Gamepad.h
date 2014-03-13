#pragma once

#include "hid.h"
#include "InputRecord.h"

#include <functional>
#include <map>
#include <memory>
#include <vector>

class Gamepad
{
public:
	class Translator
	{
	public:
		typedef std::function<void(InputRecord)> recordFunc_t;
		typedef std::shared_ptr<Translator> ptr;

	private:
		std::map<USAGE, std::string> m_ButtonBinds;
		std::map<std::pair<USAGE, bool>, std::string> m_AxesBinds;

		recordFunc_t m_RecordFunc;

	public:
		void setRecordFunc(recordFunc_t p_RecordFunc);

		void addButtonMapping(USAGE p_ButtonUsage, const std::string& p_Action);
		void addAxisMapping(USAGE p_AxisUsage, bool p_PosDir, const std::string& p_Action);

		void passButton(USAGE p_ButtonUsage, float p_Value, float p_PrevValue);
		void passAxis(USAGE p_AxisUsage, bool p_PosDir, float p_Value, float p_PrevValue);
	};

private:
	HANDLE m_Device;
	HIDP_CAPS m_Capabilities;
	HIDP_BUTTON_CAPS m_ButtonCaps;
	std::vector<HIDP_VALUE_CAPS> m_ValueCaps;

	std::vector<char> m_PrepBuffer;

	std::map<USAGE, bool> m_CurrentButtonStates;
	struct Axis
	{
		ULONG center;
		ULONG value;

		Axis(ULONG p_Center, ULONG p_Value) : center(p_Center), value(p_Value) {}
	};
	std::map<USAGE, Axis> m_CurrentAxisStates;

	Translator::ptr m_Translator;

public:
	Gamepad(HANDLE p_Device, Translator::ptr p_Translator);

	bool handleInput(RAWINPUT* p_RawInput);
	void checkButtons(const std::vector<USAGE>& p_PressedButtons);
	void checkAxis(const HIDP_VALUE_CAPS& p_Axis, ULONG p_Value);
};

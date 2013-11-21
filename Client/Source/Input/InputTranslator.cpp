#include "InputTranslator.h"

#include "../MyExceptions.h"

void InputTranslator::init(Window* p_Window)
{
	RAWINPUTDEVICE inputDevices[2];

	// Generic mouse
	inputDevices[0].usUsagePage	= 0x01;
	inputDevices[0].usUsage		= 0x02;
	inputDevices[0].dwFlags		= 0;
	inputDevices[0].hwndTarget	= 0;

	// Generic keyboard
	inputDevices[1].usUsagePage	= 0x01;
	inputDevices[1].usUsage		= 0x06;
	inputDevices[1].dwFlags		= RIDEV_NOLEGACY;
	inputDevices[1].hwndTarget	= 0;

	if (RegisterRawInputDevices(&inputDevices[1], 1, sizeof(inputDevices[0])) == FALSE)
	{
		throw Win32Exception("Failed to register raw input devices", __LINE__, __FILE__);
	}

	m_Window = p_Window;
	m_Window->registerCallback(WM_INPUT, std::bind<bool>(&InputTranslator::handleRawInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void InputTranslator::destroy()
{
	m_KeyboardMappings.clear();
	m_RecordFunction = recordFunc_t();
	m_Window = nullptr;
}

void InputTranslator::setRecordHandler(InputTranslator::recordFunc_t p_RecordHandler)
{
	m_RecordFunction = p_RecordHandler;
}

void InputTranslator::addKeyboardMapping(USHORT p_VirtualKey, const std::string& p_Action)
{
	KeyboardRecord rec = {p_VirtualKey, p_Action};
	m_KeyboardMappings.push_back(rec);
}

bool InputTranslator::handleRawInput(WPARAM p_WParam, LPARAM p_LParam, LRESULT& p_Result)
{
	if (!m_RecordFunction)
	{
		return false;
	}

	bool handled = false;
	WPARAM wParam = GET_RAWINPUT_CODE_WPARAM(p_WParam);

	UINT size;
	GetRawInputData((HRAWINPUT)p_LParam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
	std::vector<BYTE> buffer(size);

	if (GetRawInputData((HRAWINPUT)p_LParam, RID_INPUT, buffer.data(), &size, sizeof(RAWINPUTHEADER)) != size)
	{
		throw Win32Exception("Could not get raw input data", __LINE__, __FILE__);
	}

	RAWINPUT* rawInputData = reinterpret_cast<RAWINPUT*>(buffer.data());

	if (rawInputData->header.dwType == RIM_TYPEKEYBOARD)
	{
		USHORT keyCode = rawInputData->data.keyboard.VKey;
		float value = 0.f;
		if (!(rawInputData->data.keyboard.Flags & RI_KEY_BREAK))
		{
			value = 1.f;
		}

		for (const KeyboardRecord& record : m_KeyboardMappings)
		{
			if (record.m_KeyCode == keyCode)
			{
				InputRecord inRec = {record.m_Action, value};
				m_RecordFunction(inRec);
				handled = true;
			}
		}
	}
	else if (rawInputData->header.dwType == RIM_TYPEMOUSE)
	{
		// TODO: Handle raw mouse input
	}

	if (handled)
	{
		p_Result = DefWindowProcW(m_Window->getHandle(), WM_INPUT, p_WParam, p_LParam);;
		return true;
	}
	else
	{
		return false;
	}
}

#include "InputTranslator.h"
#include "../ClientExceptions.h"
#include <Logger.h>

InputTranslator::InputTranslator()
	: m_Window(nullptr), m_MouseLocked(false)
{
}

void InputTranslator::init(Window* p_Window)
{
	Logger::log(Logger::Level::INFO, "Initializing input translator");

	if (p_Window == nullptr)
	{
		throw InvalidArgument("Window must not be null", __LINE__, __FILE__);
	}

	RAWINPUTDEVICE inputDevices[3];

	// Generic mouse
	inputDevices[0].usUsagePage	= 0x01;
	inputDevices[0].usUsage		= 0x02;
	inputDevices[0].dwFlags		= 0;
	inputDevices[0].hwndTarget	= 0;

	// Generic keyboard
	inputDevices[1].usUsagePage	= 0x01;
	inputDevices[1].usUsage		= 0x06;
	inputDevices[1].dwFlags		= 0;
	inputDevices[1].hwndTarget	= 0;

	inputDevices[2].usUsagePage = 0x01;
	inputDevices[2].usUsage		= 0x04;
	inputDevices[2].dwFlags		= 0;
	inputDevices[2].hwndTarget	= 0;

	if (RegisterRawInputDevices(&inputDevices[0], 3, sizeof(inputDevices[0])) == FALSE)
	{
		throw Win32Exception("Failed to register raw input devices", __LINE__, __FILE__);
	}

	m_Window = p_Window;
	m_Window->registerCallback(WM_INPUT, std::bind<bool>(&InputTranslator::handleRawInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void InputTranslator::destroy()
{
	Logger::log(Logger::Level::INFO, "Shutting down input translator");

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
	Logger::log(Logger::Level::TRACE, "Adding keyboard mapping");

	KeyboardRecord rec = {p_VirtualKey, p_Action};
	m_KeyboardMappings.push_back(rec);
}

void InputTranslator::addMouseMapping(Axis p_Axis, bool p_PosDir, const std::string& p_MovementAction)
{
	Logger::log(Logger::Level::TRACE, "Adding mouse mapping");

	MouseRecord rec = {p_Axis, p_PosDir, p_MovementAction};
	m_MouseMappings.push_back(rec);
}

void InputTranslator::addMouseButtonMapping(MouseButton p_Button, const std::string& p_Action)
{
	Logger::log(Logger::Level::TRACE, "Adding mouse button mapping");

	USHORT buttonFlag;
	switch(p_Button)
	{
	case MouseButton::LEFT:		buttonFlag = RI_MOUSE_LEFT_BUTTON_DOWN; break;
	case MouseButton::RIGHT:	buttonFlag = RI_MOUSE_RIGHT_BUTTON_DOWN; break;
	case MouseButton::MIDDLE:	buttonFlag = RI_MOUSE_MIDDLE_BUTTON_DOWN; break;
	case MouseButton::EXTRA_1:	buttonFlag = RI_MOUSE_BUTTON_4_DOWN; break;
	case MouseButton::EXTRA_2:	buttonFlag = RI_MOUSE_BUTTON_5_DOWN; break;
	default:
		throw InvalidArgument("Invalid button", __LINE__, __FILE__);
	};

	MouseButtonRecord rec = {buttonFlag, p_Action};
	m_MouseButtonMappings.push_back(rec);
}

void InputTranslator::lockMouse(bool p_State)
{
	m_MouseLocked = p_State;
}

bool InputTranslator::handleRawInput(WPARAM p_WParam, LPARAM p_LParam, LRESULT& p_Result)
{
	if (!m_RecordFunction)
	{
		return false;
	}

	bool handled = false;
	//WPARAM wParam = GET_RAWINPUT_CODE_WPARAM(p_WParam);

	UINT size;
	GetRawInputData((HRAWINPUT)p_LParam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
	std::vector<BYTE> buffer(size);

	if (GetRawInputData((HRAWINPUT)p_LParam, RID_INPUT, buffer.data(), &size, sizeof(RAWINPUTHEADER)) != size)
	{
		throw Win32Exception("Could not get raw input data", __LINE__, __FILE__);
	}

	RAWINPUT* rawInputData = reinterpret_cast<RAWINPUT*>(buffer.data());

	switch (rawInputData->header.dwType)
	{
	case RIM_TYPEKEYBOARD:
		if (handleKeyboardInput(rawInputData->data.keyboard))
		{
			handled = true;
		}
		break;

	case RIM_TYPEMOUSE:
		if (handleMouseInput(rawInputData->data.mouse))
		{
			handled = true;
		}
		break;

	case RIM_TYPEHID:
		if (m_DeviceManager.handleInput(rawInputData))
		{
			handled = true;
		}
		break;
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

bool InputTranslator::handleKeyboardInput(const RAWKEYBOARD& p_RawKeyboard)
{
	UINT keyCode = translateKey(p_RawKeyboard, nullptr);

	float value = 0.f, prevValue = -1.f;
	if (!(p_RawKeyboard.Flags & RI_KEY_BREAK))
	{
		value = 1.f;
	}

	bool handled = false;
	for (const KeyboardRecord& record : m_KeyboardMappings)
	{
		if (record.m_KeyCode == keyCode)
		{
			InputRecord inRec = {record.m_Action, value, prevValue};
			m_RecordFunction(inRec);
			handled = true;
		}
	}

	return handled;
}

// http://molecularmusings.wordpress.com/2011/09/05/properly-handling-keyboard-input/
UINT InputTranslator::translateKey(const RAWKEYBOARD& p_RawKeyboard, UINT* p_ScanCode) const
{
	UINT keyCode = p_RawKeyboard.VKey;
	UINT scanCode = p_RawKeyboard.MakeCode;
	UINT flags = p_RawKeyboard.Flags;

	if (keyCode == 255)
	{
		return false;
	}
	else if (keyCode == VK_SHIFT)
	{
		keyCode = MapVirtualKeyA(scanCode, MAPVK_VSC_TO_VK_EX);
	}
	else if (keyCode == VK_NUMLOCK)
	{
		scanCode = MapVirtualKeyA(keyCode, MAPVK_VK_TO_VSC) | 0x100;
	}

	const bool isE0 = (flags & RI_KEY_E0) != 0;
	const bool isE1 = (flags & RI_KEY_E1) != 0;

	if (isE1)
	{
		if (keyCode == VK_PAUSE)
		{
			scanCode = 0x45;
		}
		else
		{
			scanCode = MapVirtualKeyA(keyCode, MAPVK_VK_TO_VSC);
		}
	}

	switch (keyCode)
	{
	case VK_CONTROL:
		if (isE0)
			keyCode = VK_RCONTROL;
		else
			keyCode = VK_LCONTROL;
		break;

	case VK_MENU:
		if (isE0)
			keyCode = VK_RMENU;
		else
			keyCode = VK_LMENU;
		break;

	case VK_RETURN:
		if (isE0)
			keyCode = VK_SEPARATOR;
		break;

	case VK_INSERT:
		if (!isE0)
			keyCode = VK_NUMPAD0;
		break;

	case VK_DELETE:
		if (!isE0)
			keyCode = VK_DECIMAL;
		break;

	case VK_HOME:
		if (!isE0)
			keyCode = VK_NUMPAD7;
		break;

	case VK_END:
		if (!isE0)
			keyCode = VK_NUMPAD1;
		break;

	case VK_PRIOR:
		if (!isE0)
			keyCode = VK_NUMPAD9;
		break;

	case VK_NEXT:
		if (!isE0)
			keyCode = VK_NUMPAD3;
		break;

	case VK_LEFT:
		if (!isE0)
			keyCode = VK_NUMPAD4;
		break;

	case VK_RIGHT:
		if (!isE0)
			keyCode = VK_NUMPAD6;
		break;

	case VK_UP:
		if (!isE0)
			keyCode = VK_NUMPAD8;
		break;

	case VK_DOWN:
		if (!isE0)
			keyCode = VK_NUMPAD2;
		break;

	case VK_CLEAR:
		if (!isE0)
			keyCode = VK_NUMPAD5;
		break;
	}

	if (p_ScanCode)
		*p_ScanCode = scanCode;

	return keyCode;
}

bool InputTranslator::handleMouseInput(const RAWMOUSE& p_RawMouse)
{
	const LONG lastX = p_RawMouse.lLastX;
	const LONG lastY = p_RawMouse.lLastY;
	POINT tempPos;
	DirectX::XMFLOAT2 windowSize = DirectX::XMFLOAT2(10000, 10000);
	GetCursorPos(&tempPos);
	if (m_Window)
	{
		ScreenToClient(m_Window->getHandle(), &tempPos);
		windowSize = m_Window->getSize();
	}

	static const float sensitivity = 1.f;

	const float moveX = (float)lastX * sensitivity;
	const float moveY = -(float)lastY * sensitivity;

	bool handled = false;
	if (p_RawMouse.usButtonFlags != 0)
	{
		for (const MouseButtonRecord& record : m_MouseButtonMappings)
		{
			if ((p_RawMouse.usButtonFlags & record.m_Button) != 0)
			{
				InputRecord inRec = {record.m_Action, 1.f};
				m_RecordFunction(inRec);
				handled = true;
			}
			if ((p_RawMouse.usButtonFlags & (record.m_Button << 1)) != 0)
			{
				InputRecord inRec = {record.m_Action, 0.f};
				m_RecordFunction(inRec);
				handled = true;
			}
		}
	}
	
	if (m_MouseMappings.empty())
	{
		return handled;
	}

	if(GetFocus() != NULL && m_MouseLocked)
	{
		POINT tempSize;
		tempSize.x = (LONG)(windowSize.x * 0.5f);
		tempSize.y = (LONG)(windowSize.y * 0.5f);
		ClientToScreen(m_Window->getHandle(), &tempSize);
		SetCursorPos(tempSize.x, tempSize.y);
	}

	if (moveX == 0.f && moveY == 0.f)
	{
		return handled;
	}

	for (const MouseRecord& record : m_MouseMappings)
	{
		InputRecord moveInRec = {record.m_MoveAction, 0.f};

		if (record.m_Axis == Axis::HORIZONTAL)
		{
			moveInRec.m_Value = moveX;
		}
		else if (record.m_Axis == Axis::VERTICAL)
		{
			moveInRec.m_Value = moveY;
		}
		
		if (moveInRec.m_Value == 0.f)
			continue;

		if (record.m_PosDir == (moveInRec.m_Value > 0.f))
		{
			if (moveInRec.m_Value < 0.f)
				moveInRec.m_Value *= -1.f;

			m_RecordFunction(moveInRec);
		}
	}

	return true;
}

#include "InputTranslator.h"

#include "../ClientExceptions.h"
#include "../Logger.h"

InputTranslator::InputTranslator()
	: m_Window(nullptr)
{
}

void InputTranslator::init(Window* p_Window)
{
	Logger::log(Logger::Level::INFO, "Initializing input translator");

	if (p_Window == nullptr)
	{
		throw InvalidArgument("Window must not be null", __LINE__, __FILE__);
	}

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

	if (RegisterRawInputDevices(&inputDevices[0], 2, sizeof(inputDevices[0])) == FALSE)
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

void InputTranslator::addMouseMapping(Axis p_Axis, const std::string& p_PositionAction, const std::string& p_MovementAction)
{
	Logger::log(Logger::Level::TRACE, "Adding mouse mapping");

	MouseRecord rec = {p_Axis, p_PositionAction, p_MovementAction};
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

	if (rawInputData->header.dwType == RIM_TYPEKEYBOARD)
	{
		if (handleKeyboardInput(rawInputData->data.keyboard))
		{
			handled = true;
		}
	}
	else if (rawInputData->header.dwType == RIM_TYPEMOUSE)
	{
		if (handleMouseInput(rawInputData->data.mouse))
		{
			handled = true;
		}
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
	USHORT keyCode = p_RawKeyboard.VKey;
	float value = 0.f;
	if (!(p_RawKeyboard.Flags & RI_KEY_BREAK))
	{
		value = 1.f;
	}

	bool handled = false;
	for (const KeyboardRecord& record : m_KeyboardMappings)
	{
		if (record.m_KeyCode == keyCode)
		{
			InputRecord inRec = {record.m_Action, value};
			m_RecordFunction(inRec);
			handled = true;
		}
	}

	return handled;
}

bool InputTranslator::handleMouseInput(const RAWMOUSE& p_RawMouse)
{
	const LONG lastX = p_RawMouse.lLastX;
	const LONG lastY = p_RawMouse.lLastY;
	POINT tempPos;
	UVec2 windowSize = {10000, 10000};
	GetCursorPos(&tempPos);
	if (m_Window)
	{
		ScreenToClient(m_Window->getHandle(), &tempPos);
		windowSize = m_Window->getSize();
	}

	static const float sensitivity = 1.f;

	const float moveX = (float)lastX * sensitivity;
	const float moveY = -(float)lastY * sensitivity;
	const float posX = (float)tempPos.x / (float)windowSize.x;
	const float posY = 1.f - (float)tempPos.y / (float)windowSize.y;

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

	// Filter out mouse movement outside window
	if (posX > 1.f || posX < 0.f
		|| posY > 1.f || posY < 0.f)
	{
		return handled;
	}

	if (moveX == 0.f && moveY == 0.f)
	{
		return handled;
	}

	for (const MouseRecord& record : m_MouseMappings)
	{
		InputRecord moveInRec = {record.m_MoveAction, 0.f};
		InputRecord posInRec = {record.m_PosAction, 0.f};

		if (record.m_Axis == Axis::HORIZONTAL)
		{
			moveInRec.m_Value = moveX;
			posInRec.m_Value = posX;
		}
		else if (record.m_Axis == Axis::VERTICAL)
		{
			moveInRec.m_Value = moveY;
			posInRec.m_Value = posY;
		}

		m_RecordFunction(moveInRec);
		m_RecordFunction(posInRec);
	}

	return true;
}

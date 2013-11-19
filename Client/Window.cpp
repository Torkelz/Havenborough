#include "Window.h"

#include "MyExceptions.h"

#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

const LPCWSTR Window::m_ClassName = L"Havenborough_Game_Client";

void Window::init(const std::string& p_Title, UVec2 p_WindowSize, bool p_Fullscreen)
{
	HMODULE hInstance = GetModuleHandleW(NULL);

	WNDCLASSEXW windowClassDescription;
	windowClassDescription.cbSize		= sizeof(WNDCLASSEXW);
	windowClassDescription.style		= 0;
	windowClassDescription.lpfnWndProc	= windowProc;
	windowClassDescription.cbClsExtra	= 0;
	windowClassDescription.cbWndExtra	= 0;
	windowClassDescription.hInstance	= hInstance;
	windowClassDescription.hIcon		= NULL;
	windowClassDescription.hCursor		= LoadCursor(NULL, IDC_ARROW);
	windowClassDescription.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	windowClassDescription.lpszMenuName	= NULL;
	windowClassDescription.lpszClassName	= m_ClassName;
	windowClassDescription.hIconSm		= NULL;

	if (!RegisterClassExW(&windowClassDescription))
	{
		throw WindowException("Window could not be registered", __LINE__, __FILE__);
	}

	RECT windowSize = { 0, 0, p_WindowSize.x, p_WindowSize.y };
	AdjustWindowRect(&windowSize, WS_OVERLAPPEDWINDOW, FALSE);

	std::vector<wchar_t> convertedTitle(p_Title.size() + 1);
	mbstowcs(convertedTitle.data(), p_Title.data(), p_Title.size() + 1);
	m_Handle = CreateWindowExW(
		0L,
		m_ClassName,
		convertedTitle.data(),
		WS_OVERLAPPED,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowSize.right - windowSize.left,
		windowSize.bottom - windowSize.top,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (m_Handle == NULL)
	{
		throw WindowException("Window could not be created", __LINE__, __FILE__);
	}

	ShowWindow(m_Handle, SW_SHOWDEFAULT);
}

LRESULT CALLBACK Window::windowProc(_In_ HWND p_Hwnd, _In_ UINT p_UMsg, _In_ WPARAM p_WParam, _In_ LPARAM p_LParam)
{
	return DefWindowProcW(p_Hwnd, p_UMsg, p_WParam, p_LParam);
}
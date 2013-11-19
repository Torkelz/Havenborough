#pragma once

#include <functional>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

struct UVec2
{
	unsigned int x;
	unsigned int y;
};

class Window
{
private:
	const static LPCWSTR m_ClassName;
	HWND		m_Handle;

public:
	void		init(const std::string& p_Title, UVec2 p_WindowSize, bool p_Fullscreen);
	void		destroy();

	void		pollMessages();
	void		registerCallback(UINT p_MessageType, std::function<void(WPARAM, LPARAM)> p_Callback);

	bool		getFullscreen() const;
	void		setFullscreen(bool p_Full);
	HWND		getHandle() const;
	HICON		getIcon() const;
	void		setIcon(HICON p_Icon);
	UVec2		getSize() const;
	void		setSize(UVec2 p_NewSize);
	bool		getShowCursor() const;
	void		setShowCursor(bool p_Show);
	std::string	getTitle() const;
	void		setTitle(const std::string& p_NewTitle);
	bool		getIsVisible() const;
	void		setIsVisible(bool p_Visible);

private:
	static LRESULT CALLBACK windowProc(_In_ HWND p_Hwnd, _In_ UINT p_UMsg, _In_ WPARAM p_WParam, _In_ LPARAM p_LParam);
};

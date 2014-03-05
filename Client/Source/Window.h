#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <functional>
#include <string>
#include <vector>
#include <DirectXMath.h>

/**
 * Represents a Win32 window that deals with the basic initialization and message polling.
 */
class Window
{
public:
	/**
	 * Function type to be used for registering callbacks for Win32 messages.
	 */
	typedef std::function<bool(WPARAM, LPARAM, LRESULT&)> callbackFunc_t;
	
private:
	/**
	 * Class name for the window class created by this class. Should always be completely unique.
	 */
	const static LPCWSTR m_ClassName;
	/**
	 * The different windows created that all uses the same windowProc function.
	 * Used to relay messages to the correct window.
	 */
	static std::vector<std::pair<HWND, Window*>> m_CallbackWindows;
	/**
	 * The number of windows currently using the registered window class.
	 */
	static int m_ClassUseCount;

	/**
	 * A list of all registered callbacks functions with their corresponding message type.
	 */
	std::vector<std::pair<UINT, callbackFunc_t>> m_RegisteredCallbacks;

	std::string	m_Title;
	HWND		m_Handle;
	HICON		m_Icon;
	DirectX::XMFLOAT2 m_Size;
	bool		m_ShowingCursor;
	bool		m_IsVisible;
	bool		m_Initialized;
	bool		m_Maximized;
	DWORD		m_WindowStyle;

public:
	/**
	 * constructor.
	 *
	 * Initializes default values, but is always safe to call.
	 */
	Window();

	/**
	 * destructor.
	 *
	 * If the window has been initialized but not destroyed,
	 * the destructor will destroy the window.
	 */
	~Window();

	/**
	 * Initialize and create the window. Uses the given title, size and any previously set value.
	 * Should not be called more than once on the same object, unless destroy is called in between.
	 *
	 * @param p_Title The title of the window, usually visible on the title bar.
	 * @param p_WindowSize The size of the window to create (width, height). The caller is
	 *			responsible for making sure that the values are valid for the platform.
	 */
	void init(const std::string& p_Title, DirectX::XMFLOAT2 p_WindowSize);
	/**
	 * Closes the window and cleans up any resources used. May be called on an unitialized object.
	 */
	void destroy();

	/**
	 * Poll the system messages and delivers them to any registered callback.
	 */
	void pollMessages();
	/**
	 * Register a callback to be called whenever a specified system message arrives.
	 *
	 * @param p_MessageType The system message type to listen to.
	 * @param p_Callback the callback function to call when the specified message type arrives.
	 */
	void registerCallback(UINT p_MessageType, callbackFunc_t p_Callback);

	/**
	 * Get the window handle associated with the window.
	 *
	 * @return A Win32 window handle if a window has been created, otherwise NULL.
	 */
	HWND getHandle() const;
	/**
	 * Get the icon handle associated with the window.
	 *
	 * @return A icon handle if an icon has been set, otherwise NULL.
	 */
	HICON getIcon() const;
	/**
	 * Set an icon to be used for the window.
	 *
	 * @param p_Icon A handle to an icon to be used, or NULL to use a default icon.
	 */
	void setIcon(HICON p_Icon);
	/**
	 * Get the last set size of the window.
	 *
	 * @return The size (width, height) of the window.
	 */
	DirectX::XMFLOAT2 getSize() const;
	/**
	 * Set a new size for the window.
	 *
	 * @param p_NewSize The size to resize the window to. The caller is responsible
	 *			for making sure that the size is supported by the platform.
	 */
	void setSize(DirectX::XMFLOAT2 p_NewSize);
	/**
	 * Get whether the windows cursor is currently shown.
	 *
	 * @return true if the cursor is shown, otherwise false.
	 */
	bool getShowCursor() const;
	/**
	 * Set whether the windows cursor should be shown.
	 *
	 * @param p_Show true to show the cursor, false to hide it.
	 */
	void setShowCursor(bool p_Show);
	/**
	 * Get the current title of the window.
	 *
	 * @return The window title as a multibyte string.
	 */
	std::string	getTitle() const;
	/**
	 * Set a new title for the window.
	 *
	 * @param p_NewTitle The new multibyte encoded string to be used as the window title.
	 */
	void setTitle(const std::string& p_NewTitle);
	/**
	 * Get whether the window is currently visible. Visible windows may be
	 * hidden by other windows or minimized.
	 *
	 * @return true if the window is visible and false if it is hidden.
	 */
	bool getIsVisible() const;
	/**
	 * Set the visibility of the window.
	 *
	 * @param p_Visible true to show the window, false to hide it.
	 */
	void setIsVisible(bool p_Visible);
	/**
	 * Get whether the window is currently maximized.
	 *
	 * @return true if the window is maximized and false if it is not.
	 */
	bool getIsMaximized() const;
	/**
	 * Set the maximized flag of the window.
	 *
	 * @param p_Maximized true to set the maximized flag of the window, false not maximized.
	 */
	void setIsMaximized(bool p_Maximized);

private:
	/**
	 * Common callback function for all created windows and the calling thread.
	 *
	 * For more information, ask Microsoft (http://msdn.microsoft.com/en-us/library/windows/desktop/ms633573%28v=vs.85%29.aspx)
	 */
	static LRESULT CALLBACK windowProc(_In_ HWND p_Hwnd, _In_ UINT p_UMsg, _In_ WPARAM p_WParam, _In_ LPARAM p_LParam);

	/**
	 * Dispatches a message to any listeners registered to the window.
	 *
	 * For more information, ask Microsoft (http://msdn.microsoft.com/en-us/library/windows/desktop/ms633573%28v=vs.85%29.aspx)
	 *
	 * @param p_UMsg message type
	 * @param p_WParam message info
	 * @param p_LParam message info
	 * @param p_Return Value to return to the operating system, if returning true
	 * @return true if the message has been handled, otherwise false
	 */
	bool dispatchMessage(UINT p_UMsg, WPARAM p_WParam, LPARAM p_LParam, LRESULT& p_Return);
};

/**
 * First comment of the file, please continue.
 */

#pragma once

#include "InputRecord.h"
#include "../Window.h"

#include <functional>
#include <memory>

/**
 * This class is a translator between win32 window events and registered game actions.
 */
class InputTranslator
{
public:
	/**
	 * Function definition used to pass a function to handle translated events.
	 * Create with std::bind (ex. std::bind(&Class::mem_func, this, std::placeholders::_1).
	 */
	typedef std::function<void(InputRecord)> recordFunc_t;
	/**
	 * A smart pointer.
	 */
	typedef std::unique_ptr<InputTranslator> ptr;

protected:
	/**
	 * Function to handle new records.
	 */
	recordFunc_t	m_RecordFunction;
	/**
	 * Window to translate events from.
	 */
	Window*			m_Window;

	/**
	 * Internal container of a single keyboard mapping.
	 */
	struct KeyboardRecord
	{
		/**
		 * Windows virtual key code.
		 */
		USHORT		m_KeyCode;
		/**
		 * The registered action identifier.
		 */
		std::string	m_Action;
	};
	/**
	 * A list of all registered keyboard mappings.
	 */
	std::vector<KeyboardRecord> m_KeyboardMappings;

public:
	/**
	 * Init the translator. Registers the supported devices
	 * for sending to the application.
	 *
	 * @param p_Window The window to translate input from. Must not be null.
	 * @throws InvalidArgument if p_Window is null.
	 */
	void init(Window* p_Window);
	/**
	 * Clean up resources.
	 */
	void destroy();

	/**
	 * Set the function to handle any translated message.
	 *
	 * @param p_RecordHandler A function object to call whenever a window
	 *			message has been translated. Pass recordFunc_t() to unset.
	 */
	void setRecordHandler(recordFunc_t p_RecordHandler);
	/**
	 * Add a new keyboard mapping to the translation. Duplicate calls will
	 * result in duplicated translations.
	 *
	 * @param p_VirtualKey The Win32 virtual key code to translate.
	 * @param p_Action The action identifier to translate the key to.
	 */
	void addKeyboardMapping(USHORT p_VirtualKey, const std::string& p_Action);

protected:
	/**
	 * Handler function for Win32 raw input messages.
	 *
	 * @param p_WParam The WParam parameter from the message.
	 * @param p_LParam The LParam parameter from the message.
	 * @param p_Result The result to pass back to the system, will only
	 *			be used if the function returns true.
	 * @return true if the message has been handled, otherwise false.
	 */
	bool handleRawInput(WPARAM p_WParam, LPARAM p_LParam, LRESULT& p_Result);

	bool handleKeyboardInput(const RAWKEYBOARD& p_RawKeyboard);
};

/**
 * PLACEHOLDER LICENSE: Be nice!
 */

#pragma once

#include <string>

/**
 * A representation of a defined input and its value.
 */
struct InputRecord
{
	/**
	 * An input action identifier.
	 */
	std::string	m_Action;
	/**
	 * An input action value.
	 * [0, 1] for keys and buttons, [0, 1] for positions.
	 */
	float		m_Value;
};

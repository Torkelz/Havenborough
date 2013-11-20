#pragma once

#include <string>

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

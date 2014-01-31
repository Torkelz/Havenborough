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
	 * [-MAX_LONG, MAX_LONG] for movements.
	 * Positions and movements use up and right as the positive directions.
	 */
	float m_Value;
	/**
	 * The previous input action value. When checked if zero there is no key repeats.
	 */
	float m_PrevValue;
};

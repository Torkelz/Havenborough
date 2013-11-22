/**
 * <insert funny license text here>
 */

#pragma once

#include "InputRecord.h"

#include <vector>

/**
 * A representation of a snapshot of the games input.
 */
class InputState
{
private:
	std::vector<InputRecord> m_Records;

public:
	/**
	 * If a record with the same action exists, it's value is updated,
	 * otherwise a new record is added with the action and value passed in.
	 *
	 * @param p_Record The record to add or update.
	 */
	void updateRecord(InputRecord p_Record);

	/**
	 * Remove any existing records.
	 */
	void clear();

	/**
	 * Get the number of currently stored records.
	 *
	 * @return Number of stored records.
	 */
	std::vector<InputRecord>::size_type size() const;

	/**
	 * Check if there exists a record with the given action.
	 *
	 * @param p_Identifier The action to check the existens of.
	 * @return true if a record exists, otherwise false.
	 */
	bool hasRecord(const std::string& p_Identifier) const;
	/**
	 * Get the value of an existing record.
	 *
	 * @param p_RecordIdentifier The action to get the value of.
	 * @return The records value if the record exists, otherwise 0.f.
	 */
	float getValue(const std::string& p_RecordIdentifier) const;
};

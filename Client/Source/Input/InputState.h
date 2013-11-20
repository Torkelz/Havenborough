#pragma once

#include "InputRecord.h"

#include <vector>

class InputState
{
private:
	std::vector<InputRecord> m_Records;

public:
	void updateRecord(InputRecord p_Record);

	bool hasRecord(const std::string& p_Identifier) const;
	float getValue(const std::string& p_RecordIdentifier) const;
};

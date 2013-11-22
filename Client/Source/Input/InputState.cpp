#include "InputState.h"

#include <algorithm>

class isRecord
{
private:
	std::string m_Ident;

public:
	isRecord(const std::string& p_Identifier)
		: m_Ident(p_Identifier)
	{}

	bool operator()(const InputRecord& p_Rec) const
	{
		return p_Rec.m_Action == m_Ident;
	}
};

void InputState::updateRecord(InputRecord p_Record)
{
	auto pos = std::find_if(m_Records.begin(), m_Records.end(), isRecord(p_Record.m_Action));
	if (pos != m_Records.end())
	{
		pos->m_Value = p_Record.m_Value;
	}
	else
	{
		m_Records.push_back(p_Record);
	}
}

void InputState::clear()
{
	m_Records.clear();
}

std::vector<InputRecord>::size_type InputState::size() const
{
	return m_Records.size();
}

bool InputState::hasRecord(const std::string& p_Identifier) const
{
	return std::any_of(m_Records.begin(), m_Records.end(), isRecord(p_Identifier));
}

float InputState::getValue(const std::string& p_Identifier) const
{
	auto pos = std::find_if(m_Records.begin(), m_Records.end(), isRecord(p_Identifier));
	if (pos != m_Records.end())
	{
		return pos->m_Value;
	}
	else
	{
		return 0.f;
	}
}
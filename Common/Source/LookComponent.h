#pragma once

#include "Components.h"

class LookComponent : public LookInterface
{
private:
	Vector3 m_OffsetPosition;
	Vector3 m_Forward;
	Vector3 m_Up;

public:
	void initialize(const tinyxml2::XMLElement* p_Data) override;
	void serialize(tinyxml2::XMLPrinter& p_Printer) const override;

	Vector3 getLookPosition() const override;

	Vector3 getLookForward() const override;
	void setLookForward(Vector3 p_Forward) override;

	Vector3 getLookUp() const override;
	void setLookUp(Vector3 p_Up) override;

	Vector3 getLookRight() const override;

	DirectX::XMFLOAT4X4 getRotationMatrix() const override;
};

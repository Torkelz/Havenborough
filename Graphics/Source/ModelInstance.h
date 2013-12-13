#pragma once
#include <DirectXMath.h>
#include <string>

using namespace DirectX;
using std::string;

 class ModelInstance
 {
 private:
	string m_ModelName;
	XMFLOAT3 m_Position;
	XMFLOAT3 m_Rotation;
	XMFLOAT3 m_Scale;
	
	mutable bool m_IsCalculated;
	mutable XMFLOAT4X4 m_World;

 public:
	 ModelInstance(void);
	 ~ModelInstance(void);
	 
	 void setIsCalculated(bool p_IsCalculated);
	 bool getIsCalculated(void) const;
	 void setModelName(string p_Name);
	 string getModelName(void) const;

	 const XMFLOAT4X4 &getWorldMatrix(void) const;
	 void setPosition(const XMFLOAT3 &p_Position);
	 void setRotation(const XMFLOAT3 & p_Rotation);
	 void setScale(const XMFLOAT3 &p_Scale);
	 void calculateWorldMatrix(void) const;
 };
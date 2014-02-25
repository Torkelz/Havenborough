#pragma once

#include "Utilities/XMFloatUtil.h"
#include <tinyxml2/tinyxml2.h>

#include <string>

inline void pushVector(tinyxml2::XMLPrinter& p_Printer, Vector3 p_Vec)
{
	p_Printer.PushAttribute("x", p_Vec.x);
	p_Printer.PushAttribute("y", p_Vec.y);
	p_Printer.PushAttribute("z", p_Vec.z);
}

inline void pushVector(tinyxml2::XMLPrinter& p_Printer, const std::string& p_ElementName, Vector3 p_Vec)
{
	p_Printer.OpenElement(p_ElementName.c_str());
	pushVector(p_Printer, p_Vec);
	p_Printer.CloseElement();
}

inline void pushColor(tinyxml2::XMLPrinter& p_Printer, const std::string& p_ElementName, Vector3 p_Color)
{
	p_Printer.OpenElement(p_ElementName.c_str());
	p_Printer.PushAttribute("r", p_Color.x);
	p_Printer.PushAttribute("g", p_Color.y);
	p_Printer.PushAttribute("b", p_Color.z);
	p_Printer.CloseElement();
}

inline void pushColor(tinyxml2::XMLPrinter& p_Printer, const std::string& p_ElementName, Vector4 p_Color)
{
	p_Printer.OpenElement(p_ElementName.c_str());
	p_Printer.PushAttribute("r", p_Color.x);
	p_Printer.PushAttribute("g", p_Color.y);
	p_Printer.PushAttribute("b", p_Color.z);
	p_Printer.PushAttribute("a", p_Color.w);
	p_Printer.CloseElement();
}

inline void pushRotation(tinyxml2::XMLPrinter& p_Printer, Vector3 p_YawPitchRoll)
{
	p_Printer.PushAttribute("yaw", p_YawPitchRoll.x);
	p_Printer.PushAttribute("pitch", p_YawPitchRoll.y);
	p_Printer.PushAttribute("roll", p_YawPitchRoll.z);
}

inline void pushRotation(tinyxml2::XMLPrinter& p_Printer, const std::string& p_ElementName, Vector3 p_YawPitchRoll)
{
	p_Printer.OpenElement(p_ElementName.c_str());
	pushRotation(p_Printer, p_YawPitchRoll);
	p_Printer.CloseElement();
}

inline void queryRotation(const tinyxml2::XMLElement* p_Element, Vector3& p_OutVec)
{
	if (p_Element)
	{
		p_Element->QueryAttribute("yaw", &p_OutVec.x);
		p_Element->QueryAttribute("pitch", &p_OutVec.y);
		p_Element->QueryAttribute("roll", &p_OutVec.z);
	}
}

inline void queryVector(const tinyxml2::XMLElement* p_Element, Vector3& p_OutVec)
{
	if (p_Element)
	{
		p_Element->QueryAttribute("x", &p_OutVec.x);
		p_Element->QueryAttribute("y", &p_OutVec.y);
		p_Element->QueryAttribute("z", &p_OutVec.z);
	}
}

inline void queryColor(const tinyxml2::XMLElement* p_Element, Vector4& p_OutColor)
{
	if (p_Element)
	{
		p_Element->QueryAttribute("r", &p_OutColor.x);
		p_Element->QueryAttribute("g", &p_OutColor.y);
		p_Element->QueryAttribute("b", &p_OutColor.z);
		p_Element->QueryAttribute("a", &p_OutColor.w);
	}
}

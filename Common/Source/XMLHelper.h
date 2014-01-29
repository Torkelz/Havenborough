#pragma once

#include "Utilities/XMFloatUtil.h"
#include <tinyxml2/tinyxml2.h>

#include <string>

inline void pushVector(tinyxml2::XMLPrinter& p_Printer, const std::string& p_ElementName, Vector3 p_Vec)
{
	p_Printer.OpenElement(p_ElementName.c_str());
	p_Printer.PushAttribute("x", p_Vec.x);
	p_Printer.PushAttribute("y", p_Vec.y);
	p_Printer.PushAttribute("z", p_Vec.z);
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
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <hidsdi.h>

static const struct HIDDLL {
	HMODULE _module;

#define HID_DECLARE(fname) decltype(&::fname) fname
#define HID_DEFINE(fname) fname = reinterpret_cast<decltype(fname)>(GetProcAddress(_module, #fname))
#define HID_PERFORM(MACRO) \
	MACRO(HidP_GetCaps); \
	MACRO(HidP_GetSpecificValueCaps); \
	MACRO(HidP_GetUsageValue); \
	MACRO(HidP_MaxUsageListLength); \
	MACRO(HidP_GetUsages); \
	MACRO(HidP_GetUsagesEx); \
	MACRO(HidD_GetPreparsedData); \
	MACRO(HidD_FreePreparsedData); \
	MACRO(HidP_SetUsageValue); \
	MACRO(HidP_SetUsages); \
	MACRO(HidP_UnsetUsages); \
	MACRO(HidD_SetFeature); \
	MACRO(HidD_GetFeature); \
	MACRO(HidP_GetButtonCaps); \
	MACRO(HidP_GetValueCaps);

	HID_PERFORM(HID_DECLARE);

	HIDDLL() {
		memset(this, 0, sizeof(*this));
		_module = LoadLibraryA("hid.dll");
		if (_module) {
			HID_PERFORM(HID_DEFINE);
		}
	}
	
#undef HID_PERFORM
#undef HID_DECLARE
#undef HID_DEFINE
	
	~HIDDLL() {
		if (_module)
			FreeLibrary(_module);
	}
} hid;

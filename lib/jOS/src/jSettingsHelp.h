#pragma once

#include <jCommon.h>

//TODO move this to jOS Lib


// splits up settings line into tag and value as String
// like "test=12" -> tag="test", value="12"
void ProcessSettingsLine(const String& line, String& tag, String& value);

// extract value from value string like "1234" -> 1234
bool ProcessContent_Float(const String& line_content, float& value);
bool ProcessContent_Double(const String& line_content, double& value);
bool ProcessContent_Int32(const String& line_content, int32_t& value);
bool ProcessContent_UInt32(const String& line_content, uint32_t& value);
bool ProcessContent_String(const String& line_content, String& value);
bool ProcessContent_Bool(const String& line_content, bool& value);

// converts value to value string like 1234 -> "1234"
String getSettingContent_String(const String& value);
String getSettingContent_Float(const float value);
String getSettingContent_Double(const double value);
String getSettingContent_Int32(const int32_t value);
String getSettingContent_UInt32(const uint32_t value);
String getSettingContent_Bool(const bool value);

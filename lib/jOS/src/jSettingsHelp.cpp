#include "jSettingsHelp.h"

#include <jSystem.h>

#include <jHelp.h>




String getSettingContent_String(const String& value) {
	return format("'%s'", value.c_str());
}
String getSettingContent_Float(const float value) {
	return format("%.7f", value);
}
String getSettingContent_Double(const double value) {
	return format("%.10f", value);
}
String getSettingContent_Int32(const int32_t value) {
	return format("%d", value);
}
String getSettingContent_UInt32(const uint32_t value) {
	return format("%u", value);
}
String getSettingContent_Bool(const bool value) {
	return format("%d", value);
}




void ProcessSettingsLine(const String& line, String& tag, String& value)
{

	int index_sep = line.indexOf("="); // seperator: "="
	if(index_sep != -1)
	{
		tag = line.substring(0, index_sep);

		value = "";
		value += line.substring(index_sep + 1); // -> index_sep + len("="")
	}
	else
	{
		tag = "";
		value = "";
	}
}



#define STRING_STARTEND_CHAR0 '\''
#define STRING_STARTEND_CHAR1 '\"'

bool ProcessContent_String(const String& line_content, String& value)
{
	// Check if starts with ' or "
	if ( 
		line_content.length() >= 2 &&
		(
			(line_content[0] == STRING_STARTEND_CHAR0 && line_content[line_content.length() - 1] == STRING_STARTEND_CHAR0)
			||
			(line_content[0] == STRING_STARTEND_CHAR1 && line_content[line_content.length() - 1] == STRING_STARTEND_CHAR1)
		)
	)
	{
		value = line_content.substring(1, line_content.length() - 1);
		return true;
	}
	else
		return false;
}


bool ProcessContent_Int32(const String& line_content, int32_t& value)
{
	// check if string is a number in decimal or hex format
	if (line_content.length() > 2 && line_content[0] == 'x' && isHexNumber(line_content)) // hex number like "x42AB"
	{
		value = str2int_hex(line_content);
		return true;
	}
	else if (isNumber(line_content)) // decimal number like "1234"
	{
		value = str2int32(line_content);
		return true;
	}
	else
		return false;
}

bool ProcessContent_UInt32(const String& line_content, uint32_t& value)
{
	// check if string is a number in decimal or hex format
	if (line_content.length() > 2 && line_content[0] == 'x' && isHexNumber(line_content)) // hex number like "x42AB"
	{
		value = str2int_hex(line_content);
		return true;
	}
	else if (isNumber(line_content)) // decimal number like "1234"
	{
		value = str2uint32(line_content);
		return true;
	}
	else
		return false;
}


bool ProcessContent_Float(const String& line_content, float& value)
{
	if (isFloatNumber(line_content))
	{
		value = str2float(line_content);
		return true;
	}
	else
		return false;
}

bool ProcessContent_Double(const String& line_content, double& value)
{	
	if (isFloatNumber(line_content))
	{
		value = str2double(line_content);
		return true;
	}
	else
		return false;
}

bool ProcessContent_Bool(const String& line_content, bool& value)
{
	if (isNumber(line_content))
	{
		value = 0 != str2int32(line_content);
		return true;
	}
	else
		return false;
}


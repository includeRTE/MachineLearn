#ifndef _UTIL_FUNCTIONS_H
#define _UTIL_FUNCTIONS_H

#include"stdafx.h"
class Functions
{
public:
	static std::string UTF8ToUnicode(const char*);
	static std::string WString2String(const wchar_t* wstr);
private:
	Functions();
	~Functions();
};

#endif
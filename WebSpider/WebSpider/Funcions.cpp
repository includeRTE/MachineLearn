#include "stdafx.h"
#include "Funcions.h"
#include<Windows.h>

std::string Functions::UTF8ToUnicode(const char* str)
{
	std::string str_unicode;
	if (!str)
	{
		return str_unicode;
	}
	int dwUnicodeLen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	size_t num = dwUnicodeLen * sizeof(wchar_t);
	wchar_t* pwText = new wchar_t[num];
	memset(pwText, 0, num);
	MultiByteToWideChar(CP_UTF8, 0, str, -1, pwText, dwUnicodeLen);
	str_unicode = Functions::WString2String(pwText);
	delete[] pwText;
	return str_unicode;
}

std::string Functions::WString2String(const wchar_t* wstr)
{
	std::string unicode_str;
	std::string localstr = setlocale(LC_ALL, NULL);
	setlocale( LC_ALL, "chs");
	if (!wstr)
	{
		return unicode_str;
	}

	size_t size = wcslen(wstr) * 2 + 1;
	char *pStr = new char[size];
	memset(pStr, 0, size);
	size_t destlen = wcstombs(pStr, wstr, size);
	if (destlen == size_t(0))
	{
		return unicode_str;
	}

	unicode_str = pStr;
	delete[] pStr;
	setlocale(LC_ALL, localstr.c_str());
	return unicode_str;
}

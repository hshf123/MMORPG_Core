#pragma once

class StrUtils
{
public:
	static std::wstring ToUpper(const std::wstring& str);
	static std::wstring ToLower(const std::wstring& str);

	static std::wstring ToWideStr(const std::string& str);
	static std::wstring ToWideStr(const CHAR* str);
	static std::wstring ToWideStr(const WCHAR* str);

	static std::string ToString(const std::wstring& str);
	static std::string ToString(const WCHAR* str);
};


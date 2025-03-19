#include "pch.h"
#include "StrUtils.h"
#pragma warning(disable : 4244)

std::wstring StrUtils::ToUpper(const std::wstring& str)
{
	std::wstring ret = str;
	for (int32 i = 0; i < str.size(); i++)
		ret[i] = std::toupper(str[i]);
	return ret;
}

std::wstring StrUtils::ToLower(const std::wstring& str)
{
	std::wstring ret = str;
	for (int32 i = 0; i < str.size(); i++)
		ret[i] = std::tolower(str[i]);
	return ret;
}

std::wstring StrUtils::ToWideStr(const std::string& str)
{
	std::wstring wstr;
	wstr.assign(str.begin(), str.end());
	return wstr;
}

std::wstring StrUtils::ToWideStr(const CHAR* str)
{
	wchar_t buffer[4096] = {};
	auto len = ::MultiByteToWideChar(CP_ACP, 0, str, -1, buffer, sizeof(buffer) / sizeof(wchar_t));
	if (len == 0)
		return L"";
	return buffer;
}

std::wstring StrUtils::ToWideStr(const WCHAR* str)
{
	return ToWideStr(ToString(str));
}

std::string StrUtils::ToString(const std::wstring& str)
{
	char buffer[4096] = {};
	auto len = ::WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, buffer, sizeof(buffer), nullptr, nullptr);
	return buffer;
}

std::string StrUtils::ToString(const WCHAR* str)
{
	char buffer[4096] = {};
	auto len = ::WideCharToMultiByte(CP_ACP, 0, str, -1, buffer, sizeof(buffer), nullptr, nullptr);
	return buffer;
}

std::string StrUtils::ToString(const CHAR* str)
{
	return ToString(ToWideStr(str));
}

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
	// 필요한 버퍼 크기 계산
	int narrowStrLength = static_cast<int>(::strlen(str));
	int bufferSize = MultiByteToWideChar(CP_UTF8, 0, str, narrowStrLength, NULL, 0);

	// WCHAR 버퍼 할당
	wchar_t* wstr = new wchar_t[bufferSize + 1];

	// CHAR를 WCHAR로 변환
	MultiByteToWideChar(CP_UTF8, 0, str, narrowStrLength, wstr, bufferSize);
	wstr[bufferSize] = L'\0'; // NULL 종료 와이드 문자열로 설정

	return wstr;
}

std::wstring StrUtils::ToWideStr(const WCHAR* str)
{
	return ToWideStr(ToString(str));
}

std::string StrUtils::ToString(const std::wstring& str)
{
	std::string ret;
	ret.assign(str.begin(), str.end());
	return ret;
}

std::string StrUtils::ToString(const WCHAR* str)
{
	// 필요한 버퍼 크기 계산
	int32 wideStrLength = static_cast<int32>(::wcslen(str));
	int32 bufferSize = ::WideCharToMultiByte(CP_UTF8, 0, str, wideStrLength, NULL, 0, NULL, NULL);

	// CHAR 버퍼 할당
	CHAR* ret = new CHAR[bufferSize + 1];

	// WCHAR를 CHAR로 변환
	::WideCharToMultiByte(CP_UTF8, 0, str, wideStrLength, ret, bufferSize, NULL, NULL);
	ret[bufferSize] = '\0'; // NULL 종료 문자열로 설정

	return ret;
}

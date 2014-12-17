#include "stdafx.h"
#include <string>
#pragma once

LPSTR Unicode2Ansi(LPCWSTR s);
LPSTR Unicode2UTF8(LPWSTR s);
LPWSTR Ansi2Unicode(LPSTR s);
LPWSTR UTF82Unicode(LPSTR s);


//-----------------------------------------
//read file with encode UTF8 or UTF16 or ANSI
//-----------------------------------------

enum ENCODETYPE 
{
	UNKNOW,
	ANSI,
	UTF8,
	UTF16_big_endian,
	UTF16_little_endian
};

ENCODETYPE TellEncodeType(BYTE* pBuf,int bufLen);
void CleanAfterFileCovert(BYTE* pBufOld,BYTE *pBufNew);
void CovertFileBuf2UTF16littleEndian(BYTE* pBuf,int bufLen,ENCODETYPE filetype,OUT TCHAR **pBufU,OUT int &filesizeAfterCovert);
int MyGetLine(TCHAR *pBuf,int bufLen,std::wstring &str);

int StringCmpNoCase(std::tstring a,std::tstring b);
int StrCmpIgnoreCaseAndSpace(std::tstring a,std::tstring b);
int hex2dec(char c);


void trimLeftAndRightSpace(std::tstring &str);
void TrimRightByNull(std::wstring &_str);




long Conv(int i) ;
void CreateQianQianCode(char *id,char *ar,char*ti,std::string &code);
std::string str2UnicodeCode(const WCHAR *c,int len);

LONG GetStrSizeX(HDC dc,TCHAR *str);
//void MakeShortString(HDC dc,TCHAR *str,long width);


//void DrawTriangleInRect(HDC dc,RECT &rc);



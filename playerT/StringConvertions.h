#include "stdafx.h"
#include <string>
#pragma once

LPSTR Unicode2Ansi(LPCWSTR s);
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


#include "StringConvertions.h"
#include <algorithm>

LPSTR Unicode2Ansi(LPCWSTR s)
{
	DWORD dwNum= WideCharToMultiByte (CP_ACP, 0, s, -1, NULL, 0,0,0);
	LPSTR target=new char[dwNum];
	WideCharToMultiByte(CP_ACP,0,s,-1,target,dwNum,0,0);
	return target;
}

LPWSTR Ansi2Unicode(LPSTR s)
{
	DWORD dwNum= MultiByteToWideChar (CP_ACP, 0,(LPCSTR) s, -1, NULL, 0);
	LPWSTR target=new WCHAR[dwNum];
	MultiByteToWideChar(CP_ACP,0,(LPCSTR)s,-1,target,dwNum);
	return target;
}

LPWSTR UTF82Unicode(LPSTR s)
{
	DWORD dwNum = MultiByteToWideChar (CP_UTF8, 0, s, -1, NULL, 0);
	LPWSTR target=new WCHAR[dwNum];
	MultiByteToWideChar(CP_UTF8,0,(LPCSTR)s,-1,target,dwNum);
	return target;
}


ENCODETYPE TellEncodeType(BYTE* pBuf,int bufLen)
{
	ENCODETYPE filetype=ANSI;

	if (pBuf[0]==0xFF && pBuf[1]==0xFE) //fffe,小头,windows默认
		filetype=UTF16_little_endian;
	else if(pBuf[0]==0xFE && pBuf[1]==0xFF )
		filetype=UTF16_big_endian;
	else
	{
		int    utf8Nums=0;    //符合UTF8编码的字符个数,非Ansi部分
		int count=0;
		while(count<bufLen-2)
		{
			int i=0;
			while( i<bufLen-2-count)
			{
				if (pBuf[count+i]>0xC0)
				{
					if (pBuf[count+i+1]<0x80 || pBuf[count+i+1]>0xC0)
					{
						filetype=ANSI;
						break;
					}
					else
					{
						/*
						The transformation table for UTF-8 is presented below:
						UNICODE 	            UTF-8
						00000000 - 0000007F 	0xxxxxxx
						00000080 - 000007FF 	110xxxxx 10xxxxxx
						00000800 - 0000FFFF 	1110xxxx 10xxxxxx 10xxxxxx                          //0xE0
						00010000 - 001FFFFF 	11110xxx 10xxxxxx 10xxxxxx 10xxxxxx                 //0xF0
						00200000 - 03FFFFFF  	111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx        //0xF8
						04000000 - 7FFFFFFF 	1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx   //0xFC
						*/

						BYTE *tC;//target Char to 识别

						int utfStrLen=0;
						//是否为正确的UTF8格式
						tC=pBuf+count+i;

						if (tC[0]<=0xF0)//<4字节
							if(tC[0]>0xE0)    utfStrLen=3;
							else              utfStrLen=2;	
						else if (tC[0]>=0xFC) utfStrLen=6;
						else if(tC[0]>=0xF8)  utfStrLen=5;
						else                  utfStrLen=4;

						int k=1;
						while(k<utfStrLen)
						{
							if (!(tC[k] & 0x80 && !(tC[k]& 0x40) ))//前二位为10
							{
								filetype=ANSI;
								break;
							}

							k++;
						}

						if (k==utfStrLen)
							utf8Nums++;
						if (utf8Nums==10)
							filetype=UTF8;
					}
				} 

				i++;
			}//while( i<bufLen-2-count)

			++count;
		}//while(count<bufLen-2)

	}//else


	return filetype;
}

void CleanAfterFileCovert(BYTE* pBufOld,BYTE *pBufNew)
{
	if (pBufNew==pBufOld+2)
	{
		delete[] pBufOld;
	}
	else
	{
		delete[] pBufOld;
		delete[] pBufNew;
	}
}

																								          //number of TCHAR 's
void CovertFileBuf2UTF16littleEndian(BYTE* pBuf,int bufLen,ENCODETYPE filetype,OUT TCHAR **pBufU,OUT int &filesizeAfterCovert)
{
	*pBufU=NULL;//UTF16
	filesizeAfterCovert=0;

	if (filetype==UTF16_little_endian)
	{
		*pBufU=(TCHAR*)(pBuf+2);
		filesizeAfterCovert=(bufLen-2)/sizeof(TCHAR);
	}
	else if (filetype==UTF16_big_endian)
	{
		if(bufLen%2==0)
		{		
			BYTE tmp;
			for (int i=2;i<bufLen;++(++i))
			{
				tmp=pBuf[i];
				pBuf[i]=pBuf[i+1];
				pBuf[i+1]=tmp;
			}

			*pBufU=(TCHAR*)(pBuf+2);
			filesizeAfterCovert=(bufLen-2)/sizeof(TCHAR);
		}
	}
	else if (filetype==ANSI)
	{
		*pBufU=Ansi2Unicode((LPSTR)pBuf);
		filesizeAfterCovert=_tcslen(*pBufU);
	}
	else if (filetype==UTF8)
	{
		*pBufU=UTF82Unicode((LPSTR)pBuf);
		filesizeAfterCovert=_tcslen(*pBufU);
	}


}

int MyGetLine(TCHAR *pBuf,int bufLen,std::wstring &str)
{
	str.erase();
	int i=0;
	for (;i<bufLen;i++)
	{
		if(pBuf[i]==0x000d)//\r\n  CR LF
		{
			if(i+1 <=bufLen && pBuf[i+1]==0x000a)
				return i+2 ;
		}
		else if (pBuf[i]==0x000a) //\n  LF
		{
			return i+1;
		}
		else
		{
			str+=pBuf[i];
		}
	}

	return i;
}


int StringCmpNoCase(std::tstring a,std::tstring b)
{
	_tcsupr(const_cast<TCHAR*>(a.c_str()) );
	_tcsupr(const_cast<TCHAR*>(b.c_str()));

	return _tcscmp(a.c_str(),b.c_str());
}


int hex2dec(char c)
{
	int ret=-1;
	if (c >= 'a' && c<='z')	
		ret= c - 'a'+ 10;
	else if (c >= 'A' && c<='Z')				
		ret=c - 'A'+ 10;
	else if (c >= '0' && c<='9')
		ret= c - '0' ;

	return ret;
}
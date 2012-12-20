#include "StringConvertions.h"
#include <algorithm>

LPSTR Unicode2UTF8(LPWSTR s)
{
	DWORD dwNum= WideCharToMultiByte (CP_UTF8, 0, s, -1, NULL, 0,0,0);
	LPSTR target=new char[dwNum];
	WideCharToMultiByte(CP_UTF8,0,s,-1,target,dwNum,0,0);
	return target;
}

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

void trimLeftAndRightSpace(std::tstring &str)
{
	char c=' ';
	int index=str.find_first_not_of(c);
	if (index!=str.npos && index!=0)
		str.erase(0,index);

	index=str.find_last_not_of(c);
	if(index!=str.npos)
		str.erase(index+1);
}

int StrCmpIgnoreCaseAndSpace(std::tstring a,std::tstring b)
{
	_tcsupr(const_cast<TCHAR*>(a.c_str()) );
	_tcsupr(const_cast<TCHAR*>(b.c_str()));

	trimLeftAndRightSpace(a);
	trimLeftAndRightSpace(b);
	
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

void TrimRightByNull(std::wstring &_str)
{
	char c='\0';
	int index=_str.find_first_of(c);
	if (index!=_str.npos)
		_str=_str.substr(0,index);	
}




long Conv(int i) 
{
	long r = i % 4294967296;
	if (i >= 0 && r > 2147483648)
		r -= 4294967296;

	if (i < 0 && r < 2147483648)
		r += 4294967296;
	return r;
}

void CreateQianQianCode(char *id,char *ar,char*ti,std::string &code)
{
	int iId=atoi(id);
	std::string arti(ar);
	arti+=ti;

	int length=arti.length();
	int *song=new int[length];
	for (int i=0;i<length;++i)
	{
		char *ak=(char*)(arti.c_str()+i);
		char  akk=*ak;
		song[i]=((int)akk) & 0x000000FF;	}
	int t1=0,t2=0,t3=0;

	t1 = (iId & 0x0000FF00) >> 8;

	if ((iId & 0x00FF0000) == 0)
		t3 = 0x000000FF & ~t1;
	else
		t3 = 0x000000FF & ((iId & 0x00FF0000) >> 16);

	t3 |= (0x000000FF & iId) << 8;
	t3 <<= 8;
	t3 |= 0x000000FF & t1;
	t3 <<= 8;

	if ((iId & 0xFF000000) == 0)
		t3 |= 0x000000FF & (~iId);
	else
		t3 |= 0x000000FF & (iId >> 24);

	int j = length - 1;
	while (j >= 0)
	{
		int c = song[j];
		if (c >= 0x80) c = c - 0x100;

		t1 = (int)((c + t2) & 0x00000000FFFFFFFF);
		t2 = (int)((t2 << (j % 2 + 4)) & 0x00000000FFFFFFFF);
		t2 = (int)((t1 + t2) & 0x00000000FFFFFFFF);
		j -= 1;
	}


	j = 0;
	t1 = 0;
	while (j <= length - 1) 
	{
		int c = song[j];
		if (c >= 128) c = c - 256;
		int t4 = (int)((c + t1) & 0x00000000FFFFFFFF);
		t1 = (int)((t1 << (j % 2 + 3)) & 0x00000000FFFFFFFF);
		t1 = (int)((t1 + t4) & 0x00000000FFFFFFFF);
		j += 1;
	}

	int t5 = (int)Conv(t2 ^ t3);
	t5 = (int)Conv(t5 + (t1 | iId));
	t5 = (int)Conv(t5 * (t1 | t3));
	t5 = (int)Conv(t5 * (t2 ^ iId));


	long t6 = (long)t5;
	if (t6 > 2147483648)
		t5 = (int)(t6 - 4294967296);

	char tmp[20]={0};
	_itoa(t5,tmp,10);
	code=tmp;
}


std::string str2UnicodeCode(const WCHAR *c,int len)
{
	std::string tmp;

	for (int i=0;i<len;++i)
	{
		static const WCHAR ign[]=L" ,./<>?`~!@#$%^&*()-_=+\\|[]{};':\"";
		if(_tcschr(ign,c[i])!=0)
			continue;

		char t[10]={0};
		_itoa((int)c[i],t,16);

		bool bAnsi=false;
		if (t[2]=='\0')
		{
			t[2]='0';
			bAnsi=true;
		}
		if (t[3]=='\0')
		{
			t[3]='0';
			bAnsi=true;
		}


		std::string ttmp(t,t+2);
		std::string tttmp1(t+2,t+4);
		if (bAnsi)
		{
			tmp+=ttmp;
			tmp+=tttmp1;
		}else{
			tmp+=tttmp1;
			tmp+=ttmp;
		}
	}

	return tmp;
}

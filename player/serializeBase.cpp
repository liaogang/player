#include "stdafx.h"
#include "serializeBase.h"

using namespace std;






//double
FILE& operator<<(FILE& f, const double t)
{
	fwrite(&t, sizeof(double), 1, &f);
	return f;
}

FILE& operator>>(FILE& f, double& t)
{
	fread(&t, sizeof(double), 1, &f);
	return f;
}
//int
FILE& operator<<(FILE& f, const int t)
{
	fwrite(&t, sizeof(int), 1, &f);
	return f;
}

FILE& operator>>(FILE& f, int& t)
{
	fread(&t, sizeof(int), 1, &f);
	return f;
}

//UINT
FILE& operator<<(FILE& f, const UINT t)
{
	fwrite(&t, sizeof(UINT), 1, &f);
	return f;
}

FILE& operator>>(FILE& f, UINT& t)
{
	fread(&t, sizeof(UINT), 1, &f);
	return f;
}


//long
FILE& operator<<(FILE& f, const long t)
{
	fwrite(&t, sizeof(long), 1, &f);
	return f;
}

FILE& operator>>(FILE& f, long& t)
{
	fread(&t, sizeof(long), 1, &f);
	return f;
}



//write zero terminated str array
FILE& operator<<(FILE& f, const TCHAR * str)
{
	int l = _tcslen(str) + 1;
	f << l;
	fwrite(str, sizeof(TCHAR), l, &f);
	return f;
}

FILE& operator>>(FILE& f, TCHAR * str)
{
	int l = 0;
	f >> l;
	fread(str, sizeof(TCHAR), l, &f);
	return f;
}

//tstring
FILE& operator<<(FILE& f, const tstring &str)
{
	int l = str.length();
	f << l + 1;
	fwrite(str.c_str(), sizeof(TCHAR), l, &f);
	TCHAR nullstr = '\0';
	fwrite(&nullstr, sizeof(TCHAR), 1, &f);
	return f;
}

FILE& operator>>(FILE& f, tstring &str)
{
	TCHAR buf[256];
	f >> buf;
	str = buf;
	return f;
}



//write zero terminated str array
FILE& operator<<(FILE& f, const char* str)
{
	int l = (int)strlen(str) + 1;
	f << l;
	fwrite(str, sizeof(char), l, &f);
	return f;
}

FILE& operator>>(FILE& f, char* str)
{
	int l = 0;
	f >> l;
	fread(str, sizeof(char), l, &f);
	return f;
}

//tstring
FILE& operator<<(FILE& f, const string &str)
{
	int l = (int)str.length();
	f << l + 1;
	fwrite(str.c_str(), sizeof(char), l, &f);
	char nullstr = '\0';
	fwrite(&nullstr, sizeof(char), 1, &f);
	return f;
}

FILE& operator>>(FILE& f, string &str)
{
	char buf[256];
	f >> buf;
	str = buf;
	return f;
}



//RECT
FILE& operator<<(FILE& f, const RECT &r)
{
	return f << r.left << r.top << r.right << r.bottom;
}

FILE& operator>>(FILE& f, RECT &r)
{
	return f >> r.left >> r.top >> r.right >> r.bottom;
}


//POINT
FILE& operator<<(FILE& f, const POINT &p)
{
	return f << p.x << p.y;
}

FILE& operator>>(FILE& f, POINT &p)
{
	return f >> p.x >> p.y;
}



//DWORD
FILE& operator<<(FILE& f, const DWORD t)
{
	fwrite(&t, sizeof(DWORD), 1, &f);
	return f;
}

FILE& operator>>(FILE& f, DWORD& t)
{
	fread(&t, sizeof(DWORD), 1, &f);
	return f;
}

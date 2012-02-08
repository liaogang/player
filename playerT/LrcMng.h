#ifndef LRCMNG_H
#define LRCMNG_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class LrcTime
{
public:
	LrcTime(UINT min,UINT sec)
		:minute(min),second(sec)
	{
	}
	UINT minute;
	UINT second;
};

class LrcLine{
public:
	LrcLine(UINT min,UINT sec,tstring text)
		:time(min,sec),text(text)
	{}
	LrcTime time;
	tstring text;
	/*return second between*/
	int operator - (LrcLine& r)
	{
		return (this->time.minute-r.time.minute)*60+(this->time.second-r.time.second);
	}
};




class LrcMng
{
private:
	vector<LrcLine> lib;
	void SortLrcLib();
	void InsertIntoLib(UINT minute,UINT second,tstring& pLine)
	{
		LrcLine temp(minute,second,pLine);
		lib.push_back(temp);
	}
public:
	LrcMng(void);
	static LrcMng& Get();

public:
	void Open(LPTSTR pstrPath)
	{ 
		std::locale loc1 = std::locale::global(std::locale(".936"));
		tifstream fin(pstrPath);
		tstring s;
		while(getline(fin,s))
		{
			Parse(s);
		}
		SortLrcLib();
		fin.close();
		std::locale::global(std::locale(loc1));
	}




private:
	void Parse(tstring& s)
	{
		static bool shouldparseideinfo=true;

		tstring *pLine=new tstring;
		tstring temp;
		bool Parsing=false;
		for (int i=0;i<s.length();i++)
		{
			if (s[i]=='[')
			{
				Parsing=true;
				continue;
			}
			if (s[i]==']')
			{
				if (Parsing)//if has '[' before
				{
					Parsing=false;//匹配成功
					continue;
				}
			}

			if (Parsing)    //收集标签信息,如00:00 00:11 00:18 
				temp+=s[i];
			else            //收集歌词信息
			(*pLine)+=s[i];
		}

		if (shouldparseideinfo)
			shouldparseideinfo=parseIDEinfo(temp);
		if (!shouldparseideinfo)
			parseTimeInfo(temp,pLine);
	}



	bool parseIDEinfo(tstring& temp)
	{
		const tstring torken_[]={_T("id"),_T("encoding"),_T("ti"),_T("ar"),_T("al"),_T("by")};

		for (int i=0;i<6;i++)
		{
			tstring::iterator f;
			f=find_end(temp.begin(),temp.end(),torken_[i].begin(),torken_[i].end());
			if (f!=temp.end())
			{
				return true;
			}
		}
		return false;
	}

	void parseTimeInfo(tstring& temp,tstring* pLine)
	{
		int length=temp.length();

		int timeInfoNum=length/5;

		LPCTSTR pbuf=temp.c_str();
		int minute=-2,second=-2;

		for (int i=0;i<timeInfoNum;i++)
		{
			_stscanf(pbuf,_T("%2d:%2d"),&minute,&second);
			InsertIntoLib(minute,second,*pLine);
			pbuf+=5;
		}
	}


public:
	~LrcMng(void);
};

#endif

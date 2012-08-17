/*
无论是否在行首，行内凡具有“[*:*]”形式的都应认为是标签
凡是标签都不应显示
凡是标签，且被冒号分隔的两部分都为非负数 ，则应认为是时间标签  (0或正数)
对于非标准形式的时间标签也应能识别（如[0:0]）  
凡是标签，且非时间标签的，应认为是标识标签
标识名中大小写等价
为了向后兼容 ，应对未定义的新标签作忽略处理
应对注释标签（[:]）后的同一行内容作忽略处理
应允许一行中存在多个标签，并能正确处理 
应能正确处理未排序的标签。
*/




#ifndef LRCMNG_H
#define LRCMNG_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include "StringConvertions.h"

using namespace std;
class PlayListItem;


enum TagType
{
	InvalidTag,  //不是标签      非[*:*]
	TimeTag,     //时间标签      是[*:*]  --->[非负数:非负数]
	IdentifyTag, //标识标签      是[*:*]  --->非时间标签
	NoteTag      //注释标签      是[*:*]  --->标识标签--->两端为空
};



struct TagTypeInfo
{
	TagType tag;

	UINT minute;
	UINT second;                                                                                                                                                                                                                                                                                                                                                                                                                                 
	std::tstring identity;
};


class LrcTime
{
public:
	LrcTime(UINT min,UINT sec)
		:minute(min),second(sec)
	{
	}
	UINT minute;
	UINT second;

	UINT GetTotalSec(){return minute*60+second;}
};


class LrcLine
{
public:
	LrcTime time;
	std::tstring text;

	LrcLine(UINT min,UINT sec,std::tstring text)
		:time(min,sec),text(text){}
	int operator - (LrcLine& r){return (time.minute-r.time.minute)*60+(time.second-r.time.second);}
};

typedef vector<LrcLine> LrcLines;
class LrcMng
{
public:
	vector<LrcLine> lib;
	void SortLrcLib();
	void InsertIntoLib(UINT minute,UINT second,std::tstring& pLine)
	{
		LrcLine temp(minute,second,pLine);
		lib.push_back(temp);
	}
public:
	LrcMng(void);
	static LrcMng* Get();

public:
	BOOL OpenTrackPath(PlayListItem* track);
	
	void Open(LPTSTR pstrPath)
	{ 
		lib.clear();
		

		ENCODETYPE filetype=UNKNOW;
		BYTE * pBuf;
		FILE * pFile;
		int filesize;
		pFile = _tfopen( pstrPath, _T("rb") );
		if (pFile!=NULL)
		{
			//get the file size
			fseek(pFile,0,SEEK_END);
			filesize=ftell(pFile);
			filesize+=sizeof(TCHAR);
			pBuf=(BYTE*)malloc(filesize);
			memset(pBuf+filesize-sizeof(TCHAR),0,sizeof(TCHAR));

			fseek(pFile,0,SEEK_SET);
			fread(pBuf,1,filesize-sizeof(TCHAR),pFile);

			filetype=TellEncodeType(pBuf,filesize);
			fclose (pFile);
		}



		TCHAR *pBufU;
		INT   filesizeAfterCovert=0;
		CovertFileBuf2UTF16littleEndian(pBuf,filesize,filetype,&pBufU,filesizeAfterCovert);


		int bufUsed=0;
		std::wstring s;
		while(filesizeAfterCovert>(bufUsed+=MyGetLine(pBufU+bufUsed,filesizeAfterCovert-bufUsed,s)))
			Parse(s);
		
		CleanAfterFileCovert(pBuf,(BYTE*)pBufU);
		SortLrcLib();
	}


private:
	void Parse(std::tstring& s)
	{
		TagTypeInfo tagInfo;
		tagInfo.tag=InvalidTag;

		vector<UINT> miniteList,secondList;

		std::tstring::iterator first;
		std::tstring::iterator last;

		for( first=s.begin(), last=s.end() ;
			PrepareFindTag(first,last) ;	
			s.erase(--first,++last),first=s.begin(),last=s.end()
			)
		{
			tagInfo=FindTagType(++first,last);
			if(tagInfo.tag==TimeTag){
				//save ,add to lib later
				miniteList.push_back(tagInfo.minute);
				secondList.push_back(tagInfo.second);
			}
			else
				break;
		}

		if (tagInfo.tag==TimeTag){
			vector<UINT>::iterator k,j;
			for (k=miniteList.begin(),j=secondList.begin();k!=miniteList.end();k++,j++)
				InsertIntoLib(*k,*j,s);
		}
	}

	//找到[],返回]的指针
	BOOL PrepareFindTag(std::tstring::iterator &v1,std::tstring::iterator &v2)
	{
		std::tstring::iterator first,last;
		first=find(v1,v2,'[');
		last=find(v1,v2,']');
		if (last-first>0 && last!= v2){  //find [] succeed
			v1=first;
			v2=last;
			return TRUE;
		}

		return FALSE;
	}


public:
	~LrcMng(void);

	//是否是非负数,00,或12.23
	static BOOL IsNaturalNumber(std::tstring::iterator v1,std::tstring::iterator v2)
	{
		for ( ; (isdigit(*v1)|| (*v1)=='.') && v1!=v2 ; v1++);
		return v1==v2;
	}


	//返回标签类型
	TagTypeInfo FindTagType(std::tstring::iterator v1,std::tstring::iterator &v2)
	{
		TagTypeInfo tagInfo;
		std::tstring::iterator i;

		i=find(v1,v2,':');                      //invalid tag?
		if(i==v2)	
			tagInfo.tag=InvalidTag;
		else  if( IsNaturalNumber(v1,i)           //time tag?
			&& IsNaturalNumber(++i,v2) )
		{
			std::tstring tmp(v1,v2);
			LPCTSTR pbuf=tmp.c_str();
			_stscanf(pbuf,_T("%2d:%2d"),&tagInfo.minute,&tagInfo.second);
			tagInfo.tag=TimeTag;		
		}
		else 
			tagInfo.tag=IdentifyTag;

		return tagInfo;
	}
};

#endif

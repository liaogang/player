/*
�����Ƿ������ף����ڷ����С�[*:*]����ʽ�Ķ�Ӧ��Ϊ�Ǳ�ǩ
���Ǳ�ǩ����Ӧ��ʾ
���Ǳ�ǩ���ұ�ð�ŷָ��������ֶ�Ϊ�Ǹ��� ����Ӧ��Ϊ��ʱ���ǩ  (0������)
���ڷǱ�׼��ʽ��ʱ���ǩҲӦ��ʶ����[0:0]��  
���Ǳ�ǩ���ҷ�ʱ���ǩ�ģ�Ӧ��Ϊ�Ǳ�ʶ��ǩ
��ʶ���д�Сд�ȼ�
Ϊ�������� ��Ӧ��δ������±�ǩ�����Դ���
Ӧ��ע�ͱ�ǩ��[:]�����ͬһ�����������Դ���
Ӧ����һ���д��ڶ����ǩ��������ȷ���� 
Ӧ����ȷ����δ����ı�ǩ��
*/




#ifndef LRCMNG_H
#define LRCMNG_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include "StringConvertions.h"
#include "PlayList.h"
using namespace std;



enum TagType
{
	InvalidTag,  //���Ǳ�ǩ      ��[*:*]
	TimeTag,     //ʱ���ǩ      ��[*:*]  --->[�Ǹ���:�Ǹ���]
	IdentifyTag, //��ʶ��ǩ      ��[*:*]  --->��ʱ���ǩ
	NoteTag      //ע�ͱ�ǩ      ��[*:*]  --->��ʶ��ǩ--->����Ϊ��
};



struct TagTypeInfo
{
	TagType tag;

	UINT minute;
	UINT second;
	UINT msec;

	std::tstring identity1;
	std::tstring identity2;
};


class LrcTime
{
public:

	LrcTime(UINT min,UINT sec)
		:minute(min),second(sec),msec(0)
	{
	}

	LrcTime(UINT min,UINT sec,UINT ms)
		:minute(min),second(sec),msec(ms)
	{
		
	}

	UINT minute;
	UINT second;
	UINT msec;   //100��֮һ��,���Ǻ���

	inline UINT GetTotalMillisec(){return (minute*60 +second)*1000 + msec*10;}
};


class LrcLine
{
public:
	LrcTime time;
	std::tstring text;

	LrcLine(UINT min,UINT sec,UINT msec,std::tstring text)
		:time(min,sec,msec),text(text){}
	int operator - (LrcLine& r)
	{
		return (time.minute-r.time.minute)*600+(time.second-r.time.second)*100+ (time.msec-r.time.msec);
	}

	int cmp(LrcLine &r)
	{
		int ret=0;
		if (time.minute==r.time.minute)
		{
			if (time.second==r.time.second)
			{
				if (time.msec!=r.time.msec)
					ret=time.msec-r.time.msec;
			}
			else
				ret=time.second-r.time.second;
		}
		else
			ret=time.minute-r.time.minute;
		
		return ret;
	}
};
const std::tstring strEmpty;
const LrcLine lineEmpty(0,0,0,strEmpty);
typedef vector<LrcLine> LrcLines;

class LrcMng
{
public:
	wstring ti,ar,al,by,id;
	int offset;
public:
	vector<LrcLine> lib;
	void SortLrcLib();
	void InsertIntoLib(UINT minute,UINT second,UINT msec,std::tstring& pLine)
	{
		LrcLine temp(minute,second,msec,pLine);
		lib.push_back(temp);
	}
public:
	LrcMng(void);
	static LrcMng* Get();
	

public:
	UINT MatchTrackAndLrcTags(CPlayListItem * track,std::tstring &lrcpath);
	void Clear()
	{
		lib.clear();
		ti.clear();
		ar.clear();
		al.clear();
		by.clear();
		id.clear();
		offset=0;
	}

	BOOL Open(LPTSTR pstrPath)
	{ 
		Clear();
		
		ENCODETYPE filetype=UNKNOW;
		BYTE * pBuf;
		FILE * pFile;
		int filesize;
		_wfopen_s(&pFile , pstrPath, _T("rb") );
		
		if (!pFile)
		{
			return FALSE;
		}
		else
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
		do
		{
			bufUsed+=MyGetLine(pBufU+bufUsed,filesizeAfterCovert-bufUsed,s);
			Parse(s);
		}while(filesizeAfterCovert > bufUsed);
		
		
		CleanAfterFileCovert(pBuf,(BYTE*)pBufU);
		SortLrcLib();


		return lib.size()>0;
	}


private:
	void Parse(std::tstring& s)
	{
		TagTypeInfo tagInfo;
		tagInfo.tag=InvalidTag;

		vector<UINT> miniteList,secondList,msecList;

		std::tstring::iterator first;
		std::tstring::iterator last;

		for( first=s.begin(), last=s.end() ;
			PrepareFindTag(first,last) ;	
			s.erase(--first,++last),first=s.begin(),last=s.end())
		{
			tagInfo=FindTagType(++first,last);
			if(tagInfo.tag==TimeTag){
				//save ,add to lib later
				miniteList.push_back(tagInfo.minute);
				secondList.push_back(tagInfo.second);
				msecList.push_back(tagInfo.msec);
			}
			else if(tagInfo.tag==IdentifyTag)
			{
				break;
			}
		}

		if (tagInfo.tag==TimeTag){
			vector<UINT>::iterator k,j,q;
			for (k=miniteList.begin(),j=secondList.begin(),q=msecList.begin();
				k!=miniteList.end();
				k++,j++,q++)
				InsertIntoLib(*k,*j,*q,s);
		}
	}

	//�ҵ�[],����]��ָ��
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

	//�Ƿ��ǷǸ���,00,��12.23
	static BOOL IsNaturalNumber(std::tstring::iterator v1,std::tstring::iterator v2)
	{
		for ( ; (isdigit(*v1)|| (*v1)=='.') && v1!=v2 ; v1++);
		return v1==v2;
	}


	//���ر�ǩ����
	TagTypeInfo FindTagType(std::tstring::iterator v1,std::tstring::iterator &v2)
	{
		TagTypeInfo tagInfo;
		std::tstring::iterator i,i2;

		i=find(v1,v2,':');                      //invalid tag?
		i2=find(v1,v2,'.');

		if(i==v2)	
			tagInfo.tag=InvalidTag;
		else  if( IsNaturalNumber(v1,i)           //time tag?
			&& IsNaturalNumber(++i,v2) )
		{
			tagInfo.tag=TimeTag;		
			std::tstring tmp(v1,v2);
			LPCTSTR pbuf=tmp.c_str();
			swscanf_s(pbuf,_T("%2d:%2d"),&tagInfo.minute,&tagInfo.second);

			 if(i2!=v2)
				tagInfo.msec= _wtoi(pbuf+(i2-v1));
			 else
				 tagInfo.msec=0;
		}
		else 
		{
			std::tstring tmp1(v1,i);
			std::tstring tmp2(++i,v2);
			tagInfo.tag=IdentifyTag;
			tagInfo.identity1=tmp1;
			tagInfo.identity2=tmp2;
			CollectIdentifyTag(tagInfo);
		}

		return tagInfo;
	}




	void CollectIdentifyTag(TagTypeInfo &tagInfo)
	{
		static const WCHAR *strIdendity[]={
			L"ti",L"ar",L"al",L"by",L"id",L"offset"
		};
		enum{
			IDD_TI=0,
			IDD_AR,
			IDD_AL,
			IDD_BY,
			IDD_ID,
			IDD_OFFSET
		};


		


		int len= tagInfo.identity1.length();
		//int len1=sizeof(strIdendity)/sizeof(int);
		
		const TCHAR *p2=tagInfo.identity1.c_str();
		if(0==_tcsncmp(p2,strIdendity[IDD_TI],len))
		{
			ti=tagInfo.identity2;
		}
		else if(0==_tcsncmp(p2,strIdendity[IDD_AR],len))
		{
			ar=tagInfo.identity2;
		}	
		else if(0==_tcsncmp(p2,strIdendity[IDD_AL],len))
		{
			al=tagInfo.identity2;
		}		
		else if(0==_tcsncmp(p2,strIdendity[IDD_BY],len))
		{
			by=tagInfo.identity2;
		}		
		else if(0==_tcsncmp(p2,strIdendity[IDD_ID],len))
		{
			id=tagInfo.identity2;
		}
		else if(0==_tcsncmp(p2,strIdendity[IDD_OFFSET],len))
		{
			offset=_wtoi(tagInfo.identity2.c_str());
		}

	}
};





#endif

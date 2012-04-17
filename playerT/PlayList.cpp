#include "StdAfx.h"
#include "PlayList.h"
#include "MusicFile.h"
#include "WaveFileEx.h"
#include "Mp3File.h"
#include <direct.h>
#include <id3/tag.h>

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

int win32_utf8_wide(const char *const mbptr, wchar_t **wptr, size_t *buflen)
{
	size_t len;
	wchar_t *buf;
	int ret = 0;

	len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, mbptr, -1, NULL, 0); /* Get converted size */
	buf = (wchar_t*)calloc(len + 1, sizeof (wchar_t)); /* Allocate memory accordingly */

	if(!buf) len = 0;
	else {
		if (len != 0) ret = MultiByteToWideChar (CP_UTF8, MB_ERR_INVALID_CHARS, mbptr, -1, buf, len); /* Do conversion */
		buf[len] = L'0'; /* Must terminate */
	}
	*wptr = buf; /* Set string pointer to allocated buffer */
	if (buflen != NULL) *buflen = len * sizeof (wchar_t); /* Give length of allocated memory if needed. */
	return ret; /* Number of characters written */
}


PlayList& MyLib::GetPlayListObj()
{
	static PlayList playList_;
	return playList_;
}



PlayList::PlayList(void)
{
}

PlayList::~PlayList(void)
{
}

void PlayList::AddFolderToCurrentPlayList(LPCTSTR pszFolder)
{
	MyLib::GetPlayListObj().AddFolder(pszFolder);
}

void PlayList::scanAllId3Info()
{

}

BOOL PlayList::AddFolder(LPCTSTR pszFolder)
{
	// for each mp3 file in this folder
	// data . add

	//忽略了子目录下的mp3文件

	//改变当前目录
	_tchdir(pszFolder);

	WIN32_FIND_DATA findFileData;
	HANDLE hFind;

	hFind=::FindFirstFile(_T("*.mp3"),&findFileData);

	if(hFind!=INVALID_HANDLE_VALUE)
	{
		if (1)//(findFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
		{
			TCHAR *_findName=new TCHAR[MAX_PATH];
			_tcscpy(_findName,findFileData.cFileName);
			TCHAR *pathName=new TCHAR[MAX_PATH+1];
			_tcscpy(pathName,pszFolder);
			_tcscat(pathName,_T("\\"));


			_tcscat(pathName,_findName);
			tstring str(pathName);
			//m_songList.push_back(str);

			PlayListItem item(&str);
			item.scanId3Info();
			m_songList.push_back(item);
		}

		BOOL ret;
		while(ret=FindNextFile(hFind,&findFileData))
		{
			if (1)//(!(findFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
			{
				TCHAR *_findName=new TCHAR[MAX_PATH];
				_tcscpy(_findName,findFileData.cFileName);
				TCHAR *pathName=new TCHAR[MAX_PATH+1];
				_tcscpy(pathName,pszFolder);
				_tcscat(pathName,_T("\\"));
				_tcscat(pathName,_findName);
				tstring str(pathName);
				//m_songList.push_back(str);

				PlayListItem item(&str);
				item.scanId3Info();
				m_songList.push_back(item);



			}
		}
	}
	FindClose(hFind);

	return TRUE;
}

LPTSTR id3Tag_get(ID3_Tag* tag,ID3_FrameID id)
{
	ID3_Frame *frame=NULL;
	ID3_Field *field=NULL;
	frame=tag->Find(id);
	if (frame)
	{
		field=frame->GetField(ID3FN_TEXT);
		if (field)
		{
			char *tmp;int len;
 			tmp=(char*) field->Get_Text(&len);
			if ( ID3TE_IS_DOUBLE_BYTE_ENC(field->GetEncoding()))
			{
				WCHAR *tmpW=new WCHAR[len/2];
				memcpy(tmpW,tmp,len);
				return tmpW;
			}
			else	
				return Ansi2Unicode(tmp);
		}
	}
	return NULL;
}


BOOL PlayListItem::scanId3Info()
{
// 	MusicFile *f=new Mp3File;
// 	f->OpenAndReadID3Info((LPTSTR)url.c_str());
// 
// 	mpg123_id3v1* v1=f->m_pMpg123_id3v1;
// 	mpg123_id3v2* v2=f->m_pMpg123_id3v2;
// 
// 	if (v2)
// 	{
// 		TCHAR *tmp1;size_t tmpLen;
// 		if(v2 && v2->title && v2->title->p)
// 		{
// 			win32_utf8_wide(v2->title->p,&tmp1,&tmpLen);
// 			title=tmp1;
// 		}
// 	}
// 	else if (v1)
// 	{
// 
// 	}





	//-----------------------------------------
	ID3_Tag tag;
	ID3_Frame *frame=NULL;
	ID3_Field *field=NULL;

	std::locale loc1 = std::locale::global(std::locale(".936"));
	tag.Link(Unicode2Ansi(url.c_str()),ID3TT_ALL);
	std::locale::global(std::locale(loc1));

	if (tag.HasV2Tag())
	{
		title=id3Tag_get(&tag,ID3FID_TITLE);
		artist=id3Tag_get(&tag,ID3FID_LEADARTIST);
		album=id3Tag_get(&tag,ID3FID_ALBUM);
		year=id3Tag_get(&tag,ID3FID_YEAR);
		comment=id3Tag_get(&tag,ID3FID_COMMERCIAL);

		frame=tag.Find(ID3FID_PICTURE);
		if (frame && frame->Contains(ID3FN_DATA))
		{
			frame->Field(ID3FN_DATA).ToFile("C:\\abc.pic");
		}
	}
	else if(tag.HasV1Tag())
	{
		title=id3Tag_get(&tag,ID3FID_TITLE);
		artist=id3Tag_get(&tag,ID3FID_LEADARTIST);
		album=id3Tag_get(&tag,ID3FID_ALBUM);
		year=id3Tag_get(&tag,ID3FID_YEAR);
		comment=id3Tag_get(&tag,ID3FID_COMMERCIAL);
	}
	
	//---------------------------------------
	return TRUE;
}
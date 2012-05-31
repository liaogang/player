#include "MySerialize.h"
#include "MyLib.h"


template <class T>
int Serialize (FILE *pFile,T t)
{
	return 0;
};

template <>
int Serialize (FILE *pFile,UINT t)
{
	fwrite(&t,1,4,pFile);
	return 4;
};


template <>
int Serialize (FILE *pFile,std::tstring t)
{
	const TCHAR* str=t.c_str();
	int size=t.length()*sizeof(TCHAR);
	int totalSize=size+4;

	fwrite(&totalSize,1,4,pFile);
	fwrite(str,1,size,pFile);
	return totalSize;
};

template <>
int Serialize (FILE *pFile,int t)
{
	fwrite(&t,1,4,pFile);
	return 4;
};

//-----------------------------------------

template <class T>
int ReSerialize (FILE *pFile,T *t)
{

	return 0;
};

template <>
int ReSerialize (FILE *pFile,std::tstring *str)
{
	int size;
	fread(&size,1,4,pFile);

	//string len is 0
	if(size-4<0)
	{
		MessageBox(MyLib::GetMain(),_T("Parse File Failed,Invalide Data"),_T(""),MB_OK);
		return 0;
	}
	else if (size-4==0)
	{
		*str=_T("");
	}
	else
	{
		int len=(size-4)/sizeof(TCHAR);
		TCHAR* pStr=new TCHAR[len];
		fread(pStr,sizeof(TCHAR),len,pFile);

		std::tstring tmp(pStr,pStr+len);
		delete[] pStr;
		(*str)=tmp;
	}

	return size;
};


template <>
int ReSerialize (FILE *pFile,int *pVal)
{
	fread(pVal,4,1,pFile);
	return 4;
};

template <>
int ReSerialize (FILE *pFile,uint *pVal)
{
	fread(pVal,4,1,pFile);
	return 4;
};



int MyLib::SerializeB(FILE *pFile)
{
	int size=0;
	// 	list<PlayList>::iterator i;
	// 	for (i=m_playLists.begin();i!=m_playLists.end();++i)
	// 	{
	// 		size+=(*i).Serialize(out);
	// 	}
	size+=m_pActivePlaylist->Serialize(pFile);
	return size;
}

int MyLib::ReSerialize(FILE *pFile)
{
	int totalSize=0;
	fread(&totalSize,1,4,pFile);

	int size=m_pActivePlaylist->ReSerialize(pFile);

	return size;
}

int PlayList::SerializeB(FILE *pFile)
{
	int size=0;

	//m_playlistName Serialize
	size+=::Serialize(pFile,m_playlistName);

	//m_songList Serialize
	list<PlayListItem>::iterator i;
	for (i=m_songList.begin();i!=m_songList.end();++i)
	{
		size+=(*i).Serialize(pFile);
	}

	return size;
}

int PlayList::ReSerialize(FILE *pFile)
{
	int size=0;
	int totalSize=0;
	int playlistnameSize=0;
	fread(&totalSize,1,4,pFile);

	playlistnameSize=::ReSerialize(pFile,&m_playlistName);
	size+=playlistnameSize;

	while(size<totalSize-playlistnameSize)
	{
		PlayListItem *track=new PlayListItem;
		size+=track->ReSerialize(pFile);
		m_songList.push_back(*track);
	}




	return size;
}


int PlayListItem::SerializeB(FILE *pFile)
{
	int size=0;
	size+=::Serialize(pFile,url);
	size+=::Serialize(pFile,playCount);
	size+=::Serialize(pFile,starLvl);
	size+=::Serialize(pFile,title);
	size+=::Serialize(pFile,artist);
	size+=::Serialize(pFile,album);
	size+=::Serialize(pFile,genre);
	size+=::Serialize(pFile,comment);
	size+=::Serialize(pFile,year);
	size+=::Serialize(pFile,indexInListView);
	return size;
}

int PlayListItem::ReSerialize(FILE *pFile)
{
	int size=0;

	int totalSize=0;
	fread(&totalSize,1,4,pFile);
	size+=4;

	size+=::ReSerialize(pFile,&url);
	size+=::ReSerialize(pFile,&playCount);
	size+=::ReSerialize(pFile,&starLvl);
	size+=::ReSerialize(pFile,&title);
	size+=::ReSerialize(pFile,&artist);
	size+=::ReSerialize(pFile,&album);
	size+=::ReSerialize(pFile,&genre);
	size+=::ReSerialize(pFile,&comment);
	size+=::ReSerialize(pFile,&year);
	size+=::ReSerialize(pFile,&indexInListView);
	return size;
}



BOOL MyLib::SaveCurPlaylist(LPTSTR filepath)
{
	BOOL result=FALSE;
	FILE * pFile;
	pFile = _tfopen((LPCTSTR)filepath , _T("wb") );
	if (pFile!=NULL)
	{
		result=m_pSelPlaylist->Serialize(pFile);
		fclose (pFile);
	}

	return result;
}

BOOL MyLib::LoadPlaylist(LPTSTR filepath)
{
	BOOL result=FALSE;
	FILE * pFile;
	pFile = _tfopen ((LPCTSTR)filepath, _T("rb") );
	if (pFile!=NULL)
	{
		PlayList *playlist=new PlayList;
		result=playlist->ReSerialize(pFile);
		m_playLists.push_back(*playlist);

		fclose (pFile);
	}
	return result;
}



BOOL MyLib::SaveCoreCfg()
{
	FILE * pFile;
	pFile = _tfopen( CFGFILENAME , _T("wb") );
	if (pFile!=NULL)
	{	
		//playlist section
		int len=m_playLists.size();
		::Serialize(pFile,len);
		
		list<PlayList>::iterator i;
		for (i=m_playLists.begin();i!=m_playLists.end();++i)
		{
			::Serialize(pFile,(*i).m_saveLocation);
		}

		//lrc section
		len=lrcDirs.size();
		::Serialize(pFile,len);

		vector<std::tstring>::iterator k;
		for (k=lrcDirs.begin();k!=lrcDirs.end();++k)
		{
			::Serialize(pFile,&(*k));
		}
		
		fclose (pFile);
	}


	return TRUE;
}



BOOL MyLib::LoadCoreCfg()
{
	FILE * pFile;
	pFile = _tfopen( CFGFILENAME, _T("rb") );
	if (pFile!=NULL)
	{
		//playlists
		int size=0;
		::ReSerialize(pFile,&size);
		do 
		{
			std::tstring playlistLocation;
			::ReSerialize(pFile,&playlistLocation);
			MyLib::shared()->LoadPlaylist(const_cast<LPTSTR>(playlistLocation.c_str()));
		} while (size--);


		//lrc dir list
		size=0;
		::ReSerialize(pFile,&size);
		do 
		{
			std::tstring lrcDir;
			::ReSerialize(pFile,&lrcDir);
			lrcDirs.push_back(lrcDir);
		} while (size--);

		fclose (pFile);
	}
	return TRUE;
}
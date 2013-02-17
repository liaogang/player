#include "MySerialize.h"
#include "customMsg.h"
#include "Util1.h"
#include "MyLib.h"
#include "globalStuffs.h"
#include "mytree.h"

template <class T>
int Serialize(FILE *pFile,T t)
{
	return 0;
};

template <>
int Serialize(FILE *pFile,UINT t)
{
	fwrite(&t,1,4,pFile);
	return 4;
};


template <>
int Serialize(FILE *pFile,std::tstring t)
{
	int totalSize=t.length()+4;

	fwrite(&totalSize,sizeof(int),1,pFile);

	fwrite(t.c_str(),sizeof(WCHAR),t.length(),pFile);

	return totalSize;
};

template <>
int Serialize(FILE *pFile,int t)
{
	fwrite(&t,4,1,pFile);
	return 4;
};

template <>
int Serialize (FILE *pFile,long int t)
{
	return  fwrite(&t,sizeof (long int),1,pFile);
};


template <>
int Serialize(FILE *pFile,RECT r)
{
	int size=0;
	size+=Serialize(pFile,r.left);
	size+=Serialize(pFile,r.top);
	size+=Serialize(pFile,r.right);
	size+=Serialize(pFile,r.bottom);
	return size;
};


template <> 
int Serialize(FILE *pFile,WCHAR *szStr)
{
	int len=wcslen(szStr);
	int size=len + sizeof(int);

	fwrite(&size,sizeof(int),1 ,pFile);

	fwrite(szStr,sizeof(WCHAR),len,pFile);

	return size;
}



//-----------------------------------------

template <class T>
int ReSerialize(FILE *pFile,T *t)
{
	return -1;
};

template <class T>
int ReSerialize(FILE *pFile,T &t)
{
	return -1;
};
template <>
int ReSerialize(FILE *pFile,std::tstring *str)
{
	int size;
	fread(&size,sizeof(int),1,pFile);

	//string len is 0
	if(size-4<0)
	{
		AtlTrace(L"Parse File Failed,Invalide Data\n");
		return 0;
	}
	else
	{
		int len=(size-4)/sizeof(TCHAR);
		TCHAR* pStr=new TCHAR[len];
		fread(pStr,sizeof(TCHAR),len,pFile);

		std::tstring tmp(pStr,pStr+len);
		delete[] pStr;
		(*str)=tmp;

// 		std::string strstr=*str;
// 		strstr=pStr;
// 		delete[] pStr;
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
int ReSerialize(FILE *pFile,UINT *pVal)
{
	fread(pVal,4,1,pFile);
	return 4;
};

template <> 
int ReSerialize(FILE *pFile,WCHAR *szStr)
{
	int size;
	fread(&size,sizeof(int),1,pFile);

	int len=size - sizeof(int);

	if (len<0)
	{
		AtlTrace(L"Parse format error\n");
		return -1;
	}
	

	fread(szStr,sizeof(WCHAR),len,pFile);

	return size;
}


template <>
int ReSerialize(FILE *pFile,long &t)
{
	return  fread(&t,sizeof(long),1,pFile);
};

template <>
int ReSerialize(FILE *pFile,RECT &r)
{
	int size=0;
	size+=ReSerialize(pFile,r.left);
	size+=ReSerialize(pFile,r.top);
	size+=ReSerialize(pFile,r.right);
	size+=ReSerialize(pFile,r.bottom);
	return size;
};



/*******************************************/

int MyLib::SerializeB(FILE *pFile)
{
	int size=0;
	// 	list<PlayList>::iterator i;
	// 	for (i=m_playLists.begin();i!=m_playLists.end();++i)
	// 	{
	// 		size+=(*i).Serialize(out);
	// 	}
	size+=ActivePlaylist()->Serialize(pFile);
	return size;
}

int MyLib::ReSerialize(FILE *pFile)
{
	int totalSize=0;
	fread(&totalSize,1,4,pFile);

	int size=ActivePlaylist()->ReSerialize(pFile);

	return size;
}

int PlayList::SerializeB(FILE *pFile)
{
	int size=0;

	//m_playlistName Serialize
	size+=::Serialize(pFile,m_playlistName);

	//m_songList Serialize
	_songContainer::iterator i;
	for (i=m_songList.begin();i!=m_songList.end();++i){
		size+=(*i).GetFileTrack()->Serialize(pFile);
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

	while(size<totalSize-playlistnameSize){
		PlayListItem item(this);
		size+=item.ReSerialize(pFile);
		item.SetIndex(m_songList.size());
		m_songList.push_back(item);
	}


	return size;
}


int FileTrack::SerializeB(FILE *pFile)
{
	int size=0;
	size+=::Serialize(pFile,url);
	size+=::Serialize(pFile,playCount);
	size+=::Serialize(pFile,starLvl);
	size+=::Serialize(pFile,title);
	size+=::Serialize(pFile,artist);
	size+=::Serialize(pFile,album);
	size+=::Serialize(pFile,genre);
	size+=::Serialize(pFile,year);
	return size;
}

int FileTrack::ReSerialize(FILE *pFile)
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
	size+=::ReSerialize(pFile,&year);
	return size;
}


BOOL MyLib::SavePlaylist(PlayList *pl,LPTSTR filepath)
{
	BOOL result=FALSE;
	FILE * pFile;
	pFile = _tfopen((LPCTSTR)filepath , _T("wb") );
	if (pFile!=NULL){
		result=pl->Serialize(pFile);
		pl->m_saveLocation=filepath;
		fclose (pFile);
	}

	return result;
}

PlayList* MyLib::LoadPlaylist(LPTSTR filepath)
{
	PlayList *playlist=NULL;
	BOOL result=FALSE;
	FILE * pFile;

	pFile = _tfopen ((LPCTSTR)filepath, _T("rb") );
	if (pFile!=NULL){
		playlist=new PlayList;
		playlist->m_saveLocation=filepath;
		result=playlist->ReSerialize(pFile);
		m_playLists.push_back(playlist);
		SdMsg(WM_PL_TRACKNUM_CHANGED,TRUE,(WPARAM)playlist,(LPARAM)playlist->GetItemCount());
		fclose (pFile);
	}


	SetActivePlaylist(playlist);
	return playlist;
}

bool SaveCoreCfg()
{
	FILE * pFile;
	ChangeCurDir2ModulePath();
	pFile = _tfopen( CFGFILENAME , _T("wb") );
	if (pFile!=NULL)
	{	
		/*******************************************/
		//playlist section
		int len=MyLib::shared()->m_playLists.size();
		::Serialize(pFile,len);
		
		
		for (auto i=MyLib::shared()->m_playLists.begin();i!=MyLib::shared()->m_playLists.end();++i)
			::Serialize(pFile,(*i)->m_saveLocation);
		

		/*******************************************/
		//lrc section
		len=MyLib::shared()->lrcDirs.size();
		::Serialize(pFile,len);

		
		for (auto k=MyLib::shared()->lrcDirs.begin();k!=MyLib::shared()->lrcDirs.end();++k)
			::Serialize<>(pFile,*k);
		
		

		/**********user interface split windows's section************/
		MYTREE* root=UISplitterTreeRoot();




		fclose (pFile);
	}


	return TRUE;
}



bool LoadCoreCfg()
{
	FILE * pFile;
	ChangeCurDir2ModulePath();
	pFile = _tfopen( CFGFILENAME, _T("rb") );
	if (pFile!=NULL)
	{
		/*******************************************/
		//playlists
		int size=0;

		::ReSerialize(pFile,&size);
		while (size) 
		{
			std::tstring playlistLocation;
			::ReSerialize(pFile,&playlistLocation);
			MyLib::shared()->LoadPlaylist(const_cast<LPTSTR>(playlistLocation.c_str()));
			
			size--;
		} 

		/*******************************************/
		//lrc dir list
		size=0;
		::ReSerialize(pFile,&size);
		 while (size--)
		{
			std::tstring lrcDir;

			::ReSerialize(pFile,&lrcDir);
			if(!lrcDir.empty())
				MyLib::shared()->lrcDirs.push_back(lrcDir);
		}








		 /**********user interface split windows's section************/


		fclose (pFile);
	}

	return TRUE;
}


int MYTREE::SerializeB(FILE *pFile)
{
	return 0;
}

int MYTREE::ReSerialize(FILE *pFile)
{

	return 0;
}

int SerializeTree(FILE *pFile,MYTREE *cur)
{
	int size=0;

	for (auto i=cur;i;i=i->next)
	{
		size+=i->Serialize(pFile);
		
		if (i->hasChild())
			size+=SerializeTree(pFile,i);
	}
	
	return size;
}

int ReSerializeTree(FILE *pFile,MYTREE *cur)
{
	int size=0;

	size+=cur->ReSerialize(pFile);
	
	for(int i=0;i<cur->childs;++i)
	{
		MYTREE *newTree=new MYTREE;
		newTree->ReSerialize(pFile);
		size+=ReSerializeTree(pFile,newTree);

		cur->AddChild(newTree);
	}


	return size;
}

int dataNode::SerializeB(FILE *pFile)
{
	int size=0;

	size+=::Serialize(pFile,nodeName);
	size+=::Serialize(pFile,rc);
	size+=::Serialize(pFile,(int)type);
	size+=::Serialize(pFile,m_iSplitterBar);
	size+=::Serialize(pFile,windowtype);

	int numBars=SplitterBarRects.size();
	size+=::Serialize(pFile,numBars);
	for (auto it=SplitterBarRects.begin();it!=SplitterBarRects.end();++it)
		size+=::Serialize(pFile,*it);
	
	return size;
}

int dataNode::ReSerialize(FILE *pFile)
{
	int size=0;

	int totalSize=0;
	fread(&totalSize,1,4,pFile);
	size+=4;


	size+=::ReSerialize(pFile,nodeName);
	size+=::ReSerialize(pFile,rc);
	size+=::ReSerialize(pFile,type);
	size+=::ReSerialize(pFile,m_iSplitterBar);
	size+=::ReSerialize(pFile,windowtype);


	int numBars;
	size+=::ReSerialize(pFile,numBars);
	for (int i=0;i<numBars;++i)
	{
		RECT rc;
		size+=::ReSerialize(pFile,rc);
		SplitterBarRects.push_back(rc);
	}

	return size;
}


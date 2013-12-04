#include "MySerialize.h"
#include "customMsg.h"
#include "Util1.h"
#include "MyLib.h"
#include "globalStuffs.h"
#include "mytree.h"
#include "MyConfigs.h"
#include "mainfrm.h"

template <class T>
int Serialize(FILE *pFile,T t)
{
	ATLASSERT(FALSE);
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
	int totalSize=sizeof(TCHAR)*t.length()+sizeof(int);

	fwrite(&totalSize,sizeof(int),1,pFile);

	fwrite(t.c_str(),sizeof(WCHAR),t.length(),pFile);

	return totalSize;
};

template <>
int Serialize(FILE *pFile,int t)
{
	return sizeof(int)*fwrite(&t,sizeof(int),1,pFile);
};

template <> int Serialize (FILE *pFile,double t)
{
	return sizeof(double)*fwrite(&t,sizeof(double),1,pFile);
}

template <>
int Serialize (FILE *pFile,long int t)
{
	return  sizeof (long int)*fwrite(&t,sizeof (long int),1,pFile);
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
	int len=wcslen(szStr)+1;
	int size=len*sizeof(WCHAR) + sizeof(int);

	fwrite(&size,sizeof(int),1 ,pFile);

	fwrite(szStr,sizeof(WCHAR),len,pFile);

	return size;
}



//-----------------------------------------

template <class T>
int ReSerialize(FILE *pFile,T *t)
{
	ATLASSERT(FALSE);
	return -1;
};

template <class T>
int ReSerialize(FILE *pFile,T &t)
{
	ATLASSERT(FALSE);
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

	int len=(size - sizeof(int))/sizeof(WCHAR);

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
	return  sizeof(long)*fread(&t,sizeof(long),1,pFile);
};

template <>
int ReSerialize(FILE *pFile,double &t)
{
	return  sizeof(double)*fread(&t,sizeof(double),1,pFile);
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
	size+=SelectedPlaylist()->Serialize(pFile);
	return size;
}

int MyLib::ReSerialize(FILE *pFile)
{
	int totalSize=0;
	fread(&totalSize,1,4,pFile);

	int size=SelectedPlaylist()->ReSerialize(pFile);

	return size;
}

int PlayList::SerializeB(FILE *pFile)
{
	int size=0;

	//m_playlistName Serialize
	size+=::Serialize(pFile,m_playlistName);

	//bMonitor  
	size+=::Serialize(pFile,m_bAuto);

	//m_songList Serialize
	_songContainer::iterator i;
	for (i=m_songList.begin();i!=m_songList.end();++i){
		size+=(*i)->GetFileTrack()->Serialize(pFile);
	}

	return size;
}

int PlayList::ReSerialize(FILE *pFile)
{
	int size=0;
	int totalSize=0;
	int playlistnameSize=0;
	fread(&totalSize,sizeof(int),1,pFile);

	playlistnameSize=::ReSerialize(pFile,&m_playlistName);
	size+=playlistnameSize;


	//4 byte  
	::ReSerialize(pFile,&m_bAuto);

	while(size<totalSize-playlistnameSize-4){
		PlayListItem *item=new PlayListItem(this);
		size+=item->ReSerialize(pFile);
		item->SetIndex(m_songList.size());
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
	fread(&totalSize,4,1,pFile);
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
		fclose (pFile);
	}

	return result;
}

PlayList* MyLib::LoadPlaylist(LPTSTR filepath,TCHAR* PlName)
{
	PlayList *playlist=NULL;
	BOOL result=FALSE;
	FILE * pFile;

	pFile = _tfopen ((LPCTSTR)filepath, _T("rb") );
	if (pFile)
	{
		playlist=new PlayList();
		if(PlName!=NULL)
		{
			tstring PLName2=PlName;
			playlist->m_playlistName=PlName;
		}

		result=playlist->ReSerialize(pFile);
		m_playLists.push_back(playlist);
		SetSelectedPlaylist(playlist);


		if(playlist->m_bAuto)
			MyLib::shared()->SetAutoPlayList(playlist);

		SdMsg(WM_PL_CHANGED,TRUE,(WPARAM)playlist,1);

		fclose (pFile);
	}


	
	return playlist;
}

bool LoadAllPlayList()
{
	if(!ChangeCurDir2PlaylistPath(true))
		return false;

	FILE * pFile=NULL;
	BOOL   bLoaded=TRUE;
	pFile = _tfopen( PLAYLISTINDEXFILE , _T("rb") );
	if (pFile!=NULL)
	{	
		int totalNum=0;
		::ReSerialize(pFile,&totalNum);

		for (int i=0;i<totalNum;++i)
		{
			int nIndex;
			tstring name;
			TCHAR path[MAX_PATH];
			::ReSerialize(pFile,&nIndex);
			::ReSerialize(pFile,&name);

			wsprintf(path,_T("%08d.pl"),nIndex);
			
			MyLib::shared()->LoadPlaylist((LPTSTR)path,(TCHAR*)name.c_str());
		}

		fclose(pFile);
	}

	return bLoaded;
}

bool SaveAllPlayList()
{
	if(!ChangeCurDir2PlaylistPath(true))
		return false;
	

	FILE * pFile=NULL;
	
	pFile = _tfopen( PLAYLISTINDEXFILE , _T("wb") );
	if (pFile!=NULL)
	{	
		int totalNum=MyLib::shared()->m_playLists.size();
		::Serialize(pFile,totalNum);

		for (int i=0;i<totalNum;++i)
		{
			int nIndex=i+1;
			auto i2=MyLib::shared()->m_playLists[i];
			tstring name=(*i2).m_playlistName;

			::Serialize(pFile,nIndex);
			::Serialize(pFile,name);


			TCHAR path[MAX_PATH];

			wsprintf(path,_T("%08d.pl"),nIndex);

			MyLib::shared()->SavePlaylist(&(*i2),(LPTSTR)path);
		}

		fclose(pFile);
	}

	return true;
}

bool SaveCoreCfg()
{
	CollectInfo();

	FILE * pFile;
	ChangeCurDir2ModulePath();
	pFile = _tfopen( CFGFILENAME , _T("wb") );
	if (pFile!=NULL)
	{	
		int len;
		/*******************************************/
		//lrc section
		len=MyLib::shared()->lrcDirs.size();
		::Serialize(pFile,len);

		
		for (auto k=MyLib::shared()->lrcDirs.begin();k!=MyLib::shared()->lrcDirs.end();++k)
			::Serialize<>(pFile,*k);
		

		//***************************************//
		//MyConfigs
		GetMyConfigs()->Serialize(pFile);
		

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
		int size=0;
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

		 MyLib::shared()->InitLrcLib();
 


		 //***************************************//
		 //MyConfigs
		 GetMyConfigs()->ReSerialize(pFile);
		
		 ValidateCfg();

		fclose (pFile);
	}

	
	
	return TRUE;
}



/************************************************************************/
/* UI  section                                                                     */
/************************************************************************/


int MYTREE::SerializeB(FILE *pFile)
{
	int size=0;
	size+=::Serialize(pFile,childs);
	return size+data.Serialize(pFile);
}

int MYTREE::ReSerialize(FILE *pFile)
{
	int size=4,totalsize;
	::ReSerialize(pFile,&totalsize);


	size+= ::ReSerialize(pFile,&childs);
	return size+data.ReSerialize(pFile);
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

	size+=::Serialize(pFile,type==left_right?1:0);

	int numBars=SplitterBarRects.size();
	size+=::Serialize(pFile,numBars);
	if (numBars>0)
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
	
	int leftright;
	size+=::ReSerialize(pFile,&leftright);
	type=leftright?left_right:up_down;


	int numBars;
	size+=::ReSerialize(pFile,&numBars);
	for (int i=0;i<numBars;++i)
	{
		RECT rc;
		size+=::ReSerialize(pFile,rc);
		SplitterBarRects.push_back(rc);
	}

	return size;
}


void SerializeAllTree(MYTREE *c,FILE *pFile)
{
	for (;c;c=c->next)	
	{
		c->Serialize(pFile);
		if (c->hasChild())
			SerializeAllTree(c->child,pFile);
	}
}

void ReSerializeAllTree(MYTREE *parent,FILE *pFile)
{
	if (parent->childs==NULL)
	{
		return;
	}

	MYTREE *c=new MYTREE;
	c->ReSerialize(pFile);
	c->parent=parent;
	parent->child=c;
	
	if (c->childs)
		ReSerializeAllTree(c,pFile);
	
	MYTREE *last=c;

	for (int i=1;i<parent->childs;++i)
	{
		c=new MYTREE;
		c->ReSerialize(pFile);
		
		last->next=c;
		c->prev=last;
		c->parent=last->parent;

		if (c->childs)
			ReSerializeAllTree(c,pFile);
		
		last=c;
	}
	
}


bool SaveUICfg()
{
	ChangeCurDir2ModulePath();
	FILE * pFile = _tfopen( UIFILENAME , _T("wb") );
	if (pFile)
	{	
		/***************   Main Window ******************/
		GetMainFrame()->Serialize(pFile);


		/**********user interface split windows's section************/
		MYTREE* root=UISplitterTreeRoot();
		
		SerializeAllTree(root,pFile);



		fclose (pFile);
	}


	return TRUE;
}



bool LoadUICfg()
{
	ChangeCurDir2ModulePath();
	FILE * pFile = _tfopen( UIFILENAME , _T("rb") );
	if (pFile)
	{

		/***************   Main Window ******************/
		GetMainFrame()->ReSerialize(pFile);


		/**********user interface split windows's section************/
		MYTREE *root=new MYTREE(_T(""));
		root->data.SplitterBarRects.clear();

		root->setroot();
		root->ReSerialize(pFile);

		ReSerializeAllTree(root,pFile);
		
		SetRootTree(root);
		
		fclose (pFile);
	}

	return TRUE;
}


int CMainFrame::SerializeB(FILE *pFile)
{
	int size=0;

	size+=::Serialize(pFile,m_rcMain);
	size+=::Serialize(pFile,m_rcConfig);
	size+=::Serialize(pFile,m_rcLrc);
	size+=::Serialize(pFile,m_rcProcess);
	size+=::Serialize(pFile,m_rcSearch);
	size+=::Serialize(pFile,m_rcFFT);
	size+=::Serialize(pFile,m_rcPLMng);
	size+=::Serialize(pFile,m_rcPLConsole);


	return size;
}

int CMainFrame::ReSerialize(FILE *pFile)
{
	int size=0;

	int totalSize=0;
	fread(&totalSize,1,4,pFile);
	size+=4;

	size+=::ReSerialize(pFile,m_rcMain);
	size+=::ReSerialize(pFile,m_rcConfig);
	size+=::ReSerialize(pFile,m_rcLrc);
	size+=::ReSerialize(pFile,m_rcProcess);
	size+=::ReSerialize(pFile,m_rcSearch);
	size+=::ReSerialize(pFile,m_rcFFT);
	size+=::ReSerialize(pFile,m_rcPLMng);
	size+=::ReSerialize(pFile,m_rcPLConsole);
	


	return size;
}




int MyConfigs::SerializeB(FILE *pFile)
{
	int size=0;

	size+=::Serialize(pFile,bResumeOnReboot);
	size+=::Serialize(pFile,playlistIndex);
	size+=::Serialize(pFile,trackIndex);

	size+=::Serialize(pFile,pos.used);
	size+=::Serialize(pFile,pos.left);

	size+=::Serialize(pFile,playersVolume);

	size+=::Serialize(pFile,playorder);

	return size;
}

int MyConfigs::ReSerialize(FILE *pFile)
{
	int size=0,totalsize;
	size+=::ReSerialize(pFile,&totalsize);

	size+=::ReSerialize(pFile,&bResumeOnReboot);
	size+=::ReSerialize(pFile,&playlistIndex);
	size+=::ReSerialize(pFile,&trackIndex);

	size+=::ReSerialize(pFile,pos.used);
	size+=::ReSerialize(pFile,pos.left);

	size+=::ReSerialize(pFile,&playersVolume);

	size+=::ReSerialize(pFile,&playorder);

	return size;
}
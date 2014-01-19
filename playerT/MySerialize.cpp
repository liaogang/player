#include "MySerialize.h"
#include "customMsg.h"
#include "Util1.h"
#include "MyLib.h"
#include "globalStuffs.h"
#include "mytree.h"
#include "MyConfigs.h"
#include "mainfrm.h"
#include "MyControls.h"
#include <iostream>

ADDTOSERIALIZE(CMainFrame)
ADDTOSERIALIZE(PlayList)
ADDTOSERIALIZE(FileTrack)
ADDTOSERIALIZE(MyConfigs)
ADDTOSERIALIZE(CMySimpleRebar)
ADDTOSERIALIZE(MYTREE)
ADDTOSERIALIZE(dataNode)
ADDTOSERIALIZE(PlayListItem)

//double
FILE& operator<<(FILE& f,double t)
{
	fwrite(&t,sizeof(double),1,&f);
	return f;
}

FILE& operator>>(FILE& f,double& t)
{
	fread(&t,sizeof(double),1,&f);
	return f;
}
//int
FILE& operator<<(FILE& f,int t)
{
	fwrite(&t,sizeof(int),1,&f);
	return f;
}

FILE& operator>>(FILE& f,int& t)
{
	fread(&t,sizeof(int),1,&f);
	return f;
}

//UINT
FILE& operator<<(FILE& f,UINT t)
{
	fwrite(&t,sizeof(UINT),1,&f);
	return f;
}

FILE& operator>>(FILE& f,UINT& t)
{
	fread(&t,sizeof(UINT),1,&f);
	return f;
}


//long
FILE& operator<<(FILE& f,long t)
{
	fwrite(&t,sizeof(long),1,&f);
	return f;
}

FILE& operator>>(FILE& f,long& t)
{
	fread(&t,sizeof(long),1,&f);
	return f;
}



//write zero terminated str array
FILE& operator<<(FILE& f,TCHAR * str)
{
	int l=_tcslen(str)+1;
	f<<l;
	fwrite(str,sizeof(TCHAR),l,&f);
	return f;
}

FILE& operator>>(FILE& f,TCHAR * str)
{
	int l=0;
	f>>l;
	fread(str,sizeof(TCHAR),l,&f);
	return f;
}

//tstring
FILE& operator<<(FILE& f,tstring &str)
{
	int l=str.length();
	f<<l+1;
	fwrite(str.c_str(),sizeof(TCHAR),l,&f);
	TCHAR nullstr='\0';
	fwrite(&nullstr,sizeof(TCHAR),1,&f);
	return f;
}

FILE& operator>>(FILE& f,tstring &str)
{
	TCHAR buf[128];
	f>>buf;
	str=buf;
	return f;
}


//RECT
FILE& operator<<(FILE& f,RECT &r)
{
	return f<<r.left<<r.top<<r.right<<r.bottom;
}

FILE& operator>>(FILE& f,RECT &r)
{
	return f>>r.left>>r.top>>r.right>>r.bottom;
}



//enum PlayingStatus
FILE& operator<<(FILE& f,PlayingStatus s)
{
	return f<<static_cast<int>(s);
}

FILE& operator>>(FILE& f,PlayingStatus &s)
{
	int *t=(int*)(&s);
	return f>>*t;
}




int SerializeAllTree(MYTREE *c,FILE& f)
{
	int size=0;
	for (;c;c=c->next)	
	{
		f<<*c;
		if (c->hasChild())
			size+=SerializeAllTree(c->child,f);
	}
	return 0;
}

int ReSerializeAllTree(MYTREE *parent,FILE & f)
{
	int size=0;

	if (parent->childs==NULL)
	{
		return 0;
	}

	MYTREE *c=new MYTREE;
	f>>*c;
	c->parent=parent;
	parent->child=c;

	if (c->childs)
		ReSerializeAllTree(c,f);

	MYTREE *last=c;

	for (int i=1;i<parent->childs;++i)
	{
		c=new MYTREE;
		f>>*c;

		last->next=c;
		c->prev=last;
		c->parent=last->parent;

		if (c->childs)
			ReSerializeAllTree(c,f);

		last=c;
	}


	return size;
}





BOOL MyLib::SavePlaylist(PlayList *pl,LPTSTR filepath)
{
	BOOL result=TRUE;
	FILE  * f = _tfopen( filepath , _T("wb") );
	if (f)
	{
		*f<<*pl;
		fclose(f);
	}
	return result;
}

PlayList* MyLib::LoadPlaylist(LPTSTR filepath,TCHAR* PlName)
{
	PlayList *playlist=NULL;
	BOOL result=TRUE;


	FILE  * f = _tfopen( filepath , _T("rb") );
	if (f)
	{
		playlist=new PlayList();
		if(PlName!=NULL)
		{
			tstring PLName2=PlName;
			playlist->m_playlistName=PlName;
		}

		*f>>*playlist;

		m_playLists.push_back(playlist);
		SetSelectedPlaylist(playlist);


		if(playlist->m_bAuto)
		{
			MyLib::shared()->SetAutoPlaylist(playlist);
			//MyLib::shared()->InitMonitor(playlist);
		}

		SdMsg(WM_PL_CHANGED,TRUE,(WPARAM)playlist,1);

		fclose(f);
	}


	return playlist;
}

bool LoadAllPlayList()
{
	if(!ChangeCurDir2PlaylistPath(true))
		return false;

	
	BOOL   bLoaded=TRUE;

	FILE  * f = _tfopen( PLAYLISTINDEXFILE , _T("rb") );
	if (f)
	{
		int totalNum=0;
		*f>>totalNum;

		for (int i=0;i<totalNum;++i)
		{
			int nIndex;
			tstring name;
			TCHAR path[MAX_PATH];
			*f>>nIndex>>name;

			wsprintf(path,_T("%08d.pl"),nIndex);
			
			MyLib::shared()->LoadPlaylist((LPTSTR)path,(TCHAR*)name.c_str());
		}

		fclose(f);
	}

	return bLoaded;
}

bool SaveAllPlayList()
{
	if(!ChangeCurDir2PlaylistPath(true))
		return false;
	
	FILE  * f = _tfopen( PLAYLISTINDEXFILE , _T("wb") );
	if (f)
	{
		int totalNum=MyLib::shared()->m_playLists.size();
		*f<<totalNum;

		for (int i=0;i<totalNum;++i)
		{
			int nIndex=i+1;
			auto i2=MyLib::shared()->m_playLists[i];
			tstring name=(*i2).m_playlistName;

			*f<<nIndex<<name;

			TCHAR path[MAX_PATH];

			wsprintf(path,_T("%08d.pl"),nIndex);

			MyLib::shared()->SavePlaylist(&(*i2),(LPTSTR)path);
		}


		fclose(f);
	}

	return true;
}

bool SaveCoreCfg()
{
	CollectInfo();
	ChangeCurDir2ModulePath();



	FILE  * f = _tfopen( CFGFILENAME , _T("wb") );
	if (f)
	{
		int len;
		/*******************************************/
		//lrc section
		len=MyLib::shared()->lrcDirs.size();
		*f<<len;
		
		for (auto k=MyLib::shared()->lrcDirs.begin();k!=MyLib::shared()->lrcDirs.end();++k)
		{	
			*f<<(*k);
		}
		
		
		//Media paths
		len=MyLib::shared()->mediaPaths.size();
		*f<<len;

		for (auto k=MyLib::shared()->mediaPaths.begin();k!=MyLib::shared()->mediaPaths.end();++k)
			 *f<<(*k);

		//***************************************//
		//MyConfigs
		*f<<*GetMyConfigs();
		fclose(f);
	}


	return TRUE;
}



bool LoadCoreCfg()
{
	ChangeCurDir2ModulePath();

	FILE  * f = _tfopen( CFGFILENAME , _T("rb") );
	if (f)
	{
		int size=0;
		MyLib * s=MyLib::shared();
		/*******************************************/
		//lrc dir list
		*f>>size;
		 while (size-->0)
		{
			std::tstring lrcDir;

			*f>>lrcDir;
			if(!lrcDir.empty())
				s->lrcDirs.push_back(lrcDir);
		}

		 s->InitLrcLib();
 
		 //Load the Media Lib
		*f>>size;
		 if(size>0)
		{
			 s->InitMonitor(s->GetAutoPlaylist());
			 while (size--)
			 {
				 std::tstring mediaPath;

				* f>>mediaPath;
				 if(!mediaPath.empty())
					 s->AddMediaPath(mediaPath);
			 }
		 }

		 
		 //***************************************//
		 //MyConfigs
		*f>>*GetMyConfigs();

		ValidateCfg();
		fclose(f);
	}

	
	
	return TRUE;
}



/************************************************************************/
/* UI  section                                                                     */
/************************************************************************/



int SerializeTree(FILE &f,MYTREE *cur)
{
	for (auto i=cur;i;i=i->next)
	{
		f<<(*i);
		if (i->hasChild())
			SerializeTree(f,i);
	}
	
	return 0;
}

int ReSerializeTree(FILE &f,MYTREE *cur)
{
	f<<(*cur);

	
	for(int i=0;i<cur->childs;++i)
	{
		MYTREE *newTree=new MYTREE;
		f<<*newTree;

		ReSerializeTree(f,newTree);

		cur->AddChild(newTree);
	}


	return 0;
}




bool SaveUICfg()
{
	ChangeCurDir2ModulePath();

	FILE  * f = _tfopen( UIFILENAME , _T("wb") );
	if (f)
	{
		*f<<(*GetMainFrame());

		fclose(f);
	}
	return TRUE;
}


bool LoadUICfg()
{
	ChangeCurDir2ModulePath();

	FILE  * f = _tfopen( UIFILENAME , _T("rb") );
	if (f)
	{
		*f>>*GetMainFrame();
		fclose(f);
	}
	return FALSE;
}






FILE& CMainFrame::operator>>(FILE& f)
{
	/***************   Main Window ******************/
	f<<m_rcMain<<m_rcConfig<<m_rcLrc<<m_rcProcess<<m_rcSearch<<m_rcFFT<<m_rcPLMng<<m_rcPLConsole;

	f<<m_bShowStatusBar;
	/********************Rebar section*************************/
	f<<m_wndRebar;


	//put this section to the end . 
	//it won't calc the total size.
	/**********user interface split windows's section************/
	MYTREE* root=UISplitterTreeRoot();

	SerializeAllTree(root,f);

	return f;
}

FILE& CMainFrame::operator<<(FILE& f)
{
	/***************   Main Window ******************/
	f>>m_rcMain>>m_rcConfig>>m_rcLrc>>m_rcProcess>>m_rcSearch>>m_rcFFT>>m_rcPLMng>>m_rcPLConsole;

	f>>m_bShowStatusBar;
	/********************Rebar section*************************/
	
	f>>m_wndRebar;



	/**********user interface split windows's section************/
	MYTREE *root=new MYTREE(_T(""));
	root->data.SplitterBarRects.clear();

	root->setroot();

	f>>*root;


	ReSerializeAllTree(root,f);

	SetRootTree(root);

	return f;
}


FILE& PlayList::operator>>(FILE& f)
{
	f<<m_playlistName<<m_bAuto;

	//m_songList Serialize
	int count=m_songList.size();
	f<<count;

	_songContainer::iterator i;
	for (i=m_songList.begin();i!=m_songList.end();++i)
		f<<*(*i);
	

	return f;
}

FILE& PlayList::operator<<(FILE& f)
{
	int count=0;
	f>>m_playlistName>>m_bAuto>>count;

	if(count>=0)
		while(count--){
			PlayListItem *item=new PlayListItem(this);
			f>>*item;
			item->SetIndex(m_songList.size());
			m_songList.push_back(item);
		}

		return f;
}


FILE& FileTrack::operator>>(FILE& f)
{
	return f<<url<<playCount<<starLvl<<title<<artist<<album<<genre<<year;
}

FILE& FileTrack::operator<<(FILE& f)
{
	return f>>url>>playCount>>starLvl>>title>>artist>>album>>genre>>year;
}


FILE& MyConfigs::operator>>(FILE& f)
{
	return f<<bResumeOnReboot<<playlistIndex<<trackIndex<<playingStatus<<pos.used<<pos.left<<playersVolume<<playorder;
}

FILE& MyConfigs::operator<<(FILE& f)
{
	return f>>bResumeOnReboot>>playlistIndex>>trackIndex>>playingStatus>>pos.used>>pos.left>>playersVolume>>playorder;
}



FILE& CMySimpleRebar::operator>>(FILE& f)
{
	return f<<m_bLock;

	int bandCount=GetBandCount();
	f<<bandCount;

	for (int nBand=0;nBand<bandCount;++nBand)
	{
		REBARBANDINFO rbBand={ RunTimeHelper::SizeOf_REBARBANDINFO() };
		GetBandInfo(nBand,&rbBand);

		HWND hWndBand=rbBand.hwndChild;
		TCHAR szClassName[128]={0};
		::SendMessage(hWndBand,WM_GET_BAND_CLASSNAME,WPARAM(szClassName),LPARAM(128));

		//rbBand >> file

		//szClassName >> fiile
	}

	return f;
}

FILE& CMySimpleRebar::operator<<(FILE& f)
{
	return f>>m_bLock;

	int bandCount=0;
	f>>bandCount;

	for (int nBand=0;nBand<bandCount;++nBand)
	{
		REBARBANDINFO rbBand={ RunTimeHelper::SizeOf_REBARBANDINFO() };
		TCHAR szClassName[128]={0};

		//size+=::ReSerialize(pFile,rbBand);
		f>>szClassName;
	}

	return f;
}


FILE& MYTREE::operator>>(FILE& f)
{
	return f<<childs<<data;
}

FILE& MYTREE::operator<<(FILE& f)
{
	return f>>childs>>data;
}



FILE& dataNode::operator>>(FILE& f)
{
	f<<nodeName<<rc;
	f<<(type==left_right?1:0);

	int numBars=SplitterBarRects.size();
	f<<numBars;
	if (numBars>0)
		for (auto it=SplitterBarRects.begin();it!=SplitterBarRects.end();++it)
			f<<(*it);
	return f;
}

FILE& dataNode::operator<<(FILE& f)
{
	int leftright;

	f>>nodeName>>rc>>leftright;
	type=leftright?left_right:up_down;

	int numBars;
	f>>numBars;
	for (int i=0;i<numBars;++i)
	{
		RECT rc;
		f>>rc;
		SplitterBarRects.push_back(rc);
	}

	return f;
}


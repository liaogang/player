#include "stdafx.h"
#include "globalStuffs.h"
#include "PlayList.h"
#include "customMsg.h"

#ifdef APP_PLAYER_TRAY
#include "MainFrameTray.h"
#else
#include "mainfrm.h"
#include "FunctionReg.h"
#endif

void SdMsg(UINT msg,BOOL bPost,WPARAM wparam,LPARAM lparam)
{
	static HWND HMainFrm=NULL;
	if (!HMainFrm)
	{
		HMainFrm=GetMainFrame()->m_hWnd;
		ATLASSERT(::IsWindow(HMainFrm));
	}

	if (bPost)
		::PostMessage(HMainFrm,msg,wparam,lparam);
	else
		::SendMessage(HMainFrm,msg,wparam,lparam);
}

const TCHAR *GetAppName()
{
#ifdef DEBUG
	const static TCHAR *gpAppName=_T("mp3 player(debug)");
#else
	const static TCHAR *gpAppName=_T("mp3 player");
#endif

	return gpAppName;
}
// static MYTREE *gRootTree=NULL;
// void SetRootTree(MYTREE *root)
// {
// 	gRootTree=root;
// }
// 
// MYTREE *UISplitterTreeRoot()
// {
// 	if(gRootTree==NULL)
// 		gRootTree=CreateRootTree();
// 
// 	return gRootTree;
// }




//static int pPlayingIndex=-1;
//int GetPlayingIndex()
//{
//	return pPlayingIndex;
//}
//
//void SetPlayingIndex(int index)
//{
//	pPlayingIndex=index;
//}


static bool bSearchLrcUntilEnd=false;
bool GetSearchLrcUntilEnd(){return bSearchLrcUntilEnd;}
void SetSearchLrcUntilEnd(bool b){bSearchLrcUntilEnd=b;}

static vector<LrcMatchItem> matchlist;

vector<LrcMatchItem> GetLrcMatchList()
{
	return matchlist;
}
void ClearLrcMatchList(){matchlist.clear();}
void AddToLrcMatchList(LrcMatchItem &item){matchlist.push_back(item);}

static LrcMatchItem highestmatchlrc;
LrcMatchItem GetHighestMatchLrc(){return highestmatchlrc;}
void SetHighestMatchLrc(LrcMatchItem &item){highestmatchlrc=item;}







#ifdef APP_PLAYER_UI
//  Only the mono mode has 1 channel, the others have 2 channels.
const TCHAR * mp3_mode[] = {
	_T("stereo"),	/**< Standard Stereo. */
	_T("joint"),		/**< Joint Stereo. */
	_T("dual"),		/**< Dual Channel. */
	_T("mono")/**< Single Channel. */
};

const TCHAR* TrackFormatInfo::getModeString()
{
	return mp3_mode[mode];
}




void RegisterStuffs()
{
	CMyCommandBarCtrl::RegisterSelf();
	CMyToolBar::RegisterSelf();
	CMyTrackBar::RegisterSelf();
	CMyComboBox::RegisterSelf();
	CMyVolumeBar::RegisterSelf();


	RegisterCreateWndFuns(_T("播放列表"),MYTREE_Set_Playlist);
	RegisterCreateWndFuns(_T("封面面板"),MYTREE_Set_AlbumView);
	RegisterCreateWndFuns(_T("歌词面板"),MYTREE_Set_LyricView);
	RegisterCreateWndFuns(_T("频谱面板"),MYTREE_Set_SpectrumView);
}
#endif







void SaveAll()
{
	SaveCoreCfg();

	SaveAllPlayList();

#ifdef APP_PLAYER_UI
	SaveUICfg();
#endif
}

bool LoadAll()
{
	#ifdef APP_PLAYER_UI
	LoadUICfg();
	#endif
	LoadAllPlayList();

	LoadCoreCfg();

	return true;
}





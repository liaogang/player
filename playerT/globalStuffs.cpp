#include "stdafx.h"
#include "globalStuffs.h"
#include "PlayList.h"
#include "customMsg.h"




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


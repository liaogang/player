#include "stdafx.h"
#include "globalStuffs.h"
#include "PlayList.h"
#include "mysplit.h"
#include "customMsg.h"

static PlayList *pActivePlaylist=0;
PlayList* ActivePlaylist(){return pActivePlaylist;}
void SetActivePlaylist(PlayList* p){pActivePlaylist=p;}


//file track 是NEW出来的一块随机内存
static PlayListItem playingItem=0;
PlayListItem  GetPlayingItem()
{
	return playingItem;
}
void  SetPlayingItem(PlayListItem item)
{
	playingItem=item;
}



MYTREE* CreateRootTree()
{
	MYTREE *root=new MYTREE(_T("root"));
	root->data.type=left_right;
	root->setroot();

	wcscpy(root->data.nodeName,L"水平分离器");

	return root;
}

static MYTREE *gRootTree=NULL;
void SetRootTree(MYTREE *root)
{
	gRootTree=root;
}

MYTREE *UISplitterTreeRoot()
{
	if(gRootTree==NULL)
		gRootTree=CreateRootTree();

	return gRootTree;
}




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





static trackPosInfo curPosInfo;
trackPosInfo *getTrackPosInfo()
{
	return &curPosInfo;
}



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




static TrackFormatInfo *gTrackFormatInfo;
void SetTrackFormatInfo(int type,int rate,int samplesps,int mode)
{
	if(gTrackFormatInfo==nullptr)
		gTrackFormatInfo=new TrackFormatInfo;

	gTrackFormatInfo->type=type;
	gTrackFormatInfo->rate=rate;
	gTrackFormatInfo->nSamplesPerSec=samplesps;
	gTrackFormatInfo->mode=mode;
	
}

TrackFormatInfo* GetTrackFormatInfo()
{
	return gTrackFormatInfo;
}
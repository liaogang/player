#include "globalStuffs.h"
#include "PlayList.h"
#include "mysplit.h"

static PlayList *pActivePlaylist=0;
PlayList* ActivePlaylist(){return pActivePlaylist;}
void SetActivePlaylist(PlayList* p){pActivePlaylist=p;}

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

	wcscpy(root->data.nodeName,L"Ë®Æ½·ÖÀëÆ÷");

	return root;
}

static MYTREE *gRootTree=NULL;
MYTREE *UISplitterTreeRoot()
{
	if(gRootTree==NULL)
		gRootTree=CreateRootTree();

	return gRootTree;
}




static int pPlayingIndex=-1;
int GetPlayingIndex()
{
	return pPlayingIndex;
}

void SetPlayingIndex(int index)
{
	pPlayingIndex=index;
}
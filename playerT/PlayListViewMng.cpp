#include "PlayListViewMng.h"
#include "PlayListView.h"

void PlayListViewMng::AddItem(CPlayListView* playlistview)
{
	dataCollection.push_back(playlistview);
	
	if (ActivePlaylist())
		playlistview->Reload(ActivePlaylist());
}

void PlayListViewMng::ClearAllItem()
{
	for (container::iterator i=dataCollection.begin();i!=dataCollection.end();++i)
		(*i)->ClearAllItem();
}


void PlayListViewMng::Reload(PlayList *pPL)
{
	for (container::iterator i=dataCollection.begin();
		i!=dataCollection.end();++i)
	{
		CPlayListView* plv=*i;
		plv->Reload(pPL);
	}
}



void PlayListViewMng::EnsureVisibleAndCentrePos(int index)
{
	for (container::iterator i=dataCollection.begin();i!=dataCollection.end();++i)
		(*i)->EnsureVisibleAndCentrePos(index);
}

void PlayListViewMng::EnlargeLVFont(int value)
{
	for (container::iterator i=dataCollection.begin();i!=dataCollection.end();++i)
		(*i)->EnlargeLVFont(value);
}

PlayListViewMng * AllPlayListViews()
{
	static PlayListViewMng* p=NULL;
	if (p==NULL)
		p=new PlayListViewMng;

	return p;
};
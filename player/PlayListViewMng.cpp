#include "PlayListViewMng.h"
#include "PlayListView.h"
#include "MyLib.h"
void PlayListViewMng::AddItem(CPlayListView* playlistview)
{
	dataCollection.push_back(playlistview);
}

void PlayListViewMng::ClearAllItem()
{
	for (container::iterator i=dataCollection.begin();i!=dataCollection.end();++i)
		(*i)->ClearAllItem();
}


// void PlayListViewMng::Reload(LPCPlayList pPL)
// {
// 	for (container::iterator i=dataCollection.begin();
// 		i!=dataCollection.end();++i)
// 	{
// 		CPlayListView* plv=*i;
// 		plv->Reload(pPL,false);
// 	}
// }



void PlayListViewMng::EnsureVisibleAndCentrePos(int index)
{
	for (container::iterator i=dataCollection.begin();i!=dataCollection.end();++i)
		(*i)->EnsureVisibleAndCentrePos(index);
}

void PlayListViewMng::updateListFont()
{
	for (container::iterator i=dataCollection.begin();i!=dataCollection.end();++i)
		(*i)->updateListFont();
}

void PlayListViewMng::DeleteOneTrackFirst(LPCPlayList pPL)
{
	for (auto it=dataCollection.begin();it!=dataCollection.end();++it)
	{
		CPlayListView* playlistview=*it;
		if (playlistview->GetPlayList()==pPL)
		{
			//playlistview->SetItemCount(pPL->GetItemCount()-1);
			break;
		}
	}
}


void PlayListViewMng::SelectItems(std::vector<int> &items)
{
	for (auto it=dataCollection.begin();it!=dataCollection.end();++it)
	{
		CPlayListView* playlistview=*it;
		playlistview->SelectItems(items);
	}
}


PlayListViewMng * AllPlayListViews()
{
	static PlayListViewMng* p=NULL;
	if (p==NULL)
		p=new PlayListViewMng;

	return p;
};
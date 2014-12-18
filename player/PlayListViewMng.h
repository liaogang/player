#pragma once;
#include <vector>
#include "PlayList.h"
class PlayListViewBase;
class CPlayListView;
#pragma once
class PlayListViewMng
{
public:
	typedef CPlayListView* dataItem;
	typedef std::vector<dataItem>  container;
	container dataCollection;
	void AddItem(CPlayListView* playlistview);



	void RemoveItem(CPlayListView* playlistview)
	{
		for (auto it=dataCollection.begin();it!=dataCollection.end();++it)
		{
			if ((*it)==playlistview)
			{
				dataCollection.erase(it);
				break;
			}
		}
	}

	 void ClearAllItem();

	 //void Reload(LPCPlayList pPL);

	 void Reload(LPCPlayList pPL,int itemActive=-1);

	 void EnsureVisibleAndCentrePos(int index);

	 //set the playlistview which's playlist is pPL,
	 //change nItemPlaying 
	 void PlayingItemChanged();

	 void updateListFont();

	 void DeleteOneTrackFirst(LPCPlayList pPL);


	 void SelectItems(std::vector<int> &items);
};

PlayListViewMng * AllPlayListViews();

#include <vector>

class PlayListViewBase;
class PlayList;
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

	 void Reload(PlayList *pPL);

	 void Reload(PlayList *pPL,int index);

	 void EnsureVisibleAndCentrePos(int index);

	 //set the playlistview which's playlist is pPL,
	 //change nItemPlaying 
	 void PlayingItemChanged();

	 void EnlargeLVFont(int value);
};

PlayListViewMng * AllPlayListViews();

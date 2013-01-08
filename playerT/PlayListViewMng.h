#include <vector>

class PlayListViewBase;
class PlayList;
class PlayListViewMng
{
public:
	typedef PlayListViewBase* dataItem;
	typedef std::vector<dataItem>  container;
	container dataCollection;

	void AddItem(PlayListViewBase* playlistview)
	{
		dataCollection.push_back(playlistview);
	}

	 void ClearAllItem();

	 void Reload(PlayList *pPL);

	 void Reload(PlayList *pPL,bool bCenterItem);

	 void EnsureVisibleAndCentrePos(int index);

	 //set the playlistview which's playlist is pPL,
	 //change nItemPlaying 
	 void PlayingItemChanged();
};

PlayListViewMng * AllPlayListViews();

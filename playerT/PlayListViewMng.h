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

	 void Reload(PlayList *pPL,int centerItem);

	 void EnsureVisibleAndCentrePos(int index);

};

PlayListViewMng * AllPlayListViews();
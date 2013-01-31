
class CMainFrame;
class CMultiSpliltWnd;
class PlayList;
class PlayListItem;

CMainFrame* GetMainFrame();
CMultiSpliltWnd* GetSplitter();

//当前正在操作或可见的播放列表
//不一定是正在播放的音轨所在的列表
PlayList* ActivePlaylist();
void SetActivePlaylist(PlayList* p);


PlayListItem * GetPlayingItem();
void  SetPlayingItem(PlayListItem *item);

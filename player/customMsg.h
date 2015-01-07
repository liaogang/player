#pragma once 

#define  WM_TRACK_POS_CHANGED          (WM_USER+22)  

#define  WM_TRACK_REACH_END   (WM_USER+33)//音轨到达末尾
#define  WM_TRACKSTOPPED      (WM_USER+34)//音轨播放停止

//开始音轨播放
//wparam pointer of trackPosInfo struct  , abandoned now 
#define  WM_NEW_TRACK_STARTED (WM_USER+37)

#define  WM_PAUSED            (WM_USER+38)//暂停音轨播放
#define  WM_PAUSE_START            (WM_USER+39)//开始暂停的音轨播放

//#define  WM_PLAY_DIRECTLY     (WM_USER+41)


//向播放列表添加多个文件时用消息  
//if lParam is 1 , started .the wParam is the pointer to the player list
//if lParam is 2 ,the player list has added a file ,the wParam is TCHAR *filename
//if lParam is 0 ,the adding is ended. the wParam is the number total added.
#define  WM_FILE_FINDED  (WM_USER+40) 



#define  WM_DRAWSPECTRUM (WM_PAINT+913)

 //playlistView will reload playlists
#define  WM_ADDFOLDERED  (WM_USER+35)    

//配置需要生效和保存
#define  WM_CONFIGTOSAVE (WM_USER+36)


//准备重新显示歌词
#define  WM_LYRIC_RELOAD  (WM_USER+50)


//PlayList *pl=(PlayList*)wParam;
#define  WM_PL_WILL_DELETED   (WM_USER+51)

//one certain PlayList is changed
#define  WM_PL_TRACKNUM_CHANGED (WM_USER+52)

//the lib's some play list is changed.
//then should reload the all PlayList
#define  WM_SOME_PL_CHANGED     (WM_USER+59)

//PlayList *pl=(PlayList*)wParam;int nIndex=lParam.
#define  WM_SELECTED_PL_CHANGED (WM_USER+60)



//tell the playlist view to gain the focus
#define WM_PLAYLISTVIEW_SETFOCUS (WM_USER+53)

//center the current playing item in playlist view
#define WM_PLAYLISTVIEW_CENTER_ITEM (WM_USER+54)

//the playlist view 's color need to change
#define WM_PLAYLISTVIEW_COLOR_DEFAULT  (WM_USER+55)
#define WM_PLAYLISTVIEW_COLOR_BLUE   (WM_USER+56)


#define WM_USER_TIMER (WM_USER+57)


//Play queue changed.
#define WM_PLAYQUEUE_CHANGED  (WM_USER+58)
								//(WM_USER+59)
								//(WM_USER+60)



//get windows serialize data.
//blockData *data=(blockData*)wParam;
#define WM_GET_SERIALIZE_DATA  (WM_USER+61)



#define WM_SHOW_DIALOG_CONFIGURE (WM_USER+62)
#define WM_SAVE_CONFIGURE (WM_USER+63)
#define WM_SHOW_DIALOG_SEARCH (WM_USER+64)
#define WM_SHOW_DIALOG_PLAYLIST_MANAGER (WM_USER+65)
#define WM_CHANGE_LISTVIEW_FONT_ENLARGE (WM_USER+66)
#define WM_CHANGE_LISTVIEW_FONT_REDUCE (WM_USER+67)
#define WM_CHANGE_LYRICS_FONT_ENLARGE (WM_USER+66)
#define WM_CHANGE_LYRICS_FONT_REDUCE (WM_USER+67)




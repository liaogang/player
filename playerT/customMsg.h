#pragma once 

#define  RECT_INIT(rc) 	rc.left=0;rc.right=0;rc.top=0;rc.bottom=0;




struct trackPosInfo
{
	double used;
	double left;
public:
	trackPosInfo():used(-1),left(-1)
	{}
};


enum PlayingStatus
{
	status_invalide,
	status_stopped,
	status_playing,
	status_paused
};

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

//added or deleted one playlist
//PlayList changed,added or deleted
//playlist added number
//BOOL bAdd=(BOOL)lParam;
//PlayList *pl=(PlayList*)wParam;
#define  WM_PL_CHANGED    (WM_USER+51)

//wparam , the PlayList pointer
//lparam , the this is -1 , thus the one track of player list will be deleted
#define  WM_PL_TRACKNUM_CHANGED (WM_USER+52)



//tell the playlist view to gain the focus
#define WM_PLAYLISTVIEW_SETFOCUS (WM_USER+53)

//center the current playing item in playlist view
#define WM_PLAYLISTVIEW_CENTER_ITEM (WM_USER+54)

//the playlist view 's color need to change
#define WM_PLAYLISTVIEW_COLOR_DEFAULT  (WM_USER+55)
#define WM_PLAYLISTVIEW_COLOR_BLUE   (WM_USER+56)


#define WM_USER_TIMER (WM_USER+57)



//WPARAM,the buffer to reseive rebar's band class name
//LPARAM,the max length of the buffer
#define WM_GET_BAND_CLASSNAME (WM_USER+58)

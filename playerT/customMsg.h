#pragma once 

void SdMsg(UINT msg,BOOL bPost=TRUE,WPARAM wparam=NULL,LPARAM lparam=NULL);

struct trackPosInfo
{
	double used;
	double left;
};

#define  WM_TRACKPOS          (WM_USER+22)  

#define  WM_TRACK_REACH_END   (WM_USER+53)//音轨到达末尾
#define  WM_TRACKSTOPPED      (WM_USER+34)//音轨播放停止
#define  WM_NEW_TRACK_STARTED (WM_USER+37)//开始音轨播放
#define  WM_PAUSED            (WM_USER+38)//暂停音轨播放
#define  WM_PAUSE_START            (WM_USER+39)//开始暂停的音轨播放

#define  WM_PLAY_DIRECTLY     (WM_USER+41)

//正在添加文件内容  
//wParam TCHAR *filename;
//lParam BOOL , 2 -->file ,1-->started : 0 -->ended
#define  WM_FILE_FINDED  (WM_USER+40) 
#define  WM_DRAWSPECTRUM (WM_PAINT+913)
#define  WM_ADDFOLDERED  (WM_USER+35)     //playlistView will reload playlists

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

//PlayList
//track number added
#define  WM_PL_TRACKNUM_CHANGED (WM_USER+52)



//tell the playlist view to gain the focus
#define WM_PLAYLISTVIEW_SETFOCUS (WM_USER+54)

//the playlist view 's color need to change
#define WM_PLAYLISTVIEW_COLOR_DEFAULT  (WM_USER+55)
#define WM_PLAYLISTVIEW_COLOR_BLUE   (WM_USER+56)
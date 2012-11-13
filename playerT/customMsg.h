#pragma once 
struct trackPosInfo
{
	double used;
	double left;
};

#define  WM_TRACKPOS          (WM_USER+22)  


#define  WM_TRACKSTOPPED      (WM_USER+34)//音轨播放完毕
#define  WM_NEW_TRACK_STARTED (WM_USER+37)//开始音轨播放
//#define  WM_PAUSED            (WM_USER+38)//开始音轨播放
#define  WM_PLAY_DIRECTLY     (WM_USER+39)

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
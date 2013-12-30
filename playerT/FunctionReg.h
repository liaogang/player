#include "stdafx.h"

//------------------------------------------------------
class MYTREE;
void MYTREE_Set_Playlist(MYTREE* tree);
void MYTREE_Set_AlbumView(MYTREE* tree);
void MYTREE_Set_LyricView(MYTREE* tree);
void MYTREE_Set_SpectrumView(MYTREE* tree);


void MYTREE_Add_Playlist(MYTREE* tree);
void MYTREE_Add_EmptyWnd(MYTREE* tree);
void MYTREE_Add_AlbumView(MYTREE* tree);
void MYTREE_Add_LyricView(MYTREE* tree);
void MYTREE_Add_SpectrumView(MYTREE* tree);


typedef  void (*CreateWindowFun)(MYTREE* tree);


void RegisterCreateWndFuns(TCHAR* panename,CreateWindowFun func);
void CreateHWNDbyName(MYTREE *tree);



//------------------------------------------------------
typedef  HWND (*CreateReBarBandFuns)(HWND hWndParent);

static void RegisterCreateRebarBandFuns(TCHAR* szBandClassName,CreateReBarBandFuns func);
HWND CreateRebarBand(TCHAR *szRebarClassName);

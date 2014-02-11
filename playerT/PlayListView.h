#pragma once
#include "stdafx.h"
#include "PlayList.h"
#include "PlayListViewMng.h"
#include "globalStuffs.h"
#include "customMsg.h"
#include "ListCtrl.h"
#include <map>

unsigned int BKDRHash(char *str);

HMENU LoadPlaylistMenu(BOOL bDestroy=FALSE);


#define TEXTALIGN__WIDTH 3




//for compare function
static  CPlayListView* pListCtrl;
static int columnIndexClicked;



class CPlayListView:
	public CListImpl< CPlayListView >
{
private:
	LPCPlayList  m_pPlayList;

public:
	DECLARE_WND_CLASS( _T( "listctrl" ) )

	inline void SetPlayList(LPCPlayList pPlayList){m_pPlayList=pPlayList;}
	inline LPCPlayList GetPlayList(){return m_pPlayList;}
public:
	HMENU menu;
	COLORREF clText1,clText2;
	bool bClientEdge;
	BOOL m_bManual;
	CPlayListView():m_bManual(TRUE),m_Order(0)
	{
		SetPlayList(NULL);
		menu=LoadPlaylistMenu();
	}

	~CPlayListView()
	{
		AllPlayListViews()->RemoveItem(this);

		LoadPlaylistMenu(TRUE);

	}

public:
	//DECLARE_WND_SUPERCLASS( NULL ,CListViewCtrl::GetWndClassName())

	//virtual BOOL PreTranslateMessage(MSG* pMsg)
	//{
	//	if (pMsg->message==WM_KEYDOWN){	
	//		UINT nChar=(TCHAR)pMsg->wParam;
	//		
	//		//Ctrl+A
	//		if (nChar=='A' || nChar=='a'){
	//			if (GetKeyState(VK_CONTROL) &0x80)
	//				SelectAll();
	//		}
	//		
	//		//Delete
	//		else if (nChar==VK_DELETE){
	//			;//DelSelectedItem(GetKeyState(VK_SHIFT) & 0x80);
	//		
	//		}
	//	}//if (pMsg->message!=WM_KEYDOWN)

	//	return FALSE;
	//}
	typedef CPlayListView thisClass;
	BEGIN_MSG_MAP_EX(CPlayListView)
		//user message
		MESSAGE_HANDLER(WM_PLAYLISTVIEW_SETFOCUS,OnSetFocus)
		MESSAGE_HANDLER(WM_PLAYLISTVIEW_COLOR_DEFAULT,OnChColorDefault);
		MESSAGE_HANDLER(WM_PLAYLISTVIEW_COLOR_BLUE,OnChColorBlue);
		MESSAGE_HANDLER(WM_SELECTED_PL_CHANGED,OnSelectedPlChanged)

		MSG_WM_CHAR(OnChar)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_CREATE(OnCreate)
		COMMAND_ID_HANDLER(ID_OPEN_FILE_PATH,OnOpenFilePath)
		COMMAND_ID_HANDLER(ID_PUSH_PLAYQUEUE,OnPushToPlayqueue)
		COMMAND_ID_HANDLER(ID_DELFROMPLAYQUEUE,OnDeleteFromPlayqueue)
		REFLECTED_NOTIFY_CODE_HANDLER( LCN_DBLCLICK, OnDbClicked)
		REFLECTED_NOTIFY_CODE_HANDLER(LCN_SELECTED, OnSelected)
		REFLECTED_NOTIFY_CODE_HANDLER(LCN_RIGHTCLICK, OnRightClick)
		CHAIN_MSG_MAP(CListImpl< CPlayListView >)
	END_MSG_MAP()

	void OnDestroy();

	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SetFocus();

		SetActiveWindow();
		
		SetForegroundWindow(m_hWnd);
		

		//SendMessage(WM_CHILDACTIVATE); 
		
		return 1;
	}

	LRESULT OnChColorDefault(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ChangeColorDefault();
		return 1;
	}

	LRESULT OnChColorBlue(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ChangeColorBlue();
		return 1;
	}

	LRESULT OnSelectedPlChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CPlayList *pl=(CPlayList*)wParam;
		
		Reload(pl);

		return 1;
	}
	

	int OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		DWORD dwStyle ;
		dwStyle= ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
		bClientEdge=(dwStyle & WS_EX_CLIENTEDGE)?true:false;

		// 			CMessageLoop* pLoop = _Module.GetMessageLoop();
		// 			ATLASSERT(pLoop != NULL);
		// 			pLoop->AddMessageFilter(this);

		SetMsgHandled(FALSE);
		return 0;
	}


	UINT m_Order;
	static bool CompareProc(LPCPlayListItem track1, LPCPlayListItem track2)
	{
		bool result;

		const TCHAR  * str1;
		const TCHAR  * str2;


		switch(columnIndexClicked)
		{
		case COLUMN_INDEX_TITLE:
			str1=track1->GetTitle().c_str();
			str2=track2->GetTitle().c_str();
			break;
		case COLUMN_INDEX_ARTIST:
			str1=track1->GetArtist().c_str();
			str2=track2->GetArtist().c_str();
			break;
		case COLUMN_INDEX_ALBUM:
			str1=track1->GetAlbum().c_str();
			str2=track2->GetAlbum().c_str();
			break;
		case COLUMN_INDEX_YEAR:
			{
				UINT uYear1,uYear2;
				str1=track1->GetYear().c_str();
				str2=track2->GetYear().c_str();
				if(str1==_T("?"))
					uYear1=0;
				else if(str2==_T("?"))
					uYear2=0;
				else
				{
					uYear1=_wtoi(str1);
					uYear2=_wtoi(str2);
				}
				result= uYear1>uYear2;
				goto theEnd;
			}
		case COLUMN_INDEX_GENRE:
			str1=track1->GetGenre().c_str();
			str2=track2->GetGenre().c_str();
			break;
		}


		result = _tcscmp(str1, str2) > 0;

theEnd:
		return (pListCtrl->m_Order & (1<<columnIndexClicked))?result:!result;
	}



	void SortItems( int nColumn, BOOL bAscending )
	{
		int columnOrder=nColumn;

		//index to order
		columnIndexClicked=GetColumnIndex(columnOrder);

		if(columnIndexClicked!=COLUMN_INDEX_INDEX)
		{
			pListCtrl=this;

			GetPlayList()->SortItems(CompareProc);

			m_Order = m_Order  ^ ( 1<< columnIndexClicked );

			ScrollByTop(GetPlayList()->GetSelectedIndex());	
			SelectItem(GetPlayList()->GetSelectedIndex());
		}
	}

	void ReverseItems() // overrides CListImpl::ReverseItems
	{
		GetPlayList()->ReverseItems();
	}




	void SelectItems(vector<int> &items)
	{
		ClearAllSel();

		for (auto i=items.begin();i!=items.end();i++)
			SelectItem(*i,NULL_SUBITEM,MK_CONTROL);

		EnsureVisibleAndCentrePos(items[0]);
	}


#ifdef Old_Version
	void DelSelectedItem(BOOL bDelFile=FALSE)
	{
		// 			if (bDeletable)
		// 			{
		// 				for(int i = GetItemCount()-1; i>=0;--i)
		// 					if( LVIS_SELECTED==GetItemState(i, LVNI_ALL | LVNI_SELECTED) )
		// 						DeleteItem(i);
		// 			}
		//CListCtrl ctrl;ctrl.GetNextSelectedItem()GetNextItem((UINT)nOldPos, LVIS_SELECTED)
		if (GetPlayList()->m_bAuto)
			return;

		//从vector删除某项,需要重新搬家,很费时.
		int blockBeg,blockLast;


		int nCount=GetItemCount();
		int countToDel=0;

		bool *itemDel=new bool[nCount];
		memset(itemDel,0,nCount*sizeof(bool));

		int nItem=blockBeg=GetNextItem(-1,LVIS_SELECTED);
		while(-1!=(nItem=GetNextItem(nItem,LVIS_SELECTED)) )
		{
			itemDel[nItem]=true;
			blockLast=nItem;
			++countToDel;
		}

		//isEntireBlock?			
		if ((blockLast-blockBeg+1)==countToDel)
		{
			if (countToDel==nCount)
			{
				SetItemCount(0);
				GetPlayList()->m_songList.clear();
			}
			else
			{
				for (int i=blockLast;i>=blockBeg;--i)
					DeleteItem(i);
				GetPlayList()->DeleteTrack(blockBeg,blockLast);
			}
		}
		else
		{
			for (int i=nCount-1;i>=0;--i)
			{
				if(itemDel[i])
				{
					GetPlayList()->DeleteTrack(i);
					DeleteItem(i);
				}
			}
		}


		/*
		if ( bDelFile )
		{
		TCHAR bf[5]={};
		_itow(countToDel,bf,10);

		TCHAR title[]=_T("这将会删除\0                     ");
		TCHAR titleP3[]=_T("文件 \n要继续吗?");
		_tcscat(title,bf);
		_tcscat(title,titleP3);

		bDelFile= (IDYES==::MessageBox(m_hWnd,title,_T("确认删除文件"),MB_YESNO));
		}

		if (bDelFile) ::DeleteFile(GetPlayList()->DeleteTrack(i)->url.c_str());
		*/


		delete[] itemDel;




		// 
		// 			for(int i = GetItemCount()-1; i>=0;--i)
		// 			{
		// 				if(LVIS_SELECTED == GetItemState(i, LVIS_SELECTED) )
		// 				{
		// 					PlayListItem *track=(PlayListItem*)GetItemData(i);
		// 					if(track )
		// 					{
		// 						if(bDelFile && IDYES==::MessageBox(m_hWnd,_T("这将会删除 1 文件 \n要继续吗?"),_T("确认删除文件"),MB_YESNO))
		// 						{
		// 							::DeleteFile(track->url.c_str());
		// 						}
		// 
		// 						//track->m_pPL->DeleteTrack(track);
		// 					}
		// 
		// 					DeleteItem(i);
		// 				}
		// 			}	

	}
#endif

	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);



#ifdef Old_Version

	BOOL GetFirstSelItem()
	{
		int nItem=-1;
		if (GetItemCount()>0)
			nItem=GetNextItem(nItem,LVNI_SELECTED);

		return nItem;
	}
#endif


	LRESULT OnSelected(int /**/,LPNMHDR pnmh,BOOL bHandled);
	LRESULT OnRightClick(int /**/,LPNMHDR pnmh,BOOL bHandled)
	{
		POINT pt;
		GetCursorPos(&pt);

		if(HasSeleted())
		{
			::EnableMenuItem(menu,ID_DELFROMPLAYQUEUE,MF_BYCOMMAND|IsAllSelectedItemInPlayQueue()?MF_ENABLED:(MF_DISABLED | MF_GRAYED));

			::TrackPopupMenu(menu,TPM_LEFTALIGN,pt.x,pt.y,0,m_hWnd,0);
		}
		return 0;
	}

	LRESULT OnDbClicked(int /**/,LPNMHDR pnmh,BOOL bHandled);


	enum{
		COLUMN_INDEX_INDEX,
		COLUMN_INDEX_TITLE,
		COLUMN_INDEX_ARTIST,
		COLUMN_INDEX_ALBUM,
		COLUMN_INDEX_YEAR,
		COLUMN_INDEX_GENRE
	};


	CFont m_Font;
	int   m_nFontHeight;
	void SetLVFont(int nHeight)
	{
		m_nFontHeight=nHeight;

		if (!m_Font.IsNull())
			m_Font.DeleteObject();

		m_Font.CreateFont(
			m_nFontHeight,                        // nHeight
			0,                         // nWidth
			0,                         // nEscapement
			0,                         // nOrientation
			FW_NORMAL,                 // nWeight
			FALSE,                     // bItalic
			FALSE,                     // bUnderline
			0,                         // cStrikeOut
			ANSI_CHARSET,              // nCharSet
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			DEFAULT_QUALITY,           // nQuality
			DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
			_T("Arial"));                 // lpszFacename
		SetFont(m_Font.m_hFont);
	}

	void EnlargeLVFont(int value)
	{
		m_nFontHeight+=value;
		SetLVFont(m_nFontHeight);
	}

	void CreateIsWnd();

	void Init(bool bSearch=false);

	int GetTextWidth(LPCTSTR str)
	{
		CClientDC dcClient( m_hWnd );
		CSize sizeExtent;
		if ( !dcClient.GetTextExtent( str, -1, &sizeExtent ) )
			return FALSE;
		return sizeExtent.cx;
	}

	// 		int GetIndex(PlayListItem *item)
	// 		{
	// 			// 			int nItem= GetPlayList()->GetItem(item);
	// 			// 			return nItem==GetPlayList()->GetItemCount()?-1:nItem;
	// 
	// 			return 0;
	// 		}


	// 		void SetPlayingItem(PlayListItem *item)
	// 		{
	// 			SetPlayingItem(GetIndex(item));
	// 		}

public:
	// 	void SetPlayingItem(int nItem)
	// 	{
	// 		int nItemPlaying=GetPlayingIdx();
	// 		//RedrawItems(nItemPlaying,nItemPlaying);
	// 		SetPlayingIdx(nItem);
	// 		//RedrawItems(nItem,nItem);
	// 	}

	void ClearAllSel()
	{
		ResetSelected();
	}

	//void InsertTrackItem(PlayListItem &track,int item,BOOL SetIndex=TRUE);
	//inline void InsertTrackItem(PlayListItem *track,int item,BOOL SetIndex=TRUE){InsertTrackItem(*track,item,SetIndex);}


	void LoadPlayList(LPCPlayList pPlayList)
	{
		SetPlayList(pPlayList);

		DeleteAllItems();
		if(pPlayList)
			for (int i=0;i<GetPlayList()->GetItemCount();++i)
				AddItem();
	}


	BOOL DragItem()
	{

		return TRUE;
	}

	HFONT GetItemFont( int nItem, int nSubItem )
	{
		return m_Font;
	}

	BOOL GetItemColours( int nItem, int nSubItem, COLORREF& rgbBackground, COLORREF& rgbText )
	{
		if(IsSelected( nItem ))
			return FALSE;

		rgbBackground = nItem%2==0?clText1:clText2;
		return TRUE;
	}

	CString GetItemText( int nItem, int nSubItem ) // required by CListImpl
	{
		CString result;
		LPCPlayListItem track=GetPlayList()->GetItem(nItem);
		if(!track) return result;

		int iItemIndx=nItem;

		switch(nSubItem){
		case COLUMN_INDEX_INDEX:
			{
				TCHAR strIndex[4]={0};
				_itow_s(iItemIndx+1,strIndex,10);
				result=strIndex;
			}
			break;
		case COLUMN_INDEX_TITLE: //fill in main text
			result=track->GetTitle().c_str();
			break;
		case COLUMN_INDEX_ARTIST: //fill in sub item 1 text
			result=track->GetArtist().c_str();
			break;
		case COLUMN_INDEX_ALBUM: //fill in sub item 2 text

			result=track->GetAlbum().c_str();
			break;
		case COLUMN_INDEX_YEAR:

			result=track->GetYear().c_str();
			break;
		case COLUMN_INDEX_GENRE:

			result=track->GetGenre().c_str();
			break;
		}

		return result;
	}

	int GetItemCount() // required by CListImpl
	{
		if(!GetPlayList())return 0;
		return GetPlayList()->GetItemCount();
	}

	void ClearAllItem()
	{
		DeleteAllItems();
	}

	void SelectAndFocusItem(int item)
	{
		SelectItem(item,-1,LVIS_FOCUSED|LVIS_SELECTED);
	}




	void Reload(LPCPlayList pPlayList,int itemActive=-1)
	{
		//resotre the curr croll bar's pos
		if(GetPlayList())
			GetPlayList()->SetTopVisibleIndex(GetTopItem());

		m_bManual=FALSE;

		//if some play list is deleted.
		if(pPlayList == NULL)
		{
			LoadPlayList(pPlayList);
			return;
		}

		if ( GetPlayList()!=pPlayList)
		{
			ClearAllSel();
			LoadPlayList(pPlayList);
		}

		if(itemActive!=-1)
		{
			SelectAndFocusItem(itemActive);
		}
		else
		{
			//so we resotre the last scroll pos and,
			//highlight last selected item
			SelectItem(GetPlayList()->GetSelectedIndex(),-1,LVIS_SELECTED,FALSE,FALSE);
			ScrollByTop(GetPlayList()->GetTopVisibleIndex());
		}


		m_bManual=TRUE;
	}


	void ScrollByTop(int topIndex)
	{
		EnsureItemVisible(topIndex,-1,FALSE);
	}

	void EnsureVisibleAndCentrePos(int index)
	{
		EnsureItemVisible(index,-1,TRUE);
	}

	BOOL IsAllSelectedItemInPlayQueue();
	LRESULT OnPushToPlayqueue(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnDeleteFromPlayqueue(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOpenFilePath(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		int nItem=GetFirrstSelectedItem();

		if(nItem!=-1)
		{
			LPCPlayListItem track=GetPlayList()->GetItem(nItem);

			std::tstring parameters=_T("/select,");
			parameters+=track->GetUrl();

			std::tstring dir;
			int index=track->GetUrl().find_last_of(_T("\\"));
			if (index!=track->GetUrl().npos)
				dir+=track->GetUrl().substr(0,index);

			/*   example
			*   "/select,C:\a.txt"
			*/
			ShellExecute(NULL,_T("open"),_T("explorer"),
				parameters.c_str(),dir.c_str(),SW_SHOW);
		}
		return 0;
	}

	//奇,偶,颜色
	void ChangeColor(COLORREF one,COLORREF another)
	{
		clText1=one;
		clText2=another;

		Invalidate();
	}

	void ChangeColorDefault()
	{
		HBRUSH brush= ::GetSysColorBrush(COLOR_WINDOW);
		LOGBRUSH lb;
		GetObject(brush,sizeof(LOGBRUSH),&lb);
		COLORREF c=lb.lbColor;
		INT r= GetRValue(c);
		INT g= GetGValue(c);
		INT b= GetBValue(c);

		int average=(r+g+b)/3;
		int offset=0;
		if (3<=average && average<=100)
			offset=-3;
		else if (average>100 && average <=180)
			offset=-5;
		else
			offset=-7;

		COLORREF a=RGB(r+offset,g+offset,b+offset);


		ChangeColor(c,a);
		//使用新式风格
		//SetWindowTheme(m_hWnd,_T("Explorer"),0);
		//取消焦点状态
		//SetCallbackMask(LVIS_FOCUSED);
	}

	void ChangeColorBlue()
	{
		ChangeColor(RGB(230,244,255),RGB(145,200,255));
		//使用旧式风格
		//SetWindowTheme(m_hWnd,_T(""),0);
		//使用焦点状态
		//SetCallbackMask(NULL);
	}

	void OnFinalMessage(_In_ HWND /*hWnd*/)
	{
		if(GetPlayList() && GetPlayList()->IsSearch())
			;
		else
			delete this;
	}


};



class CPlayListViewS :public CPlayListView
{
public:
	typedef  CPlayListViewS thisClass;
	BEGIN_MSG_MAP_EX(CPlayListViewS)
		MESSAGE_HANDLER(WM_PLAYLISTVIEW_SETFOCUS,OnSetFocus)
		COMMAND_ID_HANDLER(ID_PUSH_PLAYQUEUE,OnPushToPlayqueue)
		COMMAND_ID_HANDLER(ID_DELFROMPLAYQUEUE,OnDeleteFromPlayqueue)
		REFLECTED_NOTIFY_CODE_HANDLER(LCN_SELECTED, OnSelected)
		REFLECTED_NOTIFY_CODE_HANDLER( LCN_DBLCLICK, OnDbClicked)
		REFLECTED_NOTIFY_CODE_HANDLER(LCN_RIGHTCLICK, OnRightClick)
		CHAIN_MSG_MAP(CPlayListView)
	END_MSG_MAP()

	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 1;
	}

	BOOL IsAllSelectedItemInPlayQueue();
	
	LRESULT OnPushToPlayqueue(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	
	LRESULT OnDeleteFromPlayqueue(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnSelected(int /**/,LPNMHDR pnmh,BOOL bHandled);

	LRESULT OnDbClicked(int /**/,LPNMHDR pnmh,BOOL bHandled);

	//the target play-list witch is being searching
	LPCPlayList playlistParent;


	map<LPCPlayListItem,LPCPlayListItem> playListItemMap;
	typedef pair<LPCPlayListItem,LPCPlayListItem> playListItemPair;
	void Add2Map(LPCPlayListItem itemInSearch,LPCPlayListItem itemInParent)
	{
		playListItemMap.insert(playListItemPair(itemInSearch,itemInParent));
	}

	void ClearMap()
	{
		playListItemMap.clear();
	}

	LPCPlayListItem GetItemInParent(LPCPlayListItem itemInSearch)
	{
		LPCPlayListItem itemInParent=NULL;

		auto it=playListItemMap.find(itemInSearch);
		if(it!=playListItemMap.end())
			itemInParent=it->second;

		return itemInParent;
	}

	LRESULT OnRightClick(int /**/,LPNMHDR pnmh,BOOL bHandled)
	{
		POINT pt;
		GetCursorPos(&pt);

		if(HasSeleted())
		{
			::EnableMenuItem(menu,ID_DELFROMPLAYQUEUE,MF_BYCOMMAND|IsAllSelectedItemInPlayQueue()?MF_ENABLED:(MF_DISABLED | MF_GRAYED));

			::TrackPopupMenu(menu,TPM_LEFTALIGN,pt.x,pt.y,0,m_hWnd,0);
		}
		return 0;
	}
};

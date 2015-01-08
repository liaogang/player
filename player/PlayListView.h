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
	public CListImpl< CPlayListView >,
	public SerializeObj< CPlayListView >
{
private:
	CFont m_Font;
	
	static const int m_iColumnCount=6;

	INT m_iColumnOrderArrays[m_iColumnCount];

	INT m_iColumnWidths[m_iColumnCount];


	BOOL m_bLoaded;//from file
	blockData m_dData;//data to save or load.

	enum{
		COLUMN_INDEX_INDEX,
		COLUMN_INDEX_TITLE,
		COLUMN_INDEX_ARTIST,
		COLUMN_INDEX_ALBUM,
		COLUMN_INDEX_YEAR,
		COLUMN_INDEX_GENRE
	};
protected:
	LPCPlayList  m_pPlayList;
	HMENU menu;
	COLORREF clText1,clText2;
	bool bClientEdge;
	BOOL m_bManual;
	UINT m_Order;
public:


	DECLARE_WND_CLASS( _T( "listctrl" ) )

	CPlayListView():m_bManual(TRUE),m_Order(0),m_bLoaded(FALSE),
		m_dData(blockData())
	{
		SetPlayList(NULL);
		menu=LoadPlaylistMenu();
		RegisterClass();
	}

	~CPlayListView()
	{
		AllPlayListViews()->RemoveItem(this);

		LoadPlaylistMenu(TRUE);
	}

	inline void SetPlayList(LPCPlayList pPlayList){m_pPlayList=pPlayList;}
	inline LPCPlayList GetPlayList(){return m_pPlayList;}

	FILE& operator<<(FILE& f);
	FILE& operator>>(FILE& f) const ;


	BEGIN_MSG_MAP_EX(CPlayListView)
		//user message
		MESSAGE_HANDLER(WM_PLAYLISTVIEW_SETFOCUS,OnSetFocus)
		MESSAGE_HANDLER(WM_PLAYLISTVIEW_COLOR_DEFAULT,OnChColorDefault)
		MESSAGE_HANDLER(WM_PLAYLISTVIEW_COLOR_BLUE,OnChColorBlue)
		MESSAGE_HANDLER(WM_SELECTED_PL_CHANGED,OnSelectedPlChanged)
		MESSAGE_HANDLER(WM_GET_SERIALIZE_DATA,OnGetSerializeData)


		MESSAGE_HANDLER(WM_PL_TRACKNUM_CHANGED,OnPLTrackNumChanged)
		MESSAGE_HANDLER(WM_CHANGE_LISTVIEW_FONT_ENLARGE,OnMsgChangeListViewFontEnlarge)
		MESSAGE_HANDLER(WM_CHANGE_LISTVIEW_FONT_REDUCE ,OnMsgChangeListViewFontReduce)


		MSG_WM_CHAR(OnChar)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_CREATE(OnCreate)
		COMMAND_ID_HANDLER(ID_PLAYITEM,OnPlayItem)
		COMMAND_ID_HANDLER(ID_OPEN_FILE_PATH,OnOpenFilePath)
		COMMAND_ID_HANDLER(ID_PUSH_PLAYQUEUE,OnPushToPlayqueue)
		COMMAND_ID_HANDLER(ID_DELFROMPLAYQUEUE,OnDeleteFromPlayqueue)
		REFLECTED_NOTIFY_CODE_HANDLER( LCN_DBLCLICK, OnDbClicked)
		REFLECTED_NOTIFY_CODE_HANDLER(LCN_SELECTED, OnSelected)
		REFLECTED_NOTIFY_CODE_HANDLER(LCN_RIGHTCLICK, OnRightClick)
		CHAIN_MSG_MAP(CListImpl< CPlayListView >)
	END_MSG_MAP()

	void OnDestroy();

	const blockData & GetSerializeData () const {return m_dData;}
	blockData* GetSerializeData () {return &m_dData;}
	const blockData* GetSerializeDataP () const {return &m_dData;}
	void SetSerializeData(blockData *bd)
	{
		m_dData.Take(*bd);
	}

	
	LRESULT OnGetSerializeData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		blockData *data=(blockData*)wParam;
		ATLASSERT(data->GetLength() ==0 );

		Save();
		
		data->Take(m_dData);
		
		return 1;
	}	

	LRESULT OnPLTrackNumChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LPCPlayList pl=(LPCPlayList)wParam;
		
		if (pl == GetPlayList())
			Reload(pl);

		return 0;
	}

	LRESULT OnMsgChangeListViewFontEnlarge(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		EnlargeLVFont(1);
		return 1;
	}	

	LRESULT OnMsgChangeListViewFontReduce(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		EnlargeLVFont(-1);
		return 1;
	}

	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SetFocus();

		SetActiveWindow();
		
		SetForegroundWindow(m_hWnd);
		
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

		auto i=items.begin();
		auto end=items.end();
		for (;i!=end;++i)
			SelectItem(*i,NULL_SUBITEM,MK_CONTROL,FALSE,FALSE);

		EnsureVisibleAndCentrePos(items[0]);
	}

	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

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

	void SetLVFont(int nHeight)
	{
		if (!m_Font.IsNull())
			m_Font.DeleteObject();

		m_Font.CreateFont(
			nHeight,                        // nHeight
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

	void EnlargeLVFont(int value = 1);

	
	void updateListFont();

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


public:

	void ClearAllSel()
	{
		ResetSelected();
	}

	void LoadPlayList(LPCPlayList pPlayList)
	{
		SetPlayList(pPlayList);

		DeleteAllItems();
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

		rgbBackground = (nItem%2==0 ? clText1:clText2);
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
		m_bManual=FALSE;

		//resotre the curr croll bar's pos
		if(GetPlayList())
			GetPlayList()->SetTopVisibleIndex(GetTopItem());

		if ( GetPlayList()!= pPlayList)
		{
			LoadPlayList(pPlayList);
		}

		if(pPlayList == NULL)
			return;

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
		EnsureItemVisible(topIndex,-1,FALSE,FALSE);
		EnsureItemVisible(topIndex + GetCountPerPage( FALSE ) - 1,-1,FALSE);
	}

	void EnsureVisibleAndCentrePos(int index)
	{
		EnsureItemVisible(index,-1,TRUE);
	}

	
	LRESULT OnPlayItem(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
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

	void Save();
	void Load();


	void OnFinalMessage(_In_ HWND /*hWnd*/);



};



class CPlayListViewS :public CPlayListView
{
public:
	typedef  CPlayListViewS thisClass;
	BEGIN_MSG_MAP_EX(CPlayListViewS)
		MSG_WM_CHAR(OnChar)
		MESSAGE_HANDLER(WM_SELECTED_PL_CHANGED, OnSelectedPlChanged)
		MESSAGE_HANDLER(WM_PLAYLISTVIEW_SETFOCUS,OnSetFocus)
		COMMAND_ID_HANDLER(ID_PUSH_PLAYQUEUE,OnPushToPlayqueue)
		COMMAND_ID_HANDLER(ID_DELFROMPLAYQUEUE,OnDeleteFromPlayqueue)
		REFLECTED_NOTIFY_CODE_HANDLER(LCN_SELECTED, OnSelected)
		REFLECTED_NOTIFY_CODE_HANDLER( LCN_DBLCLICK, OnDbClicked)
		REFLECTED_NOTIFY_CODE_HANDLER(LCN_RIGHTCLICK, OnRightClick)
		CHAIN_MSG_MAP(CPlayListView)
	END_MSG_MAP()
	
	LRESULT OnSelectedPlChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	

	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

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

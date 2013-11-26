#pragma once
#include "stdafx.h"
#include "PlayList.h"
#include "PlayListViewMng.h"
#include "globalStuffs.h"
#include "customMsg.h"

#include "ListCtrl.h"

unsigned int BKDRHash(char *str);

HMENU LoadPlaylistMenu(BOOL bDestroy=FALSE);


#define TEXTALIGN__WIDTH 3




//for compare function
static  CPlayListView* pListCtrl;
static int columnIndexClicked;



class CPlayListView:
	public CListImpl< CPlayListView >,
 	public CMessageFilter
{
private:
	PlayList *  m_pPlayList;
protected:
	
public:
	inline int GetPlayingIdx(){return GetPlayList()->GetPlayingIndex();}
	inline void SetPlayingIdx(int i){GetPlayList()->SetPlayingIndex(i);}
	inline void SetPlayList(PlayList * pPlayList){m_pPlayList=pPlayList;}
	inline PlayList * GetPlayList(){return m_pPlayList;}

	DECLARE_WND_CLASS( _T( "listctrl" ) )

public:
	HMENU menu;
	COLORREF clText1,clText2;
	bool bClientEdge;

	CPlayListView():m_bC(TRUE),m_Order(0)
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

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (pMsg->message==WM_KEYDOWN){	
			UINT nChar=(TCHAR)pMsg->wParam;
			
			//Ctrl+A
			if (nChar=='A' || nChar=='a'){
				if (GetKeyState(VK_CONTROL) &0x80)
					SelectAll();
			}
			
			//Delete
			else if (nChar==VK_DELETE){
				;//DelSelectedItem(GetKeyState(VK_SHIFT) & 0x80);
			
			}
		}//if (pMsg->message!=WM_KEYDOWN)

		return FALSE;
	}
	 
	BEGIN_MSG_MAP_EX(CPlayListView)
		//user message
		//MESSAGE_HANDLER(WM_PLAYLISTVIEW_SETFOCUS,OnSetFocus)
		//MESSAGE_HANDLER(WM_PLAYLISTVIEW_COLOR_DEFAULT,OnChColorDefault);
		//MESSAGE_HANDLER(WM_PLAYLISTVIEW_COLOR_BLUE,OnChColorBlue);
		
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_CREATE(OnCreate)
		//MESSAGE_HANDLER(WM_PAINT,OnPaint)
		//MSG_WM_ERASEBKGND(OnEraseBkgnd)
		//MSG_WM_LBUTTONDBLCLK(OnDbClicked)
		//MSG_WM_CHAR(OnChar)
		MESSAGE_HANDLER(WM_NCPAINT,OnNcPaint)
		COMMAND_ID_HANDLER(ID_OPEN_FILE_PATH,OnOpenFilePath)
		COMMAND_ID_HANDLER(ID_PUSH_PLAYQUEUE,OnPushToPlayqueue)
		COMMAND_ID_HANDLER(ID_DELFROMPLAYQUEUE,OnDeleteFromPlayqueue)
		//REFLECTED_NOTIFY_CODE_HANDLER_EX(NM_RCLICK ,OnNotifyCodeHandlerEX)
		//REFLECTED_NOTIFY_CODE_HANDLER(LVN_ITEMCHANGED,OnItemChanged)
		//REFLECTED_NOTIFY_CODE_HANDLER(LVN_GETDISPINFO,OnGetdispInfo)
		//REFLECTED_NOTIFY_CODE_HANDLER(NM_CUSTOMDRAW,OnCustomDraw)
		//REFLECTED_NOTIFY_CODE_HANDLER(LVN_COLUMNCLICK,OnColumnClick)
		
		REFLECTED_NOTIFY_CODE_HANDLER( LCN_DBLCLICK, OnDbClicked)
		REFLECTED_NOTIFY_CODE_HANDLER(LCN_SELECTED, OnSelected)
		REFLECTED_NOTIFY_CODE_HANDLER(LCN_RIGHTCLICK, OnRightClick)
		

		CHAIN_MSG_MAP(CListImpl< CPlayListView >)
	END_MSG_MAP()

		void FillSolidRect(HDC m_hDC,LPCRECT lpRect, COLORREF clr)
		{
			ATLASSERT(m_hDC != NULL);

			COLORREF clrOld = ::SetBkColor(m_hDC, clr);
			ATLASSERT(clrOld != CLR_INVALID);
			if(clrOld != CLR_INVALID)
			{
				::ExtTextOut(m_hDC, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
				::SetBkColor(m_hDC, clrOld);
			}
		}

		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			//An application returns zero if it processes this message. 
			CPaintDC dc(m_hWnd);

			RECT headerRect;
			GetDlgItem(0).GetWindowRect(&headerRect);
			ScreenToClient(&headerRect);
			dc.ExcludeClipRect(&headerRect);

			// Paint to a memory device context to help
			// eliminate screen flicker.
			CMemDC memDC(dc.m_hDC,&dc.m_ps.rcPaint);

			RECT clip;
			GetClipBox(memDC,&clip);
			FillSolidRect(memDC, &clip ,GetSysColor(COLOR_WINDOW));		

			// Now let the window do its default painting...
			DefWindowProc(WM_PAINT, (WPARAM)memDC.m_hDC, (LPARAM)0);





			return 0;
		}


		/*
		BOOL OnEraseBkgnd(CDCHandle dc)
		{
			//no bkgnd repaint needed
				return 1;
		}

		void DrawLine(HDC dc,int nItem)
		{
			LVITEM item;
			item.iItem = nItem;
			item.iSubItem = 0;
			item.mask = LVIF_STATE;
			item.stateMask = 0XFFFF;

			GetItem(&item);
			BOOL selected = item.state & LVIS_SELECTED;
			BOOL focused  = item.state & LVIS_FOCUSED;
		   		
			RECT rcCol;
			ListView_GetItemRect(m_hWnd,nItem,&rcCol,LVIR_SELECTBOUNDS)	;

			//普通
			//白底,黑字
			int B=nItem%2?clText1 : clText2;
			int T=COLOR_WINDOWTEXT ;
			//蓝底,白字
			const int SFB=COLOR_HIGHLIGHT;
			const int SFT=COLOR_WINDOW;
			//灰底,黑字
			const int SB=COLOR_BTNFACE;
			const int ST=COLOR_WINDOWTEXT;

			COLORREF clBackgnd;
			COLORREF clText;
			if(selected)
			{
				clBackgnd=::GetSysColor(SFB);
				clText=::GetSysColor(SFT);
			}
			else if(  focused )
			{	
				clBackgnd=::GetSysColor(SB);
				clText=::GetSysColor(ST);
			}
			else
			{
				clBackgnd=B;
				clText=::GetSysColor(T);
			}


			//行背景
			HPEN hNewPen=::CreatePen(PS_SOLID,1, clBackgnd );
			HBRUSH hNewBrush= ::CreateSolidBrush(clBackgnd);

			HPEN hOldPen = (HPEN)::SelectObject(dc, hNewPen);
			HBRUSH hOldBrush = (HBRUSH)::SelectObject(dc,hNewBrush );

			::Rectangle(dc, rcCol.left, rcCol.top, rcCol.right, rcCol.bottom);

			//如果选中项具有焦点,画出虚线矩形焦点边框
			if(focused )//&& GetFocus()==m_hWnd)
			{
				::DrawFocusRect(dc,&rcCol);	
			}

			//draw text
			::DeleteObject(hNewPen);
			hNewPen=::CreatePen(PS_SOLID,1,clText);
			SelectObject(dc,hNewPen);

			::SetBkMode(dc,TRANSPARENT);
			::SetTextColor(dc,clText);

			LV_COLUMN lvc;
			lvc.mask=LVCF_FMT;
			for(int nOrder=0; GetColumn(nOrder,&lvc); ++nOrder)
			{
				int nSubItem=Header_OrderToIndex(ListView_GetHeader(m_hWnd),nOrder);

				RECT rcSubItem;
				ListView_GetSubItemRect(m_hWnd,nItem,nSubItem,LVIR_BOUNDS,&rcSubItem);
				
				UINT DT_S=DT_LEFT;
				if (lvc.fmt & HDF_CENTER)
					DT_S=DT_CENTER;	
				else if (lvc.fmt & HDF_RIGHT)
					DT_S=DT_RIGHT;

				WCHAR szText[MAX_PATH];
				int strLen=GetItemText(nItem,nSubItem,szText,MAX_PATH);

				DrawText(dc,szText, strLen, &rcSubItem, DT_S|DT_VCENTER|DT_SINGLELINE |DT_WORD_ELLIPSIS);
			}

			::SelectObject(dc, hOldBrush);
			::DeleteObject(hNewBrush);

			::SelectObject(dc,hOldPen);
			::DeleteObject(hNewPen);
		}
		*/

		void OnDestroy();

		/*
		LRESULT OnCustomDraw(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
		{
			LPNMCUSTOMDRAW lpNMCustomDraw = (LPNMCUSTOMDRAW)pnmh;
			NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( lpNMCustomDraw );  

			DWORD dwRet;
			switch(lpNMCustomDraw->dwDrawStage)
			{
			case CDDS_PREPAINT:
				dwRet= CDRF_NOTIFYITEMDRAW ;
				break;
			case CDDS_ITEMPREPAINT:
				DrawLine(pLVCD->nmcd.hdc,pLVCD->nmcd.dwItemSpec);
				dwRet= CDRF_SKIPDEFAULT ;
				break;
			}

			return dwRet;
		}
		*/

		LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			if (bClientEdge)
				DrawSunkenInnerEdge();

			return 1;
		}


		LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			SetFocus();
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
		

		LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			// return zero  if processed

			if (bClientEdge)
			{
				//let system draw scroll bar it self
				::DefWindowProc(m_hWnd,uMsg,wParam,lParam);
				DrawSunkenInnerEdge(wParam);
				return 0;
			}


			bHandled=FALSE;
			return 1;
		}

		void DrawSunkenInnerEdge(WPARAM wParam=1)
		{
			HDC hdc;
			if((int)wParam==1)
				hdc=::GetWindowDC(m_hWnd);
			else
				hdc = ::GetDCEx(m_hWnd,(HRGN)wParam, DCX_WINDOW|DCX_INTERSECTRGN);

			RECT r;
			GetWindowRect(&r);
			ScreenToClient(&r);
			r.right-=r.left;
			r.bottom-=r.top;
			r.left=0;
			r.top=0;

			DrawEdge(hdc,&r, EDGE_SUNKEN, BF_RECT | BF_ADJUST);
			ReleaseDC(hdc);
		}

		LRESULT OnMeasureItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
		{
			LPMEASUREITEMSTRUCT lpMeasureItemStruct=(LPMEASUREITEMSTRUCT)lParam;
			lpMeasureItemStruct->itemHeight=19;
			return 1;
		}

		//LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);


		LRESULT OnNotifyCodeHandlerEX(LPNMHDR pnmh)
		{
			POINT pt;
			GetCursorPos(&pt);

			if(HasSeleted())
			{
				::EnableMenuItem(menu,ID_DELFROMPLAYQUEUE,MF_BYCOMMAND|IsAllSelectedItemInPlayQueue()?MF_ENABLED:(MF_DISABLED | MF_GRAYED));

				::TrackPopupMenu(menu,TPM_LEFTALIGN,pt.x,pt.y,0,m_hWnd,0);
			}

			return 1;
		}

		// 		void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
		// 		{
		// 			SetMsgHandled(FALSE);
		// 			lpMeasureItemStruct->itemHeight=17;
		// 		}




		int OnCreate(LPCREATESTRUCT lpCreateStruct)
		{
			DWORD dwStyle ;
			dwStyle= ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
			bClientEdge=(dwStyle & WS_EX_CLIENTEDGE)?true:false;

			CMessageLoop* pLoop = _Module.GetMessageLoop();
			ATLASSERT(pLoop != NULL);
			pLoop->AddMessageFilter(this);

			SetMsgHandled(FALSE);
			return 0;
		}


		LRESULT OnGetdispInfo(int /**/,NMHDR *pNMHDR,BOOL bHandled);

		UINT m_Order;
		static bool CALLBACK CompareProc(PlayListItem *item1, PlayListItem *item2)
		{
			bool result;

			const TCHAR  * str1;
			const TCHAR  * str2;

			FileTrack *track1=item1->GetFileTrack();
			FileTrack *track2=item2->GetFileTrack();

			switch(columnIndexClicked)
			{
			case COLUMN_INDEX_TITLE:
				str1=track1->title.c_str();
				str2=track2->title.c_str();
				break;
			case COLUMN_INDEX_ARTIST:
				str1=track1->artist.c_str();
				str2=track2->artist.c_str();
				break;
			case COLUMN_INDEX_ALBUM:
				str1=track1->album.c_str();
				str2=track2->album.c_str();
				break;
			case COLUMN_INDEX_YEAR:
				{
					UINT uYear1,uYear2;
					str1=track1->year.c_str();
					str2=track2->year.c_str();
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
				str1=track1->genre.c_str();
				str2=track2->genre.c_str();
				break;
			}
		
			
			result = _tcscmp(str1, str2) > 0;

			theEnd:
			return (pListCtrl->m_Order & (1<<columnIndexClicked))?result:!result;
		}

		void SortItems2();

		void SortItems( int nColumn, BOOL bAscending )
		{
			int columnOrder=nColumn;
			columnIndexClicked=Header_OrderToIndex(ListView_GetHeader(m_hWnd),columnOrder);

			if(columnIndexClicked!=COLUMN_INDEX_INDEX)
			{
				pListCtrl=this;
				SortItems2();
				Invalidate();		
			}

			
		
		}

		void ReverseItems() // overrides CListImpl::ReverseItems
		{
			//SortItems2();

			reverse(GetPlayList()->m_songList.begin(),GetPlayList()->m_songList.end());
		}

		LRESULT OnColumnClick(int /**/,NMHDR *pNMHDR,BOOL bHandled)
		{
			LPNMLISTVIEW pnmv = (LPNMLISTVIEW) pNMHDR;
			int columnOrder=pnmv->iSubItem;
			columnIndexClicked=Header_OrderToIndex(ListView_GetHeader(m_hWnd),columnOrder);
			
			if(columnIndexClicked!=COLUMN_INDEX_INDEX)
			{
				pListCtrl=this;
				//SortItems();
				Invalidate();		
			}

			return 0;
		}

		BOOL m_bC;
		

	

		//LRESULT OnItemChanging(int /**/,LPNMHDR pnmh,BOOL bHandled);
		LRESULT OnItemChanged(int /**/,LPNMHDR pnmh,BOOL bHandled);



		void SelectItems(vector<int> &items)
		{
			ClearAllSel();
			
			for (auto i=items.begin();i!=items.end();i++)
				SelectItem(*i);

			EnsureVisibleAndCentrePos(*(items.begin()));
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

		void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
		{
			if (nChar==VK_RETURN)
			{
				//PlayItem(GetFirstSelItem());
			}
		}
#ifdef Old_Version

		BOOL GetFirstSelItem()
		{
			int nItem=-1;
			if (GetItemCount()>0)
				nItem=GetNextItem(nItem,LVNI_SELECTED);

			return nItem;
		}
#endif
		void PlayItem(int nItem);

		void PlaySelectedItem(_songContainerItem *item);

		
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
		

		LRESULT OnDbClicked(int /**/,LPNMHDR pnmh,BOOL bHandled)
		{	
			if(!GetPlayList())return 0;
			int k=GetPlayList()->selectedIndex;
			if(k!=-1)
			{
				PlayItem(k);
				GetPlayList()->topVisibleIndex=k;
			}

			SetMsgHandled(FALSE);
			return 0;
		} 



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



		int GetIndex(PlayListItem *item)
		{
			// 			int nItem= GetPlayList()->GetItem(item);
			// 			return nItem==GetPlayList()->GetItemCount()?-1:nItem;

			return 0;
		}


		void SetPlayingItem(PlayListItem *item)
		{
			SetPlayingItem(GetIndex(item));
		}

public:
	void SetPlayingItem(int nItem)
	{
		int nItemPlaying=GetPlayingIdx();
		//RedrawItems(nItemPlaying,nItemPlaying);
		SetPlayingIdx(nItem);
		//RedrawItems(nItem,nItem);
	}

	void ClearAllSel()
	{
		int i=-1;
		//while ( (i=GetNextItem(i,LVIS_SELECTED)) != -1)
		//	SetItemState(i,0,LVNI_SELECTED|LVNI_FOCUSED );
	}

	//void InsertTrackItem(PlayListItem &track,int item,BOOL SetIndex=TRUE);
	//inline void InsertTrackItem(PlayListItem *track,int item,BOOL SetIndex=TRUE){InsertTrackItem(*track,item,SetIndex);}


	void LoadPlayList(PlayList *pPlayList)
	{
// 		if (GetPlayList())
// 			GetPlayList()->pPLV=NULL;
		//if(pPlayList)
			;//SetItemCount(pPlayList->m_songList.size());
		SetPlayList(pPlayList);
		//EnsureVisible(0,0);

		DeleteAllItems();
		if(pPlayList)
			for (int i=0;i<GetPlayList()->GetItemCount();++i)
			{
				//FileTrack *track=GetPlayList()->GetItem(i)->GetFileTrack();
				AddItem();
			}
		
	}

	HFONT GetItemFont( int nItem, int nSubItem )
	{
		return m_Font;
	}

	CString GetItemText( int nItem, int nSubItem ) // required by CListImpl
	{
		CString result;
		FileTrack *track=GetPlayList()->GetItem(nItem)->GetFileTrack();
		if(!track) return result;

		int iItemIndx=nItem;

		switch(nSubItem){
		case COLUMN_INDEX_INDEX:
			{
				TCHAR strIndex[4]={0};
				_itow(iItemIndx+1,strIndex,10);
				result=strIndex;
			}
			break;
		case COLUMN_INDEX_TITLE: //fill in main text
			result=track->title.c_str();
			break;
		case COLUMN_INDEX_ARTIST: //fill in sub item 1 text
				result=track->artist.c_str();
			break;
		case COLUMN_INDEX_ALBUM: //fill in sub item 2 text

			result=track->album.c_str();
			break;
		case COLUMN_INDEX_YEAR:

			result=track->year.c_str();
			break;
		case COLUMN_INDEX_GENRE:
		
			result=track->genre.c_str();
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
		//SetItemCount(0);
	}

	void SelectAndFocusItem(int item)
	{
		SelectItem(item,-1,LVIS_FOCUSED|LVIS_SELECTED);

		//SetItemState(item,LVIS_FOCUSED|
		//	LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);
	}




	void Reload(PlayList* pPlayList,bool ActivePlaying)
	{
		//resotre the curr croll bar's pos
		if(GetPlayList())
			//GetPlayList()->topVisibleIndex=GetTopIndex();
				GetPlayList()->topVisibleIndex=GetTopItem();
		
		

		m_bC=FALSE;

		//if some play list is deleted.
		if(!pPlayList)
		{
			LoadPlayList(pPlayList);
			return;
		}

		if ( /*m_bSearch ||*/1 || GetPlayList()!=pPlayList)
		{
			ClearAllSel();
			LoadPlayList(pPlayList);
		}

		if(ActivePlaying)
		{
			int index=GetPlayList()->GetPlayingIndex();
			//EnsureVisibleAndCentrePos(index);
			SelectAndFocusItem(index);
		//	SetItemState(index,LVIS_FOCUSED|
			//	LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);
		}
		else
		{
			//so we resotre the last scroll pos and,
			//highlight last selected item
			int selItem;

			if(GetPlayingIdx()==-1)
			{
				selItem=0;
			}
			else
			{
				selItem=GetPlayingIdx();
				SelectAndFocusItem(selItem);
			}


			ScrollByTop(GetPlayList()->topVisibleIndex);
		}


		m_bC=TRUE;
	}


	void ScrollByTop(int topIndex)
	{
		EnsureItemVisible(topIndex,-1,FALSE);

// 		EnsureVisible(topIndex,FALSE);
// 
// 		top=GetTopIndex();
// 
// 		RECT rc;
// 		GetItemRect(top,&rc,LVIR_BOUNDS);
// 
// 		SIZE sz;
// 		sz.cx=0;
// 		sz.cy=(topIndex-top)*(rc.bottom-rc.top);
// 		Scroll(sz);

	}

	void EnsureVisibleAndCentrePos(int index)
	{
		EnsureItemVisible(index,-1,TRUE);
// 		int top=GetTopIndex();
// 		int countPerPage=GetCountPerPage();
// 
// 		//isVisible?
// 		if (index >= top +1  && index  <= top+countPerPage -1 )
// 			return;
// 
// 		EnsureVisible(index,FALSE);
// 
// 		top=GetTopIndex();
// 
// 		RECT rc;
// 		GetItemRect(top,&rc,LVIR_BOUNDS);
// 
// 		SIZE sz;
// 		sz.cx=0;
// 		sz.cy=(index-top-countPerPage/2)*(rc.bottom-rc.top);
// 		Scroll(sz);
	}

	BOOL IsAllSelectedItemInPlayQueue();
	LRESULT OnPushToPlayqueue(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnDeleteFromPlayqueue(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOpenFilePath(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		int nItem=GetFirrstSelectedItem();

		if(nItem!=-1)
		{
			FileTrack *track=GetPlayList()->GetItem(nItem)->GetFileTrack();

			std::tstring parameters=_T("/select,");
			parameters+=track->url;

			std::tstring dir;
			int index=track->url.find_last_of(_T("\\"));
			if (index!=track->url.npos)
				dir+=track->url.substr(0,index);

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
// 		int top=GetTopIndex();
// 		int countPerPage=GetCountPerPage();
// 		RedrawItems(top,top+countPerPage);
		::UpdateWindow(this->m_hWnd);
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
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);

		
		if(GetPlayList() && GetPlayList()->m_bSearch)
			;
		else
			delete this;
	}


};



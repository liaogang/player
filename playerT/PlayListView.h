#pragma once
#include "stdafx.h"
#include "PlayList.h"
#include "PlayListViewMng.h"
#include "globalStuffs.h"
#define INVALID_ITEM -1

unsigned int BKDRHash(char *str);

HMENU LoadPlaylistMenu(BOOL bDestroy=FALSE);



/*
class PlayListViewBase
{
private:
	PlayList *  m_pPlayList;
protected:
	int nItemPlaying;//正在播放的项目
public:
	inline int GetPlayingIdx(){return nItemPlaying;}
	inline void SetPlayingIdx(int i){nItemPlaying=i;}
public:
	//清空当前播放列表视图的所有项目,
	virtual void ClearAllItem()=0;

	virtual void ClearAllSel()=0;
	virtual void SelectAndFocusItem(int item)=0;

	virtual void LoadPlayList(PlayList *pPlayList)=0;
	virtual void EnsureVisibleAndCentrePos(int index)=0;
	virtual void Reload(PlayList* pPlayList,BOOL SetIndex=-1)=0;

	virtual int GetIndex(PlayListItem *item)=0;

	void ReLoad(PlayList *pPL)
	{
		ClearAllItem();
		LoadPlayList(pPL);
	}

	inline void SetPlayList(PlayList * pPlayList){
		m_pPlayList=pPlayList;
		if (m_pPlayList)
			m_pPlayList->pPLV=this;
	}
	inline PlayList * GetPlayList(){return m_pPlayList;}
};*/



#define TEXTALIGN__WIDTH 3

class CPlayListView:
	public CWindowImpl<CPlayListView,CListViewCtrl>,
	public CMessageFilter
	//,public PlayListViewBase
{
private:
	PlayList *  m_pPlayList;
protected:
	int nItemPlaying;//正在播放的项目
public:
	inline int GetPlayingIdx(){return nItemPlaying;}
	inline void SetPlayingIdx(int i){nItemPlaying=i;}
	inline void SetPlayList(PlayList * pPlayList){
		m_pPlayList=pPlayList;
		if (m_pPlayList)
			m_pPlayList->pPLV=this;
	}
	inline PlayList * GetPlayList(){return m_pPlayList;}


public:
	class CMainFrame *pMain;
	void SetMain(class CMainFrame *pMain);
	PlayListItem *m_pPlayTrack;

	BOOL bAuto,bDeletable;
	HMENU menu;
	BOOL m_bSearch;                  //是否为搜索列表

	COLORREF clText1,clText2;

	int indexLast;



	CPlayListView(BOOL bSearch=FALSE):
	m_bSearch(bSearch),
		bAuto(FALSE),
		bDeletable(!bAuto),
		m_pPlayTrack(NULL),m_bC(TRUE),indexLast(-1)
	{
		SetPlayList(NULL);
		//nItemPlaying=-1;
		menu=LoadPlaylistMenu();
	}

	~CPlayListView()
	{
		AllPlayListViews()->RemoveItem(this);

		LoadPlaylistMenu(TRUE);

	}

public:
	DECLARE_WND_SUPERCLASS(NULL,CListViewCtrl::GetWndClassName())


	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (pMsg->message==WM_KEYDOWN)
		{	
			UINT nChar=(TCHAR)pMsg->wParam;

			if (nChar=='A' || nChar=='a')
			{
				if (GetKeyState(VK_CONTROL) &0x80)
					SelectAll();
			}
			else if (nChar==VK_DELETE)
			{
				DelSelectedItem(GetKeyState(VK_SHIFT) & 0x80);
			}

		}//if (pMsg->message!=WM_KEYDOWN)

		return FALSE;
	}

	BEGIN_MSG_MAP_EX(CPlayListView)
		MSG_WM_CREATE(OnCreate);
		MSG_WM_LBUTTONDBLCLK(OnDbClicked)
		MSG_WM_CHAR(OnChar)
		COMMAND_ID_HANDLER(ID_OPEN_FILE_PATH,OnOpenFilePath)
		COMMAND_ID_HANDLER(ID_PUSH_PLAYQUEUE,OnPushToPlayqueue)
		COMMAND_ID_HANDLER(ID_DELFROMPLAYQUEUE,OnDeleteFromPlayqueue)
		REFLECTED_NOTIFY_CODE_HANDLER_EX(NM_RCLICK ,OnNotifyCodeHandlerEX)
		REFLECTED_NOTIFY_CODE_HANDLER(LVN_ITEMCHANGED,OnItemChanged)
		REFLECTED_NOTIFY_CODE_HANDLER(LVN_GETDISPINFO,OnGetdispInfo)
		MESSAGE_HANDLER(OCM_DRAWITEM,OnDrawItem)
		MESSAGE_HANDLER(OCM_MEASUREITEM,OnMeasureItem)
	END_MSG_MAP()

		LRESULT OnMeasureItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
		{
			LPMEASUREITEMSTRUCT lpMeasureItemStruct=(LPMEASUREITEMSTRUCT)lParam;
			lpMeasureItemStruct->itemHeight=19;
			return 1;
		}

		LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);


		LRESULT OnNotifyCodeHandlerEX(LPNMHDR pnmh)
		{
			POINT pt;
			GetCursorPos(&pt);

			if(GetNextItem(-1,LVIS_FOCUSED|LVIS_SELECTED)!=-1)
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
			CMessageLoop* pLoop = _Module.GetMessageLoop();
			ATLASSERT(pLoop != NULL);
			pLoop->AddMessageFilter(this);
			SetMsgHandled(FALSE);
			return 0;
		}


		LRESULT OnGetdispInfo(int /**/,NMHDR *pNMHDR,BOOL bHandled);

		BOOL m_bC;
		
		LRESULT OnItemChanged(int /**/,LPNMHDR pnmh,BOOL bHandled);
		

		inline void SelectAll(){SetItemState(-1, LVIS_SELECTED , LVIS_SELECTED );}

		void DelSelectedItem(BOOL bDelFile=FALSE)
		{
			// 			if (bDeletable)
			// 			{
			// 				for(int i = GetItemCount()-1; i>=0;--i)
			// 					if( LVIS_SELECTED==GetItemState(i, LVNI_ALL | LVNI_SELECTED) )
			// 						DeleteItem(i);
			// 			}
			//CListCtrl ctrl;ctrl.GetNextSelectedItem()GetNextItem((UINT)nOldPos, LVIS_SELECTED)
			if (!bDeletable)
				return;

			//从vector删除某项,需要重新搬家,很费时.
			int blockBeg=0,blockLast;

			
			int nCount=GetItemCount();
			int countToDel=0;

			bool *itemDel=new bool[nCount];
			memset(itemDel,0,nCount*sizeof(bool));

			int nItem=-1;
			while(-1!=(nItem=GetNextItem(nItem,LVIS_SELECTED)) )
			{
				if(!blockBeg)blockBeg=nItem;
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

		void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
		{
			if (nChar==VK_RETURN)
			{
				PlayItem(GetFirstSelItem());
			}
		}

		BOOL GetFirstSelItem()
		{
			int nItem=-1;
			if (GetItemCount()>0)
				nItem=GetNextItem(nItem,LVNI_SELECTED);

			return nItem;
		}

		void PlayItem(int nItem);

		void PlaySelectedItem(_songContainerItem *item);

		LRESULT OnDbClicked(UINT i,CPoint pt)
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

		void Init()
		{	
			pMain=GetMainFrame();

			if(!m_bSearch)
				AllPlayListViews()->AddItem(this);
			//CHeaderCtrl header=GetHeaderCtrl();


			ChangeColorDefault();
			SetExtendedListViewStyle(GetExtendedListViewStyle()|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP);

			const TCHAR * columnName[]={
				_T("索引"),
				_T("标题"),
				_T("艺术家"),
				_T("专辑"),
				_T("年份"),
				_T("流派")};

				const UINT alignment[]={
					LVCFMT_CENTER,
					LVCFMT_LEFT,
					LVCFMT_LEFT,
					LVCFMT_LEFT,
					LVCFMT_RIGHT,
					LVCFMT_RIGHT};

					const TCHAR* columnNamePlaceHoder[]={
						_T("Index"),
						_T("Title                        "),
						_T("艺术家艺"),
						_T("Album         "),
						_T("Year"),
						_T("Genre               ")
					};

						const int cxOffset = 15;

						for (int i=0;i<sizeof(columnName)/sizeof(int);++i)
							AddColumn(columnName[i],i,-1, LVCF_FMT| LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM ,alignment[i],GetStringWidth(columnNamePlaceHoder[i])+cxOffset);
		}

		int AddColumn(LPCTSTR strItem, int nItem, int nSubItem = -1,
			int nMask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
			int nFmt = LVCFMT_LEFT,int stringWidth=20)
		{
			ATLASSERT(::IsWindow(m_hWnd));
			LVCOLUMN lvc = { 0 };
			lvc.mask = nMask;
			lvc.fmt = nFmt;
			lvc.pszText = (LPTSTR)strItem;
			lvc.cx = stringWidth ;
			if(nMask & LVCF_SUBITEM)
				lvc.iSubItem = (nSubItem != -1) ? nSubItem : nItem;
			return InsertColumn(nItem, &lvc);
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
			RedrawItems(nItemPlaying,nItemPlaying);
			nItemPlaying=nItem;
			RedrawItems(nItemPlaying,nItemPlaying);
		}

		void ClearAllSel()
		{
			int i=-1;
			while ( (i=GetNextItem(i,LVIS_SELECTED)) != -1)
				SetItemState(i,0,LVNI_SELECTED|LVNI_FOCUSED );
		}

		void InsertTrackItem(PlayListItem &track,int item,BOOL SetIndex=TRUE);
		inline void InsertTrackItem(PlayListItem *track,int item,BOOL SetIndex=TRUE){InsertTrackItem(*track,item,SetIndex);}


		void LoadPlayList(PlayList *pPlayList)
		{
			if (GetPlayList())
				GetPlayList()->pPLV=NULL;
			
			SetItemCount(pPlayList->m_songList.size());
			SetPlayList(pPlayList);
		}

		void ClearAllItem()
		{
			SetItemCount(0);
		}

		void SelectAndFocusItem(int item)
		{
			SetItemState(item,LVIS_FOCUSED|
				LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);
		}




		void Reload(PlayList* pPlayList,int SetIndex=INVALID_ITEM)
		{
			m_bC=FALSE;

			if ( /*m_bSearch ||*/1 || GetPlayList()!=pPlayList)
			{
				ClearAllSel();
				LoadPlayList(pPlayList);
			}

			if(SetIndex==INVALID_ITEM)//so we highlight last selected item
			{
				int selItem;
				
				if(GetPlayingIdx()==-1)
				{
					selItem=0;
				}
				else
				{
					selItem=GetPlayingIdx();;
					SelectAndFocusItem(selItem);
				}

				EnsureVisibleAndCentrePos(selItem);
			}
			else
			{
				EnsureVisibleAndCentrePos(SetIndex);
				SetItemState(SetIndex,LVIS_FOCUSED|
					LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);
			}//if(!SetIndex)


			m_bC=TRUE;
		}


		void EnsureVisibleAndCentrePos(int index)
		{
			int top=GetTopIndex();
			int countPerPage=GetCountPerPage();

			// 			if ( (index> top && index-top < countPerPage) || 
			// 				 ( index<top && top-index > 0-countPerPage))
			// 			{
			// 				return;
			// 			}

			//isVisible?
			if (index >= top && index  <= top+countPerPage)
			{
				return;
			}

			EnsureVisible(index,FALSE);

			top=GetTopIndex();


			RECT rc;
			GetItemRect(top,&rc,LVIR_BOUNDS);

			SIZE sz;
			sz.cx=0;
			sz.cy=(index-top-countPerPage/2)*(rc.bottom-rc.top);
			Scroll(sz);
		}

		BOOL IsAllSelectedItemInPlayQueue();
		LRESULT OnPushToPlayqueue(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		LRESULT OnDeleteFromPlayqueue(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		LRESULT OnOpenFilePath(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{
			int nItem=GetNextItem(-1,LVIS_SELECTED);

			if(nItem!=-1)
			{
				FileTrack *track=GetPlayList()->m_songList[nItem].GetFileTrack();

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
			int top=GetTopIndex();
			int countPerPage=GetCountPerPage();
			RedrawItems(top,top+countPerPage);
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
		}

		void ChangeColorBlue()
		{
			ChangeColor(RGB(230,244,255),RGB(145,200,255));
		}

		virtual void OnFinalMessage(_In_ HWND /*hWnd*/)
		{
			CMessageLoop* pLoop = _Module.GetMessageLoop();
			ATLASSERT(pLoop != NULL);
			pLoop->RemoveMessageFilter(this);
			
			delete this;
		}
};

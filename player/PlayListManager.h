#pragma once
#include "stdafx.h"
#include "resource.h"
#include "MyLib.h"
#include "PlayList.h"
//IDR_MENU_PL_MNG
HMENU LoadPlaylistManagerMenu(BOOL bDestroy=FALSE);




class CMainFrame;
class CPlayListManager:
	public CWindowImpl<CPlayListManager,CListViewCtrl>
{
public:
	BEGIN_MSG_MAP_EX(CPlayListManager)
		MESSAGE_HANDLER(WM_PL_WILL_DELETED,OnPLWillDeleted)
		MESSAGE_HANDLER(WM_PL_TRACKNUM_CHANGED,OnPLTrackNumChanged)
		MESSAGE_HANDLER(WM_SOME_PL_CHANGED,OnReloadPL)
		MESSAGE_HANDLER(WM_SELECTED_PL_CHANGED,OnReloadPL)
		
		MSG_WM_LBUTTONDBLCLK(OnDbClicked)
		COMMAND_ID_HANDLER(ID_PLAYLISTMNG_DEL,OnDeletePlaylist)
		COMMAND_ID_HANDLER(ID_PLAYLISTMNG_ACTIVE,OnActivePlaylist)
		COMMAND_ID_HANDLER(ID_PLAYLISTMNG_NEW,OnNewPlaylist)
		COMMAND_ID_HANDLER(ID_PLAYLISTMNG_RENAME,OnRenamePlaylist)
		REFLECTED_NOTIFY_CODE_HANDLER_EX(NM_RCLICK ,OnNotifyCodeHandlerEX)
		REFLECTED_NOTIFY_CODE_HANDLER_EX(LVN_ENDLABELEDIT,OnEndLabelEdit)
		//REFLECTED_NOTIFY_CODE_HANDLER(NM_CUSTOMDRAW,OnCustomDraw)
	END_MSG_MAP()

    
	void ReFillPlaylist()
	{
		DeleteAllItems();

		MyLib *s=MyLib::shared();
		if (!s->Empty())
		{
			for (int i=0;i<s->GetItemCount();++i)
			{
				LPCPlayList l=s->GetItem(i);
				AddPlayList(l);
			}
		}
	}
	
	void AddPlayList(LPCPlayList pPL)
	{
		int item=GetItemCount();
		TCHAR strCount[256];
		_itow(pPL->GetItemCount(),strCount,10);

		
		InsertItem(item,pPL==MyLib::shared()->GetSelectedPL()?_T("*"):_T(" "));
		SetItemText(item,1,pPL->GetPlaylistName().c_str());
		SetItemText(item,2,strCount);
		SetItemText(item,3,pPL->IsAuto()?_T("自动列表"):_T("普通列表"));
		SetItemData(item,(DWORD_PTR)pPL);
	}
	
	void DelPlayList(LPCPlayList pPL)
	{
		int c=GetItemCount();
		for (int i=0;i<c;++i)
		{
			auto pl= (LPCPlayList)GetItemData(i);
			if (pl==pPL){
				DeleteItem(i);
				break;
			}
		}
	}

	void UpdateByPLTrack(LPCPlayList pPL)
	{
		int c=GetItemCount();
		for (int i=0;i<c;++i)
		{
			auto pl= (LPCPlayList)GetItemData(i);
			if (pl==pPL){
				SetItemText(i,1,pPL->GetPlaylistName().c_str());

				TCHAR strCount[256];
				_itow(pPL->GetItemCount(),strCount,10);
				SetItemText(i,2,strCount);
				break;
			}
		}
	}

// 	LRESULT OnCustomDraw(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
// 	{
// 		LPNMCUSTOMDRAW lpNMCustomDraw = (LPNMCUSTOMDRAW)pnmh;
// 		NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( lpNMCustomDraw );  
// 		int nItem=pLVCD->nmcd.dwItemSpec;
// 		
// 		DWORD dwRet;
// 		switch(lpNMCustomDraw->dwDrawStage)
// 		{
// 		case CDDS_PREPAINT:
// 			dwRet= CDRF_NOTIFYITEMDRAW ;
// 			break;
// 		case CDDS_ITEMPREPAINT:
// 			{
// 				int i=MyLib::shared()->GetSelectedIndex();
// 				if(nItem==i)
// 				pLVCD->clrTextBk=RGB(230,244,255);
// 			}
// 			dwRet= CDRF_NEWFONT ;
// 			break;
// 		}
// 
// 		return dwRet;
// 	}

	void ClearAllSel()
	{
		int i=-1;
		while ( (i=GetNextItem(i,LVIS_SELECTED)) != -1)
			SetItemState(i,0,LVNI_SELECTED|LVNI_FOCUSED );
	}

	void Init()
	{
		IWantToReceiveMessage(WM_SOME_PL_CHANGED);
		IWantToReceiveMessage(WM_SELECTED_PL_CHANGED);
		IWantToReceiveMessage(WM_PL_WILL_DELETED);
		IWantToReceiveMessage(WM_PL_TRACKNUM_CHANGED);

		SetExtendedListViewStyle(GetExtendedListViewStyle()|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP);

		const TCHAR * columnName[]={
			_T(""),
			_T("播放列表名称"),
			_T("项目"),
			_T("属性")};

		const UINT alignment[]={
			LVCFMT_LEFT,
			LVCFMT_LEFT,
			LVCFMT_RIGHT};

		const TCHAR* columnNamePlaceHoder[]={
			_T("    "),
			_T("                                   "),
			_T("        Artist"),
			_T("        属性")};

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

	BOOL GetFirstSelItem()
	{
		int nItem=-1;
		if (GetItemCount()>0)
			nItem=GetNextItem(nItem,LVNI_SELECTED);

		return nItem;
	}

	LRESULT OnDbClicked(UINT i,CPoint pt);
	
	LRESULT OnPLWillDeleted(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		LPCPlayList thePl=(LPCPlayList)wParam;
		
		DelPlayList(thePl);

		return 0;
	}

	LRESULT OnPLTrackNumChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LPCPlayList pl=(LPCPlayList)wParam;

		UpdateByPLTrack(pl);

		return 0;
	}

	LRESULT OnReloadPL(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ReFillPlaylist();
		return 0;
	}
	
	
	LRESULT OnEndLabelEdit(LPNMHDR pnmh)
	{
		NMLVDISPINFO *p=(NMLVDISPINFO *)pnmh;
		int item=p->item.iItem;
		int iSubItem=p->item.iSubItem;
		TCHAR *pszText=p->item.pszText;

		if(pszText)
		{
			LPCPlayList pPl=(LPCPlayList)GetItemData(GetFirstSelItem());
			pPl->Rename(pszText);
			UpdateByPLTrack(pPl);
		}

		return TRUE;
	}

	LRESULT OnNotifyCodeHandlerEX(LPNMHDR pnmh)
	{
		static HMENU menu=NULL;
		if(menu==NULL)
			menu=LoadPlaylistManagerMenu();


		int count=GetSelectedCount();
		if(count>0)
		{
			POINT pt;
			GetCursorPos(&pt);

			::EnableMenuItem(menu,ID_PLAYLISTMNG_ACTIVE,MF_BYCOMMAND|(count==1?MF_ENABLED:(MF_DISABLED | MF_GRAYED)));
			::EnableMenuItem(menu,ID_PLAYLISTMNG_RENAME,MF_BYCOMMAND|(count==1?MF_ENABLED:(MF_DISABLED | MF_GRAYED)));
			::TrackPopupMenu(menu,TPM_LEFTALIGN,pt.x,pt.y,0,m_hWnd,0);
		}

		return 1;
	}


	LRESULT OnDeletePlaylist(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		vector<int> items;

		int nItem=-1;
		while((nItem=GetNextItem(nItem,LVNI_SELECTED))!=-1)
			items.push_back(nItem);
		
		//倒序
		reverse(items.begin(),items.end());

		for (auto i=items.begin();i!=items.end();i++)
		{
			DeleteItem(*i);
			MyLib::shared()->DeletePlayList(*i,TRUE);
		}

		Invalidate();
		return 0;
	}

	LRESULT OnActivePlaylist(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		LPCPlayList l=(LPCPlayList)GetItemData(GetFirstSelItem());

		MyLib::shared()->SetSelectedPL(l);

		return 0;
	}
	
	LRESULT OnNewPlaylist(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		LPCPlayList pPl=MyLib::shared()->NewPlaylist(TRUE);
		Invalidate();
		return 0;
	}

	LRESULT OnRenamePlaylist(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{

		return 0;
	}
};


class DialogPLManager :
	public CMessageFilter,
	public CDialogImpl<DialogPLManager>,	
	public CDialogResize<DialogPLManager>
{
public:
	enum { IDD = IDD_DLG_PL_MNG };

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		//让非模态对话框处理模态对话框的消息
		//如Tab,Esc,Enter...
		return IsDialogMessage(pMsg);
	}

	BEGIN_MSG_MAP(DialogPLManager)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		CHAIN_MSG_MAP(CDialogResize<DialogPLManager>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(DialogPLManager)
		DLGRESIZE_CONTROL(IDC_LIST,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_LIST,DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	CPlayListManager m_list;
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DlgResize_Init(FALSE,FALSE);
		CenterWindow(GetParent());
		m_list.SubclassWindow(::GetDlgItem(m_hWnd,IDC_LIST));
		m_list.Init();
		m_list.ReFillPlaylist();

		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);

		return TRUE;
	}


	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		HideSelf();
		return 0;
	}
	
	

	void HideSelf()
	{
		ShowWindow(SW_HIDE);
	}

	void ShowSelf()
	{	
		ShowWindow(SW_SHOW);
	}


	RECT m_rc;
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled=FALSE;
		GetWindowRect(&m_rc);
		return 0;
	}
};
#pragma once
#include "stdafx.h"
#include "resource.h"
#include "MyLib.h"
//IDR_MENU_PL_MNG
HMENU LoadPlaylistManagerMenu(BOOL bDestroy=FALSE);




class CMainFrame;
class CPlayListManager:
	public CWindowImpl<CPlayListManager,CListViewCtrl>
{
public:
	BEGIN_MSG_MAP_EX(CPlayListManager)
		MSG_WM_LBUTTONDBLCLK(OnDbClicked)
		COMMAND_ID_HANDLER(ID_PLAYLISTMNG_DEL,OnDeletePlaylist)
		COMMAND_ID_HANDLER(ID_PLAYLISTMNG_ACTIVE,OnActivePlaylist)
		COMMAND_ID_HANDLER(ID_PLAYLISTMNG_NEW,OnNewPlaylist)
		COMMAND_ID_HANDLER(ID_PLAYLISTMNG_RENAME,OnRenamePlaylist)
		REFLECTED_NOTIFY_CODE_HANDLER_EX(NM_RCLICK ,OnNotifyCodeHandlerEX)
		REFLECTED_NOTIFY_CODE_HANDLER_EX(LVN_ENDLABELEDIT,OnEndLabelEdit)
		REFLECTED_NOTIFY_CODE_HANDLER(NM_CUSTOMDRAW,OnCustomDraw)
	END_MSG_MAP()

	CMainFrame *pMain;
    void ReFillPlaylist();
	void AddPlayList(PlayList *pPL);
	void DelPlayList(PlayList *pPL);
	void UpdateByPLTrack(PlayList *pPL);

	LRESULT OnCustomDraw(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
		LPNMCUSTOMDRAW lpNMCustomDraw = (LPNMCUSTOMDRAW)pnmh;
		NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( lpNMCustomDraw );  
		int nItem=pLVCD->nmcd.dwItemSpec;
		
		DWORD dwRet;
		switch(lpNMCustomDraw->dwDrawStage)
		{
		case CDDS_PREPAINT:
			dwRet= CDRF_NOTIFYITEMDRAW ;
			break;
		case CDDS_ITEMPREPAINT:
			{
				int i=MyLib::shared()->GetPlayingIndex();
				if(nItem==i)
				pLVCD->clrTextBk=RGB(230,244,255);
			}
			dwRet= CDRF_NEWFONT ;
			break;
		}

		return dwRet;
	}

	void ClearAllSel()
	{
		int i=-1;
		while ( (i=GetNextItem(i,LVIS_SELECTED)) != -1)
			SetItemState(i,0,LVNI_SELECTED|LVNI_FOCUSED );
	}

	void Init()
	{	
		SetExtendedListViewStyle(GetExtendedListViewStyle()|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP);

		const TCHAR * columnName[]={
			_T("播放列表名称"),
			_T("项目"),
			_T("属性")};

		const UINT alignment[]={
			LVCFMT_LEFT,
			LVCFMT_LEFT,
			LVCFMT_RIGHT};

		const TCHAR* columnNamePlaceHoder[]={
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
	


	
	LRESULT OnEndLabelEdit(LPNMHDR pnmh)
	{
		NMLVDISPINFO *p=(NMLVDISPINFO *)pnmh;
		int item=p->item.iItem;
		int iSubItem=p->item.iSubItem;
		TCHAR *pszText=p->item.pszText;

		if(pszText)
		{
			PlayList * pPl=(PlayList *)GetItemData(GetFirstSelItem());
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

			::EnableMenuItem(menu,ID_PLAYLISTMNG_ACTIVE,MF_BYCOMMAND|count==1?MF_ENABLED:(MF_DISABLED | MF_GRAYED));
			::EnableMenuItem(menu,ID_PLAYLISTMNG_RENAME,MF_BYCOMMAND|count==1?MF_ENABLED:(MF_DISABLED | MF_GRAYED));
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
			MyLib::shared()->DeletePlayList(*i);
		}

		
		return 0;
	}

	LRESULT OnActivePlaylist(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PlayList *l=(PlayList*)GetItemData(GetFirstSelItem());

		SetPlayingPlaylist(l);

		AllPlayListViews()->Reload(l);

		return 0;
	}
	
	LRESULT OnNewPlaylist(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PlayList *pPl=MyLib::shared()->NewPlaylist();
		AllPlayListViews()->Reload(pPl);
		SetSelectedPlaylist(pPl);
		return 0;
	}

	LRESULT OnRenamePlaylist(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{

		return 0;
	}
};


class DialogPLManager :
	public CDialogImpl<DialogPLManager>,
	public CDialogResize<DialogPLManager>
{
public:
	enum { IDD = IDD_DLG_PL_MNG };
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
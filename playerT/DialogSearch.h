#pragma once
#include <set>
#include <atlsimpcoll.h>
#include "PlayListView.h"
#include "globalStuffs.h"
using namespace std;

class DialogSearch :
	public CDialogImpl<DialogSearch>,
	public CDialogResize<DialogSearch>,
	public CMessageFilter
{
public:
	enum { IDD = IDD_DIALOG_SEARCH };
	
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return IsDialogMessage(pMsg);
	}

	BEGIN_MSG_MAP(DialogSearch)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_CODE_HANDLER(EN_CHANGE, OnSearch )
		CHAIN_MSG_MAP(CDialogResize<DialogSearch>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(DialogSearch)
		DLGRESIZE_CONTROL(IDC_EDIT,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_LIST,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_LIST,DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	CPlayListViewS m_list;
	PlayList *searchPl;
	RECT m_rc;

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DlgResize_Init(FALSE,FALSE);
		CenterWindow(GetParent());

		m_list.SubclassWindow(::GetDlgItem(m_hWnd,IDC_LIST));
		
		searchPl=new PlayList;
		searchPl->m_bSearch=TRUE;
		m_list.SetPlayList(searchPl);

		m_list.Init(true);
		
		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		return TRUE;
	}


	
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled=FALSE;
		GetWindowRect(&m_rc);
		return 0;
	}

	LRESULT OnSearch(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PlayList *pSelectedPl=SelectedPlaylist();
		if(pSelectedPl==NULL) return 0;

		m_list.ClearMap();
		m_list.ClearAllItem();
		m_list.SetPlayingIdx(0);
		m_list.playlistParent=pSelectedPl;
		searchPl->DeleteAllItems();

		TCHAR buf[MAX_PATH];
		::GetWindowText(::GetDlgItem(m_hWnd,IDC_EDIT),buf,MAX_PATH);
		//todo
		//空格分开的字符,当成多个字符
		if (buf[0]=='\0')
		{
			m_list.ClearAllItem();
			return 0;
		}

		std::tstring strBuf(buf);
		int a,b;
		a=strBuf.find_first_not_of(' ');
		if (a==std::tstring::npos){
			m_list.ClearAllItem();
			return 0;
		}
		b=strBuf.find_last_not_of(' ');
		
		if (b<a) {m_list.ClearAllItem();return 0;}
		strBuf=strBuf.substr(a,b+1-a);

		
		int count=pSelectedPl->GetItemCount();
		for (int i=0;i<count;++i)
		{
			_songContainerItem item=pSelectedPl->GetItem(i);
			FileTrack *track=item->GetFileTrack();
			if (track->HaveKeywords(const_cast<TCHAR*>( strBuf.c_str()) ))
			{
				auto cp=item->Duplicate();
				searchPl->AddItem(cp);
				m_list.Add2Map(cp,item);
			}
		}

		if (searchPl->GetItemCount()>0)
			m_list.Reload(searchPl,-1);
		else
			m_list.ClearAllItem();
		
		return 0;
	}


	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (wID==IDOK)
		{
			::SendMessage(m_list.m_hWnd,WM_CHAR,(WPARAM)VK_RETURN,NULL);
		}
		else
			HideSelf();

		return 0;
	}

	void HideSelf()
	{
		::SetWindowText(GetDlgItem(IDC_EDIT),0);
		ShowWindow(SW_HIDE);
	}

	void ShowSelf()
	{	
		ShowWindow(SW_SHOW);
		::SetFocus(GetDlgItem(IDC_EDIT));
	}
};

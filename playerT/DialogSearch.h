#pragma once

#include "PlayListView.h"

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
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_CODE_HANDLER(EN_CHANGE, OnSearch )
		MESSAGE_HANDLER(WM_MEASUREITEM,OnMeasureItem)
		CHAIN_MSG_MAP(CDialogResize<DialogSearch>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(DialogSearch)
		DLGRESIZE_CONTROL(IDC_EDIT,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_LIST,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_LIST,DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	CMainFrame *pM;
	CPlayListView m_list;
	
	

	LRESULT OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(wParam==IDC_LIST)
		{
			m_list.OnMeasureItem(OCM__BASE+uMsg,wParam,lParam,bHandled);
		}
		
		return 1;
	}

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DlgResize_Init(FALSE,FALSE);
		CenterWindow(GetParent());
		m_list.SubclassWindow(::GetDlgItem(m_hWnd,IDC_LIST));
		m_list.m_bSearch=TRUE;
		m_list.Init();
		m_list.SetMain(pM);

		//m_edit.SubclassWindow(::GetDlgItem(m_hWnd,IDC_EDIT));
		//m_edit.m_pDlgSearch=this;

		searchPl=NULL;

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);


		return TRUE;
	}


	PlayList *searchPl;

	LRESULT OnSearch(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (!searchPl)
			searchPl=new PlayList;
		else
			searchPl->m_songList.clear();


		TCHAR buf[MAX_PATH];
		::GetWindowText(::GetDlgItem(m_hWnd,IDC_EDIT),buf,MAX_PATH);
		//todo
		//空格分开的字符,当成多个字符
		if (buf[0]=='\0')
		{
			m_list.SetItemCount(0);
			return 0;
		}

		std::tstring strBuf(buf);
		int a,b;
		a=strBuf.find_first_not_of(' ');
		if (a==std::tstring::npos){
			m_list.SetItemCount(0);
			return 0;
		}
		b=strBuf.find_last_not_of(' ');
		
		if (b<a) {m_list.SetItemCount(0);return 0;}
		strBuf=strBuf.substr(a,b+1-a);


		int count=pM->m_pPlaylistView->GetItemCount();
		for (int i=0;i<count;++i)
		{
			_songContainerItem item=pM->m_pPlaylistView->GetPlayList()->m_songList[i];
			FileTrack *track=item.GetFileTrack();
			if (track->HaveKeywords(const_cast<TCHAR*>( strBuf.c_str()) ))
				searchPl->m_songList.push_back(item);
		}

		if (searchPl->m_songList.size()>0)
		{
			m_list.Reload(searchPl,-1);
			m_list.SetItemState(0,LVIS_FOCUSED|
				LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);	
		}
		else
		{
			m_list.SetItemCount(0);
		}
		
		
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


//ctrl+c,v,x被上层截获.
//收不到

// 
// void CMyEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
// {
// 	//AtlTrace(L"%u",nChar);
// 
// 	if(nChar==VK_ESCAPE)
// 	{
// 		m_pDlgSearch->HideSelf();
// 	}
// 	else if (nChar==VK_RETURN )//播放列表选中项
// 	{
// 		::SetFocus(m_pDlgSearch->m_list.m_hWnd);
// 		::SendMessage(m_pDlgSearch->m_list.m_hWnd,WM_CHAR,(WPARAM)VK_RETURN,NULL);	
// 	}
// 	else
// 	{
// 		SetMsgHandled(FALSE);
// 	}
// }
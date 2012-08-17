#pragma once
using namespace std;

class DialogSearch :
	public CDialogImpl<DialogSearch>,
	public CDialogResize<DialogSearch>
{
public:
	CMainFrame *pM;
	enum { IDD = IDD_DIALOG_SEARCH };

	BEGIN_MSG_MAP(DialogFFT)
		MSG_WM_KEYDOWN(OnKeyDown)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_CODE_HANDLER(EN_CHANGE, OnSearch )
		CHAIN_MSG_MAP(CDialogResize<DialogSearch>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(DialogSearch)
		DLGRESIZE_CONTROL(IDC_EDIT,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_BUTTON,DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_LIST,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_LIST,DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	CPlayListView m_list;
	CMyEdit       m_edit;
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DlgResize_Init(FALSE,FALSE);
		CenterWindow(GetParent());
		m_list.SubclassWindow(::GetDlgItem(m_hWnd,IDC_LIST));
		m_list.OnCreate(0);
		m_list.SetMain(pM);

		m_edit.SubclassWindow(::GetDlgItem(m_hWnd,IDC_EDIT));
		m_edit.m_pDlgSearch=this;

		//UINT style= m_edit.GetExStyle();
		//m_edit.setwindowsl

		return TRUE;
	}


	vector<PlayListItem*> m_Searchlist;
	LRESULT OnSearch(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_Searchlist.clear();

		//m_SearchPl.m_songList.clear();

		TCHAR buf[MAX_PATH];
		::GetWindowText(::GetDlgItem(m_hWnd,IDC_EDIT),buf,MAX_PATH);
		//todo
		//空格分开的字符,当成多个字符
		if (buf[0]=='\0')
		{
			m_list.DeleteAllItems();
			return 0;
		}

		std::tstring strBuf(buf);
		int a,b;
		a=strBuf.find_first_not_of(' ');
		if (a==std::tstring::npos){
			m_list.DeleteAllItems();
			return 0;
		}
		b=strBuf.find_last_not_of(' ');
		
		if (b<a) {m_list.DeleteAllItems();return 0;}
		strBuf=strBuf.substr(a,b+1-a);


		int count=pM->m_pPlaylistView->GetItemCount();
		for (int i=0;i<count;++i)
		{
			PlayListItem *track=(PlayListItem*)pM->m_pPlaylistView->GetItemData(i);
			if (track->HaveKeywords(const_cast<TCHAR*>( strBuf.c_str()) ))
				m_Searchlist.push_back(track);
		}

		
		m_list.Reload(m_Searchlist.begin(),m_Searchlist.end());
		
		m_list.SetItemState(0,LVIS_FOCUSED|
			LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);	

		return 0;
	}


	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		HideSelf();
		return 0;
	}

	void HideSelf()
	{
		::SetWindowText(GetDlgItem(IDC_EDIT),0);
		ShowWindow(SW_HIDE);
	}

	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		AtlTrace(L"dlg : %c\n",nChar);
	}
};


//ctrl+c,v,x被上层截获.
//收不到

void CMyEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	AtlTrace(L"%u",nChar);

	if(nChar==VK_ESCAPE)
	{
		m_pDlgSearch->HideSelf();
	}
	else if (nChar==VK_TAB )
	{
		::SetFocus(m_pDlgSearch->m_list.m_hWnd);
	}
	else if (nChar==VK_RETURN )//播放列表选中项
	{
		::SetFocus(m_pDlgSearch->m_list.m_hWnd);
		::SendMessage(m_pDlgSearch->m_list.m_hWnd,WM_CHAR,(WPARAM)VK_RETURN,NULL);	
	}
	else
	{
		SetMsgHandled(FALSE);
	}
}
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
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DlgResize_Init(FALSE);
		CenterWindow(GetParent());
		m_list.SubclassWindow(::GetDlgItem(m_hWnd,IDC_LIST));
		m_list.OnCreate(0);
		m_list.SetMain(pM);
		return TRUE;
	}


	PlayList m_SearchPl;
	LRESULT OnSearch(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_SearchPl.m_songList.clear();

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
		if (a==std::tstring::npos)
		{
			m_list.DeleteAllItems();
			return 0;
		}
		b=strBuf.find_last_not_of(' ');
		
		if (b<a) {m_list.DeleteAllItems();return 0;}
		strBuf=strBuf.substr(a,b+1-a);

		int count=pM->m_pPlaylistView->GetItemCount();
		m_SearchPl.m_songList.clear();

		for (int i=0;i<count;++i)
		{
			PlayListItem *track=(PlayListItem*)pM->m_pPlaylistView->GetItemData(i);
			if (track->HaveKeywords(const_cast<TCHAR*>( strBuf.c_str()) ))
			{
				m_SearchPl.m_songList.push_back(*track);
			}
		}


		m_list.Reload(&m_SearchPl);
		return 0;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ShowWindow(SW_HIDE);
		return 0;
	}
};

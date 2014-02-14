#pragma once
#include <set>
#include <atlsimpcoll.h>
#include "PlayListView.h"
#include "globalStuffs.h"
#include "MySerialize.h"
#include "MyLib.h"
using namespace std;

class DialogSearch :
	public CDialogImpl<DialogSearch>,
	public CDialogResize<DialogSearch>,
	public SerializeObj<DialogSearch>,
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
		MESSAGE_HANDLER(WM_PLAYQUEUE_CHANGED,OnPlayQueueChanged)
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
	LPCPlayList searchPl;
	CRect m_rc;
	BOOL m_bHiden;
	TCHAR *title;
	TCHAR *title2;

	FILE& operator<<(FILE& f);
	FILE& operator>>(FILE& f) const ;
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		IWantToReceiveMessage(WM_PLAYQUEUE_CHANGED);

		DlgResize_Init(FALSE,FALSE);

		if(m_rc.IsRectNull())
			CenterWindow(GetParent());
		else
			::SetWindowPos(m_hWnd, NULL, m_rc.left, m_rc.top, m_rc.right - m_rc.left, m_rc.bottom - m_rc.top,SWP_NOZORDER);

		m_list.SubclassWindow(::GetDlgItem(m_hWnd,IDC_LIST));
		

		title=new TCHAR[MAX_PATH];
		GetWindowText(title,MAX_PATH);
		title2=new TCHAR[MAX_PATH];
		_tcscpy(title2,title);
		_tcscat(title2,_T("<播放列队>"));

		m_bHiden=TRUE;

		searchPl=new CPlayList;
		searchPl->SetSearch(TRUE);
		m_list.SetPlayList(searchPl);

		m_list.Init(true);
		
		Search();

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		return TRUE;
	}

	
	LRESULT OnPlayQueueChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		TCHAR buf[MAX_PATH];
		::GetWindowText(::GetDlgItem(m_hWnd,IDC_EDIT),buf,MAX_PATH);
		if(buf[0]=='\0')
			Search();

		return 1;
	}


	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled=FALSE;
		GetWindowRect(&m_rc);
		m_list.Save();
		return 1;
	}


	BOOL Search()
	{
		LPCPlayList pSelectedPl=MyLib::shared()->GetSelectedPL();
		if(pSelectedPl==NULL) return 0;

		m_list.ClearMap();
		m_list.ClearAllItem();
		m_list.playlistParent=pSelectedPl;
		searchPl->DeleteAllItems();

		TCHAR buf[MAX_PATH];
		::GetWindowText(::GetDlgItem(m_hWnd,IDC_EDIT),buf,MAX_PATH);
		std::tstring strBuf(buf);

		//todo
		//空格分开的字符,当成多个字符
		if (buf[0]=='\0')
		{
			auto queue = MyLib::shared()->GetPlayQueue();
			if(!queue.empty())
			{
				SetWindowText(title2);
				auto beg=queue.begin();
				auto end=queue.end();
				for (;beg!=end;++beg)
				{
					auto cp=MakeDuplicate(*beg);
					searchPl->AddItem(cp);
					m_list.Add2Map(cp,*beg);
				}
			}
			goto theEnd;
		}

		SetWindowText(title);

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
			LPCPlayListItem track=pSelectedPl->GetItem(i);
			if (track->HaveKeywords(const_cast<TCHAR*>( strBuf.c_str()) ))
			{
				auto cp=MakeDuplicate(track);
				searchPl->AddItem(cp);
				m_list.Add2Map(cp,track);
			}
		}


theEnd:
		if (searchPl->GetItemCount()>0)
			m_list.Invalidate(TRUE);
			//m_list.Reload(searchPl,-1);
		else
			m_list.ClearAllItem();

		return TRUE;
	}

	LRESULT OnSearch(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		Search();
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
		ShowWindow(SW_HIDE);
		m_bHiden=TRUE;
	}

	void ShowSelf()
	{	
		//notify user i was shown
		if(!m_bHiden && GetFocus() != m_hWnd)
		{
			FLASHWINFO fi;
			fi.cbSize=sizeof(fi);
			fi.hwnd=m_hWnd;
			fi.dwFlags=FLASHW_ALL;
			fi.uCount=2;
			fi.dwTimeout=140;
			FlashWindowEx(&fi);
		}

		//show setfocus and select all
		ShowWindow(SW_SHOW);
		m_bHiden=FALSE;
		HWND edit=GetDlgItem(IDC_EDIT);
		::SetFocus(edit);
		::SendMessage(edit,EM_SETSEL,(WPARAM)0,(LPARAM)-1);
		Search();
	}
};

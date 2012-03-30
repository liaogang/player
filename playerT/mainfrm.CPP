#include "StdAfx.h"
#include "MainFrm.h"
#include "PlayList.h"
#include "DialogFFT.h"
#include "DialogLyric.h"
 #include "BasicPlayer.h"
 LRESULT CMainFrame::OnFftDialog(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
 {
//  	static DialogFFT *dlg=0;
//  	if (!dlg)
//  	{
//  		dlg=new DialogFFT;
//  		dlg->dscrl=m_pDsoundControl;
//  		dlg->Create(this->m_hWnd);
//  		m_pDsoundControl->SetShowWindow(&(dlg->m_hWnd));
//  	}
//  
//  
//  	dlg->ShowWindow(SW_SHOW);
 	return 0;
 }

LRESULT CMainFrame::OnPlay(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	//m_pDsoundControl->Play();
	//gMainFrame=this;
	return 0;
}

LRESULT CMainFrame::OnOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("wave files(*.wav) \0 *.wav\0 mp3 files\0 *.mp3"),this->m_hWnd);
	if(dlg.DoModal()!=IDOK)return 0;

	LPTSTR filepath=dlg.m_ofn.lpstrFile;

	//m_pDsoundControl->OpenFile(filepath);

	return 0;
}

LRESULT CMainFrame::OnPause(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	//m_pDsoundControl->Pause();
	return 0;
}

LRESULT CMainFrame::OnStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	//m_pDsoundControl->Stop();

	return 0;
}

LRESULT CMainFrame::OnConfig(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (!::IsWindow(m_dlgConfig.m_hWnd))
		m_dlgConfig.Create(m_hWnd);
	else
		m_dlgConfig.SetFocus();
	
	m_dlgConfig.ShowWindow(SW_SHOW);
	return 0;
}

LRESULT CMainFrame::OnTestInitplaylist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加命令处理程序代码

	return 0;
}

LRESULT CMainFrame::OnFileOpendirectory(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加命令处理程序代码

	UINT uFlags=BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE;
	uFlags&=~BIF_NONEWFOLDERBUTTON ;
	CFolderDialog dlg(m_hWnd,_T("请选择要添加的文件夹"),uFlags);
	if (dlg.DoModal()==IDOK)
	{
		LPCTSTR path=dlg.GetFolderPath();
		TCHAR *tmp=new TCHAR[MAX_PATH];
		_tcscpy(tmp,path);
		PlayList::AddFolderToCurrentPlayList(tmp);
		
		list<tstring> *songList=&(MyLib::GetPlayListObj().m_songList);
		list<tstring>::iterator i;
		int j;

		for (j=0,i=songList->begin();i!=songList->end();i++,j++)
		{
			m_view.InsertItem(j,(*i).c_str());
			m_view.SetItemData(j,(DWORD)&(*i));
		}
	}
	return 0;
}

LRESULT CMyView::OnDbClicked(UINT i,CPoint pt)
{
	LVITEM item;
	GetSelectedItem(&item);
	tstring *str=(tstring*)GetItemData(item.iItem);
	TCHAR *tmp=new TCHAR[MAX_PATH];
	_tcscpy(tmp,str->c_str());


// 	DsoundControl *shared=DsoundControl::shared();
// 	shared->Stop();
// 	shared->OpenFile(tmp);
// 	shared->Play();

	CBasicPlayer *pSharedPlayer=CBasicPlayer::shared();
	pSharedPlayer->m_pMainFrame=this->m_pMainFrame;
	pSharedPlayer->open(tmp);

	SetMsgHandled(FALSE);
	return 0;
}


LRESULT CMainFrame::OnShowLyric(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static CDialogLyric *dlg=NULL;
	if (!dlg)
	{
		dlg=new CDialogLyric;
		dlg->Create(this->m_hWnd);
	}

	dlg->ShowWindow(SW_SHOW);
	return 0;
}
#pragma  once
#include "mainfrm.h"
#include "MyLib.h"
#include "PlayList.h"
#define  ID_MENU_PIC_SAVE (0XF000-209)
class CAlbumCoverView :
	public CWindowImpl<CAlbumCoverView>
{
public:
	CMainFrame *pMainFrame;
	HPEN  newPen,oldPen; 
	HBRUSH brush,oldBrush;
	HMENU menu;

	CAlbumCoverView():bHasPic(FALSE)
	{
		brush=::GetSysColorBrush(/*COLOR_3DFACE*/COLOR_3DSHADOW);

		menu=::CreatePopupMenu();
		//::InsertMenu(menu,0,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND|MF_BYPOSITION,ID_MENU_PIC_SAVE,_T("图像保存到"));

		newPen=(HPEN)::CreatePen(PS_NULL,0,RGB(255,255,255));
	}

	~CAlbumCoverView()
	{
		DeleteObject(newPen);
		::DestroyMenu(menu);
	}

public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(CAlbumCoverView)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MSG_WM_RBUTTONUP(OnRButtonUp)
		COMMAND_ID_HANDLER(ID_MENU_PIC_SAVE, OnPicSave)
		MSG_WM_SIZE(OnSize)
	END_MSG_MAP()

	void OnSize(UINT nType, CSize size)
	{
		GetClientRect(&rc);
		SetMsgHandled(FALSE);
	}

	LRESULT OnPicSave(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
// 		const TCHAR szFilter[]=_T("BMP图像文件(*.bmp)\0*.bmp\0");
// 		const TCHAR szDefaultExt[]=_T("bmp");
// 		
// 		CFileDialog dlg(FALSE,szDefaultExt,NULL,OFN_OVERWRITEPROMPT,szFilter ,m_hWnd);
// 		if(dlg.DoModal()==IDOK)
// 		{
// 			i->img->Save(dlg.m_ofn.lpstrFile);
// 		}

		return 0;
	}
	
	void OnRButtonUp(UINT nFlags, CPoint point)
	{
		::ClientToScreen(m_hWnd,&point);
		::TrackPopupMenu(menu,TPM_LEFTALIGN,point.x,point.y,0,m_hWnd,0);
	}

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// handled, no background painting needed
		return 1;
	}

	BOOL bHasPic;
	PlayListItem *i;
	RECT rc;

	void TrackChanged()
	{
		bHasPic=FALSE;
		PlayList* ppl=MyLib::shared()->ActivePlaylist();
		if (ppl)
		{
			i=ppl->curTrack();
			if (i &&i->img)
				bHasPic=TRUE;
		}

		Invalidate();
		ResetMenu();
	}


	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		PAINTSTRUCT ps;
		::BeginPaint(m_hWnd,&ps);
		oldBrush=(HBRUSH)::SelectObject(ps.hdc,brush);
		oldPen=(HPEN )::SelectObject(ps.hdc,newPen);

		if (bHasPic)
		{
			int iw=i->img->GetWidth();
			int ih=i->img->GetHeight();

			i->img->Draw(ps.hdc,rc.left,rc.top,iw,ih,0,0,iw,ih);
			oldPen=(HPEN )::SelectObject(ps.hdc,newPen);
			::Rectangle(ps.hdc,iw,rc.top,rc.right+1,rc.bottom+1);
			::Rectangle(ps.hdc,rc.left-1,ih,iw+1,rc.bottom+1);
			::SelectObject(ps.hdc,oldPen);
		}
		else
		{
			oldPen=(HPEN )::SelectObject(ps.hdc,newPen);
			::Rectangle(ps.hdc,rc.left-1,rc.top,rc.right+1,rc.bottom+1);
			::SelectObject(ps.hdc,oldPen);
		}

		::SelectObject(ps.hdc,oldBrush);
		::SelectObject(ps.hdc,oldPen);
		::EndPaint(m_hWnd,&ps);	

		return 0;
	}

	void ResetMenu()
	{
		static INT bDisabledLast=-1;

		BOOL bDisable=!bHasPic;

		if (bDisable!=bDisabledLast)
		{
			::DeleteMenu(menu,0,MF_BYPOSITION);
			::InsertMenu(menu,0,(bDisable?(MF_DISABLED|MF_GRAYED):MF_ENABLED)|MF_BYCOMMAND|MF_BYPOSITION,ID_MENU_PIC_SAVE,_T("图像保存到"));
		}

		bDisabledLast=bDisable;
	}
	 
};
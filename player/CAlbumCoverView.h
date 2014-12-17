#pragma  once
#include "mainfrm.h"
#include "MyLib.h"
#include "PlayList.h"
#include "globalStuffs.h"
#define WIDTH(rc) ((rc).right-(rc.left))
#define HEIGHT(rc) ((rc).bottom-(rc.top))
#define  ID_MENU_PIC_SAVE (0XF000-209)
class CAlbumCoverView :
	public CWindowImpl<CAlbumCoverView>
{
public:
	HPEN  newPen,oldPen; 
	HBRUSH brush,oldBrush;
	HMENU menu;

	CAlbumCoverView():bHasPic(FALSE)
	{
		brush=::GetSysColorBrush(/*COLOR_3DFACE*/COLOR_3DSHADOW);

		menu=::CreatePopupMenu();
		//::InsertMenu(menu,0,MF_DISABLED|MF_GRAYED|MF_BYCOMMAND|MF_BYPOSITION,ID_MENU_PIC_SAVE,_T("ͼ�񱣴浽"));

		newPen=(HPEN)::CreatePen(PS_NULL,0,RGB(255,255,255));
	}

	~CAlbumCoverView()
	{
		DeleteObject(newPen);
		::DestroyMenu(menu);
		IDonotWantToReceiveMessage(WM_NEW_TRACK_STARTED);
	}

public:
	DECLARE_WND_CLASS_EX(_T("AlbumCoverView"),  CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS , COLOR_BTNSHADOW)

	BEGIN_MSG_MAP_EX(CAlbumCoverView)
		MSG_WM_DESTROY(OnDestroy)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MSG_WM_RBUTTONUP(OnRButtonUp)
		COMMAND_ID_HANDLER(ID_MENU_PIC_SAVE, OnPicSave)
		MSG_WM_SIZE(OnSize)
		MESSAGE_HANDLER(WM_NEW_TRACK_STARTED,OnNewTrackStarted)
		MESSAGE_HANDLER(WM_TRACKSTOPPED,OnStopped)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
	END_MSG_MAP()
	
	void OnLButtonDblClk(UINT nFlags, CPoint point)
	{
		::PostMessage(GetMainFrame()->m_hWnd,WM_PLAYLISTVIEW_CENTER_ITEM,NULL,NULL);
	}


	HWND CreateMyWnd();

	BOOL OnEraseBkgnd(CDCHandle dc)
	{
		//no bkgnd repaint needed
		if (bHasPic)
			return 1;
		else 
		{
			SetMsgHandled(FALSE);
			return 0;
		}
	}

	void Init()
	{
		IWantToReceiveMessage(WM_NEW_TRACK_STARTED);
		IWantToReceiveMessage(WM_TRACKSTOPPED);

		TrackChanged();
	}

	void OnSize(UINT nType, CSize size)
	{
		GetClientRect(&rc);
		SetMsgHandled(FALSE);
	}

	LRESULT OnPicSave(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		const TCHAR szFilter[]=_T("BMPͼ���ļ�(*.bmp)\0*.bmp\0");
		const TCHAR szDefaultExt[]=_T("bmp");

		CFileDialog dlg(FALSE,szDefaultExt,NULL,OFN_OVERWRITEPROMPT,szFilter ,m_hWnd);
		if(dlg.DoModal()==IDOK)
		{
			track->GetImg()->Save(dlg.m_ofn.lpstrFile);
		}

		return 0;
	}

	void OnRButtonUp(UINT nFlags, CPoint point)
	{
		::ClientToScreen(m_hWnd,&point);
		::TrackPopupMenu(menu,TPM_LEFTALIGN,point.x,point.y,0,m_hWnd,0);
	}


	BOOL bHasPic;
	LPCPlayListItem track;
	RECT rc;

	void TrackChanged()
	{
		bHasPic=FALSE;

		if (MyLib::shared()->isPlaying())
		{
			track=MyLib::shared()->GetPlayingItem();
			if (track->GetImg())
				bHasPic=TRUE;
		}

		Invalidate();
		ResetMenu();
	}


	LRESULT OnNewTrackStarted(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		TrackChanged();
		return 0;
	}

	LRESULT OnStopped(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		bHasPic=FALSE;
		Invalidate(TRUE);
		return 0;
	}


	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		PAINTSTRUCT ps;
		::BeginPaint(m_hWnd,&ps);


		if (bHasPic)
		{
			oldBrush=(HBRUSH)::SelectObject(ps.hdc,brush);
			oldPen=(HPEN )::SelectObject(ps.hdc,newPen);

			int iw=track->GetImg()->GetWidth();
			int ih=track->GetImg()->GetHeight();

			track->GetImg()->Draw(ps.hdc,WIDTH(rc)/2-iw/2,HEIGHT(rc)/2-ih/2,iw,ih,0,0,iw,ih);





			rc.bottom+=1;

			RECT rcUp,rcDown,rcLeft,rcRight;
			rcUp=rc;
			rcUp.bottom=HEIGHT(rc)/2-ih/2+1;

			rcDown=rc;
			rcDown.top=HEIGHT(rc)/2+ih/2-1;

			rcLeft=rc;
			rcLeft.top=rcUp.bottom-1;
			rcLeft.bottom=rcDown.top+1;
			rcLeft.right=WIDTH(rc)/2-iw/2+1;

			rcRight=rc;
			rcRight.top=rcUp.bottom-1;
			rcRight.bottom=rcDown.top+1;
			rcRight.left=WIDTH(rc)/2+iw/2;


			::Rectangle(ps.hdc,rcUp.left,rcUp.top,rcUp.right,rcUp.bottom);
			::Rectangle(ps.hdc,rcDown.left,rcDown.top,rcDown.right,rcDown.bottom);
			::Rectangle(ps.hdc,rcLeft.left,rcLeft.top,rcLeft.right,rcLeft.bottom);
			::Rectangle(ps.hdc,rcRight.left,rcRight.top,rcRight.right,rcRight.bottom);

			::SelectObject(ps.hdc,oldBrush);
			::SelectObject(ps.hdc,oldPen);
		}
		//else
		//{
		//	oldPen=(HPEN )::SelectObject(ps.hdc,newPen);
		//	::Rectangle(ps.hdc,rc.left-1,rc.top,rc.right+1,rc.bottom+1);
		//	::SelectObject(ps.hdc,oldPen);
		//}



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
			::InsertMenu(menu,0,(bDisable?(MF_DISABLED|MF_GRAYED):MF_ENABLED)|MF_BYCOMMAND|MF_BYPOSITION,ID_MENU_PIC_SAVE,_T("ͼ�񱣴浽"));
		}

		bDisabledLast=bDisable;
	}


	void OnDestroy()
	{
		IDonotWantToReceiveMessage(WM_NEW_TRACK_STARTED);
		IDonotWantToReceiveMessage(WM_TRACKSTOPPED);
		SetMsgHandled(FALSE);
	}

	virtual void OnFinalMessage(_In_ HWND /*hWnd*/)
	{


		delete this;
	}
};



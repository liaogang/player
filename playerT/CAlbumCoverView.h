#pragma  once
#include "mainfrm.h"
#include "MyLib.h"
#include "PlayList.h"

class CAlbumCoverView :
	public CWindowImpl<CAlbumCoverView>
{
public:
	CMainFrame *pMainFrame;
	HPEN  newPen,oldPen; 
	
	CAlbumCoverView()
	{
		newPen=(HPEN)::CreatePen(PS_SOLID,0,RGB(255,255,255));
	}

	~CAlbumCoverView()
	{
		DeleteObject(newPen);
	}

public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP(CAlbumCoverView)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// handled, no background painting needed
		return 1;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		BOOL bHasPic=FALSE;
		RECT rc;
		GetClientRect(&rc);
		PlayList* ppl=MyLib::shared()->ActivePlaylist();
		PlayListItem *i;
		if (ppl)
		{
			i=ppl->curTrack();
			if (i &&i->img)
				bHasPic=TRUE;
		}

		PAINTSTRUCT ps;
		::BeginPaint(m_hWnd,&ps);

		if (bHasPic)
		{
			i->img->Draw(ps.hdc,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,0,0,i->img->GetWidth(),i->img->GetHeight());
		}
		else
		{
			oldPen=(HPEN )::SelectObject(ps.hdc,newPen);
			::Rectangle(ps.hdc,rc.left,rc.top,rc.right,rc.bottom);
			::SelectObject(ps.hdc,oldPen);
		}

		::EndPaint(m_hWnd,&ps);	

		return 0;
	}

};
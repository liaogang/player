#pragma  once
#include "mainfrm.h"
#include "MyLib.h"
#include "PlayList.h"

class CAlbumCoverView :
	public CWindowImpl<CAlbumCoverView>
{
public:
	CMainFrame *pMainFrame;
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
		PAINTSTRUCT ps;
		::BeginPaint(m_hWnd,&ps);

		CPaintDC dc(m_hWnd);
		RECT rc;

		GetClientRect(&rc);
		
		PlayList* ppl=MyLib::shared()->ActivePlaylist();
		if (ppl){
			PlayListItem *i=ppl->curTrack();

			if (i &&i->img)
				i->img->Draw(this->GetDC(),rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,0,0,i->img->GetWidth(),i->img->GetHeight());
		}
				
		::EndPaint(m_hWnd,&ps);	

		return 0;
	}

};
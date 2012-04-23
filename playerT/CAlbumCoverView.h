#pragma  once
#include "mainfrm.h"
#include "PlayList.h"

class CAlbumCoverView :
	public CWindowImpl<CAlbumCoverView>
{
public:
	CMainFrame *pMainFrame;
public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP(CAlbumCoverView)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CPaintDC dc(m_hWnd);
		
		RECT rc;
		GetClientRect(&rc);
		PlayListItem *i=MyLib::GetPlayListObj().curPlayingItem;
		if (i && i->img)
			i->img->Draw(this->GetDC(),rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,0,0,i->img->GetWidth(),i->img->GetHeight());
		
		return 0;
	}

};
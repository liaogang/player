#include "stdafx.h"
#include "PlayListView.h"
#include "forwardMsg.h"
#include "MyLib.h"


const TCHAR * columnName[]={
	_T("索引"),
	_T("标题"),
	_T("艺术家"),
	_T("专辑"),
	_T("年份"),
	_T("流派")
};

const int columnCount=sizeof(columnName)/sizeof(columnName[0]);

const UINT alignment[]={
	ITEM_FLAGS_CENTRE,
	ITEM_FLAGS_LEFT,
	ITEM_FLAGS_LEFT,
	ITEM_FLAGS_LEFT,
	ITEM_FLAGS_RIGHT,
	ITEM_FLAGS_RIGHT
};

const TCHAR* columnNamePlaceHoder[]={
	_T("Index"),
	_T("Title                        "),
	_T("艺术家艺"),
	_T("Album         "),
	_T("年份"),
	_T("Genre               ")
};

void CPlayListView::Init(bool bSearch)
{	
	IWantToReceiveMessage(WM_SELECTED_PL_CHANGED);
	IWantToReceiveMessage(WM_PLAYLISTVIEW_SETFOCUS);
	IWantToReceiveMessage(WM_PLAYLISTVIEW_COLOR_DEFAULT);
	IWantToReceiveMessage(WM_PLAYLISTVIEW_COLOR_BLUE);

	Load();

	SetLVFont(m_nFontHeight);


	ChangeColorDefault();

	/*
	if ( !m_ilItemImages.CreateFromImage( IDB_EXAMPLE, 16, 0, RGB( 255, 0, 255 ), IMAGE_BITMAP, LR_CREATEDIBSECTION ) )
		return ;

	SetImageList( m_ilItemImages );
	*/

	ATLASSERT(m_iColumnCount == columnCount);

	const int cxOffset = 40;
	for (int i=0;i<columnCount;++i)
		AddColumn(columnName[i],
		m_bLoaded?m_iColumnWidths[i]:GetTextWidth(columnNamePlaceHoder[i])+cxOffset,
		-1,0,NULL_COLUMN,alignment[i]);


	SetDragDrop(TRUE);

	m_ilListItems.Detach();
	m_curDivider.Detach();
	m_curHyperLink.Detach();
	m_ttToolTip.Detach();


	CListImpl< CPlayListView >::Initialise();

	if(!bSearch)
	{
		AllPlayListViews()->AddItem(this);
		Reload(MyLib::shared()->GetSelectedPL());
	}

}


void CPlayListView::Load()
{
	//load m_iColumnWidths
	if(m_dData.GetLength() != 0)
	{
		//m_iColumnWidths
		//m_nFontHeight
		m_dData.CopyDataOut(m_iColumnWidths,(m_iColumnCount+1)*sizeof(m_iColumnWidths[0]) );
		
		m_bLoaded=TRUE;

		m_dData.Clear();
	}

}

void CPlayListView::Save()
{		
	//collect info
	for ( int nColumn = 0; nColumn < GetColumnCount(); nColumn++ )
	{
		m_iColumnWidths[nColumn] = GetColumnWidth(nColumn);
	}


	
	//save it
	//m_iColumnWidths
	//m_nFontHeight
	m_dData.CopyDataIn(m_iColumnWidths,(m_iColumnCount+1)*sizeof(m_iColumnWidths[0]) );
}

LRESULT CPlayListView::OnPlayItem(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MyLib *s=MyLib::shared();
	s->stop();
	s->play();

	return 0;
}


LRESULT CPlayListViewS::OnSelectedPlChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	/// doing nothing.

	return 1;
}
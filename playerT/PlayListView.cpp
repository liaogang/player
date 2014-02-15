#include "stdafx.h"
#include "PlayListView.h"
#include "forwardMsg.h"
#include "MyLib.h"


const TCHAR * columnName[]={
	_T("����"),
	_T("����"),
	_T("������"),
	_T("ר��"),
	_T("���"),
	_T("����")
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
	_T("��������"),
	_T("Album         "),
	_T("���"),
	_T("Genre               ")
};

void CPlayListView::Init(bool bSearch)
{	
	IWantToReceiveMessage(WM_SELECTED_PL_CHANGED);
	IWantToReceiveMessage(WM_PLAYLISTVIEW_SETFOCUS);
	IWantToReceiveMessage(WM_PLAYLISTVIEW_COLOR_DEFAULT);
	IWantToReceiveMessage(WM_PLAYLISTVIEW_COLOR_BLUE);

	Load();

	SetLVFont(20);


	ChangeColorDefault();


	if ( !m_ilItemImages.CreateFromImage( IDB_EXAMPLE, 16, 0, RGB( 255, 0, 255 ), IMAGE_BITMAP, LR_CREATEDIBSECTION ) )
		return ;

	SetImageList( m_ilItemImages );

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
		m_dData.CopyDataOut(m_iColumnWidths,m_iColumnCount*sizeof(m_iColumnWidths[0]) );
		m_dData.Clear();
		m_bLoaded=TRUE;
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
	m_dData.CopyDataIn(m_iColumnWidths,m_iColumnCount*sizeof(m_iColumnWidths[0]) );
}

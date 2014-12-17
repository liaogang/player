#pragma once
class CListViewBase:
	public CWindowImpl<CListViewBase,CListViewCtrl>
{
public:
	BEGIN_MSG_MAP_EX(CListViewBase)
	END_MSG_MAP()



		void ClearAllSel()
		{
			int i=-1;
			while ( (i=GetNextItem(i,LVIS_SELECTED)) != -1)
				SetItemState(i,0,LVNI_SELECTED|LVNI_FOCUSED );
		}

		/*
		void Init()
		{	
			SetExtendedListViewStyle(GetExtendedListViewStyle()|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP);

			const TCHAR * columnName[]={
				_T("播放列表名称"),
				_T("项目")};

				const UINT alignment[]={
					LVCFMT_RIGHT,
					LVCFMT_LEFT};

					const TCHAR* columnNamePlaceHoder[]={
						_T("                                   "),
						_T("Artist")};

						const int cxOffset = 15;

						for (int i=0;i<sizeof(columnName)/sizeof(int);++i)
							AddColumn(columnName[i],i,-1, LVCF_FMT| LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM ,alignment[i],GetStringWidth(columnNamePlaceHoder[i])+cxOffset);
		}
		*/

		int AddColumn(LPCTSTR strItem, int nItem, int nSubItem = -1,
			int nMask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
			int nFmt = LVCFMT_LEFT,int stringWidth=20)
		{

			ATLASSERT(::IsWindow(m_hWnd));
			LVCOLUMN lvc = { 0 };
			lvc.mask = nMask;
			lvc.fmt = nFmt;
			lvc.pszText = (LPTSTR)strItem;
			lvc.cx = stringWidth ;
			if(nMask & LVCF_SUBITEM)
				lvc.iSubItem = (nSubItem != -1) ? nSubItem : nItem;
			return InsertColumn(nItem, &lvc);
		}
		BOOL GetFirstSelItem()
		{
			int nItem=-1;
			if (GetItemCount()>0)
				nItem=GetNextItem(nItem,LVNI_SELECTED);

			return nItem;
		}


};



class CEditBase:public CWindowImpl<CEditBase,CEdit>
	,CEditCommands<CEditBase>
{
	DECLARE_WND_SUPERCLASS(NULL,CEdit::GetWndClassName())
	BEGIN_MSG_MAP(CEditBase)
		CHAIN_MSG_MAP_ALT(CEditCommands<CEditBase>, 1)
	END_MSG_MAP()

};

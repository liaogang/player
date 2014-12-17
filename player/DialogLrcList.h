#include "stdafx.h"
#include "CCtrlViewBase.h"
#include "globalStuffs.h"


class DialogLrcList: 
	public CDialogImpl<DialogLrcList>,
	public CDialogResize<DialogLrcList>
{
public:
	CListViewBase list;

	enum { IDD = IDD_DLG_LRC_LIST };

	BEGIN_MSG_MAP_EX(DialogLrcList)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL,OnCloseCmd)
		COMMAND_ID_HANDLER(IDDEL,OnDel)
		COMMAND_ID_HANDLER(IDC_LOAD,OnLoad)
		COMMAND_ID_HANDLER(IDC_SHOW_FILE,OnShowFile)
		COMMAND_RANGE_HANDLER(IDC_RADIO1,IDC_RADIO2,OnRadio)
		NOTIFY_HANDLER_EX(IDC_LIST,LVN_ITEMCHANGED,OnItemChanged)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(DialogLrcList)
		DLGRESIZE_CONTROL(IDC_SHOW_PATH,DLSZ_SIZE_X)

		BEGIN_DLGRESIZE_GROUP()
		DLGRESIZE_CONTROL(IDC_LIST,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_EDIT,DLSZ_SIZE_X)
		END_DLGRESIZE_GROUP()

		BEGIN_DLGRESIZE_GROUP()
		DLGRESIZE_CONTROL(IDC_RADIO1,DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_RADIO2,DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDOK      ,DLSZ_MOVE_X)
		END_DLGRESIZE_GROUP()

		DLGRESIZE_CONTROL(IDC_LIST,DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_EDIT,DLSZ_SIZE_Y)

		DLGRESIZE_CONTROL(IDC_RADIO1,DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_RADIO2,DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDOK,DLSZ_MOVE_Y)

	END_DLGRESIZE_MAP()


	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DlgResize_Init(false,false);
		CenterWindow(GetParent());

		::SendMessage(GetDlgItem(GetSearchLrcUntilEnd()?IDC_RADIO2:IDC_RADIO1), BM_SETCHECK, BST_CHECKED, 0L);
		

		list.SubclassWindow(GetDlgItem(IDC_LIST));
		list.SetExtendedListViewStyle(list.GetExtendedListViewStyle()|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP);
		const TCHAR * columnName[]={
			_T("ÎÄ¼þÃû"),
			_T("×´Ì¬")
		};
		const UINT alignment[]={
			LVCFMT_RIGHT,
			LVCFMT_LEFT
		};
		const TCHAR* columnNamePlaceHoder[]={
			_T("                                         "),
			_T("Artist       ")
		};

		const int cxOffset = 15;
		for (int i=0;i<sizeof(columnName)/sizeof(int);++i)
			list.AddColumn(columnName[i],i,-1, LVCF_FMT| LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM ,alignment[i],list.GetStringWidth(columnNamePlaceHoder[i])+cxOffset);

		Reload();

		return TRUE;
	}


	

	
	LRESULT OnRadio(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (wID==IDC_RADIO1)
		{
			SetSearchLrcUntilEnd(false);
		}
		else//(wID==IDC_RADIO2)
		{
			SetSearchLrcUntilEnd(true);
		}

		return 0;
	}
	
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnDel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	

	LRESULT OnLoad(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{

		return 0;
	}

	LRESULT OnShowFile(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{

		return 0;
	}

	LRESULT OnItemChanged(LPNMHDR pnmh)
	{
		int index=list.GetSelectedIndex();
		::EnableWindow(GetDlgItem(IDDEL),index==-1?FALSE:TRUE);
		::EnableWindow(GetDlgItem(IDC_LOAD),index==-1?FALSE:TRUE);
		
		if (index!=-1)
		{
			LPTSTR path;
			path=(LPTSTR)list.GetItemData(index);
			::SetWindowText(GetDlgItem(IDC_SHOW_PATH),path);
		}

		return 1;
	}

	void Reload();
};

#include "DialogLrcList.h"
#include "customMsg.h"
#include "MyLib.h"

LRESULT DialogLrcList::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (wID==IDOK)
	{
		SdMsg(WM_LYRIC_RELOAD,FALSE);
		Reload();
	}
	else
	{
		EndDialog(0);
	}

	return 0;
}


LRESULT DialogLrcList::OnDel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int nItem=list.GetSelectedIndex();
	LPTSTR path;
	path=(LPTSTR)list.GetItemData(nItem);

	if(IDOK==MessageBox(_T("是否删除文件?"),_T("提示"),MB_OKCANCEL))
	{
		if(::DeleteFile(path)==0)
		{
			::SetWindowText(GetDlgItem(IDC_SHOW_PATH),_T("删除文件失败"));
			int error=GetLastError();
		}
		else
		{
			list.DeleteItem(nItem);
			delete[] path;
			MyLib::shared()->InitLrcLib();
		}
	}

	return 0;
}


void DialogLrcList::Reload()
{
	list.DeleteAllItems();

	vector<LrcMatchItem> lrcList=GetLrcMatchList();
	int nItem=0;
	for (auto i=lrcList.begin();i!=lrcList.end();++i,++nItem)
	{
		LrcMatchItem m=*i;
		list.AddItem(nItem ,0, m.path.c_str());
		list.SetItemText(nItem,1,m.GetReason());

		int len=m.path.length()+1;
		TCHAR* tmp=new TCHAR[len];
		wcscpy(tmp,m.path.c_str());
		list.SetItemData(nItem,(DWORD)tmp);

	}
}
#include "forwardMsg.h"

static forwardMsg *pGForwardMsg=NULL;
forwardMsg* NotifyCenter()
{
	return pGForwardMsg?pGForwardMsg:pGForwardMsg=new forwardMsg;
}


void RegistMsgReceiver(UINT uMsg,HWND hWnd)
{
	NotifyCenter()->RegistMsgReceiver(uMsg,hWnd);
}

void LoginOutMsgReceiver(UINT uMsg,HWND hWnd)
{
	NotifyCenter()->LoginOutMsgReceiver(uMsg,hWnd);
}

void NotifyMsg(UINT uMsg,BOOL bPost,WPARAM wParam,LPARAM lParam)
{
#ifdef APP_PLAYER_UI
	NotifyCenter()->Msg(uMsg,bPost,wParam,lParam);
#endif
}

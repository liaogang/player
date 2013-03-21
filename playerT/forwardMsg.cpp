#include "forwardMsg.h"

static forwardMsg *pGForwardMsg=NULL;
// forwardMsg* GetSharedForwardMsg()
// {
// 	return pGForwardMsg?pGForwardMsg:pGForwardMsg=new forwardMsg;
// }

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
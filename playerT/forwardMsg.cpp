#include "forwardMsg.h"

static forwardMsg *pGForwardMsg=NULL;
forwardMsg* GetSharedForwardMsg()
{
	return pGForwardMsg?pGForwardMsg:pGForwardMsg=new forwardMsg;
}


void RegistMsgReceiver(UINT uMsg,HWND hWnd)
{
	GetSharedForwardMsg()->RegistMsgReceiver(uMsg,hWnd);
}

void LoginOutMsgReceiver(UINT uMsg,HWND hWnd)
{
	GetSharedForwardMsg()->LoginOutMsgReceiver(uMsg,hWnd);
}
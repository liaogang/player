#include "stdafx.h"

#include <vector>
#include <map>
#pragma once

#define IWantToReceiveMessage(uMsg) RegistMsgReceiver(uMsg,m_hWnd)
#define IDonotWantToReceiveMessage(uMsg) LoginOutMsgReceiver(uMsg,m_hWnd)

using namespace std;
class forwardMsg
{
	typedef  vector<HWND> ListOfOneMsg;
	map<UINT,ListOfOneMsg*> msgMap;
	typedef pair<UINT,ListOfOneMsg*> msgPair;
public:
	void RegistMsgReceiver(UINT uMsg,HWND hWnd)
	{
		ATLASSERT(IsWindow(hWnd));

		auto it=msgMap.find(uMsg);
		if (it==msgMap.end())
		{
			msgMap.insert(msgPair(uMsg,new ListOfOneMsg));
			it=msgMap.find(uMsg);
		}
		
		auto pList=it->second;
		pList->push_back(hWnd);
		
#ifdef _DEBUG
		AtlTrace(L"The Windows  %d Registed a message WM_USER+%d ",hWnd,uMsg-WM_USER);
#endif
	}
	
	void LoginOutMsgReceiver(UINT uMsg,HWND hWnd)
	{
		auto it=msgMap.find(uMsg);
		if (it!=msgMap.end())
		{
			auto pList=it->second;
			for (auto it2=pList->begin();it2!=pList->end();++it2)
			{
				if (*it2==hWnd)
				{
					pList->erase(it2);
					break;
				}
			}
		}
	}

	~forwardMsg()
	{
		for (auto it=msgMap.begin();it!=msgMap.end();++it)
		{
			auto pList=it->second;
			delete pList;
		}
	}

	//发送广播消息
	void Msg(UINT uMsg,WPARAM wParam=NULL,LPARAM lParam=NULL)
	{
#ifdef _DEBUG
		//AtlTrace(L"message WM_USER+%d come in to msg center\n",uMsg-WM_USER);
		return;
#endif
		
		auto it=msgMap.find(uMsg);
		if (it!=msgMap.end())
		{
			ListOfOneMsg* pList=it->second;
			for (ListOfOneMsg::iterator it2=pList->begin();it2!=pList->end();++it2)
			{
				::SendMessage(*it2,uMsg,wParam,lParam);
			}
		}
	}
};



forwardMsg* NotifyCenter();
void LoginOutMsgReceiver(UINT uMsg,HWND hWnd);
void RegistMsgReceiver(UINT uMsg,HWND hWnd);


inline void NotifyMsg(UINT uMsg,WPARAM wParam=NULL,LPARAM lParam=NULL){NotifyCenter()->Msg(uMsg,wParam,lParam);}
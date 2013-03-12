#include "stdafx.h"

#include <vector>
#include <map>
#pragma once
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


	void Msg(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		auto it=msgMap.find(uMsg);
		if (it!=msgMap.end())
		{
			ListOfOneMsg* pList=it->second;
			for (ListOfOneMsg::iterator it2=pList->begin();it2!=pList->end();++it2)
				::SendMessage(*it2,uMsg,wParam,lParam);
		}
	}
};




forwardMsg* GetSharedForwardMsg();
void LoginOutMsgReceiver(UINT uMsg,HWND hWnd);
void RegistMsgReceiver(UINT uMsg,HWND hWnd);

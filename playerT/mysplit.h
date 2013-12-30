#include "stdafx.h"
#include "globalStuffs.h"
#include "MySerialize.h"
#include "mytree.h"



#pragma once
using namespace std;

class CMainFrame;
class MYTREE;
class CPlaceHolderWnd;






//CSplitterWindow;
static HCURSOR m_hCursorLeftRight=NULL;
static HCURSOR m_hCursorUpDown=NULL;

class CMultiSpliltWnd:
	public ATL::CWindowImpl< CMultiSpliltWnd>
{
public:
	MYTREE *rootTree;
	MYTREE *rclickTree;
	CMainFrame *pMain;
	bool m_bFullDrag;
	splitBarInfo info;
	int m_cxyDragOffset;
	int lastXPPos;
	MYTREE *curHitPos;
	HMENU menu;
	HMENU subMenu;

public:
	DECLARE_WND_CLASS_EX(NULL,NULL,NULL)

	BEGIN_MSG_MAP_EX(CMultiSpliltWnd)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)

		//MSG_WM_RBUTTONUP(OnRButtonDblClk)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)

		//MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)

#ifndef _WIN32_WCE
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
#endif // !_WIN32_WCE
		//COMMAND_ID_HANDLER(ID_ADD_EMPTYWINDOW,OnAddEmptyPane)
		//COMMAND_ID_HANDLER(ID_ADD_LEFT,OnAddLeftRightSplit)
		//COMMAND_ID_HANDLER(ID_ADD_UP,OnAddUpDownSplit)
		//COMMAND_ID_HANDLER(ID_MULTISPLITEDITMODE_CLOSE,OnClosePane)
		//COMMAND_ID_HANDLER(ID_MULTISPLITEDITMODE_SHOWTREEVIEW,OnShowTreeView)
		//COMMAND_ID_HANDLER(ID_PANE_PLAYLIST,OnAddPlaylistView)
		REFLECT_NOTIFICATIONS()
		END_MSG_MAP()

public:

	CMultiSpliltWnd():m_bFullDrag(true),curHitPos(NULL)
	{
		menu=::LoadMenu(NULL,MAKEINTRESOURCE(IDR_MENU_EDIT_MODE));
		subMenu=::GetSubMenu(menu,0);
		rootTree=NULL;
	}

	virtual void OnFinalMessage(_In_ HWND /*hWnd*/)
	{
		delete this;
	}

	BOOL OnEraseBkgnd(CDCHandle dc)
	{
		return TRUE;
	}

	void OnRButtonDblClk(UINT nFlags, POINT point)
	{
		POINT pt;
		GetCursorPos(&pt);

		POINT ptClient=pt;
		ScreenToClient(&ptClient);
		rclickTree=hittest(ptClient);


		if(rclickTree)
		{
			//AtlTrace(L"clicked at pane %s \n",rclickTree->data.nodeName);
			::TrackPopupMenu(subMenu,TPM_LEFTALIGN,pt.x,pt.y,0,m_hWnd,0);
		}

	}


	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

	


	void UpdateTree(MYTREE *treeData);

	
	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		OnSizing();
		bHandled=FALSE;
		return 1;
	}

	void OnSizing()
	{
		static bool bFirst=true;
		if(bFirst)
		{
			PaneSizeStore(rootTree);
			bFirst=false;
		}

		RECT newRC;
		GetClientRect(&newRC);
		if (WIDTH(newRC) && HEIGHT(newRC))
		{
			rootTree->CalcChildsRect(newRC);
			UpdateLayout(rootTree,GetDC());
		}
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		PAINTSTRUCT ps;
		::BeginPaint(m_hWnd,&ps);
		rootTree->DrawSplitterPane(ps.hdc,FALSE);
		::EndPaint(m_hWnd,&ps);
		return 0;
	}


	LRESULT OnAddEmptyPane(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);



	//击中分离器时生效
	LRESULT OnAddUpDownSplit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		MYTREE *newTree=new MYTREE();
		newTree->data.type=up_down;

		MYTREE *node1=new MYTREE();
		MYTREE *node2=new MYTREE();
		newTree->AddChild(node1);

		newTree->AddChild(node2);

		rclickTree->addChildNodeAfter(newTree);

		rclickTree->EvenPanes();

		Invalidate();
		UpdateTree(rclickTree);
		return 0;
	}

	LRESULT OnAddLeftRightSplit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		MYTREE *newTree=new MYTREE();
		newTree->data.type=left_right;

		MYTREE *node1=new MYTREE();
		MYTREE *node2=new MYTREE();
		newTree->AddChild(node1);

		newTree->AddChild(node2);

		rclickTree->addChildNodeAfter(newTree);

		rclickTree->EvenPanes();


		Invalidate();
		UpdateTree(rclickTree);
		return 0;
	}

	LRESULT OnAddPlaylistView(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);




	LRESULT OnShowTreeView(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		return 0;
	}


	LRESULT OnClosePane(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if(rclickTree->isroot())
			return 0;
		else
		{
			MYTREE *parent= MYTree_RemoveFromRoot(rclickTree);
			parent->EvenPanes();
			UpdateLayout(parent);
			curHitPos=NULL;

			Invalidate();
			UpdateTree(rclickTree);
		}



		return 0;
	}






	MYTREE *hittest(POINT &pt)
	{
		if(curHitPos)
			curHitPos=curHitPos->hittest(pt);

		if (!curHitPos) 
			curHitPos=rootTree->hittest(pt);

		//AtlTrace(L"mouse move to pane %s\n",curHitPos->data.nodeName);

		return curHitPos;
	}






	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		MYTREE *curClickedTree=hittest(pt);

		if(curClickedTree && !curClickedTree->bLocked)
		{
			if(curClickedTree->GetOverSplitterBar(xPos,yPos,info))
			{
				RECT m_rcSplitter=*(info.i);
				int m_xySplitterPos=(m_rcSplitter.right-m_rcSplitter.left)/2;

				SetCapture();
				if(info.tree->parent->data.type==up_down)
				{::SetCursor(m_hCursorUpDown);
				lastXPPos=yPos;}
				else
				{::SetCursor(m_hCursorLeftRight);
				lastXPPos=xPos;
				}	
			}
		}

		bHandled = FALSE;
		return 1;
	}

	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		::ReleaseCapture();
		bHandled = FALSE;
		return 1;
	}



	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		if((wParam & MK_LBUTTON) && ::GetCapture() == m_hWnd)
		{
			RECT m_rcSplitter=*(info.i);
			bool bLR=info.tree->parent->data.type==left_right;

			int xyPos=bLR?xPos:yPos;
			int xyOffset = 0;

			xyOffset = xyPos - lastXPPos;

			//负方向,正方向.
			int maxNegativeOffset,maxPositiveOffset;

			if (bLR)
			{
				maxNegativeOffset=info.tree->getRect().left-info.tree->getRect().right;
				maxPositiveOffset=info.tree->next->getRect().right-info.tree->next->getRect().left;
			}
			else
			{
				maxNegativeOffset=info.tree->getRect().top-info.tree->getRect().bottom;
				maxPositiveOffset=info.tree->next->getRect().bottom-info.tree->next->getRect().top;
			}

			//不超过父面板
			if(xyOffset<maxNegativeOffset)
				xyOffset=maxNegativeOffset;
			if (xyOffset>maxPositiveOffset)
				xyOffset=maxPositiveOffset;

			if(xyOffset)
			{
				if(m_bFullDrag)
				{
					//设置把手位置,调整两边的面板
					//SetSplitterPos

					RECT rcErase=m_rcSplitter;
					//new splitter bar rect
					if (bLR)
					{
						m_rcSplitter.left+=xyOffset;
						m_rcSplitter.right+=xyOffset;
					}
					else
					{
						m_rcSplitter.top+=xyOffset;
						m_rcSplitter.bottom+=xyOffset;
					}

					if (bLR)
					{
						if (xyOffset>0)//右移
							rcErase.right+=xyOffset;
						else//右移
							rcErase.left+=xyOffset;
					}
					else
					{
						if (xyOffset>0)//下移
							rcErase.bottom+=xyOffset;
						else//上移
							rcErase.top+=xyOffset;
					}



					//replace new rect with old in list<RECT>
					info.tree->parent->data.SplitterBarRects.insert(info.i,m_rcSplitter);
					auto oldNext=info.tree->parent->data.SplitterBarRects.erase(info.i);
					info.i=--oldNext;


					//前面的面板
					RECT prevRC= info.tree->getRect();
					RECT nextRC =info.tree->next->getRect();

					if (bLR)
					{
						prevRC.right+=xyOffset;
						nextRC.left+=xyOffset;
					}
					else
					{
						prevRC.bottom+=xyOffset;
						nextRC.top+=xyOffset;
					}

					if(bLR)
					{
						info.tree->AdjustRect(xyOffset,ajust_right);
						info.tree->next->AdjustRect(xyOffset,ajust_left);
					}
					else
					{
						info.tree->AdjustRect(xyOffset,ajust_bottom);
						info.tree->next->AdjustRect(xyOffset,ajust_top);
					}

					UpdateLayout(info.tree->parent);
					//InvalidateRect(&rcErase);
					//UpdateWindow();
				}
				else
				{
					//DrawGhostBar();
					//pT->SetSplitterPos(xyNewSplitPos, false);
					//DrawGhostBar();
				}
			}

			//get cur split pos,after move
			lastXPPos+=xyOffset;


			PaneSizeStore(rootTree);
		}
		else		// not dragging, just set cursor
		{
			POINT pt={xPos,yPos};
			MYTREE *curClickedTree=hittest(pt);

			if(curClickedTree && !curClickedTree->bLocked && curClickedTree->GetOverSplitterBar(xPos,yPos,info))
			{
				if(info.tree->parent->data.type==left_right)
					::SetCursor(m_hCursorLeftRight);
				else
					::SetCursor(m_hCursorUpDown);
			}

			bHandled = FALSE;
		}

		return 0;
	}


};




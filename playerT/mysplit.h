#include "stdafx.h"
#include <list>


#pragma once
using namespace std;

class CMainFrame;
struct MYTREE;
class CPlaceHolderWnd;
#define max_node_name 128
#define SPLITTER_WH 3
#define TREES_ROOT NULL



//调用完calcrect之后,调用此函数,更新
void MoveToNewRect(MYTREE *parent);

void MYTREE_Add_Playlist(MYTREE* tree);
void MYTREE_Add_EmptyWnd(MYTREE* tree);
void MYTREE_Add_AlbumView(MYTREE* tree);
void MYTREE_Add_LyricView(MYTREE* tree);
void MYTREE_Add_SpectrumView(MYTREE* tree);


//删除自身,在根树中的关系
//返回父结点
MYTREE * MYTree_RemoveFromRoot(MYTREE *tree);


enum MY_DIRECTION
{
	ajust_left,
	ajust_right,
	ajust_top,
	ajust_bottom
};
bool ptInRc(POINT &pt,RECT &rc);
bool ptInRc(int x,int y,RECT &rc);
//is point is in rc



//面板的分裂方向.不是把手的方向.
enum split_type{
	left_right,
	up_down,
};

//能装
enum pane_type
{
	panetype_playlist,
	panetype_album,
	panetype_spertrum,
	panetype_lyric,
	panetype_empty
};

struct dataNode
{
public:
	dataNode(TCHAR *name):hWnd(NULL),m_iSplitterBar(SPLITTER_WH)
	{
		_tcscpy(nodeName,name);
		type=left_right;
		rc.left=0;
		rc.right=0;
		rc.top=0;
		rc.bottom=0;
	}

	dataNode():hWnd(NULL),m_iSplitterBar(SPLITTER_WH)
	{
		type=left_right;
		rc.left=0;
		rc.right=0;
		rc.top=0;
		rc.bottom=0;
	}

	~dataNode()
	{
		if (hWnd&&IsWindow(hWnd))
		{
			ShowWindow(hWnd,SW_HIDE);
			DestroyWindow(hWnd);
		}
	}

	RECT getRect(){return rc;}
	void setRect(RECT &rc){this->rc=rc;}

	//return set succeed
	bool SetSplitType(split_type type,TCHAR *nodeName=NULL)
	{
// 		if(this->type==type)
// 			return false;

		this->type=type;
		if(nodeName==NULL)
			wcscpy(this->nodeName,type==left_right?L"水平分离器":L"垂直分离器");
		else
			wcscpy(this->nodeName,nodeName);

		return true;
	}

public:
	TCHAR nodeName[max_node_name];
	RECT rc;
	split_type type;
	
	HWND hWnd;	
	void *wndHolder;


	long  m_iSplitterBar;//把手的长度或宽度
	HTREEITEM treeItem;
	list<RECT> SplitterBarRects;//把手区域列表
};


//所在的结点,把手的序号.
struct splitBarInfo
{
public:
	splitBarInfo():tree(NULL)
	{}

	//水平分裂的左边面板
	//上下分裂的上边..
	MYTREE* tree;

	list<RECT>::iterator i;
};


struct MYTREE
{
public:
	MYTREE(TCHAR *name):next(NULL),prev(NULL),
		child(NULL),data(name)
	{
		//default is not root
		parent=this;
	}

	MYTREE():next(NULL),prev(NULL),
		child(NULL)
	{
		parent=this;

		//auto name
		static int paneNumber=0;
		TCHAR paneN[max_node_name];
		_itow(++paneNumber,paneN,10);
		_tcscpy(data.nodeName,paneN);

		//AtlTrace(L"created : %s\n",paneN);
	}

	bool wndEmpty()
	{
		return data.hWnd==NULL;
	}
	HWND getWnd(){return data.hWnd;}
	void setWnd(HWND hWnd){
		ATLASSERT(IsWindow(hWnd));
		data.hWnd=hWnd;
	}

	RECT getRect(){return data.getRect();}
	void setRect(RECT &rc){data.setRect(rc);}

	//重新调整矩形区域.及子结点
	void AdjustRect(LONG offset,MY_DIRECTION direction,bool bFirst=true)
	{
		bool bSkipSibling=bFirst;
		ATLASSERT(!bSkipSibling?hasParent():1);

		MYTREE *c=this;

		//左右分裂的窗口,水平拖动,只会平移.最后一个平移+右端不变 其余同理.
		//左右分裂的窗口,上下拖动.只影响边端.
		bool bMoving=false;//not sizing all,moving last
		if(!bSkipSibling )
			if ((direction==ajust_left || direction==ajust_right)&& parent->data.type==left_right ||
				(direction==ajust_top || direction==ajust_bottom)&& parent->data.type==up_down)
				bMoving=true;

		for (;c;c=c->next)
		{
			//ajust rect
			if(bMoving)
			{
				if(direction==ajust_left)
				{
					c->data.rc.left+=offset;
					c->data.rc.right+=offset;
				}
				else if(direction==ajust_right)
					c->data.rc.right+=offset;
				else if(direction==ajust_top)
					c->data.rc.top+=offset;
				else if(direction==ajust_bottom)
					c->data.rc.bottom+=offset;
			}else
			{
				if(direction==ajust_left)
					c->data.rc.left+=offset;
				else if(direction==ajust_right)
					c->data.rc.right+=offset;
				else if(direction==ajust_top)
					c->data.rc.top+=offset;
				else if(direction==ajust_bottom)
					c->data.rc.bottom+=offset;
			}




			//splitter
			if (c->HasSplitter())
			{
				bool bX=(direction==ajust_left||direction==ajust_right);
				bool bxSplitter=c->data.type==up_down;
				if(bX==bxSplitter)
				{
					list<RECT> another;
					for (auto i=c->data.SplitterBarRects.begin();i!=c->data.SplitterBarRects.end();++i)
					{
						RECT r=*i;
						if (bxSplitter)
						{
							if(direction==ajust_left)
								r.left+=offset;
							if(direction==ajust_right)
								r.right+=offset;
						}
						else
						{
							if(direction==ajust_top)
								r.top+=offset;
							if(direction==ajust_bottom)
								r.bottom+=offset;
						}

						another.push_back(r);
					}

					c->data.SplitterBarRects=another;
				}


			}

			if(c->hasChild())
				c->child->AdjustRect(offset,direction,false);

			if(bSkipSibling)
				break;
		}
	}

	bool isleaf()
	{
		return child==NULL;
	}
	bool hasChild(){return !isleaf();}

	void setroot(){parent=NULL;}
	bool isroot(){return parent==NULL;}

	bool hasParent(){return parent!=this&&parent!=NULL;}

	bool childIsleaf()
	{
		return child->isleaf();
	}

	MYTREE *getFirstSibling()
	{
		if (isroot())
			return this;
		else
			return parent->child;
	}

	MYTREE *getLastSibling()
	{
		MYTREE *c=this;
		while(c->next)
			c=c->next;

		return c;
	}

	int getSiblingNumber()
	{
		int i=0;
		for (auto *after=getFirstSibling();after;after=after->next)
			++i;

		return i;
	}

	//计算当前之后的兄弟个数,包含自身
	int getSiblingNumberBehind()
	{
		int count=0;
		for (auto i=this;i;i=i->next)
			++count;

		return count;
	}

public:
	void setNode(HWND hWnd)
	{
		ATLASSERT(isleaf());
		ATLASSERT(hWnd && IsWindow(hWnd));
		data.hWnd=hWnd;
	}

	//添加子结点到最后
	void AddChild(MYTREE *node)
	{
		//this parent
		//first ...last , node

		node->parent=this;

		MYTREE* last=child;
		if(child)
		{while(last->next)
		last=last->next;

		last->next=node;
		node->prev=last;
		}
		else
			child=node;
	}

	//添加子结点,在mark之后.mark=TVI_FIRST,添加为第一个子结点
	//返回被添加的结点
#define MY_TVI_FIRST (MYTREE*)0
	MYTREE* addChildNodeAfter(MYTREE *newAdd,MYTREE *mark=MY_TVI_FIRST)
	{
		ATLASSERT(mark?mark->parent==this:1 && newAdd->parent==newAdd);

		newAdd->parent=this;

		//插入到第一个位置
		if (mark==MY_TVI_FIRST)   //newAdd child
		{
			newAdd->next=child;

			if (child)
				child->prev=newAdd;

			child=newAdd;
		}
		else    //mark newadd makr.oldnext
		{
			MYTREE *oldNext=mark->next;
			mark->next=newAdd;
			newAdd->prev=mark;

			newAdd->next=oldNext;

			if(oldNext)
				oldNext->prev=newAdd;
		}	

		return newAdd;
	}



	void replaceParentWithBrother(MYTREE *brother)
	{
		//we will copy brother's data to parent
		//then delete brother

		RECT oldRc=parent->data.getRect();

		//copy
		parent->data=brother->data;

		//rect retain
		parent->setRect(oldRc);

		brother->data.hWnd=NULL;
		delete brother;
	}

	bool HasSplitter(){return !data.SplitterBarRects.empty();}
public:
	MYTREE *parent;//parent item
	dataNode data;
	MYTREE *prev;
	MYTREE *next;
	MYTREE *child;//first child item

	//paint stuffs
public:

	//重新计算子结点的矩形区域
	void CalcChildsRect()
	{
		if(!hasChild())return;

		RECT rc=getRect();
		bool bLR=(data.type==left_right);
		//clear all splitter bar
		data.SplitterBarRects.clear();

		int childs=child->getSiblingNumberBehind();
		MYTREE *first=child;

		if (childs==1)
		{
			child->setRect(rc);

			if (first->hasChild())
				first->CalcChildsRect();
		}
		else
		{
			for (int count=0;first;first=first->next,++count)
			{
				int childLen=(bLR?(rc.right-rc.left):(rc.bottom-rc.top))/childs;

				RECT childRC=rc;
				if(bLR)
				{
					childRC.left=rc.left+childLen*count;
					childRC.right=childRC.left+childLen;
				}
				else
				{
					childRC.top=rc.top+childLen*count;
					childRC.bottom=childRC.top+childLen;
				}

				//再计算把手的矩形区域
				RECT rcBar=childRC;

				if (first->prev!=NULL)
				{
					if(bLR)
						childRC.left+=data.m_iSplitterBar/2;
					else
						childRC.top+=data.m_iSplitterBar/2;
				}

				if (first->next!=NULL)
				{
					if(bLR)
					{rcBar.right+=data.m_iSplitterBar/2;
					childRC.right-=data.m_iSplitterBar/2;
					rcBar.left=rcBar.right-data.m_iSplitterBar;
					}else{
						rcBar.bottom+=data.m_iSplitterBar/2;
						childRC.bottom-=data.m_iSplitterBar/2;
						rcBar.top=rcBar.bottom-data.m_iSplitterBar;
					}


					data.SplitterBarRects.push_back(rcBar);
				}

				first->setRect(childRC);

				if (first->hasChild())
					first->CalcChildsRect();
			}

		}
	}



	//非遍历
	void DrawEmptyPane(CDCHandle dc)
	{
		ATLASSERT(isleaf());

		RECT r=getRect();
		const int bar=0;
		r.left+=bar;
		r.top+=bar;
		r.right-=bar;
		r.bottom-=bar;

		dc.DrawEdge(&r, EDGE_SUNKEN, BF_RECT | BF_ADJUST);
		dc.FillRect(&r, COLOR_APPWORKSPACE);

		int x=r.left+(r.right-r.left)/2;
		int y=r.top+(r.bottom-r.top)/2;
		::SetBkMode(dc.m_hDC,TRANSPARENT);
		dc.TextOut(x,y,data.nodeName);
	}


	//遍历
	// called only if pane is empty
	void DrawSplitterPane(CDCHandle dc,BOOL invalidate=FALSE)
	{
		for (MYTREE *cur=this;cur;cur=cur->next)
		{
			if (cur->hasChild())
			{
				for (auto i=cur->data.SplitterBarRects.begin();i!=cur->data.SplitterBarRects.end();++i)
					cur->DrawSplitterBar(dc,*i);

				cur->child->DrawSplitterPane(dc);
			}
			else
			{
				if(wndEmpty())	
					cur->DrawEmptyPane(dc);
				else
				{
					//RECT r2=cur->getRect();
					//SetWindowPos(getWnd(),NULL,r2.left,r2.top,r2.right-r2.left,r2.bottom-r2.top,SWP_NOOWNERZORDER|
					//	SWP_SHOWWINDOW);
					if(invalidate)
						InvalidateRect(cur->data.hWnd,0,1);
				}
			}
		}
	}



	// Overrideables
	void DrawSplitterBar(CDCHandle dc,RECT &rect)
	{
		dc.FillRect(&rect, COLOR_3DFACE);
#define  SPLIT_GRADIENTBAR 0x00000008
#if (!defined(_WIN32_WCE) || (_WIN32_WCE >= 420))
		if(( 0 & SPLIT_GRADIENTBAR & SPLIT_GRADIENTBAR) != 0)
		{
			RECT rect2 = rect;
			if(data.type==left_right)
				rect2.left = (rect.left + rect.right) / 2 - 1;
			else
				rect2.top = (rect.top + rect.bottom) / 2 - 1;

			dc.GradientFillRect(rect2, ::GetSysColor(COLOR_3DFACE), ::GetSysColor(COLOR_3DSHADOW), data.type==up_down);
		}
#endif // !defined(_WIN32_WCE) || (_WIN32_WCE >= 420)

		// draw 3D edge if needed
		//if((WS_EX_CLIENTEDGE & WS_EX_CLIENTEDGE) != 0)
		//	dc.DrawEdge(&rect, EDGE_RAISED, data.type==left_right ? (BF_LEFT | BF_RIGHT) : (BF_TOP | BF_BOTTOM));
	}







	//遍历所有二级子结点.
	//坐标是否在把手矩形区域内?
	//返回其,结点.
	bool GetOverSplitterBar(int x, int y,splitBarInfo &info,bool bFirst=true)
	{		
		bool result=false;
		bool bSkipSibling=bFirst;

		for (MYTREE *cur=this; cur && !result ;cur=cur->next)
		{
			if (isleaf())
				continue;


			if ( cur->HasSplitter())
			{
				MYTREE *bar=cur->child;
				for (auto i=cur->data.SplitterBarRects.begin();i!=cur->data.SplitterBarRects.end();++i,bar=bar->next)
				{
					RECT rc=*i;
					if(ptInRc(x,y,rc))
					{
						info.tree=bar;
						info.i=i;
						result=true;
						break;
					}
				}
			}
			else if(child->hasChild())
			{
				result=child->GetOverSplitterBar(x,y,info,false);
			}

			if (bSkipSibling)
				break;
		}

		return result;
	}


	//todo
	//如果在把手区域里,肯定不会在子面板里.
	MYTREE* hittest(POINT &pt,bool bFirst=true)
	{
		MYTREE *ret=NULL;

		bool bSkipSibling=bFirst;

		for (MYTREE *cur=this ; cur && !ret;cur=cur->next)
		{
			if (ptInRc(pt,cur->getRect())) 
			{
				MYTREE *ret2=NULL;
				if(cur->hasChild())
					ret2=cur->child->hittest(pt,false);

				ret=ret2?ret2:cur;
			}

			if(bSkipSibling)
				break;
		}

		return ret;
	}

};






//CSplitterWindow;
static HCURSOR m_hCursorLeftRight=NULL;
static HCURSOR m_hCursorUpDown=NULL;

class CMultiSpliltWnd:
	public ATL::CWindowImpl< CMultiSpliltWnd, ATL::CWindow, ATL::CControlWinTraits >
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

	BEGIN_MSG_MAP_EX(CMultiSpliltWnd)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MSG_WM_RBUTTONUP(OnRButtonDblClk)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)

		//MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)

#ifndef _WIN32_WCE
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
#endif // !_WIN32_WCE
		COMMAND_ID_HANDLER(ID_ADD_EMPTYWINDOW,OnAddEmptyPane)
		COMMAND_ID_HANDLER(ID_ADD_LEFT,OnAddLeftRightSplit)
		COMMAND_ID_HANDLER(ID_ADD_UP,OnAddUpDownSplit)
		COMMAND_ID_HANDLER(ID_MULTISPLITEDITMODE_CLOSE,OnClosePane)
		COMMAND_ID_HANDLER(ID_MULTISPLITEDITMODE_SHOWTREEVIEW,OnShowTreeView)
		COMMAND_ID_HANDLER(ID_PANE_PLAYLIST,OnAddPlaylistView)
		REFLECT_NOTIFICATIONS()
		END_MSG_MAP()

public:

	CMultiSpliltWnd():m_bFullDrag(true),curHitPos(NULL)
	{
		menu=::LoadMenu(NULL,MAKEINTRESOURCE(IDR_MENU_EDIT_MODE));
		subMenu=::GetSubMenu(menu,0);
		rootTree=NULL;
	}

	MYTREE* CreateRootTree()
	{
		MYTREE *root=new MYTREE(_T("root"));
		root->data.type=left_right;
		root->setroot();

		wcscpy(root->data.nodeName,L"水平分离器");

		return root;
	}

	BOOL OnEraseBkgnd(CDCHandle dc)
	{
		//RECT rc;
		//GetClientRect(&rc);
		//::Rectangle(dc.m_hDC,rc.left,rc.top,rc.right,rc.bottom);
		//AtlTrace(L"OnErase");

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


	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		rootTree=CreateRootTree();


		if(m_hCursorLeftRight == NULL)
			m_hCursorLeftRight = ::LoadCursor(NULL, IDC_SIZEWE);
		if(m_hCursorUpDown == NULL)
			m_hCursorUpDown = ::LoadCursor(NULL, IDC_SIZENS );			

		bHandled = FALSE;
		return 1;
	}


	void UpdateTree(MYTREE *treeData);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		RECT rc;
		GetClientRect(&rc);
		rootTree->setRect(rc);

		if(rootTree->hasChild())
			rootTree->CalcChildsRect();

		MoveToNewRect(rootTree);

		return 1;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		PAINTSTRUCT ps;
		::BeginPaint(m_hWnd,&ps);
		CDCHandle dc(ps.hdc);
		rootTree->DrawSplitterPane(dc);
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

		rclickTree->CalcChildsRect();

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

		rclickTree->CalcChildsRect();


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
			parent->CalcChildsRect();

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

		if(!curClickedTree)
			return 0;

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

					InvalidateRect(&rcErase);
					UpdateWindow();
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
		}
		else		// not dragging, just set cursor
		{
			POINT pt={xPos,yPos};
			MYTREE *curClickedTree=hittest(pt);

			if(curClickedTree->GetOverSplitterBar(xPos,yPos,info))
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

	/*
	bool GetSplitterBarRect(LPRECT lpRect) const
	{
	ATLASSERT(lpRect != NULL);
	if(m_nSinglePane != SPLIT_PANE_NONE || m_xySplitterPos == -1)
	return false;

	if(t_bVertical)
	{
	lpRect->left = m_rcSplitter.left + m_xySplitterPos;
	lpRect->top = m_rcSplitter.top;
	lpRect->right = m_rcSplitter.left + m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge;
	lpRect->bottom = m_rcSplitter.bottom;
	}
	else
	{
	lpRect->left = m_rcSplitter.left;
	lpRect->top = m_rcSplitter.top + m_xySplitterPos;
	lpRect->right = m_rcSplitter.right;
	lpRect->bottom = m_rcSplitter.top + m_xySplitterPos + m_cxySplitBar + m_cxyBarEdge;
	}

	return true;
	}
	*/
};




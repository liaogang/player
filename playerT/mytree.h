#include "MySerialize.h"
#include <list>
#include <map>

#pragma once
#define max_node_name 40
#define SPLITTER_WH 3
#define TREES_ROOT NULL

class MYTREE;
using namespace std;

#define WIDTH(rc) ((rc).right-(rc.left))
#define HEIGHT(rc) ((rc).bottom-(rc.top))

//调用完calcrect之后,调用此函数,更新
void MoveToNewRect(MYTREE *parent);

void MYTREE_Set_Playlist(MYTREE* tree);
void MYTREE_Set_AlbumView(MYTREE* tree);
void MYTREE_Set_LyricView(MYTREE* tree);
void MYTREE_Set_SpectrumView(MYTREE* tree);


void MYTREE_Add_Playlist(MYTREE* tree);
void MYTREE_Add_EmptyWnd(MYTREE* tree);
void MYTREE_Add_AlbumView(MYTREE* tree);
void MYTREE_Add_LyricView(MYTREE* tree);
void MYTREE_Add_SpectrumView(MYTREE* tree);


typedef  void (*CreateWindowFun)(MYTREE* tree);


void RegisterCreateWndFuns(TCHAR* panename,CreateWindowFun func);
void CreateHWNDbyName(MYTREE *tree);






//删除自身,在根树中的关系
//返回父结点
MYTREE * MYTree_RemoveFromRoot(MYTREE *tree);







typedef  void (*DoSomeThing)(MYTREE* tree);
void SetLockedTrue(MYTREE *tree);
void SetLockedFalse(MYTREE *tree);
//遍历树,执行某动作
void WalkOverTree(MYTREE * tree,DoSomeThing func);





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

class dataNode:public SerializeObj
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

		treeItem=0;
	}

	dataNode():hWnd(NULL),m_iSplitterBar(SPLITTER_WH)
	{
		type=left_right;
		rc.left=0;
		rc.right=0;
		rc.top=0;
		rc.bottom=0;

		treeItem=0;
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


	virtual int SerializeB(FILE *pFile);
	virtual int ReSerialize(FILE *pFile);

public:
	//serialize object
	TCHAR nodeName[max_node_name];
	RECT rc;
	split_type type;
	long  m_iSplitterBar;//把手的长度或宽度
	list<RECT> SplitterBarRects;//把手区域列表

	//no ....
	HWND hWnd;	
	
	HTREEITEM treeItem;
	
	//vector<int> bl;//比例
	RECT rcBeforeSize;
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


class MYTREE
	:public SerializeObj
{
public:
	MYTREE(TCHAR *name):next(NULL),prev(NULL),
		child(NULL),data(name),childs(0),bLocked(false)
	{
		//default is not root
		parent=this;
	}

	MYTREE():next(NULL),prev(NULL),
		child(NULL),childs(0),bLocked(false)
	{
		parent=this;

		//auto name
		static int paneNumber=0;
		TCHAR paneN[max_node_name];
		_itow(++paneNumber,paneN,10);
		_tcscpy(data.nodeName,paneN);

		//AtlTrace(L"created : %s\n",paneN);
	}

	virtual int SerializeB(FILE *pFile);
	virtual int ReSerialize(FILE *pFile);

	
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

	RECT getRectBeforeSize(){return data.rcBeforeSize;}
	void setRectBeforeSzie(RECT &rc){data.rcBeforeSize=rc;}

	void AdjustSplitterBar(LONG offset,MY_DIRECTION direction)
	{
		MYTREE *c=getFirstSibling();

// 		for (auto it=parent->data.SplitterBarRects.begin();)
// 		{
// 		}


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
	}


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
	bool hasChild(){return childs!=0;}

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
// 		int i=0;
// 		for (auto *after=getFirstSibling();after;after=after->next)
// 			++i;
// 
// 		return i;

		return isroot()? 1 : parent->childs;
	}

// 	//计算当前之后的兄弟个数,包含自身
// 	int getSiblingNumberBehind()
// 	{
// 		int count=0;
// 		for (auto i=this;i;i=i->next)
// 			++count;
// 
// 		return count;
// 	}

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

		++childs;
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

		++childs;
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
	
	MYTREE *prev;
	MYTREE *next;
	MYTREE *child;//first child item

	int     childs;//child number

	bool    bLocked;//是否锁定.锁定之后,子面板不能移动
	dataNode data;
	//paint stuffs
public:
	void RECTHOffset(RECT &rc ,int Offset )
	{
		rc.left+=Offset;
		rc.right+=Offset;
	}

	void RECTVOffset(RECT &rc,int Offset )
	{
		rc.top+=Offset;
		rc.bottom+=Offset;
	}


	void BeginSize()
	{
		MYTREE *cur=this;
		cur->setRectBeforeSzie(cur->getRect());
		if(cur->hasChild())
		{
			cur=cur->child;
			for (int count=0;count<childs;cur=cur->next,++count)
			{
				cur->setRectBeforeSzie(cur->getRect());
				if(cur->hasChild())
					cur->BeginSize();
			}
		}
	}


	//重新计算子结点的矩形区域
	//按原比例来分配
	void CalcChildsRect(RECT &newRC)
	{
		setRect(newRC);
		if(!hasChild())
			return;
		

		RECT oldRC=getRectBeforeSize();
		
		
		bool bLR=(data.type==left_right);
		float times=bLR?WIDTH(newRC)/(float)WIDTH(oldRC) :
			HEIGHT(newRC)/(float)HEIGHT(oldRC);
		
		data.SplitterBarRects.clear();
		
		MYTREE *cur=child;
		LONG lastV=cur->getRect().left;
		for (int count=0;count<childs;cur=cur->next,++count)
		{
			RECT curRC=cur->getRect();

			oldRC=cur->getRectBeforeSize();
			
			int curW=WIDTH(oldRC) * times;
			
			if(count!=childs-1)
			{
				curRC.left=lastV;
				curRC.right=curRC.left + curW ;
				lastV=curRC.right+1;
				curRC.right-=data.m_iSplitterBar;
			}
			else
			{
				curRC.left=lastV;
				curRC.right=getRect().right;
			}

			cur->setRect(curRC);

			if(count!=childs-1)
			{
				RECT rcBar=curRC;
				rcBar.left=curRC.right;
				rcBar.right+=data.m_iSplitterBar;
				data.SplitterBarRects.push_back(rcBar);
			}


			if (cur->hasChild())
				cur->CalcChildsRect(curRC);
		}
			

	}

	//重新计算子结点的矩形区域
		//重新分配相等的空间
	void ReCalcChildsRect()
	{
		if(!hasChild())return;

		RECT rc=getRect();
		bool bLR=(data.type==left_right);
		//clear all splitter bar
		data.SplitterBarRects.clear();


		MYTREE *first=child;

		if (childs==1)
		{
			child->setRect(rc);

			if (first->hasChild())
				first->ReCalcChildsRect();
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
					first->ReCalcChildsRect();
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
	void DrawSplitterPane(HDC dc,BOOL invalidate=FALSE,BOOL bErase=FALSE)
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
						InvalidateRect(cur->data.hWnd,0,bErase);
				}
			}
		}
	}



	// Overrideables
	void DrawSplitterBar(HDC dc,RECT &rect)
	{
		FillRect(dc,&rect, (HBRUSH)LongToPtr(COLOR_3DFACE+ 1));
		
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

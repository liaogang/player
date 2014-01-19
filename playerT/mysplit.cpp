
#include "mysplit.h"
#include "DlgProgerty.h"
#include "globalStuffs.h"
#include "FunctionReg.h"


void UpdateLayout(MYTREE *parent,HDC dc)
{
	for (MYTREE *cur=parent->getFirstSibling();cur;cur=cur->next)
	{
		if(cur->hasChild())
		{
			UpdateLayout(cur->child);
		}
		else 
		{
			if (IsWindow(cur->data.hWnd))
			{
				RECT rect=cur->getRect();
				::SetWindowPos(cur->data.hWnd, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,SWP_NOZORDER);
				::InvalidateRect(cur->data.hWnd,NULL,TRUE);			
			}
			if(dc!=NULL && cur->wndEmpty())
				cur->DrawEmptyPane(dc);
		}
	}

	if(dc==NULL)
		return;
	for (auto i=parent->data.SplitterBarRects.begin();i!=parent->data.SplitterBarRects.end();++i)
		parent->DrawSplitterBar(dc,*i);
}



void MYTree_DestroyFromRoot(MYTREE *tree)
{
	if (tree->hasChild())
		for (MYTREE *cur=tree->child; cur ; )
		{
			MYTREE *next=cur->next;
			MYTree_DestroyFromRoot(cur);
			cur=next;
		}
	else
		{
			if(IsWindow(tree->data.hWnd))
			{
				ShowWindow(tree->data.hWnd,SW_HIDE);
				DestroyWindow(tree->data.hWnd);
			}

			delete tree;
	}
}


MYTREE * MYTree_RemoveFromRoot(MYTREE *tree)
{
	if(tree->isroot())
		return tree;

	MYTREE *parent=tree->parent;

	//如果当前只有一个兄弟的话
	//删除自身,用兄弟替换父亲
	/*
	if (tree->prev==NULL && tree->next!=NULL && tree->next->next==NULL)
	{
		tree->replaceParentWithBrother(tree->next);
		tree->parent->child=NULL;
	}
	else if (tree->next==NULL && tree->prev!=NULL && tree->prev->prev==NULL)
	{
		tree->replaceParentWithBrother(tree->prev);
		tree->parent->child=NULL;
	}
	else 
	*/
		
	if(tree->prev ==NULL)//第一个?
	{
		tree->parent->child=tree->next;

		if(tree->next)
			tree->next->prev=NULL;
		tree->Destroy();
		--parent->childs;
	}
	else
	{
		tree->prev->next=tree->next;

		if(tree->next)
			tree->next->prev=tree->prev;
		MYTree_DestroyFromRoot(tree);
		--parent->childs;
	}


	return parent;
}


void SetLockedTrue(MYTREE *tree)
{
	tree->bLocked=true;
}

void SetLockedFalse(MYTREE *tree)
{
	tree->bLocked=false;
}

void WalkOverTree(MYTREE * tree,DoSomeThing func)
{
	int siblingNum=tree->getSiblingNumber();

	for (int i=1; i<=siblingNum ; ++i, tree=tree->next)
	{
		func(tree);

		if (tree->hasChild())
			WalkOverTree(tree->child,func);
	}
}




void PaneSizeStore(MYTREE *parent)
{
	if(parent->hasChild())
		parent->PaneChildSizeStore();
}

HCURSOR CMultiSpliltWnd::m_hCursorLeftRight=NULL;
HCURSOR CMultiSpliltWnd::m_hCursorUpDown=NULL;

LRESULT CMultiSpliltWnd::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	rootTree=UISplitterTreeRoot();
	if (rootTree->hasChild())//reserialize from  file
		CreateHWNDbyName(rootTree);


	UpdateLayout(rootTree);

	PaneSizeStore(rootTree);

	if(m_hCursorLeftRight == NULL)
		m_hCursorLeftRight = ::LoadCursor(NULL, IDC_SIZEWE);
	if(m_hCursorUpDown == NULL)
		m_hCursorUpDown = ::LoadCursor(NULL, IDC_SIZENS );			

	SetMsgHandled(FALSE);
	return 1;
}


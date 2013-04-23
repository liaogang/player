
#include "mysplit.h"
#include "ProgertyDlg.h"
#include "globalStuffs.h"



void MoveToNewRect(MYTREE *parent)
{
	for (MYTREE *cur=parent->getFirstSibling();cur;cur=cur->next)
	{
		if(cur->hasChild())
		{
			MoveToNewRect(cur->child);
		}
		else if (IsWindow(cur->data.hWnd))
		{
			RECT rect=cur->getRect();
			::SetWindowPos(cur->data.hWnd, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,SWP_NOZORDER);
		}
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
		delete tree;
		--parent->childs;
	}
	else
	{
		tree->prev->next=tree->next;

		if(tree->next)
			tree->next->prev=tree->prev;
		delete tree;
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
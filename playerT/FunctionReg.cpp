#include "FunctionReg.h"
#include "mytree.h"

#include <map>
using namespace std;

static map<TCHAR*,CreateWindowFun> createWndFuns;
void RegisterCreateWndFuns(TCHAR* panename,CreateWindowFun func)
{
	createWndFuns.insert(map<TCHAR*,CreateWindowFun>::value_type(panename,func));

	//CreateWindowFun fun =createWndFuns.find(panename)->second;
}

void CreateHWNDbyName(MYTREE *tree)
{
	for (;tree;tree=tree->next)
	{	
		for (auto i=createWndFuns.begin();i!=createWndFuns.end();++i)
		{
			if (_tcscmp(i->first,tree->data.nodeName)==0)
			{
				CreateWindowFun fun=i->second;
				fun(tree);
				break;
			}
		}

		if (tree->hasChild())
			CreateHWNDbyName(tree->child);
	}

}





static map<TCHAR*,CreateReBarBandFuns> mapCreateRebarFuns;

void RegisterCreateRebarBandFuns(TCHAR* szBandClassName,CreateReBarBandFuns func)
{
	ATLASSERT(szBandClassName);
	mapCreateRebarFuns.insert(map<TCHAR*,CreateReBarBandFuns>::value_type(szBandClassName,func));
}

HWND CreateRebarBand(TCHAR *szRebarClassName,HWND hWndParent)
{
	for (auto i=mapCreateRebarFuns.begin();i!=mapCreateRebarFuns.end();++i)
	{
		if(_tcscmp(szRebarClassName,i->first)==0)
		{
			CreateReBarBandFuns func=i->second;
			return func(hWndParent);
		}
	}

	return NULL;
}


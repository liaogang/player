#include "FunctionReg.h"
#include "mytree.h"
#include <vector>
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
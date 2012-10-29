#include "stdafx.h"
void ChangeCurDir2ModulePath(HINSTANCE hInstance)
{
	static BOOL modulePathGetted=FALSE;
	static TCHAR moduleFileName[MAX_PATH]={0};
	if (!modulePathGetted)
	{
		ATLASSERT(hInstance);
		GetModuleFileName(hInstance,moduleFileName,MAX_PATH);
		int i;
		for (i=MAX_PATH-1;i>=0 && moduleFileName[i]!='\\';--i);
		moduleFileName[i]='\0';

		modulePathGetted=TRUE;
	}

	_tchdir(moduleFileName);
}
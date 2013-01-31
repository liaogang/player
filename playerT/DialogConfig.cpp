#include "StdAfx.h"
#include "DialogConfig.h"

bool ptInRc(POINT &pt,RECT &rc)
{
	return pt.x>=rc.left && pt.x<rc.right
		&& pt.y>=rc.top && pt.y<rc.bottom;
}

bool ptInRc(int x,int y,RECT &rc)
{
	return x>=rc.left && x<rc.right
		&& y>=rc.top && y<rc.bottom;
}
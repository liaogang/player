#include "StdAfx.h"
#include "LrcMng.h"

LrcMng::LrcMng(void)
{
}

LrcMng::~LrcMng(void)
{
}


  LrcMng* LrcMng::Get()
{
	static LrcMng *p=NULL;
	if (!p)
		p=new LrcMng;
	return p;
}

bool pred(LrcLine fir,LrcLine sec)//litter true,else flase
{
	if (fir-sec<0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void LrcMng::SortLrcLib()
{
	sort(lib.begin(),lib.end(),&pred);
}
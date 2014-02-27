#include "StdAfx.h"
#include "LrcMng.h"

LrcMng::LrcMng(void):offset(0)
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
	return fir.cmp(sec) <0?true:false;
}

void LrcMng::SortLrcLib()
{
	sort(lib.begin(),lib.end(),&pred);
}
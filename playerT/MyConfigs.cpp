#include "MyConfigs.h"


MyConfigs* GetMyConfigs()
{
	static MyConfigs *p=NULL;
	return p==NULL?p=new MyConfigs():p;
}
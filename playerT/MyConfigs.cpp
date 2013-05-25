#include "MyConfigs.h"
MyConfigs* GetMyConfigs()
{
	static MyConfigs *p=NULL;
	return p==NULL?p=new MyConfigs():p;
}



int MyConfigs::SerializeB(FILE *pFile)
{
	int size=0;

	size+=::Serialize(pFile,bResumeOnReboot);
	size+=::Serialize(pFile,playlistIndex);
	size+=::Serialize(pFile,trackIndex);
	size+=::Serialize(pFile,pos);

	size+=::Serialize(pFile,playersVolume);

	return size;
}

int MyConfigs::ReSerialize(FILE *pFile)
{
	int size=0,totalsize;
	size+=::ReSerialize(pFile,&totalsize);

	size+=::ReSerialize(pFile,&bResumeOnReboot);
	size+=::ReSerialize(pFile,&playlistIndex);
	size+=::ReSerialize(pFile,&trackIndex);
	size+=::ReSerialize(pFile,&pos);
	
	size+=::ReSerialize(pFile,&playersVolume);
	
	return size;
}
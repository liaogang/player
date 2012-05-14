#include "stdafx.h"
#include <fstream>
#include <string>
#include <iostream>
#include <ostream>
#include <iosfwd>
#pragma  once
using namespace std;

template <class T> int Serialize (FILE *pFile,T t);
template <> int Serialize (FILE *pFile,std::tstring t);
template <> int Serialize (FILE *pFile,int t);


// template <class T> T ReSerialize (FILE *pFile);
// template <> std::tstring ReSerialize (FILE *pFile);
// template <> int ReSerialize (FILE *pFile);
// template <> uint ReSerialize (FILE *pFile);


class SerializeObj
{
public:
	int Serialize(FILE *pFile)
	{
		int size=0;
		//4 byte ,ռλ
		fwrite(&size,1,4,pFile);
		size+=sizeof(int);

		//size
		size+=SerializeB(pFile);
		
		//write the total size
		fseek(pFile,-size,SEEK_CUR);

		fwrite(&size,1,sizeof(int),pFile);

		//seek back to old
		fseek(pFile,size-sizeof(int),SEEK_CUR);

		return size;
	}

	//some class store
	//figture out is size
	//return the size used
	virtual int SerializeB(FILE *pFile)=0;



	//read size
	//
	virtual int ReSerialize(FILE *pFile)=0;
};
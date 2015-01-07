#pragma  once
#include "stdafx.h"
#include <stdio.h>
using namespace std;


#define ADDTOSERIALIZE(CLASSNAME) \
inline FILE& operator<<(FILE& f,const CLASSNAME &t) \
{return t>>f;} \
inline FILE& operator>>(FILE& f,CLASSNAME &t)\
{return t<<f;}


template<typename T>
class SerializeObj
{
public:
	
	//get from
	FILE& operator<<(FILE& f)
	{
		// must be implemented in a derived class
		ATLASSERT(0);
		return f;
	}


	//save to
	FILE& operator>>(FILE& f) const
	{
		// must be implemented in a derived class
		ATLASSERT(0);
		return f;
	}
};

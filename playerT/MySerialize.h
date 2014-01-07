#include "stdafx.h"
#include <fstream>

#pragma  once
using namespace std;


#define ADDTOSERIALIZE(CLASSNAME) \
FILE& operator<<(FILE& f,CLASSNAME &t) \
{return t>>f;} \
FILE& operator>>(FILE& f,CLASSNAME &t) \
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
	FILE& operator>>(FILE& f)
	{
		// must be implemented in a derived class
		ATLASSERT(0);
		return f;
	}
};
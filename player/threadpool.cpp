#include "stdafx.h"
#include "threadpool.h"

static DWORD CALLBACK ThreadProc(LPVOID lpParameter)
{
	Job *job = (Job*)lpParameter;
	
	job->doJob();

	return 0;
}


/*
bool doJob(Job *job)
{
	HANDLE	hThread = CreateThread(NULL, 0, ThreadProc, (void*)job, 0 , 0);

	return ::WaitForSingleObject(hThread, 10 * 1000) == WAIT_OBJECT_0;
}
*/


void doJob(Job *job)
{
	CreateThread(NULL, 0, ThreadProc, (void*)job, 0, 0);
}



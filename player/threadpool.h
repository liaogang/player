
#include <functional>
#pragma  once

/// Do not use this absolute class
class Job{
public:
	virtual void doJob() = 0;
};


typedef void (*processFunc)(void *arg);
class Job1 : public Job
{
public:
	processFunc process;
	void *arg;
	Job1(){}
	Job1(processFunc f, void *a) :process(f), arg(a)
	{

	}
	
	void doJob()
	{
		process(arg);
	}
};


typedef void (*processFunc2)();
class Job2 :public Job
{
public:
	processFunc2 process;
	void doJob()
	{
		process();
	}
};

void doJob(Job *job);



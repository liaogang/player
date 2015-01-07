
#include "funcCmd.h"
#include "forwardMsg.h"
#include <map>

#ifdef DEBUG
#include <assert.h>
#endif

using namespace std;


map<string, UINT> commandList; 

void registMsgForCommandstring(std::string &commandKey, UINT msg)
{
#ifdef DEBUG
		assert(commandList.find(commandKey) == commandList.end());
		printf("The command '%s' is already registed.", commandKey.c_str());
#endif

	commandList[commandKey] = msg;
}

void performCommand(string command)
{
	UINT msg = commandList[command];
	NotifyMsg(msg,FALSE,0,0);
}

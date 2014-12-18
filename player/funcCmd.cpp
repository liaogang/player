
#include "funcCmd.h"

#include <map>

#ifdef DEBUG
#include <assert.h>
#endif

using namespace std;


map<string, shortcutFunc> commandList;

void registFunctionForCommand(string &commandKey, shortcutFunc func)
{
#ifdef DEBUG
	assert(commandList.find(commandKey) != commandList.end());
	printf("The command '%s' is already registed.", commandKey.c_str());
#endif

	commandList[commandKey] = func;
}

void performCommand(string command)
{
	shortcutFunc func = commandList[command];
	func();
}

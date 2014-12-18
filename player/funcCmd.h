
#include <string>

typedef void(*shortcutFunc)();

void registFunctionForCommand(std::string &commandKey, shortcutFunc func);

void performCommand(std::string command);


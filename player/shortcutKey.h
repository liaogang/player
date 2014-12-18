
#include <string>

void saveFileShortcutKey();

bool verifyLoadFileShortcutKey();

void shortcutKeyPressed(std::string shortcutKey);

///super: windows key or apple key
std::string msgKeytoString(bool ctrl, bool super, bool shift, bool alt, unsigned char vk);

std::string msgKeytoString(bool ctrl, unsigned char vk);


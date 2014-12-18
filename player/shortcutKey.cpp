#include "stdafx.h"
#include "shortcutKey.h"
#include "funcCmd.h"
#include <json/json.h>
#include <iostream>
#include <fstream>
#include "Util1.h"

#pragma once

using namespace std;


Json::Value root;
Json::Value rootLocal ;
Json::Value rootGlobal ;

char filepath[MAX_PATH];
const char *filepath = NULL ;

/// return file loaded.
bool verifyLoadFileShortcutKey()
{
	static bool fileLoaded = false; 
	static bool loaded = false;

	if (loaded == false)
	{
		loaded = true;
	
		const char *mpath = ChangeCurDir2ModulePathA();

		strcpy(filepath,mpath);
		strcat(filepath,"\\keymaps.cfg");


		Json::Reader reader;


		std::filebuf fb;
		if (fb.open( filepath , std::ios::in))
		{		
			fileLoaded = true;
			std::istream is(&fb);
			reader.parse(is, root);

			rootLocal = root["local"];
			rootGlobal = root["Global"];

			fb.close();
		}
		else
		{
			//@todo generate a error message.

		}

	}

	return fileLoaded;
}

void saveFileShortcutKey()
{
	std::filebuf fb;
    if (fb.open ( filepath ,std::ios::out))
    {
        std::ostream out(&fb);
            
        Json::StyledStreamWriter writer;
        writer.write( out , root);
        
        fb.close();
    }
    else
    {
    	//generate a error message.

    }

}


/**
 *	perfrom command if the right shortcut key is pressed.
 *  @return: wether shortcut key is valid.
 */
bool shortcutKeyPressed(string shortcutKey, bool bGlobal)
{
	//shortcutKey to `command`
	verifyLoadFileShortcutKey();


	Json::Value value;
	if (bGlobal)
		value = rootGlobal[shortcutKey];
	else
		value = rootLocal[shortcutKey];

	if (!value.isNull())
	{
		performCommand(value.asString());
		return true;
	}

	return false;
}



///super: windows key or apple key
string msgKeytoString(bool ctrl, bool super, bool shift, bool alt, unsigned char vk)
{
	string r;


	if (ctrl)
		r += ("ctrl+");
	if (super)
		r += ("super+");
	if (shift)
		r += ("shift+");
	if (alt)
		r += ("alt+");

	char c;
	if (vk == VK_OEM_PLUS)
		c = '+';
	//...
	else
	{
		c = vk;
	}

	r += (char)c;

	return r;
}

string msgKeytoString(bool ctrl, unsigned char vk)
{
	return msgKeytoString(ctrl, false, false, false, vk);
}

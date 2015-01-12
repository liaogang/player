//
//  Last.fm.user
//  Last.fm
//
//  Created by liaogang on 15/1/4.
//  Copyright (c) 2015年 liaogang. All rights reserved.
//

#include "Last_fm_user.h"
#include "Last_fm_api.h"
#include "serializeBase.h"


/// load it from cached file if has, else create a new session again.
bool auth(LFUser &user)
{
    const char userProfile[] = "lastFm.cfg";
    
    bool userProfileLoaded = false;
    
    FILE *file = fopen(userProfile, "r");
    if (file)
    {
        fseek(file, 0, SEEK_END );
        if(ftell(file)>sessionKeyLength)
        {
            fseek(file, 0, SEEK_SET);
            
            *file >> user.name >> user.sessionKey;
    
            userProfileLoaded = true;
        }
        fclose(file);
    }
    
    
    if (userProfileLoaded == false)
    {
        string token;
        if(auth_getToken( token ) )
        {
            openWebInstance(token);
            
            while (true)
            {
                if(auth_getSession(token,user.sessionKey ,user.name))
                {
                    userProfileLoaded = true;
                    
                    // save sessionKey and userName to file.
                    
                    FILE *file = fopen(userProfile, "w");
                    if (file)
                    {
                        *file << user.name << user.sessionKey;
                        
                        fclose(file);
                    }
                    
                    
                    
                    break;
                }
                else
                {
                    printf("please press auth in the web broswer after login in or wait a minute\n\n");
                }
#ifdef _WINDOWS
		::Sleep(100000);
#else
                sleep(10);
#endif // _WINDOWS

            }
        }
    }
    
    user.isConnected = userProfileLoaded;
    
    return userProfileLoaded;
}


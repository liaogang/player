//
//  Last.fm.user
//  Last.fm
//
//  Created by liaogang on 15/1/4.
//  Copyright (c) 2015年 liaogang. All rights reserved.
//

#include "Last_fm_api.h"
#include <cstdio> //sprintf
#include <cstring> // strcat
#include <assert.h>
#include <iostream>
#include "md5.h"
#include "time.h"
#include <algorithm>
using namespace std;

const char lastFmPath[]="/2.0/";
const char lastFmHost[] = "ws.audioscrobbler.com" ;
const char lastFmApiKey[] = "6ef0a182fcb172b557c0ca096594f288";
const char lastFmSecret[] = "3b1a4e1e970ed3a30c28cd65bb88579c";
const char lastFmLang[10] ="zh";

/// a param and it's value.
struct paramPair
{
    string param;
    string value;
    paramPair(string p , string v):param(p),value(v)
    {
        
    }
};

enum httpMethod
{
    httpMethod_post,
    httpMethod_get
};

const char *arrHttpMethod[] = {"POST","GET"};

bool cmp(paramPair a,paramPair b)
{
    return strcmp( a.param.c_str() ,b.param.c_str() ) < 0;
}


MemBuffer* lastFmSendRequest(vector<paramPair> arrParamPairs, httpMethod  method , bool mkMd5, bool acceptGzipEncoding , bool useJsonFormat );


bool artist_getInfo(string &artist ,LFArtist &lfArtist)
{
    bool result = false;
    
    vector<paramPair> arrParamPair
    (
     {
         {"artist", artist},
         {"autocorrect","1"},
         {"lang",lastFmLang},
         {"method","artist.getInfo"}
     }
     );
    
    MemBuffer *buffer = lastFmSendRequest(arrParamPair ,httpMethod_get ,false, false  , true );
    
    if (buffer)
    {
               
        //parse it by json.
        Json::Reader reader;
        Json::Value root;
        
        
        reader.parse((const char*)buffer->buffer, (const char*)buffer->buffer+buffer->length , root);
        
        lfArtist = LFArtist( root , true);
        
        result = true;
        
        deleteMemBuffer(buffer);
    }
    
    return result;
}



void track_getInfo(string &artist , string & track)
{
    vector<paramPair> arrParamPair
    (
     {
        {"artist", artist},
        {"autocorrect","1"},
        {"method","track.getInfo"},
        {"lang",lastFmLang},
        {"track", track}
     }
     );
    
   
    MemBuffer *buffer = lastFmSendRequest(arrParamPair , httpMethod_get ,false, false , true);
    
    if (buffer)
    {
        printf("%s\n",buffer->buffer);
        
        //parse it by json.
        Json::Reader reader;
        Json::Value root;
        reader.parse((const char*)buffer->buffer, (const char*)buffer->buffer+buffer->length , root);
        
        LFTrack lfTrack(root["track"]);
        
        deleteMemBuffer(buffer);
    }
}


bool auth_getToken( string &token )
{    vector<paramPair> arrParamPair
    (
     {

        {"method","auth.gettoken"},
     }
     );
 
   
    MemBuffer *buffer = lastFmSendRequest( arrParamPair ,httpMethod_get ,true,false, true);
    
    if (buffer)
    {
        printf("%s\n",buffer->buffer);
        
        //parse it by json.
        Json::Reader reader;
        Json::Value root;
        reader.parse((const char*)buffer->buffer, (const char*)buffer->buffer+buffer->length , root);
        
        token = root["token"].asString();
        
        deleteMemBuffer(buffer);
        
        return true;
    }
    
    return false;
}


void openWebInstance(const string &token)
{
    //    http://www.last.fm/api/auth/?api_key=xxxxxxxxxx&token=yyyyyy
    
    string url="open \"http://www.last.fm/api/auth/?api_key=";
    url+=lastFmApiKey;
    url+="&token=";
    url+=token;
    url+="\"";
    
    system(url.c_str());
}

bool auth_getSession(string &token,string &sessionKey,string &userName)
{    vector<paramPair> arrParamPair
    (
     {
        {"method","auth.getSession"},
        {"token",token}
     }
     );
 
    
    /// this api has a bug , if using json format.
    /// @see http://cn.last.fm/group/Last.fm+Web+Services/forum/21604/_/428269
    
    MemBuffer *buffer = lastFmSendRequest(arrParamPair ,httpMethod_get , true , false , false);
    
    if (buffer)
    {
        printf("%s\n",buffer->buffer);
        
        //parse it by json.
        Json::Reader reader;
        Json::Value root;
        reader.parse((const char*)buffer->buffer, (const char*)buffer->buffer+buffer->length , root);
        
        Json::Value v = root["session"];
        
        sessionKey = v["key"].asString();
        
        userName = v["name"].asString();
        
        deleteMemBuffer(buffer);
        
    }
    
    
    bool sessionCreated = (sessionKey.length() == sessionKeyLength) && userName.length() > 0;
    
    return sessionCreated;
}

bool track_love(string &sessionKey, string &artist , string & track )
{
    bool result = false;
    
    vector<paramPair> arrParamPair
    (
     {
        {"artist", artist},
        {"method","track.love"},
        {"sk", sessionKey},
        {"track", track }
     }
     );
    
    MemBuffer *buffer = lastFmSendRequest(arrParamPair ,httpMethod_post ,true , false , true);
    
    if (buffer)
    {
        printf("%s\n",buffer->buffer);
        
        //parse it by json.
        Json::Reader reader;
        Json::Value root;
        reader.parse((const char*)buffer->buffer, (const char*)buffer->buffer+buffer->length , root);
        
        if(root["status"].asString() == "ok")
        {
            result = true;
        }
        
        deleteMemBuffer(buffer);
    }
    
    return result;
}



bool track_updateNowPlaying(string &sessionKey, string &artist,string &track)
{
    bool result = false;
    
    vector<paramPair> arrParamPair
    (
     {
         {"artist", artist},
         {"method","track.updateNowPlaying"},
         {"sk", sessionKey},
         {"track", track }
     }
     );
    
    MemBuffer *buffer = lastFmSendRequest(arrParamPair ,httpMethod_post ,true , false , true);
    
    if (buffer)
    {
        printf("%s\n",buffer->buffer);
        
        result = true;
        deleteMemBuffer(buffer);
    }
    
    
    return result;
}


bool track_scrobble(string &sessionKey, vector<string> &artists,vector<string> &tracks,vector<string> timestamps)
{
    bool result = false;
    
    time_t t;
    time(&t);
    
    char tmp[40]={0};
    sprintf(tmp, "%ld",t);
    
    vector<paramPair> arrParamPair
    ({
         {"artist", artists[0]},
         {"method","track.scrobble"},
         {"sk", sessionKey},
         {"timestamp", tmp },
         {"track", tracks[0] }
     });

    
    
    MemBuffer *buffer = lastFmSendRequest(arrParamPair ,httpMethod_post ,true , false , true);
    
    if (buffer)
    {
        printf("%s\n",buffer->buffer);
       
        Json::Reader reader;
        Json::Value root;
        reader.parse((const char*)buffer->buffer, (const char*)buffer->buffer+buffer->length , root);
        
        
        result = true;
        deleteMemBuffer(buffer);
    }
    
    
    return result;
}


bool user_getRecentTracks(const string &username , vector<LFTrack> &tracks)
{
    bool result = false;
    
    vector<paramPair> arrParamPair
    ({
        {"extended","1"},
        {"limit","200"},
        {"method","user.getRecentTracks"},
        {"user", username}
    });
    
    MemBuffer *buffer = lastFmSendRequest(arrParamPair ,httpMethod_get , false, false, true);
    
    if (buffer)
    {
        printf("%s\n",buffer->buffer);
        
        Json::Reader reader;
        Json::Value root;
        reader.parse((const char*)buffer->buffer, (const char*)buffer->buffer+buffer->length , root);
        
        Json::Value arr = root["recenttracks"]["track"];
        
        int arrLength = arr.size();
        
        for (int i= 0; i < arrLength; i++) {
            Json::Value v = arr[i];
            LFTrack t(v);
            
            tracks.push_back( t );
        }
        
        
        result = true;
        deleteMemBuffer(buffer);
    }
    
    
    return result;
}




string utf8code(string &str)
{
    unsigned char buffer2[125]={0};
    
    size_t length = str.length();
    int ii=0;
    for ( size_t i = 0; i < length; i++)
    {
        unsigned char a = str[i];
        
        if (isalnum(a) || ispunct(a) )
        {
            buffer2[ii++]=a;
        }
        else
        {
            buffer2[ii++]='%';
            sprintf((char*)buffer2+ii,"%X",a);
            ii+=2;
        }
    }
    
    return string((const char*)buffer2,(size_t)strlen((const char*)buffer2));
}

/** return server's response content if have. else nullptr is returned.
 */
MemBuffer* lastFmSendRequest(vector<paramPair> arrParamPairs, httpMethod  method , bool mkMd5, bool acceptGzipEncoding , bool useJsonFormat )
{
    size_t numParamPairs = arrParamPairs.size();
    
    assert(numParamPairs>=1);
    
    string strParams;
    
    arrParamPairs.insert(arrParamPairs.begin(), {"api_key",lastFmApiKey});
    numParamPairs++;
    
    sort(arrParamPairs.begin(), arrParamPairs.end(), cmp);
    
    if (mkMd5)
    {
        string strMD5;
        for( int i = 0; i< numParamPairs; i++)
        {
            paramPair pPP = arrParamPairs[i];
            
            strMD5+=pPP.param;
            strMD5+=pPP.value;
        }
        
        strMD5+= lastFmSecret;
        
        arrParamPairs.push_back({"api_sig",md5(strMD5)});
        numParamPairs++;
        sort(arrParamPairs.begin(), arrParamPairs.end(), cmp);
    }
    
    
    arrParamPairs.push_back( {"format","json"} );
    numParamPairs++;
    sort(arrParamPairs.begin(), arrParamPairs.end(), cmp);
    

    for( int i = 0; i< numParamPairs; i++)
    {
        paramPair pPP = arrParamPairs[i];
        
        if(i!=0)
            strParams+='&';
        
        strParams+=pPP.param;
        strParams+='=';
        /// convert param value from utf8 to utf8 code (eaaf --> %ea%af)
        strParams+=utf8code( pPP.value );
    }
    
    
    const int senderHeaderLenMax = 2048;
    
    unsigned char senderHeader[senderHeaderLenMax];
const char senderHeaderFormatter[] =
"%s %s?%s HTTP/1.1\r\n\
Connection: Keep-Alive\r\n\
%s\
Accept-Language: zh-CN,en,*\r\n\
Host: %s\r\n\
\r\n";
    
    const char acceptEncoding[] = "Accept-Encoding: gzip,*\r\n";
    
    sprintf( (char*) senderHeader, senderHeaderFormatter ,arrHttpMethod[method]  , lastFmPath , strParams.c_str() , acceptGzipEncoding?acceptEncoding:"" ,  lastFmHost );
    
    size_t senderHeaderLen = strlen((char*)senderHeader);
    
    printf("%s\n",senderHeader);
    
    int socketClient;
    if(CreateTcpSocketClient(lastFmHost  ,(SOCKET*)&socketClient) )
    {
        if(  sendDataToSocket(socketClient, senderHeader, senderHeaderLen) == senderHeaderLen )
        {
            MemBuffer *socketBuf = recvSocketData(socketClient);
            closesocket(socketClient);
            return socketBuf;
        }
        
    }
    
    return nullptr;
}



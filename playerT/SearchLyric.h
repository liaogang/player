#define  HTTP_PORT 80
#include <winsock2.h> 
#pragma comment(lib,"WS2_32.lib") 
using namespace std;
/*千千静听歌词搜索
	网通服务器 
	125.39.78.11
	ttlrccnc.qianqian.com 
	
电信服务器
220.181.5.39
lrcct2.ttplayer.com 
ttlrcct.qianqian.com
ttlrcct2.qianqian.com
*/

const char StrServiceDianXin[]="ttlrcct2.qianqian.com";
const char  StrServiceWangTong[]="ttlrccnc.qianqian.com";
const char  StrServiceDownload[]="ttlrcct2.qianqian.com";
//ttlrcct.qianqian.com




class SearchLyric
{
public:
	//search lyric
	SOCKET outS;
	SOCKADDR_IN sAddr;
	INT socketInit;

	string strRecv;
	enum
	{
		SOCKET_NOT_INIT=-1,
		SOCKET_INIT_FAIL,
		SOCKET_INIT_SUCCESS
	};

	//download
	SOCKET socketDownload;
	SOCKADDR_IN addrDownload;
	INT downloadSocketInit;




	//id artist title ,...
	vector<string> vecLrcLines;


	SearchLyric():socketInit(SOCKET_NOT_INIT),downloadSocketInit(FALSE)
	{

	}


	INT InitDownloadSocket()
	{
		if(downloadSocketInit)return 1;

		MakeSureSocketStartUp();



		//in socket
		memset((void*)&addrDownload,0,sizeof(addrDownload));
		addrDownload.sin_family=AF_INET;
		addrDownload.sin_port=htons(HTTP_PORT);


		HOSTENT *host=::gethostbyname(StrServiceDownload);
		if (!host)
		{
			return downloadSocketInit=0;
		}
		else
		{
			if(host->h_addrtype == AF_INET)
				addrDownload.sin_addr.s_addr=*(u_long *) host->h_addr_list[0];
		}


		socketDownload=socket(AF_INET,SOCK_STREAM,0);
		if (socketDownload==INVALID_SOCKET )
		{
			downloadSocketInit=0;
			DWORD error=GetLastError();
		}

		return downloadSocketInit;
	}

	INT Init()
	{
		MakeSureSocketStartUp();
		socketInit=SOCKET_INIT_FAIL;


		//in socket
		memset((void*)&sAddr,0,sizeof(sAddr));
		sAddr.sin_family=AF_INET;
		sAddr.sin_port=htons(HTTP_PORT);


		HOSTENT *host=::gethostbyname(StrServiceDianXin);
		if(host && host->h_addrtype == AF_INET)
		{
			sAddr.sin_addr.s_addr=*(u_long *) host->h_addr_list[0];
		}
		else
		{
			goto r;
		}
		

		outS=socket(AF_INET,SOCK_STREAM,0);
		if (outS ==INVALID_SOCKET )
		{
			goto r;
		}

		return TRUE;

r:
		return socketInit;
	}

	BOOL MakeSureSocketStartUp()
	{
		static BOOL bInit=FALSE;

		if (!bInit)
		{
			WSADATA data;
			int error;
			error=WSAStartup(MAKEWORD(2,2),&data);
			if (error)
				bInit=FALSE;
			else
				bInit=TRUE;
		}
		return bInit;
	}

	//发送的是artist和title字符小写形式
	//的UNICODE编码
		BOOL Search(const WCHAR *artist,const WCHAR *title)
		{
			if (socketInit!=SOCKET_INIT_SUCCESS)
			{
				outS=socket(AF_INET,SOCK_STREAM,0);
				if (!outS)
				{
					socketInit=FALSE;
					return FALSE;
				}
				socketInit=TRUE;
			}

			BOOL connected=FALSE;
			int error=::connect(outS,(sockaddr*)&sAddr,sizeof(sAddr));
			if(error==SOCKET_ERROR)
			{
				DWORD errorNumber=GetLastError();
				if (errorNumber==10056)
				{
				}
				else
				{
					return FALSE;
				}
			}
			else
			{
				connected=TRUE;
			}
			
static	const char *header=
"GET /dll/lyricsvr.dll?sh?Artist=%s&Title=%s&Flags=2&ci=525b525b56454d7178646432492b345316101c0265631a5d5157535e55424c671c464808425c5353011d01 HTTP/1.1\r\n\
Host: %s\r\n\
\r\n\0\0";

			string codeAr=str2UnicodeCode(artist,_tcslen(artist));
			string codeTi=str2UnicodeCode(title,_tcslen(title));
			int strLen=strlen(header)-6 + codeAr.length()+codeTi.length() + strlen(StrServiceDianXin);
			char *sendStr=new char[strLen];
			sprintf(sendStr,header,codeAr.c_str(),codeTi.c_str(),StrServiceDianXin);

			//send data
			int send=0,totalsend=0;
			for (;(send=::send(outS,sendStr+totalsend,strLen-totalsend,0))>0;totalsend+=send);
			
			//recv data
			char *buf;
			const int RECV_BUF_LEN =200;
			buf=(char*)malloc(RECV_BUF_LEN);
			memset(buf,0,RECV_BUF_LEN);
			strRecv.clear();

			int recv=0,totalRecv=0;
			while((recv=::recv(outS,buf,RECV_BUF_LEN,0)) >0)
			{
				string tmp(buf,RECV_BUF_LEN);

				totalRecv+=recv;

				strRecv+=tmp;
				memset(buf,0,RECV_BUF_LEN);
			}

			free(buf);
			closesocket(outS);

			return 1;
		}

		BOOL ParseResult()
		{
			UINT index=strRecv.find("<result>");
			if(index>= strRecv.npos)
				return FALSE;
			
			strRecv.erase(0,index);
			index=0;

			vecLrcLines.clear();

			while(1)
			{
				index=strRecv.find("<lrc",index);
				UINT last=strRecv.find("</lrc>",index);
				if(index == 0 || last == string::npos)
				{
					break;
				}
				else
				{
					int findEnd;
					static const char * strToFind[]={"id=\"","artist=\"","title=\""};
					string r[3];
					for (int i=0;i<3;++i)
					{
						index=strRecv.find(strToFind[i],index);
						index+=strlen(strToFind[i]);
						findEnd=strRecv.find("\"",index);
						r[i]=strRecv.substr(index,findEnd-index);
						index=findEnd;

						vecLrcLines.push_back(r[i]);
					}
				}


			}//while(1)

			return vecLrcLines.size();
		}

		BOOL SaveLyricToFile(WCHAR *filepath)
		{
			BOOL ret=FALSE;
			const char find[]={"Server: apache"};
			const char ign[]={"\r\n\t "};

			int idx=strRecv.find(find,0);
			idx+=sizeof(find);
			strRecv.erase(0,idx);
			
			idx=strRecv.find_first_not_of(ign,0);
			strRecv.erase(0,idx);

			idx=strRecv.find_last_not_of(ign);
			if(idx!=strRecv.npos)
				strRecv.erase(idx+1);

			FILE * pFile;
			pFile = _tfopen( filepath, _T("wb") );
			if (pFile)
			{
				fwrite(strRecv.c_str(),sizeof(char),strRecv.length(),pFile);
				fclose(pFile);
				ret=TRUE;
			}

			return ret;
		}


	public:
		BOOL Download(int idx,WCHAR *savepath)
		{
			return DownloadLyric((char*)vecLrcLines[idx*3].c_str(),
				(char*)vecLrcLines[idx*3+1].c_str(),
				(char*)vecLrcLines[idx*3+2].c_str())
				?SaveLyricToFile(savepath):FALSE;
		}


	private:
		BOOL DownloadLyric(char *id,char *ar,char*ti)
		{
			InitDownloadSocket();

			socketDownload=socket(AF_INET,SOCK_STREAM,0);
			if (socketDownload==INVALID_SOCKET)
				return FALSE;
			
			if(SOCKET_ERROR==::connect(socketDownload,(sockaddr*)&addrDownload,sizeof(addrDownload)))
			{
				if (GetLastError()!=10056)
					return FALSE;
			}


			string code;
			CreateQianQianCode(id,ar,ti,code);

			static const  char *header=
"GET /dll/lyricsvr.dll?dl?Id=%s&Code=%s HTTP/1.1\r\n\
Host: %s\r\n\
Connection: keep-alive\r\n\
\r\n\0\0";

			int strLen=strlen(header)-6 + strlen(id) + code.length() + sizeof(StrServiceDownload);
			char *sendStr=new char[strLen];
			sprintf(sendStr,header,id,code.c_str(),StrServiceDownload);

			//send data
			int send=0,totalsend=0;
			for (;(send=::send(socketDownload,sendStr+totalsend,strLen-totalsend,0))>0;totalsend+=send);

			//recv data
			char *buf;
			const int RECV_BUF_LEN =300;
			buf=(char*)malloc(RECV_BUF_LEN);
			strRecv.clear();


			int recv=0;
			while((recv=::recv(socketDownload,buf,RECV_BUF_LEN,0)) >0)
			{
				string tmp(buf,buf+recv);
				strRecv+=tmp;
			}

			free(buf);

			return TRUE;
		}
};
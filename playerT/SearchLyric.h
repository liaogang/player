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
		socketInit=SOCKET_INIT_SUCCESS;


		//in socket
		memset((void*)&sAddr,0,sizeof(sAddr));
		sAddr.sin_family=AF_INET;
		sAddr.sin_port=htons(HTTP_PORT);


		HOSTENT *host=::gethostbyname(StrServiceDianXin);
		if (!host)
		{socketInit=FALSE;goto r;}
		else
		{
			if(host->h_addrtype == AF_INET)
			{
				sAddr.sin_addr.s_addr=*(u_long *) host->h_addr_list[0];
			}
		}


		outS=socket(AF_INET,SOCK_STREAM,0);
		if (outS ==INVALID_SOCKET )
		{
			socketInit=SOCKET_INIT_FAIL;
			DWORD error=GetLastError();
		}


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
				//WSAGetLastError();
				bInit=FALSE;
			else
				bInit=TRUE;

		}

		return bInit;
	}


	WCHAR* unicodeCode2Str(string &code)
	{
		char *a=(char*)code.c_str();
// 		for (int i=0;i<code.length();i+=2)
// 			swap(a[i+0],a[i+1]);
		
		WCHAR* w=UTF82Unicode(a);
		return w;
	}

	string str2UnicodeCode(const WCHAR *c,int len)
	{
		string tmp;
		
		for (int i=0;i<len;++i)
		{
			static const WCHAR ign[]=L" ,./<>?`~!@#$%^&*()-_=+\\|[]{};':\"";
			if(_tcschr(ign,c[i])!=0)
				continue;

			char t[10]={0};
			_itoa((int)c[i],t,16);

			bool bAnsi=false;
			if (t[2]=='\0')
			{
				t[2]='0';
				bAnsi=true;
			}
			if (t[3]=='\0')
			{
				t[3]='0';
				bAnsi=true;
			}
			
			
			string ttmp(t,t+2);
			string tttmp1(t+2,t+4);
			if (bAnsi)
			{
				tmp+=ttmp;
				tmp+=tttmp1;
			}else{
			tmp+=tttmp1;
			tmp+=ttmp;
			}
		}

		return tmp;
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
			



// 			WCHAR *ar=(WCHAR*)malloc(sizeof(WCHAR)*(wcslen(artist)+1));
// 			ar[sizeof(WCHAR)*wcslen(artist)]='\0';
// 			_tcscpy(ar,artist);
// 			_tcslwr(ar);
// 			WCHAR *ti=(WCHAR*)malloc(sizeof(WCHAR)*(wcslen(title)+1));
// 			ti[sizeof(WCHAR)*wcslen(title)]='\0';
// 			_tcscpy(ti,title);
// 			_tcslwr(ti);


// 			"GET /dll/lyricsvr.dll?sh?Artist=%s&Title=%s&Flags=2&ci=525b525b56454d7178646432492b345316101c0265631a5d5157535e55424c671c464808425c5353011d01 HTTP/1.1\r\n\
// 			Host: %s\r\n\
// 			User-Agent: Mozilla/5.0 (Windows NT 5.1; rv:13.0) Gecko/20100101 Firefox/13.0.1\r\n\
// 			Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n\
// 			Accept-Language: en-us,en;q=0.5\r\n\
// 			Accept-Encoding: deflate\r\n\
// 			Connection: keep-alive\r\n\
// 			\r\n\0\0

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

		void ParseResult()
		{
			int index=strRecv.find("<result>");
			strRecv.erase(0,index);

			while(1)
			{
				index=strRecv.find("<lrc");
				int last=strRecv.find("</lrc>");
				if(index>=0 && last <= string::npos)
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
					}


					
					//WCHAR *w1= unicodeCode2Str(r[1]);
					//WCHAR *w2= unicodeCode2Str(r[2]);

					
					WCHAR ttt[]=L"胡彦斌";
					LPSTR tt= Unicode2UTF8(ttt);
					

					WCHAR ttt1[]=L"男人KTV";
					LPSTR tt1= Unicode2UTF8(ttt1);

					DownloadLyric("412996",tt,tt1);
					//DownloadLyric((char*)(r[0].c_str()),(char*)(r[1].c_str()),(char*)(r[2].c_str()));
					//delete[] w1;
					//delete[] w2;
					

					SaveLyricToFile(L"c:\\a.lrc");
				}
				else
					break;
			}


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
			idx+=sizeof(ign);
			strRecv.erase(0,idx);

			FILE * pFile;
			int filesize;
			pFile = _tfopen( filepath, _T("wb") );
			if (pFile)
			{
				fwrite(strRecv.c_str(),sizeof(char),strRecv.length(),pFile);
				fclose(pFile);
				ret=TRUE;
			}

			return ret;
		}

		long Conv(int i) 
		{
			long r = i % 4294967296;
			if (i >= 0 && r > 2147483648)
				r -= 4294967296;

			if (i < 0 && r < 2147483648)
				r += 4294967296;
			return r;
		}

		string UTF8toQianQianHexStr(string &arti)
		{
			CONST char *cc=arti.c_str();

			string str;
			for (int i=0;i<arti.length();++i)
			{
				char w=arti[i];
				int k=((int)w) & 0x000000FF ;

				char wt[8]={0};
				_itoa(k,wt,16);
				str+=wt; 
			}
			
			return str;
		}

		void CreateQianQianCode(char *id,char *ar,char*ti,string &code)
		{
			int iId=atoi(id);
			std::string arti(ar);
			arti+=ti;

			int length=arti.length();
			int *song=new int[length];
			for (int i=0;i<length;++i)
			{
				char *ak=(char*)(arti.c_str()+i);
				char  akk=*ak;
				song[i]=((int)akk) & 0x000000FF;			}	
			int t1=0,t2=0,t3=0;

			t1 = (iId & 0x0000FF00) >> 8;

			if ((iId & 0x00FF0000) == 0)
				t3 = 0x000000FF & ~t1;
			else
				t3 = 0x000000FF & ((iId & 0x00FF0000) >> 16);

			t3 |= (0x000000FF & iId) << 8;
			t3 <<= 8;
			t3 |= 0x000000FF & t1;
			t3 <<= 8;

			if ((iId & 0xFF000000) == 0)
				t3 |= 0x000000FF & (~iId);
			else
				t3 |= 0x000000FF & (iId >> 24);

			int j = length - 1;
			while (j >= 0)
			{
				int c = song[j];
				if (c >= 0x80) c = c - 0x100;

				t1 = (int)((c + t2) & 0x00000000FFFFFFFF);
				t2 = (int)((t2 << (j % 2 + 4)) & 0x00000000FFFFFFFF);
				t2 = (int)((t1 + t2) & 0x00000000FFFFFFFF);
				j -= 1;
			}


			j = 0;
			t1 = 0;
			while (j <= length - 1) 
			{
				int c = song[j];
				if (c >= 128) c = c - 256;
				int t4 = (int)((c + t1) & 0x00000000FFFFFFFF);
				t1 = (int)((t1 << (j % 2 + 3)) & 0x00000000FFFFFFFF);
				t1 = (int)((t1 + t4) & 0x00000000FFFFFFFF);
				j += 1;
			}

			int t5 = (int)Conv(t2 ^ t3);
			t5 = (int)Conv(t5 + (t1 | iId));
			t5 = (int)Conv(t5 * (t1 | t3));
			t5 = (int)Conv(t5 * (t2 ^ iId));


			long t6 = (long)t5;
			if (t6 > 2147483648)
				t5 = (int)(t6 - 4294967296);
			
			char tmp[20]={0};
			_itoa(t5,tmp,10);
			code=tmp;
		}



		//char * id="70437";
		//wchar * artist= L"胡彦斌";
		//wchar * title= L"男人 KTV";
		BOOL DownloadLyric(char *id,char *ar,char*ti)
		{
			InitDownloadSocket();

			socketDownload=socket(AF_INET,SOCK_STREAM,0);
			if (socketDownload==INVALID_SOCKET)
			{
				return FALSE;
			}
			
			BOOL connected=FALSE;
			int error=::connect(socketDownload,(sockaddr*)&addrDownload,sizeof(addrDownload));
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

//"GET /dll/lyricsvr.dll?dl?Id=%s&Code=%s&ci=525b525b56454d7178646432492b345316101c0265631a5d5157535e55424c671c464808425c5353011d01 HTTP/1.1\r\n

// 			"GET /dll/lyricsvr.dll?dl?Id=%s&Code=%s HTTP/1.1\r\n\
// 			Host: %s\r\n\
// 			User-Agent: Mozilla/5.0 (Windows NT 5.1; rv:13.0) Gecko/20100101 Firefox/13.0.1\r\n\
// 			Accept: image/gif, image/x-xbitmap, image/jpg, image/pjpeg, text/html, text/xml, */*\r\n\
// 			Connection: keep-alive\r\n\
// 			\r\n\0\0			



			static const  char *header=
"GET /dll/lyricsvr.dll?dl?Id=%s&Code=%s HTTP/1.1\r\n\
Host: %s\r\n\
Connection: keep-alive\r\n\
\r\n\0\0";


			string code;
			CreateQianQianCode(id,ar,ti,code);


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

			int recv=0,totalRecv=0;
			while((recv=::recv(socketDownload,buf,RECV_BUF_LEN,0)) >0)
			{
				string tmp(buf,buf+recv);
				
				totalRecv+=recv;

				strRecv+=tmp;
			}

			free(buf);

		}



};
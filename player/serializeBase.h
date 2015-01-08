
#include <stdio.h>
#include <string>

using namespace std;


FILE& operator<<(FILE& f, const double t);
FILE& operator>>(FILE& f, double& t);

FILE& operator<<(FILE& f, const int t);
FILE& operator>>(FILE& f, int& t);


FILE& operator<<(FILE& f, const UINT t);
FILE& operator>>(FILE& f, UINT& t);

FILE& operator<<(FILE& f, const long t);
FILE& operator>>(FILE& f, long& t);



FILE& operator<<(FILE& f, const TCHAR * str);
FILE& operator>>(FILE& f, TCHAR * str);
FILE& operator>>(FILE& f, char* str);
FILE& operator<<(FILE& f, const char* str);

FILE& operator<<(FILE& f, const tstring &str);
FILE& operator>>(FILE& f, tstring &str);
FILE& operator>>(FILE& f, string &str);
FILE& operator<<(FILE& f, const string &str);


FILE& operator<<(FILE& f, const RECT &r);
FILE& operator>>(FILE& f, RECT &r);
FILE& operator<<(FILE& f, const POINT &p);
FILE& operator>>(FILE& f, POINT &p);
FILE& operator<<(FILE& f, const DWORD t);
FILE& operator>>(FILE& f, DWORD& t);




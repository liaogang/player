#include "wtypes.h"


#ifdef _UNICODE
#define tstring   wstring
#define tfstream  wfstream
#define tiostream wiostream
#define tifstream wifstream
#define tofstream wofstream
#define tfilebuf wfilebuf
#else
#define tstring   string
#define tfstream  fstream
#define tiostream iostream
#define tifstream ifstream
#define tofstream ofstream
#define tfilebuf filebuf
#endif
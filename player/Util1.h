#include "stdafx.h"
//change the current directory to where module file at
const TCHAR* ChangeCurDir2ModulePath(HINSTANCE hInstance=NULL);

const char* ChangeCurDir2ModulePathA(HINSTANCE hInstance=NULL);

const TCHAR* ChangeCurDir2PlaylistPath(bool bCreate=false);

///can not get a directory's last write time.
BOOL GetFileLastWriteTime(LPCTSTR filePath, FILETIME &lastWriteTime);



class GlassWnd
	:public CWindowImpl<GlassWnd>
{
public:

	BEGIN_MSG_MAP_EX(GlassWnd)
		MSG_WM_PAINT(OnPaint)
	END_MSG_MAP()


	
	void OnPaint(CDCHandle dc)
	{
		PAINTSTRUCT ps;
		::BeginPaint(m_hWnd,&ps);
		RECT rc;
		GetClientRect(&rc);
		::Rectangle(ps.hdc,rc.left,rc.top,rc.right,rc.bottom);
		::EndPaint(m_hWnd,&ps);	
	}

	void foo()
	{
		RECT rc;
		GetClientRect(&rc);

		InvalidateRect(&rc,1);

		COLORREF cl=RGB(0,78,152);

		SetWindowLong(GWL_EXSTYLE,GetWindowLong(GWL_EXSTYLE)|WS_EX_LAYERED);
		
		typedef BOOL (WINAPI *FSetLayeredWindowAttributes)(HWND,COLORREF,BYTE,DWORD);

		FSetLayeredWindowAttributes SetLayeredWindowAttributes;

		HINSTANCE hInst = LoadLibrary(_T("User32.DLL"));

		SetLayeredWindowAttributes = (FSetLayeredWindowAttributes)GetProcAddress(hInst,"SetLayeredWindowAttributes");

		if (SetLayeredWindowAttributes)
			SetLayeredWindowAttributes(m_hWnd,cl,20,LWA_COLORKEY);//|LWA_ALPHA

		FreeLibrary(hInst);
		

		
	}
};

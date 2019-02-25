//********************************************************************************
//新加+++++
//Dlg2_1相关？？？？？？？？？？？？？？？？？


#ifndef _createwnd_H_
#define _createwnd_H_

#ifndef _createwnd_
#define _createwnd_ extern "C" __declspec(dllimport)
#endif



//定义Wnd类
static class Wnd
{
public:
	BOOL create(HWND hwnd, LPCREATESTRUCT lpcs);
	void size(HWND hwnd, UINT state, int cx, int cy);
	void hscroll(HWND hwnd, HWND hctrl, UINT code, int pos);
	void vscroll(HWND hwnd, HWND hctrl, UINT code, int pos);
	void command(HWND hwnd, int id, HWND hctrl, unsigned int code_notify);
	void paint(HWND hwnd);
	void close(HWND hwnd);

private:
	SCROLLINFO si;
	int pos_x;
	int pos_y;
};

BOOL Wnd::create(HWND hwnd, LPCREATESTRUCT lpcs)
{
	// 	//////////////////////////////////////////////////////////////////////////
	// make sure no duplicated program running at same time.
	unique_mutex = CreateMutex(NULL, FALSE, "stest_8");
	DWORD err_code = GetLastError();
	if (err_code == ERROR_ALREADY_EXISTS){
		Sleep(2000);
		CloseHandle(unique_mutex);
		unique_mutex = CreateMutex(NULL, FALSE, "stest_8");
		err_code = GetLastError();
		if (err_code == ERROR_ALREADY_EXISTS){
			MessageBox(hwnd, "stest_8 is running!", "Warning", 0);
			CloseHandle(unique_mutex);
			PostQuitMessage(0);
		}
	}
	return TRUE;
}

void Wnd::size(HWND hwnd, UINT state, int cx, int cy)
{
	char path[MAX_PATH] = {0};
	//SCROLLINFO设定方法

}

void Wnd::hscroll(HWND hwnd, HWND hctrl, UINT code, int pos)
{
	si.cbSize = sizeof(si);
	si.fMask  = SIF_ALL;
	GetScrollInfo(hwnd, SB_HORZ, &si);

	switch (code)
	{
	case SB_LEFT:		si.nPos  = si.nMin;		 break;
	case SB_RIGHT:		si.nPos  = si.nMax;		 break;
	case SB_LINELEFT:	si.nPos -= 1;			 break;
	case SB_LINERIGHT:	si.nPos += 1;			 break;
	case SB_PAGELEFT:	si.nPos -= si.nPage;	 break;
	case SB_PAGERIGHT:	si.nPos += si.nPage;	 break;
	case SB_THUMBTRACK:	si.nPos  = si.nTrackPos; break;
	default:	break;
	}

	si.fMask = SIF_POS;		
	SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
	GetScrollInfo(hwnd, SB_HORZ, &si);

}

void Wnd::vscroll(HWND hwnd, HWND hctrl, UINT code, int pos)
{
	si.cbSize = sizeof(si);
	si.fMask  = SIF_ALL;
	GetScrollInfo(hwnd, SB_VERT, &si);

	switch (code) 
	{
	case SB_TOP:		si.nPos = si.nMin;		break;
	case SB_BOTTOM:		si.nPos = si.nMax;		break;
	case SB_LINEUP:		si.nPos -= 1;			break;
	case SB_LINEDOWN:	si.nPos += 1;			break;
	case SB_PAGEUP:		si.nPos -= si.nPage;	break;
	case SB_PAGEDOWN:	si.nPos += si.nPage;	break;
	case SB_THUMBTRACK:	si.nPos = si.nTrackPos;	break;
	default: break;
	}
	si.fMask = SIF_POS;
	SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
	GetScrollInfo(hwnd, SB_VERT, &si);
}

void Wnd::paint(HWND hwnd)
{
	int flag = 0;
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	si.cbSize = sizeof(si);
	si.fMask  = SIF_POS;
	GetScrollInfo(hwnd, SB_HORZ, &si);
	GetScrollInfo(hwnd, SB_VERT, &si);

	HDC memdc = CreateCompatibleDC(hdc);
	BitBlt(hdc, 10, 10, 328, 60, memdc, 0, 0, SRCCOPY);

	DeleteDC(memdc);
	EndPaint(hwnd, &ps);
}

void Wnd::close(HWND hwnd)
{

	PostQuitMessage(0);
}


Wnd wnd;
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hwnd, WM_CREATE,    wnd.create			);
		HANDLE_MSG(hwnd, WM_SIZE,      wnd.size				);
		HANDLE_MSG(hwnd, WM_HSCROLL,   wnd.hscroll			);
		HANDLE_MSG(hwnd, WM_VSCROLL,   wnd.vscroll			);
		HANDLE_MSG(hwnd, WM_PAINT,     wnd.paint			);
		HANDLE_MSG(hwnd, WM_CLOSE,     wnd.close			);


	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
#endif
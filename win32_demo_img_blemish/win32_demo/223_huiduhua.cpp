#include <Windows.h>
#include <stdio.h>
#include <direct.h>
#include "s_win.h"
#include "s_img.h"
#include "resource.h"
#include "tuxiang.h"


#pragma comment(lib, "s_win.lib")
#pragma comment(lib, "s_img.lib")

using namespace ai_win;

extern HMENU hmenu;



class Dlg2_23
{
public:
	virtual BOOL init(HWND hdlg, HWND hfocus, LPARAM linit);
	virtual void command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify);
	virtual void hscroll(HWND hdlg, HWND hctrl, UINT code, int pos);
	virtual void size(HWND hwnd, unsigned int state, int width, int height);
	virtual void close(HWND hdlg);
	virtual LRESULT ctlcolor_listbox(HWND hdlg, HDC hdc, HWND hcolor, int mark/* =CTLCOLOR_EDIT */);

private:
	HBRUSH mybrush;
};
Dlg2_23 dlg2_23;

BOOL CALLBACK menuDlg_2_23(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_DLGMSG(hdlg, WM_INITDIALOG,	    dlg2_23.init);
		HANDLE_DLGMSG(hdlg, WM_COMMAND,		    dlg2_23.command);
		HANDLE_DLGMSG(hdlg, WM_HSCROLL,			dlg2_23.hscroll );
		HANDLE_DLGMSG(hdlg, WM_SIZE,		    dlg2_23.size);
		HANDLE_DLGMSG(hdlg, WM_CLOSE,		    dlg2_23.close);
		HANDLE_DLGMSG(hdlg, WM_CTLCOLORLISTBOX, dlg2_23.ctlcolor_listbox);
	}
	return FALSE;
}



BOOL Dlg2_23::init(HWND hdlg, HWND hfocus, LPARAM linit)
{
	RECT dlg_rect;
	//	ai_win::set_icon(hdlg, IDI_LOG, IDI_LOG);

	GetClientRect(hdlg, &dlg_rect);
	MoveWindow(hdlg, 50, 150, dlg_rect.right, dlg_rect.bottom, TRUE);
	return TRUE;
}

void Dlg2_23::command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify)
{
	/*switch(id){
	=case IDC_HDH:																//图片灰度化
		if(GET_CHECKER(hdlg,IDC_CHDH)){
			//显示灰度图像时，img_ch=1
			img_out.clear_bmp_stream();
			img_out.display(GetDlgItem(hdlg,IDC_SHOW6),bmp_buf_y, img_w, img_h, 1);	//显示灰度图像
		}
		break;
	}*/
	switch (code_notify)
	{

	}
}

void Dlg2_23::size(HWND hdlg, unsigned int state, int width, int height)
{
	//MoveWindow(GetDlgItem(hdlg, IDC_LOG), 0, 0, width, height, TRUE);
}

void Dlg2_23::close(HWND hdlg)
{
	//dinfo.show_log = FALSE;
	//CheckMenuItem(s_aa::hmenu, ID_FILE_LOG, MF_UNCHECKED);
	menu_223=0;
	CheckMenuItem(hmenu, ID_2_23, FALSE?MF_CHECKED:MF_UNCHECKED);
	EndDialog(hdlg, FALSE);
}

LRESULT Dlg2_23::ctlcolor_listbox(HWND hdlg, HDC hdc, HWND hcolor, int mark)
{
	DeleteObject(mybrush);
	SetBkMode(hdc, TRANSPARENT);
	mybrush = CreateSolidBrush(RGB(0x0,0x0,0x88));
	HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, mybrush);

	HFONT font     = CreateFont(15, 0, 0, 0, FW_BLACK, 0,0,0, ANSI_CHARSET, 0,0,0,0, "黑体");
	HFONT old_font = (HFONT)SelectObject(hdc, font);
	LISTBOX_SETLHEIGHT(hdlg, IDC_LOG, 17);
	SetTextColor(hdc, COLOR_YELLOW_GREEN);

	DeleteObject(font);

	return (LRESULT)mybrush;
}

void Dlg2_23::hscroll(HWND hdlg, HWND hctrl, UINT code, int pos)			
{

}
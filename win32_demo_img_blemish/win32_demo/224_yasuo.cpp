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



class Dlg2_24
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
Dlg2_24 dlg2_24;

BOOL CALLBACK menuDlg_2_24(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_DLGMSG(hdlg, WM_INITDIALOG,	    dlg2_24.init);
		HANDLE_DLGMSG(hdlg, WM_COMMAND,		    dlg2_24.command);
		HANDLE_DLGMSG(hdlg, WM_HSCROLL,			dlg2_24.hscroll );
		HANDLE_DLGMSG(hdlg, WM_SIZE,		    dlg2_24.size);
		HANDLE_DLGMSG(hdlg, WM_CLOSE,		    dlg2_24.close);
		HANDLE_DLGMSG(hdlg, WM_CTLCOLORLISTBOX, dlg2_24.ctlcolor_listbox);
	}
	return FALSE;
}



BOOL Dlg2_24::init(HWND hdlg, HWND hfocus, LPARAM linit)
{
	RECT dlg_rect;
	//	ai_win::set_icon(hdlg, IDI_LOG, IDI_LOG);
	//******************************************************************
	//Ñ¹ËõÏµÊý
	SetDlgItemInt(hdlg,IDC_XSX,1,FALSE);
	SetDlgItemInt(hdlg,IDC_XSY,1,FALSE);
	SET_CHECKER(hdlg,IDC_CX,1);	
	SET_CHECKER(hdlg,IDC_CY,1);	


	GetClientRect(hdlg, &dlg_rect);
	MoveWindow(hdlg, 50, 150, dlg_rect.right, dlg_rect.bottom, TRUE);
	return TRUE;
}

void Dlg2_24::command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify)
{
	/*switch(id){
	case IDC_YSTX:															//Í¼ÏñÑ¹Ëõ
		if(GET_CHECKER(hdlg,IDC_CX))
			sizex=GetDlgItemInt(hdlg,IDC_XSX,0,FALSE);
		if(GET_CHECKER(hdlg,IDC_CY))
			sizey=GetDlgItemInt(hdlg,IDC_XSY,0,FALSE);
		if(GET_CHECKER(hdlg,IDC_CTXYS)){
			img_whys=img_w/sizex;
			img_hhys=img_h/sizey;
			yasuo(bmp_buf_y,img_w,img_h,sizex,sizey,img_whys,img_hhys);

			img_out.clear_bmp_stream();
			img_out.display(GetDlgItem(hdlg,IDC_SHOW7),bmp_buf_ys,img_w/sizex,img_h/sizey, 1);	//ÏÔÊ¾Í¼Ïñ
		}

		sizex=1;
		sizey=1;
		break;
	}*/
	switch (code_notify)
	{

	}
}

void Dlg2_24::size(HWND hdlg, unsigned int state, int width, int height)
{
	//MoveWindow(GetDlgItem(hdlg, IDC_LOG), 0, 0, width, height, TRUE);
}

void Dlg2_24::close(HWND hdlg)
{
	//dinfo.show_log = FALSE;
	//CheckMenuItem(s_aa::hmenu, ID_FILE_LOG, MF_UNCHECKED);
	menu_224=0;
	CheckMenuItem(hmenu, ID_2_24, FALSE?MF_CHECKED:MF_UNCHECKED);
	EndDialog(hdlg, FALSE);
}

LRESULT Dlg2_24::ctlcolor_listbox(HWND hdlg, HDC hdc, HWND hcolor, int mark)
{
	DeleteObject(mybrush);
	SetBkMode(hdc, TRANSPARENT);
	mybrush = CreateSolidBrush(RGB(0x0,0x0,0x88));
	HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, mybrush);

	HFONT font     = CreateFont(15, 0, 0, 0, FW_BLACK, 0,0,0, ANSI_CHARSET, 0,0,0,0, "ºÚÌå");
	HFONT old_font = (HFONT)SelectObject(hdc, font);
	LISTBOX_SETLHEIGHT(hdlg, IDC_LOG, 17);
	SetTextColor(hdc, COLOR_YELLOW_GREEN);

	DeleteObject(font);

	return (LRESULT)mybrush;
}

void Dlg2_24::hscroll(HWND hdlg, HWND hctrl, UINT code, int pos)			
{

}
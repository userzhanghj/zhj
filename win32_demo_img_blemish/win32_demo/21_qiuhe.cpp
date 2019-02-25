

#include <Windows.h>
#include <stdio.h>
#include <direct.h>
#include "s_win.h"
#include "s_img.h"
#include "resource.h"
#include "qiuhe.h"
#include "tuxiang.h"

#pragma comment(lib, "s_win.lib")
#pragma comment(lib, "s_img.lib")

using namespace ai_win;

extern HMENU hmenu;



class Dlg2_1
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
Dlg2_1 dlg2_1;

BOOL CALLBACK menuDlg_2_1(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_DLGMSG(hdlg, WM_INITDIALOG,	    dlg2_1.init);
		HANDLE_DLGMSG(hdlg, WM_COMMAND,		    dlg2_1.command);
		HANDLE_DLGMSG(hdlg, WM_HSCROLL,			dlg2_1.hscroll );
		HANDLE_DLGMSG(hdlg, WM_SIZE,		    dlg2_1.size);
		HANDLE_DLGMSG(hdlg, WM_CLOSE,		    dlg2_1.close);
		HANDLE_DLGMSG(hdlg, WM_CTLCOLORLISTBOX, dlg2_1.ctlcolor_listbox);
	}
	return FALSE;
}



BOOL Dlg2_1::init(HWND hdlg, HWND hfocus, LPARAM linit)
{
	RECT dlg_rect;
	//	ai_win::set_icon(hdlg, IDI_LOG, IDI_LOG);

	GetClientRect(hdlg, &dlg_rect);
	MoveWindow(hdlg, 50, 150, dlg_rect.right, dlg_rect.bottom, TRUE);
	
	ai_win::get_current_directory(dir);		//获取文件位置
	sprintf(config_path,"%s\\config.ini",dir);

	d1=ai_win::read_integer_key(config_path,"calculate","d1");
	SetDlgItemInt(hdlg,IDC_DIT1,d1,FALSE);
	d2=ai_win::read_integer_key(config_path,"calculate","d2");
	SetDlgItemInt(hdlg,IDC_DIT2,d2,FALSE);
	d3=ai_win::read_integer_key(config_path,"calculate","d3");
	SetDlgItemInt(hdlg,IDC_DIT3,d3,FALSE);
	SET_SLIDER_POS(hdlg,IDC_HKD1, d1);
	SetDlgItemInt(hdlg,IDC_D1,d1,FALSE);
	SET_SLIDER_POS(hdlg,IDC_HKD2, d2);
	SetDlgItemInt(hdlg,IDC_D2,d2,FALSE);

	SET_SLIDER_POS(hdlg,IDC_HKD3, d3);
	SetDlgItemInt(hdlg,IDC_D3,d3,FALSE);

	return TRUE;
}

void Dlg2_1::command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify)
{
	int gc;
	switch (id){
	case IDC_SUM:													//求和
		d1=GetDlgItemInt(hdlg,IDC_DIT1,0,FALSE);					//获取输入框的值
		d2=GetDlgItemInt(hdlg,IDC_DIT2,0,FALSE);
		d3=GetDlgItemInt(hdlg,IDC_DIT3,0,FALSE);

		gc=GET_CHECKER(hdlg,IDC_CHECK1);							//获取确认选项的值
		if(gc){
			s=sum(d1,d2,d3);
		}

		if(jiorou(s))
			SET_CHECKER(hdlg,IDC_CHECK2,1);							//赋予确认选项的值
		else
			SET_CHECKER(hdlg,IDC_CHECK2,0);
		SetDlgItemInt(hdlg,IDC_HE,s,0);

		ai_win::write_integer_key(config_path,"calculate","d1",d1);					//数据的存储
		ai_win::write_integer_key(config_path,"calculate","d2",d2);
		ai_win::write_integer_key(config_path,"calculate","d3",d3);

		SET_SLIDER_POS(hdlg,IDC_HKD1, d1);							//赋予滑块的值
		SetDlgItemInt(hdlg,IDC_D1,d1,FALSE);						//赋予输出框的值
		SET_SLIDER_POS(hdlg,IDC_HKD2, d2);
		SetDlgItemInt(hdlg,IDC_D2,d2,FALSE);
		SET_SLIDER_POS(hdlg,IDC_HKD3, d3);
		SetDlgItemInt(hdlg,IDC_D3,d3,FALSE);
		break;
	}
	switch (code_notify)
	{

	}
}

void Dlg2_1::size(HWND hdlg, unsigned int state, int width, int height)
{
	//MoveWindow(GetDlgItem(hdlg, IDC_LOG), 0, 0, width, height, TRUE);
}

void Dlg2_1::close(HWND hdlg)
{
	//dinfo.show_log = FALSE;
	//CheckMenuItem(s_aa::hmenu, ID_FILE_LOG, MF_UNCHECKED);
	menu_21=0;
	CheckMenuItem(hmenu, ID_2_1, FALSE?MF_CHECKED:MF_UNCHECKED);
	EndDialog(hdlg, FALSE);
}

void Dlg2_1::hscroll(HWND hdlg, HWND hctrl, UINT code, int pos)			//滑块相关操作
{
	//**********************************************************************//求和相关插件关联
	d1=GET_SLIDER_POS(hdlg,IDC_HKD1);										//获取滑块的值
	SetDlgItemInt(hdlg,IDC_D1,d1,FALSE);
	SetDlgItemInt(hdlg,IDC_DIT1,d1,FALSE);
	
	d2=GET_SLIDER_POS(hdlg,IDC_HKD2);
	SetDlgItemInt(hdlg,IDC_D2,d2,FALSE);
	SetDlgItemInt(hdlg,IDC_DIT2,d2,FALSE);
	
	d3=GET_SLIDER_POS(hdlg,IDC_HKD3);
	SetDlgItemInt(hdlg,IDC_D3,d3,FALSE);
	SetDlgItemInt(hdlg,IDC_DIT3,d3,FALSE);
}

LRESULT Dlg2_1::ctlcolor_listbox(HWND hdlg, HDC hdc, HWND hcolor, int mark)
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

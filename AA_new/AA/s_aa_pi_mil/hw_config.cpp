#include "interface.h"
#include "inf_cap_bd.h"


HWND hsubcap[CAP_BD_CNT] = {NULL};
Para para;
class HWConfigDlg:public ai_win::Dlg
{
public:
	virtual BOOL init(HWND hdlg, HWND hfocus, LPARAM linit);
	virtual void command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify);
	virtual BOOL notify(HWND hdlg, int idctrl, LPNMHDR pnmh);
	virtual void draw_item(HWND hdlg, const DRAWITEMSTRUCT *pdis);
	virtual void close(HWND hdlg);

private:

	void save_para(HWND hdlg);
	void create_sub_dlg(HWND hdlg);
	HBRUSH  hbr[CAP_BD_CNT];
	HWND htab;
};

void HWConfigDlg::save_para(HWND hdlg)
{
	ai_win::write_integer_key(s_aa::config_path, "img", "select_img_source",		 SELECT_IMG_SOURCE);
//	ai_win::write_integer_key(s_aa::config_path, "img", "device_start_init",		 DEVICE_START_INIT);
}

void HWConfigDlg::create_sub_dlg(HWND hdlg)
{
// 	int tab_idd[CAP_BD_CNT] = {IDD_AIU3, IDD_WEB_CAM};
// 
// 	hbr[0] = CreateSolidBrush(COLOR_74_LIGHT_STEEL_BLUE);
// 	hbr[1] = CreateSolidBrush(COLOR_73_LIGHT_SLATE_GRAY);
// 	InitCommonControls();
// 	htab = GetDlgItem(hdlg, IDC_CONFIG_TAB);
// 	double n = GetLastError();
// 	TCITEM item_struct;
// 	item_struct.mask = TCIF_TEXT;
// 	item_struct.iImage = 1;
// 	item_struct.lParam = 0;
// 
// 	for (int i=0; i<CAP_BD_CNT; i++){
// 		
// 		item_struct.cchTextMax = CAP_BD_CNT;
// 		SendMessage(htab, TCM_INSERTITEM, i, (LPARAM)&item_struct);
// 
// 		hsubcap[i] = CreateDialogParam((HINSTANCE)GetWindowLong(hdlg, GWL_HINSTANCE), MAKEINTRESOURCE(tab_idd[i]), htab, (DLGPROC)tab_proc[i], 0);
// 		ShowWindow(hsubcap[i], SW_HIDE);
// 	}
// 	ShowWindow(hsubcap[0], SW_SHOWDEFAULT);
}

BOOL HWConfigDlg::notify(HWND hdlg, int idctrl, LPNMHDR pnmh)
{
	int i=0;
	if (pnmh->code == TCN_SELCHANGE){
		for (int j=0; j<CAP_BD_CNT; j++){
			ShowWindow(hsubcap[j], SW_HIDE);
		}
		i = SendMessage(htab, TCM_GETCURSEL, 0, 0);
		ShowWindow(hsubcap[i], SW_SHOWDEFAULT);
	}

	return FALSE;
}

void HWConfigDlg::draw_item(HWND hdlg, const DRAWITEMSTRUCT *pdis)
{
	TC_ITEM     tci;
	char        szTabText[100];
	HWND hTabCtrl = GetDlgItem(hdlg, IDC_CONFIG_TAB);

	HBRUSH myhbr = NULL;
	COLORREF bkColor;
	if (htab == pdis->hwndItem){
		switch (pdis->itemID){
		case 0:
			myhbr = hbr[0];
			bkColor = COLOR_74_LIGHT_STEEL_BLUE;
			break;

		case 1:
			myhbr = hbr[1];
			bkColor = COLOR_73_LIGHT_SLATE_GRAY;
			break;
		}
		memset(szTabText, '\0', sizeof(szTabText));

		tci.mask = TCIF_TEXT;
		tci.pszText = szTabText;
		tci.cchTextMax = sizeof(szTabText)-1;

		TabCtrl_GetItem(hTabCtrl, pdis->itemID, &tci);

		FillRect(pdis->hDC, &pdis->rcItem, myhbr);
		SetBkColor(pdis->hDC, bkColor);

		TextOut(pdis->hDC,
			pdis->rcItem.left+4,
			pdis->rcItem.top+3,
			tci.pszText,
			lstrlen(tci.pszText));
	}
}

BOOL HWConfigDlg::init(HWND hdlg, HWND hfocus, LPARAM linit)
{
	COMBO_ADDSTR(hdlg, IDC_SELECT_IMG_DEVICE, "Image Files");
	COMBO_ADDSTR(hdlg, IDC_SELECT_IMG_DEVICE, "TSW_306E");
	COMBO_ADDSTR(hdlg, IDC_SELECT_IMG_DEVICE, "Web_Cam");
	COMBO_ADDSTR(hdlg, IDC_SELECT_IMG_DEVICE, "Mil_Cap");
	sprintf(s_aa::config_path, "%s\\recipe\\config.ini", s_aa::dir);
	SELECT_IMG_SOURCE = ai_win::read_integer_key(s_aa::config_path, "img", "select_img_source");
	
	COMBO_SETCURSEL(hdlg, IDC_SELECT_IMG_DEVICE, SELECT_IMG_SOURCE);
	create_sub_dlg(hdlg);

	return TRUE;
}

void HWConfigDlg::command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify)
{
	switch (code_notify){
	case CBN_SELCHANGE:
		{
			int index = COMBO_GETCURSEL(hdlg, IDC_SELECT_IMG_DEVICE);
			SELECT_IMG_SOURCE = index;
			save_para(hdlg);
		}
		break;
	}
}

void HWConfigDlg::close(HWND hdlg)
{
	CheckMenuItem(s_aa::hmenu, ID_HW_CONFIG, MF_UNCHECKED);
	s_aa::dinfo.show_hw_config = FALSE;
	EndDialog(hdlg, TRUE);
}

HWConfigDlg hwdlg;

BOOL CALLBACK dlg_hw_config(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam)
{
	switch (message){
		HANDLE_DLGMSG(hdlg, WM_INITDIALOG, hwdlg.init);
		HANDLE_DLGMSG(hdlg, WM_COMMAND,	   hwdlg.command);
		HANDLE_DLGMSG(hdlg, WM_NOTIFY,	   hwdlg.notify);
		HANDLE_DLGMSG(hdlg, WM_DRAWITEM,   hwdlg.draw_item);
		HANDLE_DLGMSG(hdlg, WM_CLOSE,	   hwdlg.close);
	}

	return FALSE;
}


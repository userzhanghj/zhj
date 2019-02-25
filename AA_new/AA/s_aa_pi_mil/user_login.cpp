#include "interface.h"

class DlgUser:public ai_win::Dlg
{
public:
	virtual BOOL init(HWND hdlg, HWND hfocus, LPARAM linit);
	virtual void command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify);
	virtual void close(HWND hdlg);
private:
	HBRUSH mybrush;
};

DlgUser user_login;

BOOL CALLBACK user_login_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam)
{
	switch (message){
		HANDLE_DLGMSG(hdlg, WM_INITDIALOG,  user_login.init);
		HANDLE_DLGMSG(hdlg, WM_COMMAND,		user_login.command);
		HANDLE_DLGMSG(hdlg, WM_CLOSE,		user_login.close);
		//		HANDLE_DLGMSG(hdlg, WM_CTLCOLORDLG, motion_dlg.ctlcolor_dlg);
	}
	return FALSE;
}

BOOL DlgUser::init(HWND hdlg, HWND hfocus, LPARAM linit)
{
	if(!IS_LOGIN_SUCCEED){
		ITEM_GRAYED(hdlg , IDC_LOGOUT);
		ITEM_GRAYED(hdlg , IDC_NEW_PASSWORD_INPUT);
		ITEM_GRAYED(hdlg , IDC_CHANGE_PASSWORD);

		ITEM_ENABLED(hdlg , IDC_LOGIN);
		ITEM_ENABLED(hdlg , IDC_PASSWORD_INPUT);

		MENU_GRAYED(HWND_MAIN, ID_SETTING_RECIPE);
		MENU_GRAYED(HWND_MAIN, ID_TEST_CONFIG);
		MENU_GRAYED(HWND_MAIN, ID_REGMARKCHART_CONFIGURE);
		MENU_GRAYED(HWND_MAIN, ID_AA_CONFIG);
		MENU_GRAYED(HWND_MAIN, ID_PI_CONFIG);
		MENU_GRAYED(HWND_MAIN, ID_HW_CONFIG);
	}
	else{
		ITEM_GRAYED(hdlg , IDC_LOGIN);
		ITEM_GRAYED(hdlg , IDC_PASSWORD_INPUT);
		ITEM_ENABLED(hdlg , IDC_LOGOUT);
		ITEM_ENABLED(hdlg , IDC_NEW_PASSWORD_INPUT);
		ITEM_ENABLED(hdlg , IDC_CHANGE_PASSWORD);

		MENU_ENABLED(HWND_MAIN, ID_SETTING_RECIPE);
		MENU_ENABLED(HWND_MAIN, ID_TEST_CONFIG);
		MENU_ENABLED(HWND_MAIN, ID_REGMARKCHART_CONFIGURE);
		MENU_ENABLED(HWND_MAIN, ID_AA_CONFIG);
		MENU_ENABLED(HWND_MAIN, ID_PI_CONFIG);
		MENU_ENABLED(HWND_MAIN, ID_HW_CONFIG);
	}
	return TRUE;
}

void DlgUser::command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify)
{
	int timeout;
	int err = 0;
	switch (id){
	case IDC_LOGIN:
		{
			char default_pw[100] = {0};
			//sprintf(s_aa::config_path, "%s\\recipe\\config.ini", s_aa::dir);
			ai_win::read_string_key(s_aa::config_path, "system", "password", default_pw, 100);
			char input_pw[100] = {0};
			GetDlgItemText(hdlg, IDC_PASSWORD_INPUT, input_pw, 100);
			if (!strcmp(input_pw, default_pw)){
				IS_LOGIN_SUCCEED = TRUE;
				
				MENU_ENABLED(HWND_MAIN, ID_SETTING_RECIPE);
				MENU_ENABLED(HWND_MAIN, ID_TEST_CONFIG);
				MENU_ENABLED(HWND_MAIN, ID_REGMARKCHART_CONFIGURE);
				MENU_ENABLED(HWND_MAIN, ID_AA_CONFIG);
				MENU_ENABLED(HWND_MAIN, ID_PI_CONFIG);
				MENU_ENABLED(HWND_MAIN, ID_HW_CONFIG);
				SetDlgItemText(hdlg, IDC_LOGIN_INFO, "Login Success!");
				ITEM_GRAYED(hdlg, IDC_PASSWORD_INPUT);
				ITEM_GRAYED(hdlg, IDC_LOGIN);

				ITEM_ENABLED(hdlg, IDC_LOGOUT);
				ITEM_ENABLED(hdlg, IDC_NEW_PASSWORD_INPUT);
				ITEM_ENABLED(hdlg, IDC_CHANGE_PASSWORD);

			}
			else{
				IS_LOGIN_SUCCEED = FALSE;
				MENU_GRAYED(HWND_MAIN, ID_SETTING_RECIPE);
				MENU_GRAYED(HWND_MAIN, ID_TEST_CONFIG);
				MENU_GRAYED(HWND_MAIN, ID_REGMARKCHART_CONFIGURE);
				MENU_GRAYED(HWND_MAIN, ID_AA_CONFIG);
				MENU_GRAYED(HWND_MAIN, ID_PI_CONFIG);
				MENU_GRAYED(HWND_MAIN, ID_HW_CONFIG);
				SetDlgItemText(hdlg, IDC_LOGIN_INFO, "Wrong Password!");
			}
		}
		break;
	case IDC_LOGOUT:
		IS_LOGIN_SUCCEED = FALSE;
		SetDlgItemText(hdlg, IDC_LOGIN_INFO, "Logout Success!");
		SetDlgItemText(hdlg, IDC_PASSWORD_INPUT, "");
		ITEM_ENABLED(hdlg, IDC_PASSWORD_INPUT);
		ITEM_ENABLED(hdlg, IDC_LOGIN);

		ITEM_GRAYED(hdlg, IDC_LOGOUT);
		ITEM_GRAYED(hdlg, IDC_NEW_PASSWORD_INPUT);
		ITEM_GRAYED(hdlg, IDC_CHANGE_PASSWORD);
		MENU_GRAYED(HWND_MAIN, ID_SETTING_RECIPE);
		MENU_GRAYED(HWND_MAIN, ID_TEST_CONFIG);
		MENU_GRAYED(HWND_MAIN, ID_REGMARKCHART_CONFIGURE);
		MENU_GRAYED(HWND_MAIN, ID_AA_CONFIG);
		MENU_GRAYED(HWND_MAIN, ID_PI_CONFIG);
		MENU_GRAYED(HWND_MAIN, ID_HW_CONFIG);
		break;

	case IDC_CHANGE_PASSWORD:
		if (IS_LOGIN_SUCCEED){
			char new_pw[100];
			GetDlgItemText(hdlg, IDC_NEW_PASSWORD_INPUT, new_pw, 100);
			//check password rules
			//...
			////...
			//sprintf(s_aa::config_path, "%s\\recipe\\config.ini", s_aa::dir);
			ai_win::write_string_key(s_aa::config_path, "system", "password", new_pw);
			SetDlgItemText(hdlg, IDC_NEW_PASSWORD_INPUT, "");
			SetDlgItemText(hdlg, IDC_LOGIN_INFO, "Modify Password Success!");

		}
		break;
	}
	switch (code_notify){

	}
}
	
void DlgUser::close(HWND hdlg)
{
	EndDialog(hdlg, FALSE);
//	PI_CloseConnection ( ID );
}

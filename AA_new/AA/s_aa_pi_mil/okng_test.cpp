#include "interface.h"

namespace ok_ng_test{
	bool ok_test = 0;
	bool ng_test = 0;
	bool open_okng_test;

	char ok_sn[100];
	char ng_sn[100];
	char test_sn[100];
}

extern SlantedBW mysbw;

int start_ok_test=0;
int start_ng_test=0;

char okng_test_path[MAX_PATH] = {0};
using namespace ok_ng_test;

INIEdit okng_test_config[] = {
	//{IDC_OKTEST_CHECK,			DLGTYPE_COMBO_INT, &ok_test,				"ok_test"},
	//{IDC_NGTEST_CHECK,			DLGTYPE_COMBO_INT, &ng_test,				"ng_test"},
	{IDC_OPEN_OKNG_TEST,		DLGTYPE_CHECK_BOOL, &open_okng_test,			"open_okng_test"},
	//*
	{NULL, NULL, NULL, NULL}

};

class OKNGTestDlg:public ai_win::Dlg
{
public:
	virtual BOOL init(HWND hdlg, HWND hfocus, LPARAM linit);
	virtual void command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify);
	virtual void close(HWND hdlg);

};

OKNGTestDlg okng_test_dlg;

BOOL CALLBACK okng_test_process(HWND hdlg, unsigned int message, 
	WPARAM wParam, LPARAM lParam)
{
	switch (message){
		HANDLE_DLGMSG(hdlg, WM_INITDIALOG,  okng_test_dlg.init);
		HANDLE_DLGMSG(hdlg, WM_COMMAND,		okng_test_dlg.command);
		HANDLE_DLGMSG(hdlg, WM_CLOSE,		okng_test_dlg.close);
	}
	return FALSE;
}


BOOL OKNGTestDlg::init(HWND hdlg, HWND hfocus, LPARAM linit)
{
	sprintf(okng_test_path, "%s\\recipe\\%s\\okng_set.ini", s_aa::dir, s_aa::recipe_name);
	
	ai_win::load_keys(okng_test_path, "okng", hdlg, okng_test_config, sizeof(okng_test_config)/sizeof(okng_test_config[0]));

	
	ai_win::read_string_key(okng_test_path,"okng","ok_sn",ok_ng_test::ok_sn,100);
	SetDlgItemText(hdlg, IDC_OK_SN, ok_ng_test::ok_sn);
	ai_win::read_string_key(okng_test_path,"okng","ng_sn",ok_ng_test::ng_sn,100);
	SetDlgItemText(hdlg, IDC_NG_SN, ok_ng_test::ng_sn);

	SET_CHECKER		(hdlg, IDC_OKTEST_CHECK,		ok_test		);
	SET_CHECKER		(hdlg, IDC_NGTEST_CHECK,		ng_test				);
	SET_CHECKER		(hdlg, IDC_OPEN_OKNG_TEST,		open_okng_test		);
	
	return TRUE;
}

void OKNGTestDlg::command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify)
{
	switch (id)
	{
	case IDC_RETEST:
		ok_ng_test::ok_test = FALSE;
		SET_CHECKER	(HWND_OKNG_TEST, IDC_OKTEST_CHECK, ok_ng_test::ok_test);
		ok_ng_test::ng_test = FALSE;
		SET_CHECKER	(HWND_OKNG_TEST, IDC_NGTEST_CHECK, ok_ng_test::ng_test);
		break;
	case IDC_OK_TEST:
		//顶起气缸
		//旋转气缸工作位
		//上电
		if(!strcmp(ok_ng_test::test_sn, ok_ng_test::ok_sn))
			start_ok_test = 1;
		break;
	case IDC_NG_TEST:
		//顶起气缸
		//旋转气缸工作位
		//上电
		if(!strcmp(ok_ng_test::test_sn, ok_ng_test::ng_sn))
			start_ng_test = 1;
		break;
	case IDC_OKNG_SAVE:
		ai_win::save_keys(okng_test_path, "okng", hdlg, okng_test_config, sizeof(okng_test_config)/sizeof(okng_test_config[0]));

		GetDlgItemText(hdlg, IDC_TEST_SN, ok_ng_test::test_sn,100);
		break;
	}
	
	switch (code_notify){

	}
}

void OKNGTestDlg::close(HWND hdlg)
{
	CheckMenuItem(s_aa::hmenu, ID_OKNG_TEST, MF_UNCHECKED);
	EndDialog(hdlg, TRUE);
}



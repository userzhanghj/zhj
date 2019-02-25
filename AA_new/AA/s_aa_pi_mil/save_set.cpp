#include "interface.h"
#include "inf_cap_bd.h"

namespace save_set{
	int sfr_save_set;
	int save_sfr_only;
	int save_avg_only;

	int mes_saveimg;
	int mes_aa_boffset;
	int mes_aa_aoffset;
	int mes_uv_bload;
	int mes_uv_aload;
	
	int save_scan;
	int save_scanimg;
	int saveimg;
	int aa_boffset;
	int aa_aoffset;
	int uv_bload;
	int uv_aload;
}

char save_set_path[MAX_PATH] = {0};
using namespace save_set;

INIEdit save_set_config[] = {
	{IDC_SFR_SAVE_SET,			DLGTYPE_COMBO_INT, &sfr_save_set,			"sfr_save_set"},
	{IDC_MES_SAVEIMG,			DLGTYPE_CHECK_BOOL, &mes_saveimg,			"mes_saveimg"},
	{IDC_MES_AA_BOFFSET,		DLGTYPE_CHECK_BOOL, &mes_aa_boffset,			"mes_aa_boffset"},
	{IDC_MES_AA_AOFFSET,		DLGTYPE_CHECK_BOOL, &mes_aa_aoffset,			"mes_aa_aoffset"},
	{IDC_MES_UV_BLOAD,			DLGTYPE_CHECK_BOOL, &mes_uv_bload,			"mes_uv_bload"},
	{IDC_MES_UV_ALOAD,			DLGTYPE_CHECK_BOOL, &mes_uv_aload,			"mes_uv_aload"},
	{IDC_SAVEIMG,				DLGTYPE_CHECK_BOOL, &saveimg,				"saveimg"},
	{IDC_AA_BOFFSET,			DLGTYPE_CHECK_BOOL, &aa_boffset,				"aa_boffset"},
	{IDC_AA_AOFFSET,			DLGTYPE_CHECK_BOOL, &aa_aoffset,				"aa_aoffset"},
	{IDC_UV_BLOAD,				DLGTYPE_CHECK_BOOL, &uv_bload,				"uv_bload"},
	{IDC_UV_ALOAD,				DLGTYPE_CHECK_BOOL, &uv_aload,				"uv_aload"},

	//*
	{NULL, NULL, NULL, NULL}

};

class SaveDlg:public ai_win::Dlg
{
public:
	virtual BOOL init(HWND hdlg, HWND hfocus, LPARAM linit);
	virtual void command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify);
	virtual void close(HWND hdlg);

};

SaveDlg save_set_dlg;

BOOL CALLBACK motion_save_set_process(HWND hdlg, unsigned int message, 
	WPARAM wParam, LPARAM lParam)
{
	switch (message){
		HANDLE_DLGMSG(hdlg, WM_INITDIALOG,  save_set_dlg.init);
		HANDLE_DLGMSG(hdlg, WM_COMMAND,		save_set_dlg.command);
		HANDLE_DLGMSG(hdlg, WM_CLOSE,		save_set_dlg.close);
	}
	return FALSE;
}


BOOL SaveDlg::init(HWND hdlg, HWND hfocus, LPARAM linit)
{
	COMBO_ADDSTR(hdlg, IDC_SFR_SAVE_SET, "1.仅存储平均值");
	COMBO_ADDSTR(hdlg, IDC_SFR_SAVE_SET, "2.仅分开存储值");
	COMBO_ADDSTR(hdlg, IDC_SFR_SAVE_SET, "3.存储平均值以及分开存储");
	sprintf(save_set_path, "%s\\recipe\\%s\\save_set.ini", s_aa::dir, s_aa::recipe_name);
	log("%s****",save_set_path);
	ai_win::load_keys(save_set_path, "save", hdlg, save_set_config, sizeof(save_set_config)/sizeof(save_set_config[0]));

	COMBO_SETCURSEL(hdlg, IDC_SELECT_IMG_DEVICE, sfr_save_set);

	SET_CHECKER		(hdlg, IDC_MES_SAVEIMG,			mes_saveimg		);
	SET_CHECKER		(hdlg, IDC_MES_AA_BOFFSET,		mes_aa_boffset		);
	SET_CHECKER		(hdlg, IDC_MES_AA_AOFFSET,		mes_aa_aoffset		);
	SET_CHECKER		(hdlg, IDC_MES_UV_BLOAD	,		mes_uv_bload		);
	SET_CHECKER		(hdlg, IDC_MES_UV_ALOAD	,		mes_uv_aload		);
	
	//SET_CHECKER		(hdlg, IDC_SAVE_SCAN	,		save_scan	);
	//SET_CHECKER		(hdlg, IDC_SAVE_SCANIMG,		save_scanimg		);

	SET_CHECKER		(hdlg, IDC_SAVEIMG		,		saveimg		);
	SET_CHECKER		(hdlg, IDC_AA_BOFFSET	,		aa_boffset		);
	SET_CHECKER		(hdlg, IDC_AA_AOFFSET	,		aa_aoffset		);
	SET_CHECKER		(hdlg, IDC_UV_BLOAD		,		uv_bload		);
	SET_CHECKER		(hdlg, IDC_UV_ALOAD		,		uv_aload		);
	
	if(sfr_save_set == 0){
		save_sfr_only = FALSE;
		save_avg_only = TRUE;
	}
	else if(sfr_save_set == 1){
		save_sfr_only = TRUE;
		save_avg_only = FALSE;
	}
	else if(sfr_save_set == 2){
		save_sfr_only = TRUE;
		save_avg_only = TRUE;
	}
	return TRUE;
}

void SaveDlg::command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify)
{
	switch (id)
	{
	case IDC_SAVE:
		ai_win::save_keys(save_set_path, "save", hdlg, save_set_config, sizeof(save_set_config)/sizeof(save_set_config[0]));

		if(sfr_save_set == 0){
			save_sfr_only = FALSE;
			save_avg_only = TRUE;
		}
		else if(sfr_save_set == 1){
			save_sfr_only = TRUE;
			save_avg_only = FALSE;
		}
		else if(sfr_save_set == 2){
			save_sfr_only = TRUE;
			save_avg_only = TRUE;
		}
		break;
	}
	
	switch (code_notify){

	}
}

void SaveDlg::close(HWND hdlg)
{
	CheckMenuItem(s_aa::hmenu, ID_HW_CONFIG, MF_UNCHECKED);
	EndDialog(hdlg, TRUE);
}



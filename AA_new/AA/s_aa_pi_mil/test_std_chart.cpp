#define _CRT_SECURE_NO_WARNINGS
#include "interface.h"
#include "test_std_chart.h"

StdChart st;
PCurveNode cv_root;
extern SlantedBW mysbw;

HWND std_chart_hwnd = NULL;
HWND std_curve_hwnd = NULL;
extern BOOL CALLBACK st_focus_dlg(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK st_color_dlg(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK st_gray_dlg(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK st_oc_dlg(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK st_rotation_dlg(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);

extern BOOL CALLBACK st_curve_dlg_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);

char st_path[MAX_PATH] = {0};

TabInfo st_tab[] = {
	{NULL, "Color",  st_color_dlg,	  IDD_STD_COLOR,	TRUE},
	{NULL, "Gray",   st_gray_dlg,	  IDD_STD_GRAY,		TRUE},
	{NULL, "OC",     st_oc_dlg,		  IDD_STD_OC,		TRUE},
	{NULL, "Rotate", st_rotation_dlg, IDD_STD_ROTATION, TRUE}
};

INIEdit st_fm[] = {
	{IDC_SB_TO_IMG_CENTER_X,	  DLGTYPE_INPUT_INT,	&st.sb_to_img_cx,		"scanbox_to_img_center_x"	},
	{IDC_SB_TO_IMG_CENTER_Y,	  DLGTYPE_INPUT_INT,	&st.sb_to_img_cy,		"scanbox_to_img_center_y"	},
	{IDC_SB_SIZE,				  DLGTYPE_INPUT_INT,	&st.sb_size,			"scanbox_size"				},
	{IDC_CL_THINKNESS,			  DLGTYPE_INPUT_INT,	&st.cl_thinkness,		"crossline_thinkness"		},
	{IDC_CL_LENGTH,				  DLGTYPE_INPUT_INT,	&st.cl_length,			"crossline_length"			},
	{NULL, NULL, NULL, NULL}
};

INIEdit st_sfr[] = {
	{IDC_USE_MTF50P,			  DLGTYPE_CHECK_BOOL,    &st.is_use_mtf50p,  "use_mtf50p"   },
	{IDC_SFR_FIELD_A_ON,		  DLGTYPE_CHECK_BOOL,    &st.is_field_a_on,  "is_field_a_on"},
	{IDC_SFR_FIELD_B_ON,		  DLGTYPE_CHECK_BOOL,    &st.is_field_b_on,  "is_field_b_on"},
	{IDC_SFR_FIELD_C_ON,		  DLGTYPE_CHECK_BOOL,    &st.is_field_c_on,  "is_field_c_on"},
	{IDC_SFR_FREQ1,				  DLGTYPE_INPUT_DOUBLE,  &st.freq1,			"spec1_freq"   },
	{IDC_SFR_FREQ2,				  DLGTYPE_INPUT_DOUBLE,  &st.freq2,			"spec2_freq"   },
	{IDC_SFR_SPEC2,				  DLGTYPE_CHECK_BOOL,    &st.is_spec_2_on,   "is_spec2_on"  },

	{IDC_SFR_FIELD0_SPEC1,		  DLGTYPE_INPUT_DOUBLE,  &st.spec1[0],	   "field_0_spec1"},
	{IDC_SFR_FIELD0_SPEC2,		  DLGTYPE_INPUT_DOUBLE,  &st.spec2[0],	   "field_0_spec2"},
	{IDC_SFR_ROI0_X,			  DLGTYPE_INPUT_DOUBLE,  &st.roi_to_ccx[0], "field_0_roi_x"},
	{IDC_SFR_ROI0_Y,			  DLGTYPE_INPUT_DOUBLE,  &st.roi_to_ccy[0], "field_0_roi_y"},
	{IDC_SFR_ROI0_WIDTH,		  DLGTYPE_INPUT_INT,	 &st.roi_w[0],	   "field_0_roi_h"},
	{IDC_SFR_ROI0_HEIGHT,		  DLGTYPE_INPUT_INT,     &st.roi_h[0],	   "field_0_roi_w"},

	{IDC_SFR_FIELDA_SPEC1,		  DLGTYPE_INPUT_DOUBLE,  &st.spec1[1],	   "field_a_spec1"},
	{IDC_SFR_FIELDA_SPEC2,		  DLGTYPE_INPUT_DOUBLE,  &st.spec2[1],	   "field_a_spec2"},
	{IDC_SFR_ROIA_X,			  DLGTYPE_INPUT_DOUBLE,  &st.roi_to_ccx[1], "field_a_roi_x"},
	{IDC_SFR_ROIA_Y,			  DLGTYPE_INPUT_DOUBLE,  &st.roi_to_ccy[1], "field_a_roi_y"},
	{IDC_SFR_ROIA_WIDTH,		  DLGTYPE_INPUT_INT,     &st.roi_w[1],	   "field_a_roi_h"},
	{IDC_SFR_ROIA_HEIGHT,		  DLGTYPE_INPUT_INT,     &st.roi_h[1],	   "field_a_roi_w"},

	{IDC_SFR_FIELDB_SPEC1,		  DLGTYPE_INPUT_DOUBLE,  &st.spec1[2],	   "field_b_spec1"},
	{IDC_SFR_FIELDB_SPEC2,		  DLGTYPE_INPUT_DOUBLE,  &st.spec2[2],	   "field_b_spec2"},
	{IDC_SFR_ROIB_X,			  DLGTYPE_INPUT_DOUBLE,  &st.roi_to_ccx[2], "field_b_roi_x"},
	{IDC_SFR_ROIB_Y,			  DLGTYPE_INPUT_DOUBLE,  &st.roi_to_ccy[2], "field_b_roi_y"},
	{IDC_SFR_ROIB_WIDTH,		  DLGTYPE_INPUT_INT,     &st.roi_w[2],	   "field_b_roi_h"},
	{IDC_SFR_ROIB_HEIGHT,		  DLGTYPE_INPUT_INT,     &st.roi_h[2],	   "field_b_roi_w"},

	{IDC_SFR_FIELDC_SPEC1,		  DLGTYPE_INPUT_DOUBLE,  &st.spec1[3],	   "field_c_spec1"},
	{IDC_SFR_FIELDC_SPEC2,		  DLGTYPE_INPUT_DOUBLE,  &st.spec2[3],	   "field_c_spec2"},
	{IDC_SFR_ROIC_X,			  DLGTYPE_INPUT_DOUBLE,  &st.roi_to_ccx[3], "field_c_roi_x"},
	{IDC_SFR_ROIC_Y,			  DLGTYPE_INPUT_DOUBLE,  &st.roi_to_ccy[3], "field_c_roi_y"},
	{IDC_SFR_ROIC_WIDTH,		  DLGTYPE_INPUT_INT,     &st.roi_w[3],	   "field_c_roi_h"},
	{IDC_SFR_ROIC_HEIGHT,		  DLGTYPE_INPUT_INT,     &st.roi_h[3],	   "field_c_roi_w"},

	{IDC_SFR_DIRECTION_H_ON,	  DLGTYPE_CHECK_BOOL,    &st.is_direct_h_on, "is_direction_h_on"		},
	{IDC_SFR_DIRECTION_V_ON,	  DLGTYPE_CHECK_BOOL,    &st.is_direct_v_on, "is_direction_v_on"		},
	{IDC_SFR_V_OFFSET_X,		  DLGTYPE_INPUT_DOUBLE,  &st.v_offset_x,		"direction_v_offset_x"	},
	{IDC_SFR_V_OFFSET_Y,		  DLGTYPE_INPUT_DOUBLE,  &st.v_offset_y,		"direction_v_offset_y"	},

	{IDC_DEV_TEST_ON,			  DLGTYPE_CHECK_BOOL,   &st.is_tilt_test_on, "is_dev_test_on"},
	{IDC_H_DEV_SPEC,			  DLGTYPE_INPUT_INT,	&st.h_tilt_spec,	 "h_dev_spec"	},
	{IDC_V_DEV_SPEC,			  DLGTYPE_INPUT_INT,    &st.v_tilt_spec,	 "v_dev_spec"	},

	{IDC_FOCUS_TARGET_VALUE,	  DLGTYPE_INPUT_INT,    &st.target_value,		"target_value"	    },
	{IDC_FOCUS_CURVE_IGNORE_SPEC, DLGTYPE_INPUT_INT,    &st.target_ignore_spec,	"target_ignore_spec"},

	{IDC_SAVE_RAW_IMAGE,		  DLGTYPE_CHECK_BOOL,   &st.is_save_raw_image,    "is_save_raw_image"   },
	{IDC_SAVE_BMP_IMAGE,		  DLGTYPE_CHECK_BOOL,   &st.is_save_bmp_image,    "is_save_bmp_image"   },
	{IDC_SAVE_RESULT_IMAGE,		  DLGTYPE_CHECK_BOOL,   &st.is_save_result_image, "is_save_result_image"},
// 	{IDC_SAVE_CURVE_DATA,		  DLGTYPE_CHECK_BOOL,   &st.is_save_curve_data,	  "is_save_curve_data"  },
// 	{IDC_SAVE_CURVE_IMAGE,		  DLGTYPE_CHECK_BOOL,   &st.is_save_curve_image,  "is_save_curve_image" },
	{IDC_SAVE,					  DLGTYPE_CHECK_BOOL,   &st.is_save_report,		  "is_save_report"		},
	{NULL, NULL, NULL, NULL}
};

INIEdit st_color[] = {
	{IDC_COLOR_ONOFF,	 DLGTYPE_CHECK_BOOL,  &st.is_color_test_on, "is_color_test_on"},
	{IDC_USE_LAB,		 DLGTYPE_CHECK_BOOL,  &st.use_lab,			"use_lab"},
	{IDC_ONOFF_1,		 DLGTYPE_CHECK_BOOL,  &st.color_box_on[0],  "color_box1_on"},
	{IDC_ONOFF_2,		 DLGTYPE_CHECK_BOOL,  &st.color_box_on[1],  "color_box2_on"},
	{IDC_ONOFF_3,		 DLGTYPE_CHECK_BOOL,  &st.color_box_on[2],  "color_box3_on"},
	{IDC_ONOFF_4,		 DLGTYPE_CHECK_BOOL,  &st.color_box_on[3],  "color_box4_on"},
	{IDC_ONOFF_5,		 DLGTYPE_CHECK_BOOL,  &st.color_box_on[4],  "color_box5_on"},
	{IDC_ONOFF_6,		 DLGTYPE_CHECK_BOOL,  &st.color_box_on[5],  "color_box6_on"},

	{IDC_COLOR_USPEC1_R, DLGTYPE_INPUT_INT, &st.ref_r[0], "ref_r1"},
	{IDC_COLOR_USPEC2_R, DLGTYPE_INPUT_INT, &st.ref_r[1], "ref_r2"},
	{IDC_COLOR_USPEC3_R, DLGTYPE_INPUT_INT, &st.ref_r[2], "ref_r3"},
	{IDC_COLOR_USPEC4_R, DLGTYPE_INPUT_INT, &st.ref_r[3], "ref_r4"},
	{IDC_COLOR_USPEC5_R, DLGTYPE_INPUT_INT, &st.ref_r[4], "ref_r5"},
	{IDC_COLOR_USPEC6_R, DLGTYPE_INPUT_INT, &st.ref_r[5], "ref_r6"},

	{IDC_COLOR_USPEC1_G, DLGTYPE_INPUT_INT, &st.ref_g[0], "ref_g1"},
	{IDC_COLOR_USPEC2_G, DLGTYPE_INPUT_INT, &st.ref_g[1], "ref_g2"},
	{IDC_COLOR_USPEC3_G, DLGTYPE_INPUT_INT, &st.ref_g[2], "ref_g3"},
	{IDC_COLOR_USPEC4_G, DLGTYPE_INPUT_INT, &st.ref_g[3], "ref_g4"},
	{IDC_COLOR_USPEC5_G, DLGTYPE_INPUT_INT, &st.ref_g[4], "ref_g5"},
	{IDC_COLOR_USPEC6_G, DLGTYPE_INPUT_INT, &st.ref_g[5], "ref_g6"},

	{IDC_COLOR_USPEC1_B, DLGTYPE_INPUT_INT, &st.ref_b[0], "ref_b1"},
	{IDC_COLOR_USPEC2_B, DLGTYPE_INPUT_INT, &st.ref_b[1], "ref_b2"},
	{IDC_COLOR_USPEC3_B, DLGTYPE_INPUT_INT, &st.ref_b[2], "ref_b3"},
	{IDC_COLOR_USPEC4_B, DLGTYPE_INPUT_INT, &st.ref_b[3], "ref_b4"},
	{IDC_COLOR_USPEC5_B, DLGTYPE_INPUT_INT, &st.ref_b[4], "ref_b5"},
	{IDC_COLOR_USPEC6_B, DLGTYPE_INPUT_INT, &st.ref_b[5], "ref_b6"},

	{IDC_COLOR_DISSPEC1, DLGTYPE_INPUT_FLOAT, &st.color_spec[0], "color_spec1"	},
	{IDC_COLOR_DISSPEC2, DLGTYPE_INPUT_FLOAT, &st.color_spec[1], "color_spec2"	},
	{IDC_COLOR_DISSPEC3, DLGTYPE_INPUT_FLOAT, &st.color_spec[2], "color_spec3"	},
	{IDC_COLOR_DISSPEC4, DLGTYPE_INPUT_FLOAT, &st.color_spec[3], "color_spec4"	},
	{IDC_COLOR_DISSPEC5, DLGTYPE_INPUT_FLOAT, &st.color_spec[4], "color_spec5"	},
	{IDC_COLOR_DISSPEC6, DLGTYPE_INPUT_FLOAT, &st.color_spec[5], "color_spec6"	},

	{IDC_XPOS_1, DLGTYPE_INPUT_INT,   &st.color_box_x[0], "color_box1_x"},
	{IDC_XPOS_2, DLGTYPE_INPUT_INT,   &st.color_box_x[1], "color_box2_x"},
	{IDC_XPOS_3, DLGTYPE_INPUT_INT,   &st.color_box_x[2], "color_box3_x"},
	{IDC_XPOS_4, DLGTYPE_INPUT_INT,   &st.color_box_x[3], "color_box4_x"},
	{IDC_XPOS_5, DLGTYPE_INPUT_INT,   &st.color_box_x[4], "color_box5_x"},
	{IDC_XPOS_6, DLGTYPE_INPUT_INT,   &st.color_box_x[5], "color_box6_x"},

	{IDC_YPOS_1, DLGTYPE_INPUT_INT,   &st.color_box_y[0], "color_box1_y"},
	{IDC_YPOS_2, DLGTYPE_INPUT_INT,   &st.color_box_y[1], "color_box2_y"},
	{IDC_YPOS_3, DLGTYPE_INPUT_INT,   &st.color_box_y[2], "color_box3_y"},
	{IDC_YPOS_4, DLGTYPE_INPUT_INT,   &st.color_box_y[3], "color_box4_y"},
	{IDC_YPOS_5, DLGTYPE_INPUT_INT,   &st.color_box_y[4], "color_box5_y"},
	{IDC_YPOS_6, DLGTYPE_INPUT_INT,   &st.color_box_y[5], "color_box6_y"},

	{IDC_BOX_WIDTH,		 DLGTYPE_INPUT_INT,   &st.color_box_w,	  "color_box_w"},
	{IDC_BOX_HEIGHT,	 DLGTYPE_INPUT_INT,   &st.color_box_h,	  "color_box_h"},
	{NULL, NULL, NULL, NULL}
};

INIEdit st_gray[] = {
	{IDC_DYNAMIC_ONOFF,   DLGTYPE_CHECK_BOOL,  &st.is_gray_test_on, "is_gray_test_on"},
	{IDC_XPOS_7,		  DLGTYPE_INPUT_INT,   &st.gray_box_x[0],   "gray_box1_x"},
	{IDC_XPOS_8,		  DLGTYPE_INPUT_INT,   &st.gray_box_x[1],   "gray_box2_x"},
	{IDC_XPOS_9,		  DLGTYPE_INPUT_INT,   &st.gray_box_x[2],   "gray_box3_x"},
	{IDC_XPOS_10,		  DLGTYPE_INPUT_INT,   &st.gray_box_x[3],   "gray_box4_x"},
	{IDC_XPOS_11,		  DLGTYPE_INPUT_INT,   &st.gray_box_x[4],   "gray_box5_x"},
	{IDC_XPOS_12,		  DLGTYPE_INPUT_INT,   &st.gray_box_x[5],   "gray_box6_x"},

	{IDC_YPOS_7,		  DLGTYPE_INPUT_INT,   &st.gray_box_y[0],   "gray_box1_y"},
	{IDC_YPOS_8,		  DLGTYPE_INPUT_INT,   &st.gray_box_y[1],   "gray_box2_y"},
	{IDC_YPOS_9,		  DLGTYPE_INPUT_INT,   &st.gray_box_y[2],   "gray_box3_y"},
	{IDC_YPOS_10,		  DLGTYPE_INPUT_INT,   &st.gray_box_y[3],   "gray_box4_y"},
	{IDC_YPOS_11,		  DLGTYPE_INPUT_INT,   &st.gray_box_y[4],   "gray_box5_y"},
	{IDC_YPOS_12,		  DLGTYPE_INPUT_INT,   &st.gray_box_y[5],   "gray_box6_y"},

	{IDC_DR_BOXNUM,		  DLGTYPE_INPUT_INT,   &st.gray_test_box_count,		   "gray_test_box_count"	},
	{IDC_DR_VAR,		  DLGTYPE_INPUT_FLOAT, &st.gray_luminance_change_spec, "gray_luminance_change_spec"	},
	{IDC_DR_PATCHCNT,	  DLGTYPE_INPUT_INT,   &st.gray_pass_box_count_spec,   "gray_pass_box_count_spec"	},
	{IDC_BOX_WIDTH2,	  DLGTYPE_INPUT_INT,   &st.gray_box_w,				   "gray_box_width"		},
	{IDC_BOX_HEIGHT2,	  DLGTYPE_INPUT_INT,   &st.gray_box_h,				   "gray_box_height"		},

	{IDC_SN_TEST_ON,	  DLGTYPE_CHECK_BOOL,  &st.is_sn_test_on,	"is_sn_test_on"},
	{IDC_SN_GRAY_ROI_NUM, DLGTYPE_COMBO_INT,   &st.sn_gray_box_num,	"sn_gray_box_num"},
	{IDC_SN_SPEC,		  DLGTYPE_INPUT_FLOAT, &st.sn_spec,			"sn_spec"},
	{NULL, NULL, NULL, NULL}
};

INIEdit st_oc[] = {
	{IDC_OAC_TEST_ON,		DLGTYPE_CHECK_BOOL, &st.is_oc_test_on,		"is_oc_test_on"		},
	{IDC_OAC_X_OFFSET_SPEC, DLGTYPE_INPUT_INT,	&st.oc_x_offset_spec,	"oc_x_offset_spec"	},
	{IDC_OAC_Y_OFFSET_SPEC, DLGTYPE_INPUT_INT,	&st.oc_y_offset_spec,	"oc_y_offset_spec"	}
};
INIEdit st_rotate[] = {
	{IDC_ROTATE_TEST_ON,	DLGTYPE_CHECK_BOOL,	 &st.is_rotate_test_on,	"is_rotate_test_on"	},
	{IDC_ROTATE_SPEC,		DLGTYPE_INPUT_FLOAT, &st.rotate_spec,		"rotate_spec"		},
	{NULL, NULL, NULL, NULL}
};

class StdChartDlg:public ai_win::Dlg
{
public:
	virtual BOOL init(HWND hdlg, HWND hfocus, LPARAM linit);
	virtual void command(HWND hdlg, int id, HWND hctrl, UINT code_notify);
	virtual BOOL notify(HWND hdlg, int idctrl, LPNMHDR pnmh);
	virtual void close(HWND hdlg);

private:
	int tab_cnt;
	HWND htab;

};

StdChartDlg st_dlg;

BOOL CALLBACK stdchart_setting_dlg_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_DLGMSG(hdlg, WM_INITDIALOG,	st_dlg.init);
		HANDLE_DLGMSG(hdlg, WM_COMMAND,		st_dlg.command);
		HANDLE_DLGMSG(hdlg, WM_NOTIFY,		st_dlg.notify);
		HANDLE_DLGMSG(hdlg, WM_CLOSE,		st_dlg.close);
	}

	return FALSE;
}

BOOL StdChartDlg::init(HWND hdlg, HWND hfocus, LPARAM linit)
{	
	sprintf(st_path, "%s\\recipe\\%s\\standard_chart.ini", s_aa::dir, s_aa::recipe_name);
	ai_win::load_keys(st_path, "FiducialMark", hdlg, st_fm, sizeof(st_fm)/sizeof(st_fm[0]));

	htab = GetDlgItem(hdlg, IDC_ST_TAB);

	TCITEM tc_item;
	tc_item.mask = TCIF_TEXT;
	tc_item.iImage = 1;
	tc_item.lParam = 0;
	HINSTANCE st_inst = (HINSTANCE)GetWindowLong(hdlg, GWL_HINSTANCE);
	RECT rt;
	GetClientRect(htab, &rt);
	tab_cnt = sizeof(st_tab)/sizeof(TabInfo);
	for (int i=0; i<tab_cnt; i++){
		if (!st_tab[i].is_enabled) continue;
		tc_item.pszText    = st_tab[i].title;
		tc_item.cchTextMax = tab_cnt;
		SendMessage(htab, TCM_INSERTITEM, i, (LPARAM)&tc_item);
		st_tab[i].hchild = CreateDialogParam((HINSTANCE)GetWindowLong(hdlg, GWL_HINSTANCE), MAKEINTRESOURCE(st_tab[i].idd), htab, (DLGPROC)st_tab[i].proc, 0);
		MoveWindow(st_tab[i].hchild, rt.left+1, rt.top+22, rt.right-rt.left-5, rt.bottom-rt.top-24, TRUE);
	}
	ShowWindow(st_tab[0].hchild, SW_NORMAL);

	return TRUE;
}

BOOL StdChartDlg::notify(HWND hdlg, int idctrl, LPNMHDR pnmh)
{
	int i = 0;
	if (pnmh->code == TCN_SELCHANGE){
		for (int j=0; j<tab_cnt; j++){
			ShowWindow(st_tab[j].hchild, SW_HIDE);
		}
		i = SendMessage(htab, TCM_GETCURSEL, 0, 0);
		//		send_log(av_img::hlog, "i=%d", i);
		int real_count = 0;
		int m = 0;
		for (m=0; m<=tab_cnt; m++){
			if (st_tab[m].is_enabled) real_count++;
			if (real_count == i+1) break;
		}

		ShowWindow(st_tab[m].hchild, SW_SHOWDEFAULT);
	}

	return FALSE;
}

void StdChartDlg::command(HWND hdlg, int id, HWND hctrl, UINT code_notify)
{
	switch (id)
	{
	case IDC_SAVE:
		ai_win::save_keys(st_path, "FiducialMark", hdlg,		  st_fm,	 sizeof(st_fm)/sizeof(st_fm[0]));
		ai_win::save_keys(st_path, "SFR",		st_tab[0].hchild, st_sfr,	 sizeof(st_sfr)/sizeof(st_sfr[0]));
		ai_win::save_keys(st_path, "Color",		st_tab[1].hchild, st_color,  sizeof(st_color)/sizeof(st_color[0]));
		ai_win::save_keys(st_path, "Gray",		st_tab[2].hchild, st_gray,	 sizeof(st_gray)/sizeof(st_gray[0]));
		ai_win::save_keys(st_path, "OC",		st_tab[3].hchild, st_oc,	 sizeof(st_oc)/sizeof(st_oc[0]));
		ai_win::save_keys(st_path, "Rotation",	st_tab[3].hchild, st_rotate, sizeof(st_rotate)/sizeof(st_rotate[0]));
		break;
	}
}

void StdChartDlg::close(HWND hdlg)
{
	EndDialog(hdlg, TRUE);
}

class STFocusDlg:public ai_win::Dlg
{
public:
	virtual BOOL init(HWND hdlg, HWND hfocus, LPARAM linit);
	virtual void command(HWND hdlg, int id, HWND hctrl, UINT code_notify);

private:
};

STFocusDlg stf_dlg;
BOOL CALLBACK st_focus_dlg(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_DLGMSG(hdlg, WM_INITDIALOG, stf_dlg.init);
		HANDLE_DLGMSG(hdlg, WM_COMMAND,	   stf_dlg.command);
	}

	return FALSE;
}

BOOL STFocusDlg::init(HWND hdlg, HWND hfocus, LPARAM linit)
{
	ai_win::load_keys(st_path, "SFR", hdlg, st_sfr, sizeof(st_sfr)/sizeof(st_sfr[0]));
	SetDlgItemInt(hdlg, IDC_FOCUS_TARGET_TOLERANCE, STD_FOCUS_CURVE_RANGE, FALSE);

	return TRUE;
}

void STFocusDlg::command(HWND hdlg, int id, HWND hctrl, UINT code_notify)
{

}

class STColorDlg:public ai_win::Dlg
{
public:
	virtual BOOL init(HWND hdlg, HWND hfocus, LPARAM linit);
	virtual void command(HWND hdlg, int id, HWND hctrl, UINT code_notify);

private:
	int is_init_done;
};

STColorDlg stc_dlg;
BOOL CALLBACK st_color_dlg(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_DLGMSG(hdlg, WM_INITDIALOG, stc_dlg.init);
		HANDLE_DLGMSG(hdlg, WM_COMMAND,	   stc_dlg.command);
	}

	return FALSE;
}

BOOL STColorDlg::init(HWND hdlg, HWND hfocus, LPARAM linit)
{
	is_init_done = FALSE;
	int color_box_x_spin[] = {IDC_SPIN40, IDC_SPIN42, IDC_SPIN44, IDC_SPIN46, IDC_SPIN48, IDC_SPIN50};
	int color_box_y_spin[] = {IDC_SPIN41, IDC_SPIN43, IDC_SPIN45, IDC_SPIN47, IDC_SPIN49, IDC_SPIN51};
	for (int i=0; i<6; i++){
		SET_SPINNER_RANGE(hdlg, color_box_x_spin[i], 0, IMG_WIDTH);
		SET_SPINNER_RANGE(hdlg, color_box_y_spin[i], 0, IMG_HEIGHT);
	}
	ai_win::load_keys(st_path, "Color", hdlg, st_color, sizeof(st_color)/sizeof(st_color[0]));
	is_init_done = TRUE;
	return TRUE;
}

void STColorDlg::command(HWND hdlg, int id, HWND hctrl, UINT code_notify)
{
	switch (code_notify)
	{
	case EN_CHANGE:
		if (is_init_done == TRUE){
			switch (id)
			{
			case IDC_XPOS_1: st.color_box_x[0] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_XPOS_2: st.color_box_x[1] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_XPOS_3: st.color_box_x[2] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_XPOS_4: st.color_box_x[3] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_XPOS_5: st.color_box_x[4] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_XPOS_6: st.color_box_x[5] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;

			case IDC_YPOS_1: st.color_box_y[0] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_YPOS_2: st.color_box_y[1] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_YPOS_3: st.color_box_y[2] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_YPOS_4: st.color_box_y[3] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_YPOS_5: st.color_box_y[4] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_YPOS_6: st.color_box_y[5] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			}
		}
		break;
	}
}

class STGrayDlg:public ai_win::Dlg
{
public:
	virtual BOOL init(HWND hdlg, HWND hfocus, LPARAM linit);
	virtual void command(HWND hdlg, int id, HWND hctrl, UINT code_notify);

private:
	int is_init_done;
};

STGrayDlg stg_dlg;
BOOL CALLBACK st_gray_dlg(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_DLGMSG(hdlg, WM_INITDIALOG, stg_dlg.init);
		HANDLE_DLGMSG(hdlg, WM_COMMAND,	   stg_dlg.command);
	}

	return FALSE;
}

BOOL STGrayDlg::init(HWND hdlg, HWND hfocus, LPARAM linit)
{
	is_init_done = FALSE;
	int gray_box_x_spin[] = {IDC_SPIN1, IDC_SPIN2, IDC_SPIN3, IDC_SPIN4, IDC_SPIN5, IDC_SPIN6};
	int gray_box_y_spin[] = {IDC_SPIN11, IDC_SPIN12, IDC_SPIN13, IDC_SPIN14, IDC_SPIN15, IDC_SPIN16};
	char temp[20] = {0};
	for (int i=0; i<6; i++){
		sprintf(temp, "%d", i);
		COMBO_ADDSTR(hdlg, IDC_SN_GRAY_ROI_NUM, temp);
		SET_SPINNER_RANGE(hdlg, gray_box_x_spin[i], IMG_WIDTH, 0);
		SET_SPINNER_RANGE(hdlg, gray_box_y_spin[i], 0, IMG_HEIGHT);
	}

	ai_win::load_keys(st_path, "Gray", hdlg, st_gray, sizeof(st_gray)/sizeof(st_gray[0]));
	is_init_done = TRUE;
	return TRUE;
}

void STGrayDlg::command(HWND hdlg, int id, HWND hctrl, UINT code_notify)
{
	switch (code_notify)
	{
	case EN_CHANGE:
		if (is_init_done == TRUE){
			switch (id)
			{
			case IDC_XPOS_7: st.gray_box_x[0]  = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_XPOS_8: st.gray_box_x[1]  = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_XPOS_9: st.gray_box_x[2]  = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_XPOS_10: st.gray_box_x[3] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_XPOS_11: st.gray_box_x[4] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_XPOS_12: st.gray_box_x[5] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;

			case IDC_YPOS_7: st.gray_box_y[0]  = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_YPOS_8: st.gray_box_y[1]  = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_YPOS_9: st.gray_box_y[2]  = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_YPOS_10: st.gray_box_y[3] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_YPOS_11: st.gray_box_y[4] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_YPOS_12: st.gray_box_y[5] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			}
		}
		break;
	}
}

class STOCDlg:public ai_win::Dlg
{
public:
	virtual BOOL init(HWND hdlg, HWND hfocus, LPARAM linit);
	virtual void command(HWND hdlg, int id, HWND hctrl, UINT code_notify);

private:
};

STOCDlg oc_dlg;
BOOL CALLBACK st_oc_dlg(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_DLGMSG(hdlg, WM_INITDIALOG, oc_dlg.init);
		HANDLE_DLGMSG(hdlg, WM_COMMAND,	   oc_dlg.command);
	}

	return FALSE;
}

BOOL STOCDlg::init(HWND hdlg, HWND hfocus, LPARAM linit)
{
	ai_win::load_keys(st_path, "OC", hdlg, st_oc, sizeof(st_oc)/sizeof(st_oc[0]));
	return TRUE;
}

void STOCDlg::command(HWND hdlg, int id, HWND hctrl, UINT code_notify)
{

}


class STRotationDlg:public ai_win::Dlg
{
public:
	virtual BOOL init(HWND hdlg, HWND hfocus, LPARAM linit);
	virtual void command(HWND hdlg, int id, HWND hctrl, UINT code_notify);

private:
};

STRotationDlg rotate_dlg;
BOOL CALLBACK st_rotation_dlg(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_DLGMSG(hdlg, WM_INITDIALOG, rotate_dlg.init);
		HANDLE_DLGMSG(hdlg, WM_COMMAND,	   rotate_dlg.command);
	}

	return FALSE;
}

BOOL STRotationDlg::init(HWND hdlg, HWND hfocus, LPARAM linit)
{
	ai_win::load_keys(st_path, "Rotation", hdlg, st_rotate, sizeof(st_rotate)/sizeof(st_rotate[0]));
	return TRUE;
}

void STRotationDlg::command(HWND hdlg, int id, HWND hctrl, UINT code_notify)
{

}



//////////////////////////////////////////////////////////////////////////
//
void recognize_crossline(const BYTE *bmp, int bmp_w, int bmp_h)
{
	int thickness = st.cl_thinkness;
	int length    = st.cl_length;
	int sb_size	  = st.sb_size;

	if (sb_size>(bmp_w/4)) sb_size = bmp_w/4;
	else if (sb_size<1)	   sb_size = 1;

	int sb_to_img_cx = st.sb_to_img_cx;
	int sb_to_img_cy = st.sb_to_img_cy;

	if ((sb_to_img_cx+sb_size/2) > bmp_w/2) sb_to_img_cx = bmp_w/2 - sb_size/2 - 5;
	if (sb_to_img_cx<sb_size/2)				sb_to_img_cx = sb_size/2;

	if ((sb_to_img_cy+sb_size/2) > bmp_h/2) sb_to_img_cy = bmp_h/2 - sb_size/2 - 5;
	if (sb_to_img_cy<sb_size/2)				sb_to_img_cy = sb_size/2;

	st.scanbox[0].left = bmp_w/2 - sb_size/2 - sb_to_img_cx;
	st.scanbox[0].top  = bmp_h/2 - sb_size/2;
	st.scanbox[1].left = bmp_w/2 - sb_size/2;
	st.scanbox[1].top  = bmp_h/2 - sb_size/2 - sb_to_img_cy;
	st.scanbox[2].left = bmp_w/2 - sb_size/2 + sb_to_img_cx;
	st.scanbox[2].top  = bmp_h/2 - sb_size/2;
	st.scanbox[3].left = bmp_w/2 - sb_size/2;
	st.scanbox[3].top  = bmp_h/2 - sb_size/2 + sb_to_img_cy;

	if (length<3)		length = 3;
	if (length>sb_size) length = sb_size;

	if (thickness<3)	  thickness = 3;
	if (thickness>length) thickness = length;

	for (int i=0; i<4; i++){
		st.scanbox[i].right  = st.scanbox[i].left + sb_size;
		st.scanbox[i].bottom = st.scanbox[i].top + sb_size;
		cross_line::get_center(bmp, bmp_w, bmp_h, st.scanbox[i], length, thickness, 
			st.cl_center_x[i], st.cl_center_y[i]);
	}

	st.cl_x_distance = cross_line::get_2p_distance((float)st.cl_center_x[2], (float)st.cl_center_y[2], (float)st.cl_center_x[0], (float)st.cl_center_y[0]);
	st.cl_y_distance = cross_line::get_2p_distance((float)st.cl_center_x[3], (float)st.cl_center_y[3], (float)st.cl_center_x[1], (float)st.cl_center_y[1]);

	st.chart_center_x = (double)(st.cl_center_x[0]+st.cl_center_x[1]+st.cl_center_x[2]+st.cl_center_x[3])/4;
	st.chart_center_y = (double)(st.cl_center_y[0]+st.cl_center_y[1]+st.cl_center_y[2]+st.cl_center_y[3])/4;

	st.oc_x_pos = (int)st.chart_center_x;
	st.oc_y_pos = (int)st.chart_center_y;

	st.center_x_offset = (int)(st.chart_center_x - bmp_w/2);
	st.center_y_offset = (int)(st.chart_center_y - bmp_h/2);

	st.chart_rotate_angle = cross_line::get_rotated_angle(st.cl_center_x, st.cl_center_y);

}

void init_sfr_roi(void)
{
	int index[SFR_ROI_NUM]={0,			// C_H
		1,1,1,1,	// Fa_H
		2,2,2,2,	// Fb_H
		3,3,3,3,	// Fc_H
		0,			// C_V
		1,1,1,1,	// Fa_V
		2,2,2,2,	// Fb_V
		3,3,3,3,	// Fc_V
	};

	int sign_x[SFR_ROI_NUM] = {  0,				
		-1, 1, -1, 1,
		-1, 1, -1, 1,
		-1, 1, -1, 1,
		1,
		-1, 1, -1, 1,
		-1, 1, -1, 1,
		-1, 1, -1, 1
	};

	int sign_y[SFR_ROI_NUM] = {  0,				
		-1, -1, 1, 1,
		-1, -1, 1, 1,
		-1, -1, 1, 1,
		1,
		-1, -1, 1, 1,
		-1, -1, 1, 1,
		-1,- 1, 1, 1
	};

	double signed_roi_to_ccx[SFR_ROI_NUM] = {0.0};
	double signed_roi_to_ccy[SFR_ROI_NUM] = {0.0};
	int   sfr_roi_w[SFR_ROI_NUM] = {0};
	int	  sfr_roi_h[SFR_ROI_NUM] = {0};

	double roi_to_ccx_units = 0.0;
	double roi_to_ccy_units = 0.0;
	double rotated_roi_to_ccx_unit = 0.0;
	double rotated_roi_to_ccy_unit = 0.0;

	for(int i=0; i<SFR_ROI_NUM; i++){
		if (i< (SFR_ROI_NUM/2)){	//horizontal SFR
			signed_roi_to_ccx[i] = st.roi_to_ccx[index[i]]*sign_x[i];
			signed_roi_to_ccy[i] = st.roi_to_ccy[index[i]]*sign_y[i];
		}
		else{
			signed_roi_to_ccx[i] = (st.roi_to_ccx[index[i]] + st.v_offset_x)*sign_x[i];
			signed_roi_to_ccy[i] = (st.roi_to_ccy[index[i]] + st.v_offset_y)*sign_y[i];
		}

		sfr_roi_w[i] = st.roi_w[index[i]];
		sfr_roi_h[i] = st.roi_h[index[i]];

		roi_to_ccx_units = signed_roi_to_ccx[i]*st.cl_x_distance;
		roi_to_ccy_units = signed_roi_to_ccy[i]*st.cl_y_distance;

		cross_line::get_rotated_new_xy(roi_to_ccx_units, roi_to_ccy_units, st.chart_rotate_angle, rotated_roi_to_ccx_unit, rotated_roi_to_ccy_unit);

		st.sfr_roi[i].left = (long)(st.chart_center_x + rotated_roi_to_ccx_unit - sfr_roi_w[i]/2);
		if (st.sfr_roi[i].left < 0)			 st.sfr_roi[i].left = 0;
		if (st.sfr_roi[i].left >= IMG_WIDTH) st.sfr_roi[i].left = IMG_WIDTH-1;

		st.sfr_roi[i].top = (long)(st.chart_center_y + rotated_roi_to_ccy_unit - sfr_roi_h[i]/2);
		if (st.sfr_roi[i].top < 0)			 st.sfr_roi[i].top = 0;
		if (st.sfr_roi[i].top >= IMG_HEIGHT) st.sfr_roi[i].top = IMG_HEIGHT-1;

		st.sfr_roi[i].right = (long)(st.chart_center_x + rotated_roi_to_ccx_unit + sfr_roi_w[i]/2);
		if (st.sfr_roi[i].right < 0)		  st.sfr_roi[i].right = 0;
		if (st.sfr_roi[i].right >= IMG_WIDTH) st.sfr_roi[i].right = IMG_WIDTH-1;

		st.sfr_roi[i].bottom = (long)(st.chart_center_y + rotated_roi_to_ccy_unit + sfr_roi_h[i]/2);
		if (st.sfr_roi[i].bottom < 0)			st.sfr_roi[i].bottom = 0;
		if (st.sfr_roi[i].bottom >= IMG_HEIGHT) st.sfr_roi[i].bottom = IMG_HEIGHT-1;
	}
}

BOOL insert_sfr_info_to_linked_list(double *sfr_val, int x_distance)
{
	int cnt = 0;
	PCurveNode new_node = new CurveNode[sizeof(CurveNode)];
	new_node->center = sfr_val[0];
	for (int i=0; i<4; i++){
		new_node->edge[i] = sfr_val[i+1];
	}
	new_node->x_distance = x_distance;
	new_node->next = NULL;

	PCurveNode last_node = cv_root;

	if (cv_root == NULL){
		cv_root = new_node;
		return TRUE;
	}

	int d_up_spec = st.target_value*10 + STD_FOCUS_CURVE_RANGE;
	int d_dn_spec = st.target_value*10 - STD_FOCUS_CURVE_RANGE;

	for (PCurveNode p=cv_root; p!=NULL; last_node=p, p=p->next){
		cnt++;
	}

	if (sfr_val == NULL || x_distance >d_up_spec || x_distance <d_dn_spec || sfr_val[0] <st.target_ignore_spec){
		delete [] new_node;
		return FALSE;
	}

	for (PCurveNode p=cv_root; p!=NULL; last_node=p, p=p->next){
		if (x_distance < p->x_distance && p == cv_root){ //at the begin
			new_node->next = p;
			cv_root = new_node;
			break;
		}
		else if (x_distance < p->x_distance && p != cv_root){ //add to list, not begin or end;
			last_node->next = new_node;
			new_node->next  = p;
			break;
		}
		else if (x_distance == p->x_distance){ // save distance, modify
			p->center = sfr_val[0];
			for (int i=0; i<4; i++){
				p->edge[i] = sfr_val[i+1];
			}
			delete[] new_node;
			break;
		}
		else if (x_distance > p->x_distance && p->next == NULL){ // at the end
			p->next = new_node;
			new_node->next = NULL;
			break;
		}
		else if (x_distance > p->x_distance && p->next != NULL && x_distance < p->next->x_distance){
			new_node->next = p->next;
			p->next = new_node;
			break;
		}
		else{	// check next data
			continue;
		}
	}
	cnt++;
	return cnt;
}

int std_sfr_test(const BYTE *bmp, int bmp_w, int bmp_h)
{
	char file_name[100] = {0};
	int result = TRUE;
	int w=0, h=0, t=0;
	BYTE *roi_buf = NULL;
	double spec1=0.0, spec2=0.0;
	double esf[5000] = {0.0};
	double lsf[5000];
	double freq[5000];
	double amp[5000];
	double mtfxp_sfr = 0.0;
	double mtfxp_freq = 0.0;
	for (int i=0; i<SFR_ROI_NUM; i++){
		if ((i==0  && (!st.is_direct_h_on)) ||
			(i==13 && (!st.is_direct_v_on)) ||
			(i>=1  && i<5   &&((!st.is_direct_h_on) || (!st.is_field_a_on))) ||
			(i>=5  && i<9   &&((!st.is_direct_h_on) || (!st.is_field_b_on))) ||
			(i>=9  && i<13  &&((!st.is_direct_h_on) || (!st.is_field_c_on))) ||
			(i>=14 && i<18  &&((!st.is_direct_v_on) || (!st.is_field_a_on))) ||
			(i>=18 && i<22  &&((!st.is_direct_v_on) || (!st.is_field_b_on))) ||
			(i>=22 && i<=25 &&((!st.is_direct_v_on) || (!st.is_field_c_on))) ){
				st.sfr_result1[i] = 999;
				continue;
		}

		w = st.sfr_roi[i].right-st.sfr_roi[i].left;
		h = st.sfr_roi[i].bottom-st.sfr_roi[i].top;
		roi_buf = new BYTE[w*h];
		s_aa::cimg.bmp24_to_y8(bmp, bmp_w, bmp_h, &st.sfr_roi[i], roi_buf);
		if (st.is_direct_v_on && i>=13){
			ai_img::roi_column_to_row(roi_buf, w, h);
			t = w;
			w = h;
			h = t;
		}
		//	sprintf(file_name, "d:\\sfr_%d.bmp", i);
		//	ai_img::save_bmp8_image(file_name, roi_buf, w, h);
		st.sfr_val1[i] = 0.0f;
		st.sfr_val2[i] = 0.0f;
		sfr_proc(roi_buf, w, h, 1, &st.freq1, &st.sfr_val1[i], st.slope[i], st.offset[i], 
			esf, lsf, freq, amp);  

		if (st.is_use_mtf50p){
			mtfp(freq, amp, 70, mtfxp_sfr, mtfxp_freq, w*2);
			log("mtfxp=%d",mysbw.mtfxp);
			st.sfr_val1[i] = 100.0f*mtfxp_freq;
		}
		if (st.is_spec_2_on){
			sfr_proc(roi_buf, w, h, 1, &st.freq2, &st.sfr_val2[i], st.slope[i], st.offset[i], 
				esf, lsf, freq, amp);
			if (st.is_use_mtf50p){
				mtfp(freq, amp, 70, mtfxp_sfr, mtfxp_freq, w*2);
				st.sfr_val2[i] = 100.0f*mtfxp_freq;
			}
		}
		if (i<SFR_ROI_NUM/2){
			spec1 = st.spec1[(int)((i-1)/4.0+1)];
			spec2 = st.spec2[(int)((i-1)/4.0+1)];
		}
		else{
			spec1 = st.spec1[(int)((i-14)/4.0+1)];
			spec2 = st.spec2[(int)((i-14)/4.0+1)];
		}
		if (st.sfr_val1[i]<spec1){ 
			st.sfr_result1[i] = FALSE; 
			result = FALSE;
		}
		else st.sfr_result1[i] = TRUE;

		if (st.is_spec_2_on && st.sfr_val2[i]<spec2){ 
			st.sfr_result2[i] = FALSE; 
			result = FALSE;
		}
		else st.sfr_result2[i] = TRUE;


		delete [] roi_buf;
	}

	st.sfr_test_result = result;

	//check horizontal&vertical sfr diff
	if (st.is_tilt_test_on){
		st.tilt_test_result = TRUE;
		if (abs(st.sfr_val1[1]-st.sfr_val1[2])>st.h_tilt_spec ||
			abs(st.sfr_val1[3]-st.sfr_val1[4])>st.h_tilt_spec ||
			abs(st.sfr_val1[1]-st.sfr_val1[3])>st.v_tilt_spec ||
			abs(st.sfr_val1[2]-st.sfr_val1[4])>st.v_tilt_spec ){
				st.tilt_test_result = FALSE;
		}
		if (st.is_direct_v_on && st.tilt_test_result == TRUE){
			if (abs(st.sfr_val1[14]-st.sfr_val1[15])>st.h_tilt_spec ||
				abs(st.sfr_val1[16]-st.sfr_val1[17])>st.h_tilt_spec ||
				abs(st.sfr_val1[14]-st.sfr_val1[16])>st.v_tilt_spec ||
				abs(st.sfr_val1[15]-st.sfr_val1[17])>st.v_tilt_spec ){
					st.tilt_test_result = FALSE;
			}
		}
		if (st.tilt_test_result == FALSE){
			result = FALSE;
		}
	}

	return result;
}

int std_color_reproduction_test(BYTE *bmp, int bmp_w, int bmp_h, int is_lab)
{
	int result = TRUE;
	int start_x = 0, start_y = 0;
	int roi_w = st.color_box_w;
	int roi_h = st.color_box_h;
	float ref_y =0.0f, ref_cb=0.0f, ref_cr=0.0f;
	float avg_y=0.0f, avg_cb=0.0f, avg_cr=0.0f;
	float delta_e=0.0f, delta_cb=0.0f, delta_cr=0.0f;

	double ref_l=0, ref_a=0, ref_b=0;
	double avg_l=0, avg_a=0, avg_b=0;
	RECT rt;
	for (int i=0; i<6; i++){
		if (st.color_box_on[i] == TRUE){
			start_x = st.color_box_x[i] + st.center_x_offset;
			start_y = st.color_box_y[i] + st.center_y_offset;
			rt.left   = start_x;
			rt.top    = start_y;
			rt.right  = start_x+roi_w;
			rt.bottom = start_y+roi_h;

			if ((rt.right)>(bmp_w-1)) continue;
			if ((rt.bottom)>(bmp_h-1)) continue;

			start_x = (start_x<0)?1:start_x;
			start_y = (start_y<0)?1:start_y;

			start_x = (rt.right>bmp_w?(start_x-1-roi_w):start_x);
			start_y = (rt.bottom>bmp_h?(start_y-1-roi_h):start_y);

			if (is_lab){
				ai_img::rgb_to_lab(st.ref_r[i], st.ref_g[i], st.ref_b[i], ref_l, ref_a, ref_b);
				ai_img::get_bmp24_avg_lab(bmp, bmp_w, bmp_h, avg_l, avg_a, avg_b);
				delta_cb = (float)avg_a;
				delta_cr = (float)avg_b;
				delta_e = (float)(100*(sqrt(avg_a*avg_a+avg_b*avg_b)/sqrt(ref_a*ref_a+ref_b*ref_b)));

			}
			else{
				ai_img::rgb_to_yuv(st.ref_r[i], st.ref_g[i], st.ref_b[i], ref_y, ref_cb, ref_cr);
				st.color_cb[i] = ref_cb;
				st.color_cr[i] = ref_cr;
				ai_img::get_bmp24_avg_yuv(bmp, bmp_w, bmp_h, avg_y, avg_cb, avg_cr, &rt);

				delta_cb = ref_cb-avg_cb;
				delta_cr = ref_cr-avg_cr;
				delta_e = (float)sqrt((long double)(delta_cb*delta_cb+delta_cr*delta_cr));
			}
			if (delta_e <= st.color_spec[i]){
				st.color_test_result[i] = TRUE;
			}
			else{
				st.color_test_result[i] = FALSE;
				result = FALSE;
			}
			st.color_delta[i] = delta_e;
		}
	}
	st.color_reproduction_test_result = result;
	return result;
}

int std_gray_dynamic_range_test(BYTE *bmp, int bmp_w, int bmp_h)
{
	int result = TRUE;
	int roi_w = st.gray_box_w;
	int roi_h = st.gray_box_h;
	int start_x=0, start_y=0;
	RECT rt;
	float avg_y[6]={0.0f}, avg_cb=0.0f, avg_cr=0.0f;
	float y_ratio[6]={0.0f};

	for (int i=0; i<st.gray_test_box_count; i++){
		start_x = st.gray_box_x[i] + st.center_x_offset;
		start_y = st.gray_box_y[i] + st.center_y_offset;
		rt.left   = start_x;
		rt.top    = start_y;
		rt.right  = start_x+roi_w;
		rt.bottom = start_y+roi_h;
		if (rt.right>bmp_w-1) continue;
		if (rt.bottom>bmp_h-1) continue;

		start_x = ((start_x<0)?1:start_x);
		start_y = ((start_y<0)?1:start_y);

		start_x = ((rt.right>=bmp_w)?(bmp_w-1-roi_w):start_x);
		start_y = ((rt.bottom>=bmp_h)?(bmp_h-1-roi_h):start_y);
		ai_img::get_bmp24_avg_yuv(bmp, bmp_w, bmp_h, avg_y[i], avg_cb, avg_cr, &rt);
		st.gray_y_val[i] = avg_y[i];

	}

	int patch_cnt = 0;
	float min_val = 1000.0f;
	for (int i=0; i<st.gray_test_box_count-1; i++){
		if (avg_y[i+1]==0.0f) avg_y[i+1] = 0.0001f;

		y_ratio[i] = (float)(100.0f-(avg_y[i]*100.0f/avg_y[i+1]));
		if (y_ratio[i]<0.0f) y_ratio[i] *= (-1.0f);
		if (y_ratio[i]>=st.gray_luminance_change_spec){
			patch_cnt++;
			st.gray_var_result[i] = TRUE;
		}
		else st.gray_var_result[i] = FALSE;

		st.gray_var_val[i] = y_ratio[i];
		if (st.gray_var_val[i]<min_val){
			min_val = st.gray_var_val[i];
		}
	}
	st.gray_min_var_result = min_val;
	st.gray_pass_box_count = patch_cnt+1;
	if (st.gray_pass_box_count >= st.gray_pass_box_count_spec){
		st.gray_test_result = TRUE;
	}
	else{
		st.gray_test_result = FALSE;
		result = FALSE;
	}

	return result;
}

int std_snr_test(BYTE *bmp_y, int bmp_w, int bmp_h)
{
	double result_db;
	double s1, n1;
	int n = st.sn_gray_box_num;
	RECT rt;
	int start_x = st.gray_box_x[n]+st.center_x_offset;
	int start_y = st.gray_box_y[n]+st.center_y_offset;
	int roi_w = st.gray_box_w;
	int roi_h = st.gray_box_h;
	rt.left	  = start_x;
	rt.top	  = start_y;
	rt.right  = start_x+roi_w;
	rt.bottom = start_y+roi_h;
	BYTE *p = NULL;
	double sum = 0;
	for (int j=rt.top; j<rt.bottom; j++){
		for (int i=rt.left; i<rt.right; i++){
			p = bmp_y+PIXEL_Y(i,j,bmp_w,bmp_h);
			sum += (*p);
		}
	}
	double avg_val = sum/(roi_w*roi_h);
	s1 = avg_val;
	sum = 0;
	BYTE data =0;
	for (int j=rt.top; j<rt.bottom; j++){
		for (int i=rt.left; i<rt.right; i++){
			p = bmp_y+PIXEL_Y(i,j,bmp_w,bmp_h);
			data = (*p);
			sum += pow((double)(data-avg_val), 2);
		}
	}

	avg_val = sum/(roi_w*roi_h);
	n1 = sqrt(avg_val);
	if (n1 == 0){
		result_db = -1;
	}
	else result_db = 20.0*log10(s1/n1);
	st.sn_result = result_db;
	st.sn_test_result = (result_db>st.sn_spec)?TRUE:FALSE;

	return st.sn_test_result;
}

int std_oc_test(void)
{
	st.oc_test_result = TRUE;
	if (abs(st.center_x_offset)>st.oc_x_offset_spec || 
		abs(st.center_y_offset)>st.oc_y_offset_spec){
			st.oc_test_result = FALSE;
	}

	return st.oc_test_result;
}

int std_rotate_angle_test(void)
{
	st.rotate_test_result = TRUE;
	st.rotate_result = (float)st.chart_rotate_angle;
	if (fabs(st.chart_rotate_angle) > st.rotate_spec){
		st.rotate_test_result = FALSE;
	}

	return st.rotate_test_result;
}

void save_st_chart_report(void)
{
	//focus 

}

//////////////////////////////////////////////////////////////////////////
//interface function

int init_std_chart_test(void)
{
	char path[MAX_PATH] = {0};
	sprintf(path, "%s\\recipe\\%s\\standard_chart.ini", s_aa::dir, s_aa::recipe_name);

	ai_win::load_keys(path, "FiducialMark", HWND_STD_CHART, st_fm,	   sizeof(st_fm)/sizeof(st_fm[0]));
	ai_win::load_keys(path, "SFR",			HWND_STD_CHART, st_sfr,    sizeof(st_sfr)/sizeof(st_sfr[0]));
	ai_win::load_keys(path, "Color",		HWND_STD_CHART, st_color,  sizeof(st_color)/sizeof(st_color[0]));
	ai_win::load_keys(path, "Gray",			HWND_STD_CHART, st_gray,   sizeof(st_gray)/sizeof(st_gray[0]));
	ai_win::load_keys(path, "OC",			HWND_STD_CHART, st_oc,	   sizeof(st_oc)/sizeof(st_oc[0]));
	ai_win::load_keys(path, "Rotation",		HWND_STD_CHART, st_rotate, sizeof(st_rotate)/sizeof(st_rotate[0]));

	return TRUE;
}

int exec_std_chart_test(BYTE *bmp, int bmp_w, int bmp_h)
{
	recognize_crossline(bmp, bmp_w, bmp_h);
	init_sfr_roi();

	int result = TRUE;
	result = std_sfr_test(bmp, bmp_w, bmp_h);

	insert_sfr_info_to_linked_list(st.sfr_val1, (int)(st.cl_x_distance*10));

	if (st.is_color_test_on){
		std_color_reproduction_test(bmp, bmp_w, bmp_h, st.use_lab);
		if (st.color_reproduction_test_result == FALSE){
			result = FALSE;
		}
	}

	if (st.is_gray_test_on){
		std_gray_dynamic_range_test(bmp, bmp_w, bmp_h);
		if (st.gray_test_result == FALSE){
			result = FALSE;
		}
	}

	if (st.is_sn_test_on){
		BYTE *bmp_y = new BYTE[bmp_w*bmp_h];
		s_aa::cimg.bmp24_to_y8(bmp, bmp_w, bmp_h, NULL, bmp_y);
		//	ai_img::save_bmp8_image("d:\\x.bmp", bmp_y, bmp_w, bmp_h);
		std_snr_test(bmp_y, bmp_w, bmp_h);
		if (st.sn_test_result == FALSE){
			result = FALSE;
		}

		delete [] bmp_y;
	}

	if (st.is_oc_test_on){
		std_oc_test();
		if (st.oc_test_result == FALSE){
			result = FALSE;
		}
	}

	if (st.is_rotate_test_on){
		std_rotate_angle_test();
		if (st.rotate_test_result == FALSE){
			result = FALSE;
		}
	}

	return result;


}

void draw_std_chart(HDC memdc)
{
	int start_x = 0;
	int start_y = 0;
	int roi_w = 0;
	int roi_h = 0;
	//	DeleteObject(hfont);


	HFONT hfont	 = CreateFont(30, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "黑体");
	HFONT old_font = (HFONT)SelectObject(memdc, hfont);

	for (int i=0; i<4; i++){
		Rectangle(memdc, st.scanbox[i].left, st.scanbox[i].top, st.scanbox[i].right, st.scanbox[i].bottom);
	}

	//	DeleteObject(pen);
	HPEN pen		= CreatePen(PS_DASH, st.cl_thinkness, COLOR_YELLOW);
	HPEN old_pen = (HPEN)SelectObject(memdc, pen);
	for (int i=0; i<4; i++){
		ai_img::Output::draw_crossline(memdc, st.cl_center_x[i], st.cl_center_y[i], st.cl_length, st.cl_length);
	}
	int final_sfr_result = TRUE;
	char buffer[100] = {0};
	int spec2_offset_x = 0;
	int spec2_offset_y = 40;
	COLORREF cr;
	for (int i=0; i<SFR_ROI_NUM; i++){
		if (st.sfr_result1[i] == 999) continue;

		if (st.is_spec_2_on != TRUE){
			cr = ((st.sfr_result1[i]!=TRUE)?COLOR_RED:COLOR_GREEN); 
		}
		else{
			cr = ((st.sfr_result1[i]!=TRUE || st.sfr_result2[i]!=TRUE)?COLOR_RED:COLOR_GREEN);
		}
		DeleteObject(pen);
		pen = CreatePen(PS_SOLID, 3, cr);
		old_pen = (HPEN)SelectObject(memdc, pen);

		Rectangle(memdc, st.sfr_roi[i].left, st.sfr_roi[i].top, st.sfr_roi[i].right, st.sfr_roi[i].bottom);
		SetTextColor(memdc, (st.sfr_result1[i])?COLOR_BLUE:COLOR_RED);
		sprintf(buffer, "[%d.1] %.1f", i, st.sfr_val1[i]);
		TextOut(memdc, st.sfr_roi[i].left, st.sfr_roi[i].bottom + 10, buffer, strlen(buffer));

		if (st.is_spec_2_on){
			SetTextColor(memdc, (st.sfr_result2[i])?COLOR_BLUE:COLOR_RED);
			memset(buffer, 0, sizeof(buffer));
			sprintf(buffer, "[%d.2] %.1f", i, st.sfr_val2[i]);
			TextOut(memdc, st.sfr_roi[i].left+spec2_offset_x, st.sfr_roi[i].bottom+spec2_offset_y, buffer, strlen(buffer));
		}
	}

	ai_win::set_dlg_item_float(st_tab[0].hchild, IDC_FOCUS_REAL_VALUE, st.cl_x_distance, 2);
	DeleteObject(hfont);
	hfont	 = CreateFont(40, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "黑体");
	old_font = (HFONT)SelectObject(memdc, hfont);
	int index = 0;
	int offset = 40;
	if (st.sfr_test_result == FALSE){
		SetTextColor(memdc, COLOR_RED);
		TextOut(memdc, 20, 100+index, "-SFR Fail", 9);
	}

	if (st.is_tilt_test_on && st.tilt_test_result == FALSE){
		index += offset;
		SetTextColor(memdc, COLOR_RED);
		TextOut(memdc, 20, 100+index, "-Tilt Fail", 10);
	}

	if (st.is_color_test_on){

		SetBkMode(memdc, OPAQUE);
		DeleteObject(hfont);
		hfont	 = CreateFont(30, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "黑体");
		old_font = (HFONT)SelectObject(memdc, hfont);
		for (int i=0; i<6; i++){
			if (st.color_box_on[i]){
				DeleteObject(pen);
				pen = CreatePen(PS_SOLID, 3, (st.color_test_result[i]?COLOR_GREEN:COLOR_RED));
				old_pen = (HPEN)SelectObject(memdc, pen);
				start_x = st.color_box_x[i]+st.center_x_offset;
				start_y = st.color_box_y[i]+st.center_y_offset;
				roi_w = st.color_box_w;
				roi_h = st.color_box_h;
				Rectangle(memdc, start_x, start_y, start_x+roi_w, start_y+roi_h);
				SetTextColor(memdc, COLOR_BLUE);
				sprintf(buffer, "%d", i);
				TextOut(memdc, start_x+3, start_y+3, buffer, strlen(buffer));
				sprintf(buffer, "%.2f", st.color_delta[i]);
				SetTextColor(memdc, (st.color_test_result[i]?COLOR_GREEN:COLOR_RED));
				TextOut(memdc, start_x, start_y+roi_h+10, buffer, strlen(buffer));
			}
		}
		SetBkMode(memdc, TRANSPARENT);
		DeleteObject(hfont);
		hfont	 = CreateFont(40, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "黑体");
		old_font = (HFONT)SelectObject(memdc, hfont);
		if (st.color_reproduction_test_result == FALSE){
			index += offset;
			SetTextColor(memdc, COLOR_RED);
			TextOut(memdc, 20, 100+index, "-Color Fail", 11);
		}
	}


	if (st.is_gray_test_on){
		SetBkMode(memdc, OPAQUE);
		DeleteObject(hfont);
		hfont	 = CreateFont(30, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "黑体");
		old_font = (HFONT)SelectObject(memdc, hfont);
		for (int i=0; i<st.gray_test_box_count; i++){
			DeleteObject(pen);
			if (i == 0){
				pen = CreatePen(PS_SOLID, 3, COLOR_BLUE);
			}
			else pen = CreatePen(PS_SOLID, 3, (st.gray_var_result[i]?COLOR_GREEN:COLOR_RED));
			old_pen = (HPEN)SelectObject(memdc, pen);
			start_x = st.gray_box_x[i]+st.center_x_offset;
			start_y = st.gray_box_y[i]+st.center_y_offset;
			roi_w = st.gray_box_w;
			roi_h = st.gray_box_h;
			Rectangle(memdc, start_x, start_y, start_x+roi_w, start_y+roi_h);
			SetTextColor(memdc, COLOR_BLUE);
			sprintf(buffer, "%d", i);
			TextOut(memdc, start_x+3, start_y+3, buffer, strlen(buffer));
			sprintf(buffer, "%.2f", st.gray_y_val[i]);

			TextOut(memdc, start_x, start_y+roi_h+10, buffer, strlen(buffer));

			if (i>0){
				sprintf(buffer, "%.2f%%", st.gray_var_val[i-1]);
				SetTextColor(memdc, (st.gray_var_result[i-1]?COLOR_GREEN:COLOR_RED));
				TextOut(memdc, start_x, start_y+roi_h+40, buffer, strlen(buffer));
			}

		}
		SetBkMode(memdc, TRANSPARENT);
		DeleteObject(hfont);
		hfont	 = CreateFont(40, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "黑体");
		old_font = (HFONT)SelectObject(memdc, hfont);
		if (st.gray_test_result == FALSE){
			index += offset;
			SetTextColor(memdc, COLOR_RED);
			TextOut(memdc, 20, 100+index, "-Gray Fail", 10);
		}
	}

	if (st.is_sn_test_on){
		SetBkMode(memdc, OPAQUE);
		DeleteObject(hfont);
		hfont	 = CreateFont(30, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "黑体");
		old_font = (HFONT)SelectObject(memdc, hfont);

		int sn_index = st.sn_gray_box_num;
		int sn_center_x = st.gray_box_x[sn_index]+ st.center_x_offset+st.gray_box_w/2;
		int sn_center_y = st.gray_box_y[sn_index] + st.center_y_offset + st.gray_box_h/2;
		DeleteObject(pen);
		pen = CreatePen(PS_SOLID, 3, (st.sn_test_result?COLOR_GREEN:COLOR_RED));
		old_pen = (HPEN)SelectObject(memdc, pen);
		ai_img::Output::draw_crossline(memdc, sn_center_x, sn_center_y, st.gray_box_w*2/3, st.gray_box_h*2/3);
		SetTextColor(memdc, (st.sn_test_result?COLOR_GREEN:COLOR_RED));
		sprintf(buffer, "%.2fdB", st.sn_result);
		TextOut(memdc, st.gray_box_x[sn_index], st.gray_box_y[sn_index]-50, buffer, strlen(buffer));
		SetBkMode(memdc, TRANSPARENT);
		DeleteObject(hfont);
		hfont	 = CreateFont(40, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "黑体");
		old_font = (HFONT)SelectObject(memdc, hfont);
		if (st.sn_test_result == FALSE){
			index += offset;
			SetTextColor(memdc, COLOR_RED);
			TextOut(memdc, 20, 100+index, "-SNR Fail", 9);
		}
	}

	if (st.is_oc_test_on){
		SetBkMode(memdc, OPAQUE);
		DeleteObject(hfont);
		hfont	 = CreateFont(30, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "黑体");
		old_font = (HFONT)SelectObject(memdc, hfont);
		DeleteObject(pen);
		pen = CreatePen(PS_SOLID, 3, (st.oc_test_result?COLOR_GREEN:COLOR_RED));
		old_pen = (HPEN)SelectObject(memdc, pen);
		ai_img::Output::draw_crossline(memdc, (int)st.chart_center_x, (int)st.chart_center_y, 20, 20);
		SetTextColor(memdc, (st.oc_test_result?COLOR_GREEN:COLOR_RED));
		sprintf(buffer, "[%.2f, %.2f]", st.chart_center_x-IMG_WIDTH/2, st.chart_center_y-IMG_HEIGHT/2);
		TextOut(memdc, (int)st.chart_center_x+10, (int)st.chart_center_y+10, buffer, strlen(buffer));
		SetBkMode(memdc, TRANSPARENT);
		DeleteObject(hfont);
		hfont	 = CreateFont(40, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "黑体");
		old_font = (HFONT)SelectObject(memdc, hfont);
		if (st.oc_test_result == FALSE){
			index += offset;
			SetTextColor(memdc, COLOR_RED);
			TextOut(memdc, 20, 100+index, "-OC Fail", 8);
		}
	}

	if (st.is_rotate_test_on){
		SetBkMode(memdc, OPAQUE);
		DeleteObject(hfont);
		hfont	 = CreateFont(30, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "黑体");
		old_font = (HFONT)SelectObject(memdc, hfont);
		DeleteObject(pen);
		SetTextColor(memdc, (st.rotate_test_result?COLOR_GREEN:COLOR_RED));
		sprintf(buffer, "Rotate Deg:%.2f", st.rotate_result);
		TextOut(memdc, (int)st.chart_center_x+10, (int)st.chart_center_y-30, buffer, strlen(buffer));
		SetBkMode(memdc, TRANSPARENT);
		DeleteObject(hfont);
		hfont	 = CreateFont(40, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "黑体");
		old_font = (HFONT)SelectObject(memdc, hfont);
		if (st.rotate_test_result == FALSE){
			index += offset;
			SetTextColor(memdc, COLOR_RED);
			TextOut(memdc, 20, 100+index, "-Rotate Fail", 12);
		}
	}


	DeleteObject(hfont);
	hfont	 = CreateFont(90, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "黑体");
	old_font = (HFONT)SelectObject(memdc, hfont);
	if (st.final_result == TRUE){
		SetTextColor(memdc, COLOR_GREEN);
		TextOut(memdc, 10, 10, "PASS", 4);
	}
	else{
		SetTextColor(memdc, COLOR_RED);
		TextOut(memdc, 20, 20, "FAIL", 4);
	}
	DeleteObject(hfont);
	DeleteObject(pen);
}
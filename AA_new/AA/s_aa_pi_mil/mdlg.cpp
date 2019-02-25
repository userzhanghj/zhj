#define _CRT_SECURE_NO_WARNINGS
#include "interface.h"
#include "XTrace.h"
#include "s_win.h"
#include "inf_motion_aa.h"
/*各窗口功能线程及主界面显示*/
extern HINSTANCE app_inst;

extern INIEdit config_dlg[];
extern INIEdit gl_conf[];

extern void	MakeSpaceTableITU(void);
extern DWORD CALLBACK showtimeThread(LPVOID temp);
extern DWORD CALLBACK mouse_operation_thread(LPVOID temp);
extern DWORD CALLBACK test_thread(LPVOID temp);
extern DWORD CALLBACK error_monitor_thread(LPVOID temp);
extern DWORD CALLBACK get_image_thread(LPVOID temp);
extern DWORD CALLBACK display_image_thread(LPVOID temp);
extern DWORD CALLBACK reg_mark_chart_test_thread(LPVOID temp);
extern DWORD CALLBACK extern_in_handshake_thread(LPVOID temp);
extern DWORD CALLBACK image_info_thread(LPVOID temp);
extern DWORD CALLBACK vision_link_thread(LPVOID temp);
extern DWORD CALLBACK plc_link_thread(LPVOID temp);
extern DWORD CALLBACK pi_link_thread(LPVOID temp);
extern DWORD CALLBACK vision_recv_thread(LPVOID temp);
extern DWORD CALLBACK vision_cmd_thread(LPVOID temp);
extern DWORD CALLBACK plc_online_check_thread(LPVOID temp);
extern DWORD CALLBACK plc_online_counting_thread(LPVOID temp);
extern DWORD CALLBACK plc_read_thread(LPVOID temp);
extern DWORD CALLBACK plc_read_thread2(LPVOID temp);
extern DWORD CALLBACK plc_sn_read_thread(LPVOID temp);
extern DWORD CALLBACK check_img_exist_thread(LPVOID temp);

extern BOOL CALLBACK guide_line_dlg_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK histogram_dlg_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK measure_dlg_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK dlg_img_info(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);

extern BOOL CALLBACK dlg_log(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK dlg_recipe(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK dlg_hw_config(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK dlg_test_config(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK dlg_img_info(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);

extern BOOL CALLBACK stdchart_setting_dlg_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK reg_mark_chart_setting_dlg_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK reg_mark_chart_sfrcurve_dlg_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK image_test_dlg_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK motion_aa_dlg_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK motion_pi_dlg_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK motion_save_set_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK user_login_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK okng_test_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);

extern BOOL CALLBACK reg_mark_tf_dlg_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern void set_guide_line_color(void);

char connect_status[2][20] = {"External Connect>>", "External Connected"};
extern int item_id[];
extern int order_id[];
extern int loop_id[];
extern int check_id[];
extern int result_id[];
extern int time_id[];
extern SlantedBW mysbw;
class MDlg:public ai_win::Dlg
{
public:
	MDlg();
	virtual BOOL	init(HWND hdlg, HWND hfocus, LPARAM linit);
	virtual void	command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify);
	virtual LRESULT ctlcolor_static(HWND hdlg, HDC hdc, HWND hcolor, int mark = CTLCOLOR_STATIC );
	virtual LRESULT ctlcolor_dlg(HWND hdlg, HDC hdc, HWND hcolor, int mark =CTLCOLOR_DLG);
	virtual void	draw_item(HWND hdlg, const DRAWITEMSTRUCT *pdis);
	virtual void	size(HWND hdlg, unsigned int state, int width, int height);
	virtual void	close(HWND hdlg);
	virtual LRESULT ctlcolor_listbox(HWND hdlg, HDC hdc, HWND hcolor, int mark/* =CTLCOLOR_EDIT */);

private:
	HBRUSH static_brush;
	HMENU menu_handle;
	HBRUSH dlg_brush, old_dlg_brush;

//	BOOL open_single_img_file(HWND hdlg);
};

MDlg mdlg;
BOOL CALLBACK mdlg_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_DLGMSG(hdlg, WM_INITDIALOG,	    mdlg.init);
		HANDLE_DLGMSG(hdlg, WM_COMMAND,		    mdlg.command);
		HANDLE_DLGMSG(hdlg, WM_CTLCOLORSTATIC,  mdlg.ctlcolor_static);
		HANDLE_DLGMSG(hdlg, WM_CTLCOLORDLG,     mdlg.ctlcolor_dlg);
		HANDLE_DLGMSG(hdlg, WM_CTLCOLORLISTBOX, mdlg.ctlcolor_listbox);
		HANDLE_DLGMSG(hdlg, WM_DRAWITEM,	    mdlg.draw_item);
		HANDLE_DLGMSG(hdlg, WM_CLOSE,		    mdlg.close);

	case WM_LBUTTONDBLCLK:
		MOUSE_LDBCLICK = TRUE;
		break;

	case WM_LBUTTONDOWN:
		{
			//SetWindowPos(HWND_LOG, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
			MEASURE_DRAW_RECT_DONE = FALSE;
			RECT rt;
			GetClientRect(GetDlgItem(hdlg, IDC_DISPLAY), &rt);
			IMG_RATIO_X = (float)DISPLAY_IMG_WIDTH/(rt.right-rt.left);
		//	log("img_w=%d",DISPLAY_IMG_WIDTH);
			IMG_RATIO_Y = (float)DISPLAY_IMG_HEIGHT/(rt.bottom-rt.top);
			MOUSE_X = LOWORD(lParam);
			MOUSE_Y = HIWORD(lParam)-MYTOOLBAR_HEIGHT;
		//	log("mouseX=%d,mouseY=%d",MOUSE_X,MOUSE_Y);
			MOUSE_LDOWN = TRUE;
		}

		break;

	case WM_MOUSEMOVE:
		if ((wParam&MK_LBUTTON)){
			MOUSE_LDOWN_MOVE = TRUE;
			MOUSE_X = LOWORD(lParam);
			MOUSE_Y = HIWORD(lParam)-MYTOOLBAR_HEIGHT;
		}


		/*if ((wParam&MK_LBUTTON) && ENABLE_IMG_ZOOM_IN && DRAW_ZOOM_IN_AREA_DONE == FALSE){
		MOUSE_END_X = LOWORD(lParam);
		MOUSE_END_Y = HIWORD(lParam)-MYTOOLBAR_HEIGHT;
		DRAW_MOVING_MOUSE = TRUE;
		}

		if ((wParam&MK_LBUTTON) && ENABLE_MEASURE == TRUE){
		MEASURE_END_X = LOWORD(lParam);
		MEASURE_END_Y = HIWORD(lParam)-MYTOOLBAR_HEIGHT;
		}*/
		break;

	case WM_LBUTTONUP:
		MOUSE_X = LOWORD(lParam);
		MOUSE_Y = HIWORD(lParam)-MYTOOLBAR_HEIGHT;
		MOUSE_LUP = TRUE;

		/*if (ENABLE_MEASURE == TRUE){
		MEASURE_END_X = LOWORD(lParam);
		MEASURE_END_Y = HIWORD(lParam)-MYTOOLBAR_HEIGHT;

		if (MEASURE_END_X>MEASURE_START_X && MEASURE_END_Y>MEASURE_START_Y){
		MEASURE_DRAW_RECT_DONE = TRUE;
		}
		}*/
		break;
	}

	return FALSE;
}

MDlg::MDlg()
{
	static_brush = NULL;
}

LRESULT MDlg::ctlcolor_dlg(HWND hdlg, HDC hdc, HWND hcolor, int mark /* =CTLCOLOR_DLG */)
{
	if (dlg_brush != NULL){
		DeleteObject(dlg_brush);
	}

	dlg_brush = CreateSolidBrush(RGB(0xbd, 0xc7, 0xd7));//主窗口背景色
	old_dlg_brush = (HBRUSH)SelectObject(hdc, (HBRUSH)dlg_brush);

	return (LRESULT)dlg_brush;
}

LRESULT MDlg::ctlcolor_listbox(HWND hdlg, HDC hdc, HWND hcolor, int mark)//主界面log窗口背景色
{
	DeleteObject(dlg_brush);
	SetBkMode(hdc, TRANSPARENT);
	dlg_brush = CreateSolidBrush(RGB(156,170,193));
	HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, dlg_brush);

	HFONT font     = CreateFont(15, 0, 0, 0, FW_BLACK, 0,0,0, ANSI_CHARSET, 0,0,0,0, "黑体");
	HFONT old_font = (HFONT)SelectObject(hdc, font);
	LISTBOX_SETLHEIGHT(hdlg, IDC_LOG, 17);
	SetTextColor(hdc, COLOR_WHITE);

	DeleteObject(font);

	return (LRESULT)dlg_brush;
}


int load_dll_module(LPVOID temp)
{
	HWND hdlg = (HWND)temp;

	int result = s_aa::cimg.init_lib();
	log("Lib:Image convert function init:%s", result?"OK":"Fail");

	//////////////////////////////////////////////////////////////////////////
	//hardware
	char hw_list[HW_CNT][FOLDER_NAME_LENGTH] = {0};
	char hw_path[MAX_PATH] = {0};
	sprintf(hw_path, "%s\\hw\\", s_aa::dir);
	int n = ai_win::get_sub_files_name(hw_path, hw_list, HW_CNT, "hw_*.dll");
	HW_NUM = 0;
	if (n>0){
		for (int i=0; i<n; i++){
			result = s_aa::imhw[i].load_module(hw_list[i]);
			(RESULT_LOAD_HW(i)) = result;
			log("Lib::Load HW [ %s ] :%s", hw_list[i], result?"OK":"Fail");
			if (result == TRUE){
				HW_NUM++;
				s_aa::imhw[i].add_menu(s_aa::hmenu, ID_MENU_HW(i));
				char mystr[TEST_CNT][MAX_PATH] = {0};
				strcpy(mystr[0], s_aa::dir);
				strcpy(mystr[1], s_aa::recipe_name);
				s_aa::imhw[i].init(HWND_MAIN, s_aa::ram_int, mystr, log);
			}
		}
	}

	char hw_str[100] = {0};

	if (HW_NUM>=1){
		for (int i=0; i<HW_NUM; i++){
			memset(hw_str, 0, sizeof(hw_str));
			result = s_aa::imhw[i].get_hw_str(hw_str);
			
		}
		SELECT_INTERNAL_DEVICE = ai_win::read_integer_key(s_aa::config_path, "img", "select_internal_device");
		
		if (SELECT_INTERNAL_DEVICE > HW_NUM-1){
			SELECT_INTERNAL_DEVICE = HW_NUM-1;
		}

		s_aa::pimhw = &s_aa::imhw[SELECT_INTERNAL_DEVICE];
		DEVICE_START_INIT      = ai_win::read_integer_key(s_aa::config_path, "img", "device_start_init");
		if (DEVICE_START_INIT == TRUE){
			result = s_aa::pimhw->init_device();
			if (result == TRUE) IMG_DEVICE_INITED = TRUE;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//img tools
	char tool_list[TOOL_CNT][FOLDER_NAME_LENGTH] = {0};
	char tool_path[MAX_PATH] = {0};
	sprintf(tool_path, "%s\\tools\\", s_aa::dir);
	n = ai_win::get_sub_files_name(tool_path, tool_list, TOOL_CNT, "tool_*.dll");
	if (n>0){
		for (int i=0; i<n; i++){
			result = s_aa::imtool[i].load_module(tool_list[i]);
			(RESULT_LOAD_TOOL(i)) = result;
			log("Lib::Load TOOL [ %s ] :%s", tool_list[i], result?"OK":"Fail");
			if (result == TRUE){
				s_aa::imtool[i].add_menu(s_aa::hmenu, ID_MENU_TOOL(i));
			}
		}
	}


	return FALSE;
}

BOOL MDlg::init(HWND hdlg, HWND hfocus, LPARAM linit)
{

	ai_win::set_icon(hdlg, IDI_LOGO, IDI_LOGO);
	//////////////////////////////////////////////////////////////////////////
	//Menu
	MENUINFO mi;
	s_aa::hmenu = ai_win::create_menu(hdlg, IDR_MENU);
	mi.fMask   = MIM_BACKGROUND;
	mi.hbrBack = CreateSolidBrush(RGB(255, 0, 0));
	SetMenuInfo(s_aa::hmenu, &mi);
	DrawMenuBar(hdlg);

	MENU_BMP(hdlg, ID_FILE_LOG,				 IDB_T_LOG		);
	MENU_BMP(hdlg, ID_TEST_CONFIG,			 IDB_T_CONFIG	);
	MENU_BMP(hdlg, ID_SETTING_RECIPE,		 IDB_T_RECIPE	);
	MENU_BMP(hdlg, ID_FILE_LOGIN,			 IDB_T_LOGIN	);
	MENU_BMP(hdlg, ID_EDIT_IMAGEINFO,		 IDB_T_IMG_INFO	);
	MENU_BMP(hdlg, ID_TOOL_HISTOGRAM,		 IDB_T_HISTOGRAM);
	MENU_BMP(hdlg, ID_EDIT_GUIDELINE,		 IDB_T_GUIDE	);
	MENU_BMP(hdlg, ID_TOOL_MEASURE,			 IDB_T_MEASURE	);
	MENU_BMP(hdlg, ID_FILE_SAVEINFOIMAGE,    IDB_T_SAVE		);

	///////////////////////////////////////////////////////////////////////////
	//status bar
	int sub_width[6] = {90, 150, 270, 390, 470, -1};
	HWND_STATUS = ai_win::create_statusbar(hdlg, sub_width, sizeof(sub_width)/sizeof(int), 0);
	char version[20] = {0};
	ai_win::get_version(NULL, TRUE, version);

	char status_string1[100] = {0};
	sprintf(status_string1, " %s", version);

	SendMessage(HWND_STATUS, SB_SETTEXT, 0, (LPARAM)" @2018");
	SendMessage(HWND_STATUS, SB_SETTEXT, 1, (LPARAM)status_string1);
	SendMessage(HWND_STATUS, SB_SETTEXT, 2, (LPARAM)"Image Files");
	SendMessage(HWND_STATUS, SB_SETTEXT, 3, (LPARAM)"");
	SendMessage(HWND_STATUS, SB_SETTEXT, 4, (LPARAM)"");
	SendMessage(HWND_STATUS, SB_SETTEXT, 5, (LPARAM)"");

	//main dlg layout
	RECT rt, rt_m;
	GetClientRect(hdlg, &rt_m);
	GetClientRect(HWND_STATUS, &rt);
// 	MoveWindow(GetDlgItem(hdlg, IDC_DISPLAY), 640, 
// 											  MYTOOLBAR_HEIGHT, 
// 											  640/*rt_m.right-3*/, 
// 											  480/*rt_m.bottom-MYTOOLBAR_HEIGHT-rt.bottom-1*/, FALSE);
	//MoveWindow(GetDlgItem(hdlg, IDC_STATUS), 1, 1, 150, MYTOOLBAR_HEIGHT-2, FALSE);

	//////////////////////////////////////////////////////////////////////////
	//create sub dlg

	HWND_LOG		 = CreateDialog(app_inst, MAKEINTRESOURCE(IDD_LOG),		    hdlg, (DLGPROC)dlg_log);
	MoveWindow(HWND_LOG, s_aa::dinfo.rt_log.left,	
						 s_aa::dinfo.rt_log.top,	
						 s_aa::dinfo.rt_log.right-s_aa::dinfo.rt_log.left,	
						 s_aa::dinfo.rt_log.bottom-s_aa::dinfo.rt_log.top, FALSE);
	CheckMenuItem(s_aa::hmenu, ID_FILE_LOG,	  s_aa::dinfo.show_log?MF_CHECKED:MF_UNCHECKED);
//	ShowWindow(HWND_LOG, s_aa::dinfo.show_log?SW_NORMAL:SW_HIDE);

	log("===Start Saker AA Software===");

	HWND_RECIPE		 = CreateDialog(app_inst, MAKEINTRESOURCE(IDD_RECIPE),	    hdlg, (DLGPROC)dlg_recipe);
	HWND_HW_CONFIG	 = CreateDialog(app_inst, MAKEINTRESOURCE(IDD_HW_CONFIG),   hdlg, (DLGPROC)dlg_hw_config);
	HWND_TEST_CONFIG = CreateDialog(app_inst, MAKEINTRESOURCE(IDD_TEST_CONFIG),	hdlg, (DLGPROC)dlg_test_config);
	HWND_AA_MOTION   = CreateDialog(app_inst, MAKEINTRESOURCE(IDD_AA_CONFIG),   hdlg, (DLGPROC)motion_aa_dlg_process);
	HWND_PI_MOTION   = CreateDialog(app_inst, MAKEINTRESOURCE(IDD_PI_CONFIG),   hdlg, (DLGPROC)motion_pi_dlg_process);
	HWND_SAVE_SET    = CreateDialog(app_inst, MAKEINTRESOURCE(IDD_SAVE_SET),    hdlg, (DLGPROC)motion_save_set_process);
	HWND_OKNG_TEST   = CreateDialog(app_inst, MAKEINTRESOURCE(IDD_OKNG_TEST),   hdlg, (DLGPROC)okng_test_process);

	RECT rt_tc;	
	GetClientRect(HWND_TEST_CONFIG, &rt_tc);
	MoveWindow(HWND_TEST_CONFIG, s_aa::dinfo.tcdlg_x, 
								 s_aa::dinfo.tcdlg_y, 
								 rt_tc.right-rt_tc.left+4, 
								 rt_tc.bottom-rt_tc.top+20, FALSE);
	CheckMenuItem(s_aa::hmenu, ID_TEST_CONFIG, s_aa::dinfo.show_tc?MF_CHECKED:MF_UNCHECKED);
	ShowWindow(HWND_TEST_CONFIG, s_aa::dinfo.show_tc?SW_NORMAL:SW_HIDE);	

	GetClientRect(HWND_AA_MOTION, &rt_tc);
	MoveWindow(HWND_AA_MOTION, s_aa::dinfo.aadlg_x, 
		s_aa::dinfo.aadlg_y, 
		rt_tc.right-rt_tc.left+4, 
		rt_tc.bottom-rt_tc.top+20, FALSE);
//	CheckMenuItem(s_aa::hmenu, ID_REGMARKCHART_AA, s_aa::dinfo.show_aa_dlg?MF_CHECKED:MF_UNCHECKED);
//	ShowWindow(HWND_AA_MOTION, s_aa::dinfo.show_aa_dlg?SW_NORMAL:SW_HIDE);	

	HWND_IMG_INFO    = CreateDialog(app_inst, MAKEINTRESOURCE(IDD_IMAGE_INFO),	hdlg, (DLGPROC)dlg_img_info);
	update_mdlg_title(hdlg);

	//////////////////////////////////////////////////////////////5////////////
	//Create thread
#define PLC_LINK_ON
#define THREAD_NUM	16
	DWORD my_id[THREAD_NUM] = {0};
	HANDLE my_handle[THREAD_NUM] = {NULL};

	my_handle[0]  = CreateThread(NULL, 0, test_thread,				  (LPVOID)hdlg, 0, &my_id[0]);
	my_handle[1]  = CreateThread(NULL, 0, showtimeThread,			  (LPVOID)hdlg, 0, &my_id[1]);
	my_handle[2]  = CreateThread(NULL, 0, mouse_operation_thread,	  (LPVOID)hdlg, 0, &my_id[2]);
	my_handle[3]  = CreateThread(NULL, 0, error_monitor_thread,		  (LPVOID)hdlg, 0, &my_id[3]);
	my_handle[4]  = CreateThread(NULL, 0, get_image_thread,			  (LPVOID)hdlg, 0, &my_id[4]);
	my_handle[5]  = CreateThread(NULL, 0, display_image_thread,		  (LPVOID)hdlg, 0, &my_id[4]);
	my_handle[6]  = CreateThread(NULL, 0, reg_mark_chart_test_thread, (LPVOID)hdlg, 0, &my_id[6]);
	//my_handle[7]  = CreateThread(NULL, 0, check_img_exist_thread,	  (LPVOID)hdlg, 0, &my_id[7]);
	my_handle[8] = CreateThread(NULL, 0, pi_link_thread,			  (LPVOID)hdlg, 0, &my_id[8]);
#ifdef PLC_LINK_ON
	my_handle[10] = CreateThread(NULL, 0, plc_link_thread,			  (LPVOID)hdlg, 0, &my_id[10]);
	my_handle[11] = CreateThread(NULL, 0, plc_sn_read_thread,		  (LPVOID)hdlg, 0, &my_id[11]);
	my_handle[12] = CreateThread(NULL, 0, plc_online_check_thread,	  (LPVOID)hdlg, 0, &my_id[12]);
	my_handle[13] = CreateThread(NULL, 0, plc_online_counting_thread, (LPVOID)hdlg, 0, &my_id[13]);
	my_handle[14] = CreateThread(NULL, 0, plc_read_thread,			  (LPVOID)hdlg, 0, &my_id[14]);
	my_handle[15] = CreateThread(NULL, 0, plc_read_thread2,			  (LPVOID)hdlg, 0, &my_id[15]);
#endif
	

	for (int i=0; i<THREAD_NUM; i++){
		CloseHandle(my_handle[i]);
	}
	
	//////////////////////////////////////////////////////////////////////////
	//Load hw, test, tool module
	load_dll_module(hdlg);

	//////////////////////////////////////////////////////////////////////////
	//other initial
	//guide line
	ai_win::load_keys(s_aa::config_path, "guide_line", hdlg, gl_conf, 100);
	set_guide_line_color();

	create_report_folder();
	log("Dlg: Main UI init OK.");
	TESTER_ID = ai_win::read_integer_key(".\\recipe\\config.ini", "system", "tester_id");
	log("tester_id=%d",TESTER_ID);
	char path[MAX_PATH] = {0};
	sprintf(path, "%s\\recipe\\%s\\base.ini", s_aa::dir, s_aa::recipe_name);
	log("base:%s", path);
	IMG_WIDTH	   = ai_win::read_integer_key(path, "image", "img_width");
	IMG_HEIGHT	   = ai_win::read_integer_key(path, "image", "img_height");

	log("read w:%d, h:%d", IMG_WIDTH, IMG_HEIGHT);
	MakeSpaceTableITU();

	if (SELECT_IMG_SOURCE == IMG_DEV_FILE){
		s_aa::cap_dev = (ImgCapDev *)&s_aa::img_file;
		SendMessage(HWND_STATUS, SB_SETTEXT, 2, (LPARAM)"Image Files");
	}
	else if (SELECT_IMG_SOURCE == IMG_DEV_360E){
		
	}
	else if (SELECT_IMG_SOURCE == IMG_DEV_MIL){
		s_aa::cap_dev = (ImgCapDev *)&s_aa::img_mil;
		SendMessage(HWND_STATUS, SB_SETTEXT, 2, (LPARAM)"Mil");
	}

	if (ERROR_ON != TRUE){
		set_status(APP_READY);//主窗口状态
	}
	//***************************************************************

	return TRUE;
}

void MDlg::size(HWND hdlg, unsigned int state, int width, int height)
{
	RECT rt;
	GetClientRect(HWND_STATUS, &rt);
	
// 	MoveWindow(GetDlgItem(hdlg, IDC_DISPLAY), 2, MYTOOLBAR_HEIGHT, width-3, height-MYTOOLBAR_HEIGHT-rt.bottom-1, TRUE);
// 	MoveWindow(HWND_STATUS, 0, height-rt.bottom, width, rt.bottom, TRUE);
}

void MDlg::command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify)
{
	HANDLE hand;
	char *p="A";
	int status  = 0;
	BYTE read_b = 0;
	BYTE write_b = 0;
	int s_num;
	double x[1000],y[1000];
	switch (id)
	{
	//////////////////////////////////////////////////////////////////////////
	//menu
	case IDC_ACC_LOAD:
		load_mes(p);
		break;
	case IDC_ACC_UNLOAD:
		unload_mes(p, 1, 1, 1);
		break;
	case ID_FILE_SAVEINFOIMAGE:
		save_info_image(hdlg);
		break;

	case ID_FILE_LOGIN:
		if (HWND_USER == NULL){
			HWND_USER = CreateDialog((HINSTANCE)GetWindowLong(hdlg, GWL_HINSTANCE),
				MAKEINTRESOURCE(IDD_USER), hdlg, (DLGPROC)user_login_process);
		}
		ShowWindow(HWND_USER, SW_NORMAL); 
		break;

		break;
	case ID_FILE_LOG:
		s_aa::dinfo.show_log ^= TRUE;
		CheckMenuItem(s_aa::hmenu, id, s_aa::dinfo.show_log?MF_CHECKED:MF_UNCHECKED);
		ShowWindow(HWND_LOG, s_aa::dinfo.show_log?SW_NORMAL:SW_HIDE);
		break;

	case ID_TEST_CONFIG:
		s_aa::dinfo.show_tc ^= TRUE;
		CheckMenuItem(s_aa::hmenu, id, s_aa::dinfo.show_tc?MF_CHECKED:MF_UNCHECKED);
		ShowWindow(HWND_TEST_CONFIG, s_aa::dinfo.show_tc?SW_NORMAL:SW_HIDE);
		break;

	case ID_SETTING_RECIPE:
		s_aa::dinfo.show_recipe ^= TRUE;
		CheckMenuItem(s_aa::hmenu, id, s_aa::dinfo.show_recipe?MF_CHECKED:MF_UNCHECKED);
		ShowWindow(HWND_RECIPE, s_aa::dinfo.show_recipe?SW_NORMAL:SW_HIDE);
		break;

	case ID_EDIT_GUIDELINE:
		if (HWND_GUIDELINE == NULL){
			HWND_GUIDELINE = CreateDialog((HINSTANCE)GetWindowLong(hdlg, GWL_HINSTANCE), 
				MAKEINTRESOURCE(IDD_GUIDE_LINE), hdlg, (DLGPROC)guide_line_dlg_process);
		}
		ShowWindow(HWND_GUIDELINE, SW_NORMAL);
		break;

	case ID_EDIT_IMAGEINFO:
		s_aa::dinfo.show_img_info ^= TRUE;
		CheckMenuItem(s_aa::hmenu, id, s_aa::dinfo.show_img_info?MF_CHECKED:MF_UNCHECKED);
		ShowWindow(HWND_IMG_INFO, s_aa::dinfo.show_img_info?SW_NORMAL:SW_HIDE);
		break;
	//////////////////////////////////////////////////////////////////////////
	//test
	case ID_AA_CONFIG:
		if (HWND_AA_MOTION == NULL){
			HWND_AA_MOTION = CreateDialog((HINSTANCE)GetWindowLong(hdlg, GWL_HINSTANCE),
				MAKEINTRESOURCE(IDD_AA_CONFIG), hdlg, (DLGPROC)motion_aa_dlg_process);
		}
		ShowWindow(HWND_AA_MOTION, SW_NORMAL); 
		break;

	case ID_PI_CONFIG:
		if (HWND_PI_MOTION == NULL){
			HWND_PI_MOTION = CreateDialog((HINSTANCE)GetWindowLong(hdlg, GWL_HINSTANCE),
				MAKEINTRESOURCE(IDD_PI_CONFIG), hdlg, (DLGPROC)motion_pi_dlg_process);
		}
		ShowWindow(HWND_PI_MOTION, SW_NORMAL); 
		break;
	case ID_SAVE_SET:
		if (HWND_SAVE_SET == NULL){
			HWND_SAVE_SET = CreateDialog((HINSTANCE)GetWindowLong(hdlg, GWL_HINSTANCE),
				MAKEINTRESOURCE(IDD_SAVE_SET), hdlg, (DLGPROC)motion_save_set_process);
		}
		ShowWindow(HWND_SAVE_SET, SW_NORMAL);
		break;
	case ID_OKNG_TEST:
		if (HWND_OKNG_TEST == NULL){
			HWND_OKNG_TEST = CreateDialog((HINSTANCE)GetWindowLong(hdlg, GWL_HINSTANCE),
				MAKEINTRESOURCE(IDD_OKNG_TEST), hdlg, (DLGPROC)okng_test_process);
		}
		ShowWindow(HWND_OKNG_TEST, SW_NORMAL);
		break;

	case ID_REGMARKCHART_CONFIGURE:
		if (HWND_REG_MARK_CHART == NULL){
			HWND_REG_MARK_CHART = CreateDialog((HINSTANCE)GetWindowLong(hdlg, GWL_HINSTANCE),
				MAKEINTRESOURCE(IDD_REG_MARK_CHART), hdlg, (DLGPROC)reg_mark_chart_setting_dlg_process);
		}
		ShowWindow(HWND_REG_MARK_CHART, SW_NORMAL);
		break;

	//tool
	case ID_TOOL_HISTOGRAM:
		if (HWND_HISTOGRAM == NULL){
			HWND_HISTOGRAM = CreateDialog((HINSTANCE)GetWindowLong(hdlg, GWL_HINSTANCE), 
				MAKEINTRESOURCE(IDD_HISTOGRAM), hdlg, (DLGPROC)histogram_dlg_process);
		}
		ShowWindow(HWND_HISTOGRAM, SW_NORMAL);
		IS_HISTOGRAM_DLG_SHOW = TRUE;
		break;

	case ID_TOOL_MEASURE:
		if (HWND_MEASURE == NULL){
			HWND_MEASURE = CreateDialog((HINSTANCE)GetWindowLong(hdlg, GWL_HINSTANCE), 
				MAKEINTRESOURCE(IDD_MEASURE), hdlg, (DLGPROC)measure_dlg_process);
		}

		if (IS_IMAGE_SCENERY_OCCUPIED){
			MessageBox(hdlg, "Image scenery is occupied!", "Error", 0);
		}
		else{
			ENABLE_MEASURE = TRUE;
			ShowWindow(HWND_MEASURE, SW_NORMAL);
			IS_IMAGE_SCENERY_OCCUPIED = TRUE;
		}
		break;

	case ID_HW_CONFIG:
		s_aa::dinfo.show_hw_config ^= TRUE;
		CheckMenuItem(s_aa::hmenu, id, s_aa::dinfo.show_hw_config?MF_CHECKED:MF_UNCHECKED);
		ShowWindow(HWND_HW_CONFIG, s_aa::dinfo.show_hw_config?SW_NORMAL:SW_HIDE);
		break;

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//mdlg items
	//case IDC_OPEN_JZ2:
	//	write_b = 1;					
	//	status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &write_b);//
	//	log("open catch");
	//	break;
	//case IDC_CLOSE_JZ:
	//	write_b = 2;					
	//	status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &write_b);//close
	//	log("close catch");
	//	break;

	case IDC_START:	
		if (TEST_START == FALSE){
			do_close=1;
			log("Cmd:[Start]");
			set_status(APP_START);
			TEST_START = TRUE;
			ITEM_GRAYED(hdlg, id);
		}
		break;

	case IDC_STOP:
		log("Cmd:[Stop]");
		do_close=0;
		set_status(APP_STOP);
		TEST_STOP = TRUE;
		TEST_START = FALSE;
		IMG_DEVICE_INITED = FALSE;
		s_aa::test.set_test_stop();
		if(SELECT_IMG_SOURCE != IMG_DEV_MIL)
			s_aa::cap_dev->stop();
		
// 		if (SELECT_IMG_SOURCE == IMG_SOURCE_FILE){
// 			IMG_DEVICE_INITED = FALSE;
// 		}
		SetEvent(s_aa::test.get_event());
		ITEM_ENABLED(hdlg, IDC_START);
		s_aa::test.reset_test_items();

		ERROR_ON = FALSE;
		IMG_CAPTURE_READY = FALSE;

		IMG_DEVICE_RUN = FALSE;
		disconnect_device(IMG_DEVICE_INDEX);

		break;

	case IDC_SNAPSHOT:
		if (IMG_CAPTURE_READY == TRUE){
			save_img_file(hdlg);
		}
		else log("Image Capture not valid.");
		break;

	case IDC_SAVE:
	//	save_reg_mark_report_c(s_aa::bmp_buf, IMG_WIDTH, IMG_HEIGHT,SAVE_OK_REG_PRE_REPORT	 );
	//	save_reg_mark_report_c(s_aa::bmp_buf, IMG_WIDTH, IMG_HEIGHT,SAVE_NG_REG_PRE_REPORT	   );
	//	save_reg_mark_report_c(s_aa::bmp_buf, IMG_WIDTH, IMG_HEIGHT,SAVE_OK_REG_REPORT		   );
	//	save_reg_mark_report_c(s_aa::bmp_buf, IMG_WIDTH, IMG_HEIGHT,SAVE_NG_REG_REPORT		   );
		save_reg_mark_report_c(s_aa::bmp_buf, IMG_WIDTH, IMG_HEIGHT,SAVE_REG_REPORT		   );
		//save_reg_mark_report_c(s_aa::bmp_buf, IMG_WIDTH, IMG_HEIGHT,SAVE_MES		   );
	//	hand=creat_test_file();
		//for(int i=0;i<5;i++)
		//	for(int j=0;j<4;j++)
		//	{
				//log("%d  %d,%.2f,%.2f",i,j,mysbw.freq[i][j][10],mysbw.amp[i][j][10]);
				//save_reg_mark_report_test(100);
			/*}*/
		//s_num = aa_num;
		//aa_num = 99;
		//save_reg_mark_report_test(1,3);
		//aa_num = s_num;
				if(save_set::mes_uv_bload)
					save_reg_mark_report_test2(2,1);
				if(save_set::uv_bload)
					save_reg_mark_report_test2(1,4);
		log("report saved.");													

		break;

	default:
		{
			//////////////////////////////////////////////////////////////////////////
			//tools
			int i = id - ID_MENU_TOOL(0);
			if (i>=0 && i<TOOL_CNT){
				if (RESULT_LOAD_TOOL(i) == TRUE){
					char mystr[TOOL_CNT][MAX_PATH] = {0};
					strcpy(mystr[0], s_aa::dir);
					strcpy(mystr[1], s_aa::recipe_name);
					if (HWND_TOOL(i) == NULL){
						HWND_TOOL(i) = s_aa::imtool[i].create_dlg(hdlg, s_aa::img_buf, s_aa::bmp_buf, s_aa::ram_int, mystr, log);
					}
					ShowWindow(HWND_TOOL(i), SW_NORMAL);
				}
			}

			//////////////////////////////////////////////////////////////////////////
			//hw
			i = id - ID_MENU_HW(0);
			if (i>=0 && i<HW_CNT){
				if (RESULT_LOAD_HW(i) == TRUE && HWND_HW(i) == NULL){
					HWND_HW(i) = s_aa::imhw[i].create_dlg(hdlg, 0);
				}
				ShowWindow(HWND_HW(i), SW_NORMAL);
			}
		}

		break;
	}
}

LRESULT MDlg::ctlcolor_static(HWND hdlg, HDC hdc, HWND hcolor, int mark /* = CTLCOLOR_STATIC */ )
{	
	COLORREF cr;
	char text[20] = {0};
	if (static_brush != NULL) DeleteObject(static_brush);

	static_brush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE)/*BRUSH_LIGHTGRAY*/);
	HBRUSH old_brush = (HBRUSH)SelectObject(hdc, (HBRUSH)static_brush);
	HFONT hfont = CreateFont(14, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "黑体");
	HFONT old_font = (HFONT)SelectObject(hdc, hfont);

	SetBkMode(hdc, TRANSPARENT);

	switch (GetDlgCtrlID(hcolor))
	{
	case IDC_STATUS:
		DeleteObject(hfont);
		hfont = CreateFont(26, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "黑体");
		old_font = (HFONT)SelectObject(hdc, hfont);
		DeleteObject(static_brush);
		SetTextColor(hdc, COLOR_LIGHT_DARK);
		switch (APP_STATUS)
		{
		case APP_ERR:	  cr = COLOR_RED;				strcpy(text, "Error");		break;
		case APP_NOHOME:  cr = COLOR_TOMATO;			strcpy(text, "Not home");	break;
		case APP_READY:	  cr = COLOR_MEDIUM_STATE_BLUE;	strcpy(text, "Ready");		break;
		case APP_INIT:	  cr = COLOR_MAGENTA;			strcpy(text, "Init...");	break;
		case APP_TESTING: cr = COLOR_GOLD;				strcpy(text, "Test...");	break;
		case APP_START:	  cr = COLOR_YELLOW_GREEN;		strcpy(text, "Start");		break;
		case APP_NEXT:	  cr = COLOR_MAGENTA;			strcpy(text, "Next");		break;
		case APP_STOP:	  cr = COLOR_TOMATO;			strcpy(text, "Stop");		break;
		case APP_PASS:    cr = COLOR_GREEN;				strcpy(text, "Pass");		break;
		case APP_FAIL:	  cr = COLOR_RED;				strcpy(text, "Fail");		break;
		case APP_RESET:	  cr = COLOR_PLUM;				strcpy(text, "Reset");		break;

		default:		  cr = COLOR_RED;				strcpy(text, "Unknown");	break;
		}
		static_brush = CreateSolidBrush(cr);
		SetDlgItemText(hdlg, IDC_STATUS, text);
		break;

	case IDC_STATIC_mtfScore:
	case IDC_STATIC_process:
	case IDC_STATIC_sn:
	case IDC_STATIC_PLC:
	case IDC_STATIC_PLC_R:
	case IDC_STATIC_PI:
	case IDC_STATIC_c:
	case IDC_STATIC_lt:
	case IDC_STATIC_rt:
	case IDC_STATIC_lb:
	case IDC_STATIC_rb:
		SetTextColor(hdc, RGB(0x00, 0x00, 0x00));
		DeleteObject(static_brush);
		static_brush = CreateSolidBrush(RGB(189,199,215));//注释背景色
		break;

	case IDC_LINK_PLC:
		GetDlgItemText(hdlg, GetDlgCtrlID(hcolor), text, sizeof(text));
		DeleteObject(static_brush);
		if (!strcmp(text, "Connected")){
			static_brush = CreateSolidBrush(RGB(0x70, 0xad, 0x47));
		}
		else{
			static_brush = CreateSolidBrush(RGB(0xff, 0x11, 0x11));
		}
		break;
	case IDC_LINK_PI:
		GetDlgItemText(hdlg, GetDlgCtrlID(hcolor), text, sizeof(text));
		DeleteObject(static_brush);
		if (!strcmp(text, "Connected")){
			static_brush = CreateSolidBrush(RGB(0x70, 0xad, 0x47));
		}
		else{
			static_brush = CreateSolidBrush(RGB(0xff, 0x11, 0x11));
		}
		break;
	case IDC_REPORT_SFR_C:
		GetDlgItemText(hdlg, GetDlgCtrlID(hcolor), text, sizeof(text));
		DeleteObject(static_brush);

		if (mysbw.sfr_val1[0][AA_SELECT_ROI]>(1.0*mysbw.spec1[0])){
			static_brush = CreateSolidBrush(RGB(0x70, 0xad, 0x47));
		}
		else{
			static_brush = CreateSolidBrush(RGB(0xff, 0x00, 0x00));
		}
		break;
	case IDC_REPORT_SFR_RB:
		GetDlgItemText(hdlg, GetDlgCtrlID(hcolor), text, sizeof(text));
		DeleteObject(static_brush);

		if (mysbw.sfr_val1[4][AA_SELECT_ROI]>(1.0*mysbw.spec1[1])){
			static_brush = CreateSolidBrush(RGB(0x70, 0xad, 0x47));
		}
		else{
			static_brush = CreateSolidBrush(RGB(0xff, 0x00, 0x00));
		}
		break;
	case IDC_REPORT_SFR_RT:
		GetDlgItemText(hdlg, GetDlgCtrlID(hcolor), text, sizeof(text));
		DeleteObject(static_brush);

		if (mysbw.sfr_val1[2][AA_SELECT_ROI]>(1.0*mysbw.spec1[1])){
			static_brush = CreateSolidBrush(RGB(0x70, 0xad, 0x47));
		}
		else{
			static_brush = CreateSolidBrush(RGB(0xff, 0x00, 0x00));
		}
		break;
	case IDC_REPORT_SFR_LB:
		GetDlgItemText(hdlg, GetDlgCtrlID(hcolor), text, sizeof(text));
		DeleteObject(static_brush);

		if (mysbw.sfr_val1[3][2]>(1.0*mysbw.spec1[1])){
			static_brush = CreateSolidBrush(RGB(0x70, 0xad, 0x47));
		}
		else{
			static_brush = CreateSolidBrush(RGB(0xff, 0x00, 0x00));
		}
		break;
	case IDC_REPORT_SFR_LT:
		GetDlgItemText(hdlg, GetDlgCtrlID(hcolor), text, sizeof(text));
		DeleteObject(static_brush);

		if (mysbw.sfr_val1[1][AA_SELECT_ROI]>(1.0*mysbw.spec1[1])){
			static_brush = CreateSolidBrush(RGB(0x70, 0xad, 0x47));
		}
		else{
			static_brush = CreateSolidBrush(RGB(0xff, 0x00, 0x00));
		}
		break;
	case IDC_REPORT_CENTER_X:
		SetTextColor(hdc, RGB(0xff, 0xff, 0xff));
		DeleteObject(static_brush);
		if(abs(mysbw.oc_x[0]-IMG_WIDTH/2)<(double)mysbw.oc_limit){
			static_brush = CreateSolidBrush(RGB(0x70, 0xad, 0x47));
		}
		else{
			static_brush = CreateSolidBrush(RGB(0xff, 0x11, 0x11));
		}
		break;	
	case IDC_REPORT_CENTER_Y:
		SetTextColor(hdc, RGB(0xff, 0xff, 0xff));
		DeleteObject(static_brush);
		if(abs(mysbw.oc_y[0]-IMG_HEIGHT/2)<(double)mysbw.oc_limit){
			static_brush = CreateSolidBrush(RGB(0x70, 0xad, 0x47));
		}
		else{
			static_brush = CreateSolidBrush(RGB(0xff, 0x11, 0x11));
		}
		break;	
	case IDC_REPORT_eCoorDiff:
		SetTextColor(hdc, RGB(0xff, 0xff, 0xff));
		DeleteObject(static_brush);
		if(abs(mysbw.eCoorDiff)<(double)mysbw.coorDiff_spec){
			static_brush = CreateSolidBrush(RGB(0x70, 0xad, 0x47));
		}
		else{
			static_brush = CreateSolidBrush(RGB(0xff, 0x11, 0x11));
		}
		break;	
	case IDC_REPORT_eSfrDiff:
		SetTextColor(hdc, RGB(0xff, 0xff, 0xff));
		DeleteObject(static_brush);
		if(abs(mysbw.eSfrDiff)<(double)mysbw.sfrDiff_spec){
			static_brush = CreateSolidBrush(RGB(0x70, 0xad, 0x47));
		}
		else{
			static_brush = CreateSolidBrush(RGB(0xff, 0x11, 0x11));
		}
		break;
// 	case IDC_REPORT_SFR_CENTER:
// 		SetTextColor(hdc, RGB(0xff, 0xff, 0xff));
// 		DeleteObject(static_brush);
// 		if(mysbw.sfr_val1[0][2]>(double)mysbw.spec2[0]){
// 			static_brush = CreateSolidBrush(RGB(0x70, 0xad, 0x47));
// 		}
// 		else{
// 			static_brush = CreateSolidBrush(RGB(0xff, 0x11, 0x11));
// 		}
// 		break;
// 	case IDC_REPORT_SFR_EDGE:
// 		SetTextColor(hdc, RGB(0xff, 0xff, 0xff));
// 		
// 		for(int i=1;i<5;i++){
// 			DeleteObject(static_brush);
// 			if(mysbw.sfr_val1[i][0]>(double)mysbw.spec2[1]||mysbw.sfr_val1[i][2]>(double)mysbw.spec2[1]){
// 				static_brush = CreateSolidBrush(RGB(0x70, 0xad, 0x47));
// 			}
// 			else{
// 				static_brush = CreateSolidBrush(RGB(0xff, 0x11, 0x11));
// 			}
// 		}	
// 		break;
	case IDC_PROCESS_AA_MOTION:
	case IDC_PROCESS_CHECK1:
	case IDC_PROCESS_CHECK2:
		GetDlgItemText(hdlg, GetDlgCtrlID(hcolor), text, sizeof(text));
		DeleteObject(static_brush);
		if (!strcmp(text, "OK")){
			static_brush = CreateSolidBrush(RGB(0x00, 0xff, 0x00));
		}
		else if (!strcmp(text, "NG")){
			static_brush = CreateSolidBrush(RGB(0xff, 0x00, 0x00));
		}
		else{
			static_brush = CreateSolidBrush(RGB(0xff, 0x11, 0x11));
		}
		break;
}


	DeleteObject(hfont);
	SelectObject(hdc, (HFONT)old_font);
	return (LRESULT)static_brush;
}

void MDlg::draw_item(HWND hdlg, const DRAWITEMSTRUCT *pdis)
{
	int icon_id = 0;
	switch (pdis->CtlID)
	{
	case IDC_START:		icon_id = IDI_START; break;
	case IDC_STOP:		icon_id = IDI_STOP;	 break;
//	case IDC_NEXT:		icon_id = IDI_NEXT;	 break;
	case IDC_SAVE:		icon_id = IDI_SAVE;	 break;
	case IDC_SNAPSHOT:	icon_id = IDI_SNAPSHOT;	 break;
	default:
		return;
		break;
	}

	if (pdis->CtlType == ODT_BUTTON){
		int style = BUTTON_FRAME_FLAT | BUTTON_IMG_ICON | BUTTON_INNER_IMGTEXT;
		if (icon_id==IDI_SNAPSHOT){
			style = BUTTON_FRAME_FLAT | BUTTON_IMG_ICON | BUTTON_INNER_IMGONLY;
		}
		
		ai_win::draw_own_button(hdlg, pdis, icon_id, style);
	}
}

void MDlg::close(HWND hdlg)
{
	if(IS_PLC_CONNECTED == TRUE){
		BYTE write_b = 0;					
		write_to_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+1, PLC_DB_LENS_R, &write_b);//clear
	}
	do_close=1;
	is_stop=1;

	IMG_CAPTURE_READY = FALSE;
	if (TEST_START)	ITEM_ACTIVE(HWND_MAIN, IDC_STOP);

	Sleep(100);

	RECT rt_mdlg, rt_tc;
	GetWindowRect(HWND_MAIN, &rt_mdlg);
	s_aa::dinfo.mdlg_x = rt_mdlg.left;
	s_aa::dinfo.mdlg_y = rt_mdlg.top;
	GetWindowRect(HWND_TEST_CONFIG, &rt_tc);
	s_aa::dinfo.tcdlg_x = rt_tc.left;
	s_aa::dinfo.tcdlg_y = rt_tc.top;



	RECT rt;
	GetWindowRect(HWND_APP, &rt);
	s_aa::dinfo.rt_win = rt;
	GetWindowRect(HWND_LOG, &rt);
	s_aa::dinfo.rt_log = rt;
	GetWindowRect(HWND_AA_MOTION, &rt);
	s_aa::dinfo.aadlg_x = rt.left;
	s_aa::dinfo.aadlg_y = rt.top;
	ai_win::save_keys(s_aa::config_path, "dlg", NULL, config_dlg, 100);

	for (int i=0; i<TOOL_CNT; i++){
		if (s_aa::ram_int[200+i] == TRUE){
			s_aa::imtool[i].destroy_dlg();
			Sleep(10);
		}
	}

	PostQuitMessage(0);
}

DWORD CALLBACK  extern_in_handshake_thread(LPVOID temp)
{
	HWND hdlg = (HWND)temp;
	int index = 0;
	int cnt = 0;
	int status = TRUE;
	HANDSHAKE_GET_SHM = FALSE;

	log("Thread:3rd Party Image Device Handshake Loop Ready.");

	while (1){
		while (HANDSHAKE_GET_SHM == FALSE) {
			if (SELECT_IMG_SOURCE == IMG_SOURCE_FILE){
				SET_STATUSBAR_TEXT(HWND_STATUS, SBINDEX_LINK, "Image File");
				if (HANDSHAKE_GET_SHM == TRUE){
					close_shm();
					HANDSHAKE_GET_SHM = FALSE;
				}
			}
			else if (SELECT_IMG_SOURCE == IMG_SOURCE_INTERNAL_DEVICE){
				SET_STATUSBAR_TEXT(HWND_STATUS, SBINDEX_LINK, "Internal Device");
				if (HANDSHAKE_GET_SHM == TRUE){
					close_shm();
					HANDSHAKE_GET_SHM = FALSE;
				}
			}
			else{
				SET_STATUSBAR_TEXT(HWND_STATUS, SBINDEX_LINK, connect_status[status]);
				status ^=TRUE;
				index = 0;
				if (SELECT_IMG_SOURCE == IMG_SOURCE_EXTERN_DEVICE){
					close_shm();
					HANDSHAKE_GET_SHM = open_shm(HWND_APP);
					ITEM_GRAYED(hdlg,  IDC_START);
					ITEM_GRAYED(hdlg,  IDC_STOP);
				}
			}
			Sleep(500); 
		}

		switch (index)
		{
		case 0: // linked, disable all function related to image buf;
			ITEM_GRAYED(hdlg, IDC_START);
			ITEM_GRAYED(hdlg, IDC_STOP);
// 			ITEM_GRAYED(hdlg, IDC_NEXT);
// 
// 			SetDlgItemText(hdlg, IDC_CONNECT_STATUS, "Stream Mode OK!");
			SET_STATUSBAR_TEXT(HWND_STATUS, SBINDEX_LINK, "Stream Connected");
			index = 10;
			break;

		case 10:  // send trigger signal, wait for feedback
			HANDSHAKE_TRIGGER_SIGNAL = TRUE;
			index = 20;
			break;

		case 20:// wait for image capture b/d send ready signal
			if (HANDSHAKE_TRIGGER_SIGNAL == FALSE){
				ITEM_ENABLED(hdlg, IDC_START);
				ITEM_ENABLED(hdlg, IDC_STOP);
				IMG_WIDTH  = EXT_CAM_IMG_WIDTH;
				IMG_HEIGHT = EXT_CAM_IMG_HEIGHT;
				IMG_DEPTH  = EXT_CAM_IMG_DEPTH;

				DISPLAY_IMG_START_X = 0;
				DISPLAY_IMG_START_Y = 0;
				DISPLAY_IMG_WIDTH  = IMG_WIDTH;
				DISPLAY_IMG_HEIGHT = IMG_HEIGHT;

				HANDSHAKE_LINKED = TRUE;
//				SetDlgItemText(hdlg, IDC_CONNECT_STATUS, "Connected");
				index = 30;
			}
			break;

		case 30:
			HANDSHAKE_TRIGGER_SIGNAL = TRUE;
			index = 40;
			break;

		case 40:
			if (SELECT_IMG_SOURCE == IMG_SOURCE_FILE || SELECT_IMG_SOURCE == IMG_SOURCE_INTERNAL_DEVICE){
				index = 100;
			}
			else{
				if (HANDSHAKE_TRIGGER_SIGNAL == FALSE){
					index = 30;
					cnt = 0;
				}
				else{
					if (++cnt >5){
						cnt = 0;
						index = 100;
					}
				}
			}
			Sleep(500);
			break;

		case 100:
			HANDSHAKE_LINKED  = FALSE;
			HANDSHAKE_GET_SHM = FALSE;
			log("Handshake:Lost 3rd Party Image Device Link!");
			break;

		}
		Sleep(10);
	}
}

DWORD CALLBACK mouse_operation_thread(LPVOID temp)
{
	char str[MAX_PATH] = {0};
	while (1){
		if(!is_move){
			MEASURE_END_X=MEASURE_START_X;
			MEASURE_END_Y=MEASURE_START_Y;
		
		}


		if (MOUSE_LDBCLICK == TRUE){
			if (ENABLE_IMG_ZOOM_IN == TRUE){

				DRAW_ZOOM_IN_AREA_DONE = FALSE;
				DISPLAY_IMG_START_X = 0;
				DISPLAY_IMG_START_Y = 0;
				DISPLAY_IMG_WIDTH  = IMG_WIDTH;
				DISPLAY_IMG_HEIGHT = IMG_HEIGHT;
				SET_STATUSBAR_TEXT(HWND_STATUS, SBINDEX_TEXT, "");
			}
			MOUSE_LDBCLICK = FALSE;
		}

		if (MOUSE_LDOWN == TRUE){
			//zoom in;
			is_move=0;
			if (ENABLE_IMG_ZOOM_IN && DRAW_ZOOM_IN_AREA_DONE == FALSE){
				MOUSE_START_X = MOUSE_X-628;
				MOUSE_START_Y = MOUSE_Y+40;
			}

			if (ENABLE_MEASURE == TRUE && DRAW_ZOOM_IN_AREA_DONE != TRUE){
				MEASURE_DRAW_RECT_DONE = FALSE;
				MEASURE_START_X = MOUSE_X-628;
				MEASURE_START_Y = MOUSE_Y+40;
			}
			MOUSE_LDOWN = FALSE;
		}

		if (MOUSE_LDOWN_MOVE == TRUE){
			is_move=1;
			if (ENABLE_IMG_ZOOM_IN && DRAW_ZOOM_IN_AREA_DONE == FALSE){
				MOUSE_END_X = MOUSE_X-628;
				MOUSE_END_Y = MOUSE_Y+40;
				DRAW_MOVING_MOUSE = TRUE;
			}

			if (ENABLE_MEASURE == TRUE && DRAW_ZOOM_IN_AREA_DONE != TRUE){
				MEASURE_END_X = MOUSE_X-628;
				MEASURE_END_Y = MOUSE_Y+40;
			}
			MOUSE_LDOWN_MOVE = FALSE;
		}

		if (MOUSE_LUP == TRUE){
			if (ENABLE_IMG_ZOOM_IN && DRAW_ZOOM_IN_AREA_DONE == FALSE && DRAW_MOVING_MOUSE == TRUE){
				DRAW_MOVING_MOUSE = FALSE;
				Sleep(100);

				MOUSE_END_X = MOUSE_X-628;
				MOUSE_END_Y = MOUSE_Y+40;
				DISPLAY_IMG_START_X += (int)(MOUSE_START_X*IMG_RATIO_X);
				DISPLAY_IMG_START_Y += (int)(MOUSE_START_Y*IMG_RATIO_Y);
				DISPLAY_IMG_WIDTH    = (int)((MOUSE_END_X-MOUSE_START_X)*IMG_RATIO_X);
				DISPLAY_IMG_HEIGHT   = (int)((MOUSE_END_Y-MOUSE_START_Y)*IMG_RATIO_Y);

				log("zoom in: x=%d, y=%d, w=%d, h=%d", DISPLAY_IMG_START_X, DISPLAY_IMG_START_Y, DISPLAY_IMG_WIDTH, DISPLAY_IMG_HEIGHT);
				DRAW_ZOOM_IN_AREA_DONE = TRUE;
			}

			if (ENABLE_MEASURE && DRAW_ZOOM_IN_AREA_DONE != TRUE){
				MEASURE_END_X = MOUSE_X-628;
				MEASURE_END_Y = MOUSE_Y+40;


				if (MEASURE_END_X>MEASURE_START_X && MEASURE_END_Y>MEASURE_START_Y){
					MEASURE_DRAW_RECT_DONE = TRUE;
				}
			}
			MOUSE_LUP = FALSE;
		}

		Sleep(10);
	}//while loop

	return FALSE;
}

DWORD CALLBACK error_monitor_thread(LPVOID temp)
{
	char err_str[MAX_PATH] = {0};
	while(1)
	{
		while (ERROR_ON == FALSE) Sleep(10);

		set_status(APP_ERR);
		sprintf(err_str, "[%d]Place Error Code Str Here!!!", ERROR_CODE);
		SET_STATUSBAR_TEXT(HWND_STATUS, SBINDEX_TEXT, err_str);

		Sleep(100);
	}

	return FALSE;
}

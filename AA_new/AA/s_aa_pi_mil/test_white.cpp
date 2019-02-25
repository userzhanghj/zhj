#define _CRT_SECURE_NO_WARNINGS
#include "interface.h"
#include "test_white.h"

White_V white_v;

PARTICLE_POS particle_dark[MAX_PARTICLE_CNT];
PARTICLE_POS particle_white[MAX_PARTICLE_CNT];
int particle_dark_cnt;
int particle_white_cnt;
int blemish_point[MAX_IMG_WIDTH*MAX_IMG_HEIGHT];

extern int particle_proc(BYTE *bmp_y, int img_w, int img_h, float limit_low, float limit_up, 
	float &result_low, float &result_up, int &dark_cnt, int &white_cnt, 
	PARTICLE_POS *particle_dark, PARTICLE_POS *particle_white);

INIEdit white_image_config[] =
{
	{IDC_H_WIDTH,				  DLGTYPE_INPUT_INT,   &white_v.area_h_width,			"area_h_width"			 },
	{IDC_V_HEIGHT,				  DLGTYPE_INPUT_INT,   &white_v.area_v_height,			"area_v_height"		 },
	{IDC_PARTICLE_TEST_ON,		  DLGTYPE_CHECK_BOOL,  &white_v.is_particle_test_on,    "is_particle_test_on"	 },
	{IDC_UP_LIMIT,				  DLGTYPE_INPUT_FLOAT, &white_v.particle_up_limit,		"up_limit"				 },
	{IDC_LOW_LIMIT,				  DLGTYPE_INPUT_FLOAT, &white_v.particle_low_limit,		"low_limit"			 },
	{IDC_BLEMISH_TEST_ON,		  DLGTYPE_CHECK_BOOL,  &white_v.is_blemish_test_on,		"is_blemish_test_on"	 },
	{IDC_BLEMISH_RADIUS_VALUE,	  DLGTYPE_INPUT_INT,   &white_v.blemish_radius,			"blemish_radius"		 },
	{IDC_BLEMISH_THRESHOLD_VALUE, DLGTYPE_INPUT_INT,   &white_v.blemish_threshold,		"blemish_threshold"	 },
	{IDC_BLEMISH_CLOSE_VALUE,	  DLGTYPE_INPUT_INT,   &white_v.blemish_close,			"blemish_close"		 },
	{IDC_BLEMISH_SHRINK_DIVIDER,  DLGTYPE_INPUT_INT,   &white_v.blemish_shrink_divider, "blemish_shrink_divider"},
	{IDC_BLEMISH_PATTERN_WIDTH,	  DLGTYPE_INPUT_INT,   &white_v.blemish_pattern_width,  "blemish_pattern_width" },
	{IDC_BLEMISH_PATTERN_HEIGHT,  DLGTYPE_INPUT_INT,   &white_v.blemish_pattern_height, "blemish_pattern_height"},
	{IDC_BLEMISH_SCAN_X,		  DLGTYPE_INPUT_INT,   &white_v.blemish_useX,		    "blemish_useX"},
	{IDC_BLEMISH_SCAN_Y,		  DLGTYPE_INPUT_INT,   &white_v.blemish_useY,		    "blemish_useY"},
														
	{IDC_LS_TEST_ON,			  DLGTYPE_CHECK_BOOL,  &white_v.is_ls_test_on,			"is_ls_test_on"		 },
	{IDC_LS_START_X,			  DLGTYPE_INPUT_INT,   &white_v.ls_start_x,				"ls_start_x"			 },
	{IDC_LS_START_Y,			  DLGTYPE_INPUT_INT,   &white_v.ls_start_y,				"ls_start_y"			 },
	{IDC_LS_BOX_SIZE_RATIO,		  DLGTYPE_INPUT_INT,   &white_v.ls_box_size_ratio,		"ls_box_size_ratio"	 },
	{IDC_LS_TEST_SPEC,			  DLGTYPE_INPUT_INT,   &white_v.ls_test_spec,			"ls_test_spec"			 },
														
	{IDC_OC_TEST_ON,			  DLGTYPE_CHECK_BOOL,  &white_v.is_oc_test_on,			"is_oc_test_on"		 },
	{IDC_OC_X_LOW_LIMIT,		  DLGTYPE_INPUT_INT,   &white_v.oc_x_low_limit,			"oc_x_low_limit"		 },
	{IDC_OC_X_UP_LIMIT,			  DLGTYPE_INPUT_INT,   &white_v.oc_x_up_limit,			"oc_x_up_limit"		 },
	{IDC_OC_Y_LOW_LIMIT,		  DLGTYPE_INPUT_INT,   &white_v.oc_y_low_limit,			"oc_y_low_limit"		 },
	{IDC_OC_Y_UP_LIMIT,			  DLGTYPE_INPUT_INT,   &white_v.oc_y_up_limit,			"oc_y_up_limit"		 },
														
	{IDC_SAVE_RAW_IMAGE,		  DLGTYPE_CHECK_BOOL,  &white_v.is_save_raw_image,      "is_save_raw_image"	 },
	{IDC_SAVE_BMP_IMAGE,		  DLGTYPE_CHECK_BOOL,  &white_v.is_save_bmp_image,      "is_save_bmp_image"	 },
	{IDC_SAVE_RESULT_IMAGE,		  DLGTYPE_CHECK_BOOL,  &white_v.is_save_result_image,   "is_save_result_image"	 },
	{IDC_SAVE_WHITE_REPORT,		  DLGTYPE_CHECK_BOOL,  &white_v.is_save_white_report,   "is_save_white_report"	 },

	{NULL, NULL, NULL, NULL}
};

class WhiteDlg:public ai_win::Dlg
{
public:
	virtual BOOL init(HWND hdlg, HWND hfocus, LPARAM linit);
	virtual void command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify);
	virtual void close(HWND hdlg);

private:
	char path[MAX_PATH];
	int is_initial_finished;

};

BOOL WhiteDlg::init(HWND hdlg, HWND hfocus, LPARAM linit)
{
	is_initial_finished = FALSE;
	sprintf(path, "%s\\recipe\\%s\\white.ini", s_aa::dir, s_aa::recipe_name);
	ai_win::load_keys(path, "setting", hdlg, white_image_config, sizeof(white_image_config)/sizeof(INIEdit));

	// 	ai_win::set_dlg_item_float(hdlg, IDC_LOW_RESULT, aim::image_v.particle_low_result, 3);
	// 	ai_win::set_dlg_item_float(hdlg, IDC_UP_RESULT,	 aim::image_v.particle_up_result,  3);

	SetDlgItemInt(hdlg, IDC_OC_RESULT,	 white_v.oc_test_result, FALSE);
	SetDlgItemInt(hdlg, IDC_OC_X_RESULT, white_v.oc_x_result,	  FALSE);
	SetDlgItemInt(hdlg, IDC_OC_Y_RESULT, white_v.oc_y_result,	  FALSE);

	SET_CHECKER(hdlg, IDC_SAVE_BMP_IMAGE,    white_v.is_save_bmp_image);
	SET_CHECKER(hdlg, IDC_SAVE_RAW_IMAGE,    white_v.is_save_raw_image);
	SET_CHECKER(hdlg, IDC_SAVE_RESULT_IMAGE, white_v.is_save_result_image);

	is_initial_finished = TRUE;

	return TRUE;
}

void WhiteDlg::command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify)
{
	switch (id){
	case IDC_SAVE:
		ai_win::save_keys(path, "setting", hdlg, white_image_config, sizeof(white_image_config)/sizeof(INIEdit));
		break;
	}

	switch (code_notify){
	case EN_CHANGE:
	case BN_CLICKED:
		if (is_initial_finished){
			ai_win::save_keys(path, "setting", hdlg, white_image_config, sizeof(white_image_config)/sizeof(INIEdit));
		}
		break;
	}
}

void WhiteDlg::close(HWND hdlg)
{
	DestroyWindow(hdlg);
	HWND_WHITE = NULL;
}

WhiteDlg white;
BOOL CALLBACK image_test_dlg_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_DLGMSG(hdlg, WM_INITDIALOG,	white.init	);
		HANDLE_DLGMSG(hdlg, WM_COMMAND,		white.command);
		HANDLE_DLGMSG(hdlg, WM_CLOSE,		white.close  );
	}

	return FALSE;
}


int blemish_test(HWND hdlg, BYTE *bmp_y, int img_w, int img_h)
{
	int sub_cx = 0;
	int sub_cy = 0;
	int close_w = white_v.blemish_pattern_width;
	int close_h = white_v.blemish_pattern_height;

	s_blemish::get_sub_image_size(img_w, img_h, white_v.blemish_shrink_divider, sub_cx, sub_cy);
	white_v.blemish_scale_x = sub_cx;
	white_v.blemish_scale_y = sub_cy;
	log("blemish: sub_cx=%d, cy=%d", sub_cx, sub_cy);
	ai_img::Output ai_out;
	BYTE *new_img		= new BYTE[img_w*img_h*3];
	BYTE *shrink_image_y = new BYTE[sub_cx*sub_cy];
	BYTE *scan_image_y = new BYTE[sub_cx*sub_cy];
	BYTE *scan_image_large = new BYTE[9*sub_cx*sub_cy];
	memset(scan_image_y, 255, sub_cx*sub_cy);

	s_blemish::shrink_image(bmp_y, shrink_image_y, img_w, img_h, white_v.blemish_shrink_divider, sub_cx, sub_cy);
	s_blemish::scan_image(shrink_image_y, scan_image_y, sub_cx, sub_cy, white_v.blemish_radius, white_v.blemish_threshold,white_v.blemish_useX,white_v.blemish_useY);
	log("close_val=%d, w=%d, h=%d", white_v.blemish_close, close_w, close_h);
	int avg = s_blemish::filter_close_image(scan_image_y, new_img, sub_cx,sub_cy, close_w, close_h, white_v.blemish_close);

	log("blemish avg=%d", avg);
	int result = TRUE;
	if (avg < 255){
		for (int m=0; m<sub_cx*sub_cy; m++){
			blemish_point[m] = new_img[m*3];
		}
		result = FALSE;
	}

	delete [] shrink_image_y;
	delete [] scan_image_y;
	delete [] new_img;


	white_v.blemish_test_result = result;
	return result;
}

int particle_test(HWND hdlg, BYTE *bmp_y, int img_w, int img_h)
{
	float result_low = 0.0f;
	float result_up = 0.0f;

	int result = particle_proc(bmp_y, img_w, img_h, white_v.particle_low_limit, white_v.particle_up_limit,
		result_low, result_up, particle_dark_cnt, particle_white_cnt, particle_dark, particle_white);
	log("particle result range[%.2f, %.2f]", result_low, result_up);
	log("particle dark cnt:%d, white cnt:%d", particle_dark_cnt, particle_white_cnt);

	if (particle_dark_cnt == 0 && particle_white_cnt == 0){
		white_v.particle_test_result = TRUE;
	}
	else white_v.particle_test_result = FALSE;
	log("dark_cnt=%d, white_cnt=%d", particle_dark_cnt, particle_white_cnt);
	log("particle test end.");

	return white_v.particle_test_result;
}


BYTE *get_bmp_pointer(BYTE *bmp_y, int x, int y, int cx, int cy)
{
	BYTE *pixel = bmp_y+(cy-(1+y))*cx*3 + x*3 + (cy - y -1)*(cx%4);

	if(NULL != pixel)
		return pixel;
	return NULL;
}

int lens_shading_test(HWND hdlg, BYTE *bmp_y, int img_w, int img_h)
{
	RECT rt;
	white_v.ls_box_w = white_v.ls_box_size_ratio*img_w/100;
	white_v.ls_box_h = white_v.ls_box_size_ratio*img_h/100;

	white_v.ls_box[0].x = white_v.ls_start_x;
	white_v.ls_box[0].y = white_v.ls_start_y;

	white_v.ls_box[1].x = img_w - white_v.ls_start_x - white_v.ls_box_w;
	white_v.ls_box[1].y = white_v.ls_start_y;

	white_v.ls_box[2].x = img_w/2 - white_v.ls_box_w/2;
	white_v.ls_box[2].y = img_h/2 - white_v.ls_box_h/2;

	white_v.ls_box[3].x = white_v.ls_start_x;
	white_v.ls_box[3].y = img_h - white_v.ls_start_y - white_v.ls_box_h;

	white_v.ls_box[4].x = img_w - white_v.ls_start_x - white_v.ls_box_w;
	white_v.ls_box[4].y = img_h - white_v.ls_start_y - white_v.ls_box_h;

	for (int i=0; i<5; i++){
		white_v.ls_val[i] = 0.0f;
		white_v.ls_result[i] = TRUE;
	}

	double dsum = 0;
	int start_x = 0, start_y = 0;
	BYTE *pAccess = NULL;
	double davg[5] = {0.0};
	for(int ibox=0; ibox<5; ibox++){
		rt.left = white_v.ls_box[ibox].x;
		rt.top  = white_v.ls_box[ibox].y;
		rt.right = rt.left + white_v.ls_box_w;
		rt.bottom = rt.top + white_v.ls_box_h;

		davg[ibox] = ai_img::get_bmp_y_roi_avg_val(bmp_y, img_w, img_h, &rt);
		log("davg[%d]=%f", ibox, davg[ibox]);
	}

	int result = TRUE;
	for (int i=0; i<5; i++){
		white_v.ls_val[i] = davg[i]*100/davg[2];
		if (white_v.ls_val[i]>= white_v.ls_test_spec){
			white_v.ls_result[i] = TRUE;
		}
		else{
			white_v.ls_result[i] = FALSE;
			result = FALSE;
		}

	}
	white_v.ls_test_result = result;
	return result;
}

BOOL draw_blemish_position(HDC hdc)
{
	HPEN pen = CreatePen(PS_DOT, 2*IMG_WIDTH/3264, RGB(255, 0, 0));
	HPEN old_pen = (HPEN)SelectObject(hdc, pen);

	int divider = white_v.blemish_shrink_divider;
	int shift = divider/2;
	int w = 3;
	int offset = 50;

	if (white_v.blemish_scale_y<=0) white_v.blemish_scale_y = 160;
	if (white_v.blemish_scale_x<=0) white_v.blemish_scale_x = 120;

	if (white_v.blemish_test_result == FALSE){
		for (int i=1; i<white_v.blemish_scale_y; i++){
			for (int j=1; j<white_v.blemish_scale_x; j++){
				if (blemish_point[i*white_v.blemish_scale_x+j] == 0){
					Rectangle(hdc, j*divider+shift-w+white_v.area_start_x, white_v.blemish_scale_y*divider-(i*divider+shift-w)+white_v.area_start_y+offset, 
						j*divider+shift+w+white_v.area_start_x, white_v.blemish_scale_y*divider-(i*divider+shift+w)+white_v.area_start_y+offset);	
				}
			}
		}

	}
	char buffer[20] = {0};
	HFONT font     = CreateFont(120*IMG_WIDTH/3264, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "黑体");
	HFONT old_font = (HFONT)SelectObject(hdc, font);

	if (white_v.blemish_test_result){
		SetTextColor(hdc, RGB(0, 255, 0));
		sprintf(buffer, "Blemish PASS");
	}
	else{
		SetTextColor(hdc, RGB(255, 0, 0));
		sprintf(buffer, "Blemish FAIL");
	}
	int start_x_offset = IMG_WIDTH/4;

	TextOut(hdc, start_x_offset*IMG_WIDTH/3264, 150*IMG_WIDTH/3264, buffer, strlen(buffer));
	SelectObject(hdc, (HFONT)old_font);
	DeleteObject(font);

	SelectObject(hdc, (HPEN)old_pen);
	DeleteObject(pen);

	return TRUE;
}

BOOL draw_defect_position(HDC hdc)
{
	HPEN MyPen  = CreatePen(PS_SOLID, 1, RGB(255,0,0));
	HPEN OldPen = (HPEN)SelectObject(hdc,(HPEN)MyPen);

	int box_size = 25;

	if(white_v.particle_test_result != TRUE){
		for (int i=0; i<particle_dark_cnt; i++){
			Rectangle(hdc, particle_dark[i].x - box_size+white_v.area_start_x, particle_dark[i].y-box_size+white_v.area_start_y,
				particle_dark[i].x+box_size+white_v.area_start_x, particle_dark[i].y+box_size+white_v.area_start_y);
		}

		for (int i=0; i<particle_white_cnt; i++){
			Rectangle(hdc, particle_white[i].x - box_size+white_v.area_start_x, particle_white[i].y-box_size+white_v.area_start_y,
				particle_white[i].x+box_size+white_v.area_start_x, particle_white[i].y+box_size+white_v.area_start_y);
		}
	}

	char buffer[20] = {0};
	HFONT font     = CreateFont(120*IMG_WIDTH/3264, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "黑体");
	HFONT old_font = (HFONT)SelectObject(hdc, font);

	if (white_v.particle_test_result){
		SetTextColor(hdc, RGB(0, 255, 0));
		sprintf(buffer, "Particle PASS");
	}
	else{
		SetTextColor(hdc, RGB(255, 0, 0));
		sprintf(buffer, "Particle FAIL");
	}

	int start_x_offset = IMG_WIDTH/4;

	TextOut(hdc, start_x_offset*IMG_WIDTH/3264, 20*IMG_WIDTH/3264, buffer, strlen(buffer));
	SelectObject(hdc, (HFONT)old_font);
	DeleteObject(font);

	SelectObject(hdc, (HPEN)OldPen);
	DeleteObject(MyPen);
	return TRUE;
}

BOOL draw_lens_shading(HDC hdc)
{
	const int pen_size = 3*IMG_WIDTH/3264;
	HPEN MyPen  = CreatePen(PS_SOLID, pen_size, RGB(255,0,0));
	HPEN OldPen = (HPEN)SelectObject(hdc,(HPEN)MyPen);
	char buffer[20] = {0};
	HFONT font     = CreateFont(60*IMG_WIDTH/3264, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "黑体");
	HFONT old_font = (HFONT)SelectObject(hdc, font);

	for (int i=0; i<5; i++){
		if (white_v.ls_result[i] == FALSE){
			SelectObject(hdc, (HPEN)OldPen);
			DeleteObject(MyPen);
			MyPen = CreatePen(PS_SOLID, pen_size, RGB(255,0,0));
			OldPen = (HPEN)SelectObject(hdc,(HPEN)MyPen);
			SetTextColor(hdc, RGB(255,0,0));
		}
		else{
			SelectObject(hdc, (HPEN)OldPen);
			DeleteObject(MyPen);
			MyPen = CreatePen(PS_SOLID, pen_size, RGB(0,255,0));
			OldPen = (HPEN)SelectObject(hdc,(HPEN)MyPen);
			SetTextColor(hdc, RGB(0,255,0));
		}
		Rectangle(hdc, white_v.ls_box[i].x+white_v.area_start_x, white_v.ls_box[i].y+white_v.area_start_y, 
			white_v.ls_box[i].x+white_v.ls_box_w+white_v.area_start_x, white_v.ls_box[i].y+white_v.ls_box_h+white_v.area_start_y);
		sprintf(buffer, "%2.1f%%", white_v.ls_val[i]);
		TextOut(hdc, white_v.ls_box[i].x+10+white_v.area_start_x, white_v.ls_box[i].y+white_v.ls_box_h/2+white_v.area_start_y, buffer, strlen(buffer));
	}

	SelectObject(hdc, (HFONT)old_font);
	DeleteObject(font);
	font     = CreateFont(120*IMG_WIDTH/3264, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "黑体");
	old_font = (HFONT)SelectObject(hdc, font);

	if (white_v.ls_test_result){
		SetTextColor(hdc, RGB(0, 255, 0));
		sprintf(buffer, "Lens Shading PASS");

	}
	else{
		SetTextColor(hdc, RGB(255, 0, 0));
		sprintf(buffer, "Lens Shading FAIL");
	}

	int start_x_offset = IMG_WIDTH/4;
	TextOut(hdc, start_x_offset*IMG_WIDTH/3264, 280*IMG_WIDTH/3264, buffer, strlen(buffer));

	SelectObject(hdc, (HFONT)old_font);
	DeleteObject(font);

	SelectObject(hdc, (HPEN)OldPen);
	DeleteObject(MyPen);

	return TRUE;
}

BOOL draw_oc_position(HDC hdc)
{
	const int pen_size = 3*IMG_WIDTH/3264;
	HPEN MyPen  = CreatePen(PS_SOLID, pen_size, RGB(0,255,0));
	HPEN OldPen = (HPEN)SelectObject(hdc,(HPEN)MyPen);

	int l = 50;

	int center_x = IMG_WIDTH/2;
	int center_y = IMG_HEIGHT/2;

	MoveToEx(hdc, center_x-l, center_y, NULL);
	LineTo(hdc, center_x+l, center_y);

	MoveToEx(hdc, center_x, center_y-l, NULL);
	LineTo(hdc, center_x, center_y+l);


	SelectObject(hdc, (HPEN)OldPen);
	DeleteObject(MyPen);
	MyPen  = CreatePen(PS_SOLID, pen_size, RGB(255,0,0));
	OldPen = (HPEN)SelectObject(hdc,(HPEN)MyPen);

	MoveToEx(hdc, white_v.oc_result_pos.x-l, white_v.oc_result_pos.y, NULL);
	LineTo(hdc, white_v.oc_result_pos.x+l, white_v.oc_result_pos.y);

	MoveToEx(hdc, white_v.oc_result_pos.x, white_v.oc_result_pos.y-l, NULL);
	LineTo(hdc, white_v.oc_result_pos.x, white_v.oc_result_pos.y+l);

	SelectObject(hdc, (HPEN)OldPen);
	DeleteObject(MyPen);

	return TRUE;
}

void create_report_title()
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	char date_string[MAX_PATH] = {0};
	sprintf(date_string, "%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	sprintf(white_v.report_path, "%s\\report\\image\\%s", s_aa::dir, date_string);

	_mkdir(white_v.report_path);

	white_v.is_report_path_on = TRUE;

}

void save_test_result(BYTE *org_img, BYTE *bmp, int cx, int cy)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	char date_string[MAX_PATH] = {0};
	sprintf(date_string, "%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	char file_name[MAX_PATH] = {0};

	if (white_v.is_save_bmp_image){
		sprintf(file_name, "%s\\[%s]%s-org.bmp", white_v.report_path, s_aa::sn_str, date_string);
		ai_img::save_bmp24_image(file_name, bmp, cx, cy);
	}

	if (white_v.is_save_result_image){
		sprintf(file_name, "%s\\[%s]%s-result.bmp", white_v.report_path, s_aa::sn_str, date_string);
		s_aa::myimg.save_dib_to_bmp(file_name, cx, cy);
		
	}

	if (white_v.is_save_raw_image)
	{
		int pixel_width = 8;
		if (IMG_RAW_FORMAT == 2) pixel_width = 10;
		

		sprintf(file_name, "%s\\[%s]%s-result.raw", white_v.report_path, s_aa::sn_str, date_string);
		ai_img::save_raw_image(file_name, s_aa::img_buf, cx, cy, 10);
	}
}

void save_white_test_report(BYTE *org_img, BYTE *bmp, int cx, int cy)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	char date_string[MAX_PATH] = {0};
	sprintf(date_string, "%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	char report_folder_path[MAX_PATH] = {0};
	char report_folder_name[MAX_PATH] = {0};
	int is_report_exist = create_report_file("white", report_folder_path, report_folder_name);

	if (white_v.is_save_white_report){
		char report_path[MAX_PATH] = {0};
		sprintf(report_path, "%s\\white_report_%s.csv", report_folder_path, report_folder_name);

		HANDLE report_handle = ai_win::create_define_path_csv(report_path);
		char str[500] = {0};

		if (is_report_exist == FALSE)
		{
			SetFilePointer(report_handle, 0, NULL, FILE_BEGIN);
			sprintf(str, "sn, date, test_time, result, p_result, p_up, p_down, p_small, p_middle, p_big, b_result, s_result, s_lt, s_rt, s_c, s_lb, s_rb, o_result, o_x, o_y\r\n");
			ai_win::write_csv(report_handle, str);
		}
		else
		{
			SetFilePointer(report_handle, 0, NULL, FILE_END);
		}

		sprintf(str, "%s, %s, %.2f, %d, %d, %.2f, %.2f, %d , %d, %.2f, %.2f, %.2f, %.2f, %.2f, %d, %d, %d\r\n", 
			s_aa::sn_str, date_string, white_v.test_time, white_v.test_result,
			white_v.particle_test_result, white_v.particle_low_result, white_v.particle_up_result, white_v.blemish_test_result,
			white_v.ls_test_result, white_v.ls_val[0], white_v.ls_val[1], white_v.ls_val[2], white_v.ls_val[3], white_v.ls_val[4],
			white_v.oc_test_result, white_v.oc_x_result, white_v.oc_y_result);
		ai_win::write_csv(report_handle, str);
		CloseHandle(report_handle);
	}

	char file_name[MAX_PATH] = {0};

	if (white_v.is_save_bmp_image){
		sprintf(file_name, "%s\\[%s]white_%s-org.bmp", report_folder_path, s_aa::sn_str, date_string);
		ai_img::save_bmp24_image(file_name, bmp, cx, cy);
	}

	if (white_v.is_save_result_image){
		sprintf(file_name, "%s\\[%s]white_%s-result.bmp", report_folder_path, s_aa::sn_str, date_string);
		s_aa::myimg.save_dib_to_bmp(file_name, cx, cy);
	}

	if (white_v.is_save_raw_image)
	{
		int pixel_width = 8;
		if (IMG_RAW_FORMAT == 2) pixel_width = 10;

		sprintf(file_name, "%s\\[%s]white_%s-result.raw", report_folder_path, s_aa::sn_str, date_string);
		ai_img::save_raw_image(file_name, s_aa::img_buf, cx, cy, 10);
	}
}

int optical_center_test(BYTE *ybuffer, int width, int height, float limit_per)
{
	if(ybuffer == NULL)						  return -1;
	if(width <= 0) 							  return -2;
	if(height <= 0)							  return -3;
	if(limit_per < 0.0f || limit_per >100.0f) return -4;

	BOOL bresult=TRUE;

	double image_half_x = (double)(width/2.0f);
	double image_half_y = (double)(height/2.0f);

	white_v.oc_result_percent = 0.0f;
	white_v.oc_result_pos.x = (LONG)image_half_x;
	white_v.oc_result_pos.y = (LONG)image_half_y;
	white_v.oc_test_result = FALSE;

	int threshold = 200;

	POINT max_pos;
	int center_x = 0, center_y = 0;
	oc_proc(ybuffer, width, height, threshold, center_x, center_y);

	max_pos.x = center_x;
	max_pos.y = center_y;
	white_v.oc_x_result = max_pos.x;
	white_v.oc_y_result = max_pos.y;

	//////////////////////////////////////////////////////////////////////////
	char dir[MAX_PATH] = {0};
	ai_win::get_current_directory(dir);

	double image_field_distance = sqrt((image_half_x*image_half_x) + (image_half_y*image_half_y));
	double result_xpos		    = labs((LONG)(image_half_x - max_pos.x));
	double result_ypos			= labs((LONG)(image_half_y - max_pos.y));
	double max_distance			= sqrt( (result_xpos*result_xpos) + (result_ypos*result_ypos) );
	double result_percent		= (max_distance*100)/image_field_distance;

	if (max_pos.x>= white_v.oc_x_low_limit && max_pos.x <= white_v.oc_x_up_limit &&
		max_pos.y>= white_v.oc_y_low_limit && max_pos.y <= white_v.oc_y_up_limit){
			bresult = TRUE;
	}
	else bresult = FALSE;

	white_v.oc_result_percent = result_percent;
	white_v.oc_result_pos     = max_pos;
	white_v.oc_test_result	   = bresult;
	white_v.oc_limit_pixel    = (int)(image_field_distance * (limit_per*0.01));

	log("Optical Center");
	log("Center Pos: %d, %d", width/2, height/2);
	log("OC Limit: (%d, %d)~(%d, %d)", white_v.oc_x_low_limit, white_v.oc_y_low_limit, 
		white_v.oc_x_up_limit, white_v.oc_y_up_limit);
	log("OC Pos: %d, %d", max_pos.x, max_pos.y);

	log("Center: %s", ((bresult==TRUE)?"PASS":"FAIL"));
	log("------------------------------");

	return bresult;
}

//////////////////////////////////////////////////////////////////////////
//interface function

int init_white_test(void)
{
	char path[MAX_PATH] = {0};
	sprintf(path, "%s\\recipe\\%s\\white.ini", s_aa::dir, s_aa::recipe_name);

	ai_win::load_keys(path, "setting", HWND_WHITE, white_image_config, 100);

	return TRUE;
}

int exec_white_test(BYTE *bmp, int w, int h)
{
	int roi_w = white_v.area_h_width;
	int roi_h = white_v.area_v_height;
	int start_x = (IMG_WIDTH-roi_w)/2;
	int start_y = (IMG_HEIGHT-roi_h)/2;

	RECT rt;
	SetRect(&rt, start_x, start_y, start_x+roi_w, start_y+roi_h);

	BYTE *bmp_y = new BYTE[roi_w*roi_h];
	int result =FALSE;

	if (white_v.is_particle_test_on){
		result = particle_test(HWND_WHITE, bmp_y, roi_w, roi_h);
		white_v.is_particle_draw_on = TRUE;
	}

	if (result && white_v.is_blemish_test_on){
		result = blemish_test(HWND_WHITE, bmp_y, roi_w, roi_h);
		white_v.is_blemish_draw_on = TRUE;
	}

	if (result && white_v.is_ls_test_on){
		result = lens_shading_test(HWND_WHITE, bmp_y, roi_w, roi_h);
		white_v.is_lens_shading_draw_on = TRUE;
	}

	if (result && white_v.is_oc_test_on){
		BYTE *bmp24_y = new BYTE[IMG_WIDTH*IMG_HEIGHT*3];
		s_aa::cimg.bmp24_to_y24(s_aa::bmp_buf, IMG_WIDTH, IMG_HEIGHT, NULL, bmp24_y);
		result = optical_center_test(bmp24_y, IMG_WIDTH, IMG_HEIGHT, white_v.oc_limit_percent);
		white_v.is_oc_draw_on = TRUE;
		delete [] bmp24_y;
	}

	delete [] bmp_y;

	return result;
}

void draw_white_test(HDC memdc)
{
	if (white_v.is_particle_draw_on) draw_defect_position(memdc);
	if (white_v.is_blemish_draw_on)  draw_blemish_position(memdc);
	if (white_v.is_lens_shading_draw_on) draw_lens_shading(memdc);
	if (white_v.is_oc_draw_on)	draw_oc_position(memdc);
}

int particle_proc(BYTE *bmp_y, int img_w, int img_h, float limit_low, float limit_up, 
	float &result_low, float &result_up, int &dark_cnt, int &white_cnt, 
	PARTICLE_POS *particle_dark, PARTICLE_POS *particle_white)

{
	int index = 0;
	double sum_val = 0;
	double avg_val = 0.000f;
	double target_percent = 0.000f;

	double max_val = 0.000;
	double min_val = 1.000;

	int over_limit = FALSE;

	for (int j=0; j<img_h-STEP_H; j += STEP_H){
		for (int i=0; i<img_w-STEP_W; i += STEP_W){
			index = j*img_w+i;
			sum_val = 0;
			for (int n=0; n<STEP_H; n++){
				for (int m=0; m<STEP_W; m++){
					sum_val += bmp_y[index + n*img_w+m];
				}
			}
			avg_val = (float)sum_val/(STEP_W*STEP_H);
			for (int n=0; n<STEP_H; n++){
				for (int m=0; m<STEP_W; m++){
					target_percent = bmp_y[index + n*img_w+m]/avg_val;
					if (target_percent<min_val)	min_val = target_percent;
					if (target_percent>max_val)	max_val = target_percent;
					if (over_limit == FALSE){
						if (target_percent<limit_low){
							particle_dark[dark_cnt].val = bmp_y[index + n*img_w+m];
							particle_dark[dark_cnt].x = i+m; 
							particle_dark[dark_cnt].y = img_h-1-(j+n);
							dark_cnt++;	
							if (dark_cnt>=MAX_PARTICLE_CNT){
								//return FALSE;
								over_limit = TRUE;
							}
						}
						else if(target_percent>limit_up){
							particle_white[white_cnt].val = bmp_y[index + n*img_w+m];
							particle_white[white_cnt].x = i+m;
							particle_white[white_cnt].y = img_h-1-(j+n);						
							white_cnt++;
							if (white_cnt>=MAX_PARTICLE_CNT){
								//	return FALSE;
								over_limit = TRUE;
							}
						}
					}


				}
			}
		}
	}

	result_low = min_val;
	result_up  = max_val;

	if (over_limit) return FALSE;

	return TRUE;
}
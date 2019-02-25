#define _CRT_SECURE_NO_WARNINGS
#include "XTrace.h"
#include "interface.h"
#include "test_reg_mark_chart.h"
#include "inf_motion_aa.h"
#include "inf_motion_pi.h"
#define ABS_WHITE							255
#define ABS_BLACK							0


#define PI						(acos(-1.0)) //3.1415926535   

SlantedBW mysbw;
SlantedBW cam_sbw;
extern SlantedBW_SM FS;//FOCUS_SCAN struct
extern int get_eSfrDiff(void);
extern int get_eCoorDiff(void);

int curve1_start_x = 0;
int curve1_start_y = 0;
int curve2_start_x = 0;
int curve2_start_y = 0;

ai_win::Painting pt;
ai_win::Painting pt2;
HWND hpainting = NULL;
HWND hpainting2 = NULL;
COLORREF mycolor[SLANTED_BW_NUM][4] = {
	{RGB(255, 0, 0),	RGB(255, 62, 62),	RGB(255, 125, 125), RGB(255, 181, 181)},
	{RGB(0, 255, 0),	RGB(62, 255, 62),	RGB(125, 255, 125), RGB(181, 255, 181)},
	{RGB(0, 0, 255),	RGB(62, 62, 255),	RGB(125, 125, 255), RGB(181, 181, 255)},
	{RGB(160, 61, 153), RGB(193, 91, 186),	RGB(210, 136, 205), RGB(226, 177, 223)},
	{RGB(206, 139, 15), RGB(240, 174, 51),	RGB(244, 193, 100), RGB(248, 213, 148)}
};

int start_x_spin[SLANTED_BW_NUM] = {IDC_START_X_SPIN, IDC_START_X_SPIN2, IDC_START_X_SPIN3, IDC_START_X_SPIN4, IDC_START_X_SPIN5};
int start_y_spin[SLANTED_BW_NUM] = {IDC_START_Y_SPIN, IDC_START_Y_SPIN2, IDC_START_Y_SPIN3, IDC_START_Y_SPIN4, IDC_START_Y_SPIN5};

int get_roi_wb_left_right_order(const BYTE *roi, int w, int h)//roi���Һڰ�����ж�
{
	long	sum_left = 0, sum_right = 0;
	int		offset;
	for(int y=0; y<h; y++){
		offset = y*w;
		for(int x=0; x<w/4; x++)	 sum_left  += roi[offset+x];
		for(int x=(w*3)/4; x<w; x++) sum_right += roi[y*w+x];
	}

	int devide_factor = (h*(w/4));
	if(devide_factor <= 0) devide_factor = 1;

	int	avg_left  = sum_left/devide_factor;
	int avg_right = sum_right/devide_factor;

	//	if (abs(avg_left-avg_right) < BLACK_WHITE_DIFF) return -1; 

	return (sum_left>sum_right)?0:1; //check if left is black
}

int get_roi_wb_bottom_up_order(const BYTE *roi, int w, int h)
{
	int sum_up = 0, sum_down=0;
	//int offset;
	for (int y=0; y<h/4; y++){
		for (int x=0; x<w; x++){
			sum_down += roi[y*w+x];
			sum_up += roi[(h-1-y)*w+x];
		}
	}
	int devide_factor = (h*(w/4));
	if(devide_factor <= 0) devide_factor = 1;

	int avg_up = sum_up/devide_factor;
	int avg_down = sum_down/devide_factor;

	//	if (abs(avg_down-avg_up)<BLACK_WHITE_DIFF) return -1;

	return (sum_down>sum_up)?0:1; //check if bottom is black
}

double get_binary_image_edge_line(const BYTE *line, int num, int start_black)
{
	int white_count = 0, black_count = 0;
// 	for (int i=0; i<num; i++){
// 		if (line[i]>= mysbw.binary_threhold){
// 			white_count++;
// 		}
// 		else black_count++;
// 	}

	long white_sum, black_sum;
	double white_mean, black_mean;

	long sum = 0;
	for(int i=0; i<num; i++) sum += line[i];
	if(num == 0) return 0;

	double mean = (double)sum/(double)num;

	for(int j=0; j<2; j++){
		white_sum=0, white_count=0;
		black_sum=0, black_count=0;
		for(int i=0; i<num; i++){
			if((double)line[i]<mean){
				black_sum += line[i];
				black_count++;
			}
			else{
				white_sum += line[i];
				white_count++;
			}
		}
		if(black_count==0)	black_mean = 0; 
		else 			    black_mean = (double)black_sum/(double)black_count;

		if(white_count==0)	white_mean = 0; 
		else  				white_mean = (double)white_sum/(double)white_count;

		mean = (black_mean+white_mean)/2.0f;
	}

	return (double)(start_black?black_count:white_count);
}

int get_best_fit_line(const double *x, const double *y, int n, double &slope, double &offset)
{
	int result = TRUE;
	double x_mean=0, y_mean=0, numerator=0, denominator=0;

	for(int i=0; i<n; i++)  x_mean+=x[i],y_mean+=y[i];

	x_mean /= (double)n;
	y_mean /= (double)n;

	for(int i=0; i<n; i++){
		numerator	+= y[i]*(x[i] - x_mean);
		denominator += x[i]*(x[i] - x_mean);
	}

	if (denominator == 0){
		slope = 0;
		offset = 0;
		result = FALSE;
	}
	else{
		slope  = numerator/denominator;
		offset = y_mean - x_mean*slope;
		result = TRUE;
	}
	return result;
}

#define MAX_SFR_ROI_HEIGHT			5000
#define MIN_EDGE_CENTER_WIDTH		32  //32

int find_roi_h_edge(const BYTE *roi, int start_x, int start_y, int w, int h, double *edge_x, double &slope, double &offset, int &left_black)
{
	left_black = get_roi_wb_left_right_order(roi, w, h);
	if (left_black == -1){
		log("find edge fail.");
		return FALSE;
	}

	int index_y = 0;
	double temp_edge_x[MAX_SFR_ROI_HEIGHT] = {0.0};
	double temp_edge_y[MAX_SFR_ROI_HEIGHT] = {0.0};
	for (int y=0; y<h; y++){
		index_y = y*w;
		temp_edge_x[y] = get_binary_image_edge_line(&roi[index_y], w, left_black);
		temp_edge_y[y] = (double)y;

		// 		if (temp_edge_x[y]<(double)(MIN_EDGE_CENTER_WIDTH/2) || (double)(w-temp_edge_x[y])<(double)(MIN_EDGE_CENTER_WIDTH/2)){
		// 			return -1;
		// 		}
		temp_edge_x[y] += start_x;
		temp_edge_y[y] = start_y + h-1-temp_edge_y[y];


	}
	int result = get_best_fit_line(temp_edge_x, temp_edge_y, h, slope, offset);
	//	XTRACE("h slope=%f\n", slope);
	if (edge_x!=NULL){
		if (result == TRUE){
			for (int y=0; y<h; y++) edge_x[y] = (double)temp_edge_y[y]/slope-(offset/slope);
		}
		else{
			for (int y=0; y<h; y++) edge_x[y] = temp_edge_x[y];
		}
	}

	return result;
}

int find_roi_v_edge(const BYTE *roi, int start_x, int start_y, int w, int h, double *edge_y, double &slope, double &offset, int &bottom_black)
{
	bottom_black = get_roi_wb_bottom_up_order(roi, w, h);
	if (bottom_black == -1) return FALSE;

	int index_y = 0;
	double temp_edge_x[MAX_SFR_ROI_HEIGHT] = {0.0};
	double temp_edge_y[MAX_SFR_ROI_HEIGHT] = {0.0};

	BYTE *y_line = new BYTE[h];

	for (int x=0; x<w; x++){
		for (int y=0; y<h; y++){
			y_line[y] = roi[y*w+x];
		}
		temp_edge_y[x] = get_binary_image_edge_line(y_line, h, bottom_black);
		temp_edge_x[x] = (double)x; 

		// 		if (temp_edge_y[x]<(double)(MIN_EDGE_CENTER_WIDTH/2) || (double)(h-temp_edge_y[x])<(double)(MIN_EDGE_CENTER_WIDTH/2)){
		// 			return -1;
		// 		}

		temp_edge_x[x] += start_x;
		temp_edge_y[x] = start_y + h-1-temp_edge_y[x];

	//	XTRACE("bf xy, %f, %f\n", temp_edge_x[x], temp_edge_y[x]);
	}
	int result = get_best_fit_line(temp_edge_x, temp_edge_y, w, slope, offset);
	//	XTRACE("v slope=%f\n", slope);
	if (edge_y != NULL && result == TRUE){
		for (int x=0; x<w; x++){
			edge_y[x] = temp_edge_x[x]*(slope)+offset;
			//XTRACE("af xy, %f, %f, %f\n", temp_edge_x[x], edge_y[x], temp_edge_y[x]);
		}
	}
	//XTRACE("============================================\n");
	delete [] y_line;

	return result;

}

INIEdit config[] = {
	{IDC_SFR_FIELD_A_ON,	DLGTYPE_CHECK_BOOL,		&mysbw.use_field_a,			"use_field_a"},
	{IDC_USE_MTF50P,		DLGTYPE_CHECK_BOOL,		&mysbw.use_mtf50p,			"use_mtf50p"},
	{IDC_USE_MTFXP,			DLGTYPE_INPUT_INT,		&mysbw.mtfxp,				"mtfxp"},
	{IDC_SFR_FREQ1,			DLGTYPE_INPUT_DOUBLE,	&mysbw.freq_val1,			"freq_val1"},
	{IDC_SFR_FIELD0_SPEC1,  DLGTYPE_INPUT_INT,		&mysbw.spec1[0],			"center_spec1"},
	{IDC_SFR_FIELDA_SPEC1,  DLGTYPE_INPUT_INT,		&mysbw.spec1[1],			"edge1_spec1"},
 	{IDC_SFR_FIELD0_SPEC2,  DLGTYPE_INPUT_INT,		&mysbw.spec2[0],			"center_spec2"},
 	{IDC_SFR_FIELDA_SPEC2,  DLGTYPE_INPUT_INT,		&mysbw.spec2[1],			"edge1_spec2"},
// 	{IDC_SFR_FIELDB_SPEC2,  DLGTYPE_INPUT_INT,		&mysbw.spec2[2],			"edge2_spec2"},
	{IDC_EDGE_COOR_DIFF,    DLGTYPE_INPUT_INT,		&mysbw.coorDiff_spec,		"eCoorspec"},
	{IDC_EDGE_SFR_DIFF,		DLGTYPE_INPUT_INT,		&mysbw.sfrDiff_spec,		"eSfrspec"},
	{IDC_OC_LIMIT,			DLGTYPE_INPUT_INT,		&mysbw.oc_limit,			"edge2_oc_limit"},
	{IDC_TILT_LIMIT,		DLGTYPE_INPUT_DOUBLE,	&mysbw.tilt_limit,			"edge2_tilt_limit"},

	{IDC_ROI_W,				DLGTYPE_INPUT_INT,		&mysbw.roi_w[0],			"center_roi_w"},
	{IDC_ROI_H,				DLGTYPE_INPUT_INT,		&mysbw.roi_h[0],			"center_roi_h"},
	{IDC_ROI_W2,			DLGTYPE_INPUT_INT,		&mysbw.roi_w[1],			"edge1_roi_w"},
	{IDC_ROI_H2,			DLGTYPE_INPUT_INT,		&mysbw.roi_h[1],			"edge1_roi_h"},


// 	{IDC_SFR_X,				DLGTYPE_INPUT_INT,		&mysbw.sfr_chartsize[0],	"sfr_x_begin"},
// 	{IDC_SFR_Y,				DLGTYPE_INPUT_INT,		&mysbw.sfr_chartsize[1],	"sfr_y_begin"},
// 	{IDC_SFR_X_END,			DLGTYPE_INPUT_INT,		&mysbw.sfr_chartsize[2],	"sfr_x_end"},
// 	{IDC_SFR_Y_END,			DLGTYPE_INPUT_INT,		&mysbw.sfr_chartsize[3],	"sfr_y_end"},    ����sfr��fs���߻�ͼ

	{IDC_SEARCH_W,			DLGTYPE_INPUT_INT,		&mysbw.search_w[0],			"search_w0"},
	{IDC_SEARCH_H,			DLGTYPE_INPUT_INT,		&mysbw.search_h[0],			"search_h0"},
	{IDC_SEARCH_W2,			DLGTYPE_INPUT_INT,		&mysbw.search_w[1],			"search_w1"},
	{IDC_SEARCH_H2,			DLGTYPE_INPUT_INT,		&mysbw.search_h[1],			"search_h1"},

	{IDC_PATTERN_SIZE,		DLGTYPE_INPUT_INT,		&mysbw.pattern_size[0],		"pattern_size0"},
	{IDC_PATTERN_SIZE2,		DLGTYPE_INPUT_INT,		&mysbw.pattern_size[1],		"pattern_size1"},

	{IDC_ROI_DISTANCE,		DLGTYPE_INPUT_INT,		&mysbw.roi_distance[0],		"roi_distance0"},
	{IDC_ROI_DISTANCE2,		DLGTYPE_INPUT_INT,		&mysbw.roi_distance[1],		"roi_distance1"},

	{IDC_START_X,			DLGTYPE_INPUT_INT,		&mysbw.start_x[0],			"start_x0"},
	{IDC_START_Y,			DLGTYPE_INPUT_INT,		&mysbw.start_y[0],			"start_y0"},
	{IDC_START_X2,			DLGTYPE_INPUT_INT,		&mysbw.start_x[1],			"start_x1"},
	{IDC_START_Y2,			DLGTYPE_INPUT_INT,		&mysbw.start_y[1],			"start_y1"},
	{IDC_START_X3,			DLGTYPE_INPUT_INT,		&mysbw.start_x[2],			"start_x2"},
	{IDC_START_Y3,			DLGTYPE_INPUT_INT,		&mysbw.start_y[2],			"start_y2"},
	{IDC_START_X4,			DLGTYPE_INPUT_INT,		&mysbw.start_x[3],			"start_x3"},
	{IDC_START_Y4,			DLGTYPE_INPUT_INT,		&mysbw.start_y[3],			"start_y3"},
	{IDC_START_X5,			DLGTYPE_INPUT_INT,		&mysbw.start_x[4],			"start_x4"},
	{IDC_START_Y5,			DLGTYPE_INPUT_INT,		&mysbw.start_y[4],			"start_y4"},

	{IDC_ROI_LEFT,			DLGTYPE_CHECK_BOOL,		&mysbw.roi_enable[3],		"enable_roi_left"},
	{IDC_ROI_BOTTOM,		DLGTYPE_CHECK_BOOL,		&mysbw.roi_enable[0],		"enable_roi_bottom"},
	{IDC_ROI_RIGHT,			DLGTYPE_CHECK_BOOL,		&mysbw.roi_enable[1],		"enable_roi_right"},
	{IDC_ROI_TOP,			DLGTYPE_CHECK_BOOL,		&mysbw.roi_enable[2],		"enable_roi_top"},
	{IDC_SAVE_BMP_IMAGE,	DLGTYPE_CHECK_BOOL,		&mysbw.is_save_bmp,			"save_bmp_img"},
	{IDC_SAVE_RESULT_IMAGE,	DLGTYPE_CHECK_BOOL,		&mysbw.is_save_result_bmp,	"save_result_bmp_img"},
	{IDC_SAVE_SFR_CURVE,	DLGTYPE_CHECK_BOOL,		&mysbw.is_save_sfr_curve,	"save_sfr_curve"},
	{IDC_BIN_THRESHOLD,		DLGTYPE_INPUT_INT,		&mysbw.binary_threhold,		"bin_threshold"},

	{NULL, NULL, NULL, NULL}
};


//////////////////////////////////////////////////////////////////////////
//interface parameter&function pointers

extern BOOL CALLBACK reg_mark_chart_setting_dlg_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK reg_mark_chart2_setting_dlg_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);

extern BOOL CALLBACK reg_mark_chart_sfrcurve_dlg_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK reg_mark_tf_dlg_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);

class SlantedBWDlg:public ai_win::Dlg
{
public:
	virtual BOOL init(HWND hdlg, HWND hfocus, LPARAM linit);
	virtual void command(HWND hdlg, int id, HWND hctrl, UINT code_notify);
	virtual void close(HWND hdlg);

private:
	char config_path[MAX_PATH];
	int is_init_done;
};

SlantedBWDlg sbw_dlg;

BOOL CALLBACK reg_mark_chart_setting_dlg_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_DLGMSG(hdlg, WM_INITDIALOG, sbw_dlg.init);
		HANDLE_DLGMSG(hdlg, WM_COMMAND,	  sbw_dlg.command);
		HANDLE_DLGMSG(hdlg, WM_CLOSE, sbw_dlg.close);
	}

	return FALSE;
}

BOOL SlantedBWDlg::init(HWND hdlg, HWND hfocus, LPARAM linit)
{
	log("init reg.");
	is_init_done = FALSE;
	sprintf(config_path, "%s\\recipe\\%s\\slanted_bw.ini", s_aa::dir, s_aa::recipe_name);
	ai_win::load_keys(config_path, "test", hdlg, config, sizeof(config)/sizeof(config[0]));

	char str[100] = {0};
	for (int i=0; i<SLANTED_BW_NUM; i++){
		sprintf(str, "ROI_%d", i);
		COMBO_ADDSTR(hdlg, IDC_SELECT_ROI, str);

		SET_SPINNER_RANGE(hdlg, start_x_spin[i], 0, IMG_WIDTH);
		SET_SPINNER_RANGE(hdlg, start_y_spin[i], 0, IMG_HEIGHT);
	}
	COMBO_SETCURSEL(hdlg, IDC_SELECT_ROI, 0);
	is_init_done = TRUE;
	return TRUE;
}

void SlantedBWDlg::command(HWND hdlg, int id, HWND hctrl, UINT code_notify)
{
	switch (id)
	{
	case IDC_SAVE_RM_CONFIG:
		ai_win::save_keys(config_path, "test", hdlg, config, sizeof(config)/sizeof(config[0]));
		log("test1 configure saved.");
		break;
	}

	switch (code_notify){
	case EN_CHANGE:
//		log("init down=%d",is_init_done);  roi���걻�ܸ�debug
		if (is_init_done == TRUE){
			switch (id){
			case IDC_START_X:  mysbw.start_x[0] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_START_X2: mysbw.start_x[1] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_START_X3: mysbw.start_x[2] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_START_X4: mysbw.start_x[3] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_START_X5: mysbw.start_x[4] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;

			case IDC_START_Y:  mysbw.start_y[0] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_START_Y2: mysbw.start_y[1] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_START_Y3: mysbw.start_y[2] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_START_Y4: mysbw.start_y[3] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
			case IDC_START_Y5: mysbw.start_y[4] = GetDlgItemInt(hdlg, id, NULL, FALSE); break;
				

			}
		}
		break;
	}
}

void SlantedBWDlg::close(HWND hdlg)
{
	EndDialog(hdlg, FALSE);
}

void set_roi(void)
{
	mysbw.search_rt[0].left   = mysbw.start_x[0];
	mysbw.search_rt[0].top    = mysbw.start_y[0];
	mysbw.search_rt[0].right  = mysbw.search_rt[0].left + mysbw.search_w[0];
	mysbw.search_rt[0].bottom = mysbw.search_rt[0].top+mysbw.search_h[0];

	mysbw.search_rt[1].left   = mysbw.start_x[1];
	mysbw.search_rt[1].top    = mysbw.start_y[1];
	mysbw.search_rt[1].right  = mysbw.search_rt[1].left + mysbw.search_w[1];
	mysbw.search_rt[1].bottom = mysbw.search_rt[1].top+mysbw.search_h[1];

	mysbw.search_rt[2].left   = mysbw.start_x[2];
	mysbw.search_rt[2].top	  = mysbw.start_y[2];
	mysbw.search_rt[2].right  = mysbw.search_rt[2].left + mysbw.search_w[1];
	mysbw.search_rt[2].bottom = mysbw.search_rt[2].top+mysbw.search_h[1];

	mysbw.search_rt[3].left   = mysbw.start_x[3];
	mysbw.search_rt[3].top	  = mysbw.start_y[3];
	mysbw.search_rt[3].right  = mysbw.search_rt[3].left + mysbw.search_w[1];
	mysbw.search_rt[3].bottom = mysbw.search_rt[3].top+mysbw.search_h[1];

	mysbw.search_rt[4].left   = mysbw.start_x[4];
	mysbw.search_rt[4].top	  = mysbw.start_y[4];
	mysbw.search_rt[4].right  = mysbw.search_rt[4].left + mysbw.search_w[1];
	mysbw.search_rt[4].bottom = mysbw.search_rt[4].top+mysbw.search_h[1];
}


//////////////////////////////////////////////////////////////////////////
void draw_sfr_curve(int x_cnt)//SFR curve
{
	HWND mypainting = GetDlgItem(HWND_MAIN, IDC_SFR_CURVE);
	int x_ratio = (int)(2560.0/(x_cnt*10)+1);
	int w = x_cnt*10*x_ratio, h = 210;
	int begin_x = 600, end_x = 600;
	int begin_y = 25, end_y = 50;

	float factor_x = x_cnt*x_ratio*1.0f, interval_x = 1.0f;
	float factor_y = 2.0f, interval_y = 20.0f;

	int full_w = w + begin_x + end_x;
	int full_h = h + begin_y + end_y;

	int font_size = 14;
	int axis_x_offset[4] = {-200,5,-100,-20};
	//                     ������ƽ�ƣ������󣻺����굥λƽ�ƣ����������£������ϣ������굥λ����
	int axis_y_offset[4] = {-500,-8,-500,-15};
	//������
	
	HDC memdc = pt.create(mypainting, w, h, 600, 600, 25, 50, RGB(156,170,193));//���񱳾�ɫ,
	SetTextColor(memdc, COLOR_WHITE);
	pt.set_axis(font_size, axis_x_offset, axis_y_offset);
	pt.draw_axis_x_float(memdc,factor_x, interval_x, "cy/px", 0.0f, 0.1f);
	pt.draw_axis_y_float(memdc, factor_y, interval_y, "SFR");

	HFONT font     = CreateFont(15, 0, 0, 0, FW_BLACK, 0,0,0, ANSI_CHARSET, 0,0,0,0, "����");
	HFONT old_font = (HFONT)SelectObject(memdc, font);
	TextOut(memdc, 2560, 5, "MTF����", 7);//��ͷλ���趨�����ң����£����ݣ�����
	DeleteObject(font);
	font     = CreateFont(font_size, 0, 0, 0, FW_BLACK, 0,0,0, ANSI_CHARSET, 0,0,0,0, "����");
	old_font = (HFONT)SelectObject(memdc, font);
	HPEN pen, old_pen;
	pen = CreatePen(PS_SOLID, 1, COLOR_BLUE);
	old_pen = (HPEN)SelectObject(memdc, pen);

	int select_num = 0;
	for (int j=0; j<SLANTED_BW_NUM; j++){
		select_num = (int)((j-1)/4.0+1);
		if (select_num == 1 && !mysbw.use_field_a) continue;
		else if (select_num == 2 && !mysbw.use_field_b) continue;

		for (int i=0; i<4; i++){
			if (mysbw.roi_enable[i] == FALSE) continue;
			DeleteObject(pen);
			pen = CreatePen(PS_SOLID, 1, mycolor[j][i]);
			SelectObject(memdc, pen);
			MoveToEx(memdc, begin_x, full_h-(begin_y+(int)(mysbw.amp[j][i][0]*factor_y)), NULL);

#ifdef SFR_METHOD_EX
			for (int m=1; m<x_cnt/2+1; m++){
				LineTo(memdc, begin_x+(int)((mysbw.freq[j][i][m]*x_cnt)*10*x_ratio), full_h-(begin_y+(int)(mysbw.amp[j][i][m]*factor_y)));
			}
#else
			for (int m=1; m<x_cnt/2; m++){
				LineTo(memdc, begin_x+(int)((mysbw.freq[j][i][m]*x_cnt)*10*x_ratio), full_h-(begin_y+(int)(mysbw.amp[j][i][m]*factor_y)));
			}
#endif
		}
	}

	DeleteObject(pen);
	DeleteObject(font);
	pt.release(hpainting, memdc);
}

void draw_fs_curve2(int x_cnt)//fs curve
{
	HWND mypainting = GetDlgItem(HWND_MAIN, IDC_SFR_SCAN);
	int x_ratio = (int)(2560.0/(x_cnt)+1);
	int w = x_cnt*x_ratio, h = 210/*(int)6*mysbw.spec1[0]*/;
	int begin_x = w/9, end_x = w/9;
	int begin_y = h/9, end_y = h/20;

	int factor_x = x_ratio, interval_x = 1;
	float factor_y = 2.0f, interval_y = 20.0f;

	int full_w = w + begin_x + end_x;
	int full_h = h + begin_y + end_y;

	int font_size = 10;
	int axis_x_offset[4] = {-w/100, 5, w/40, -10};
	int axis_y_offset[4] = {-w/10, -5, -w/10, -20};

	HDC memdc = pt.create(mypainting, w, h, begin_x, end_x, begin_y, end_y, RGB(156,170,193));//���񱳾�ɫ
	SetTextColor(memdc, COLOR_WHITE);
	pt.set_axis(font_size, axis_x_offset, axis_y_offset);
	pt.draw_axis_x_int(memdc,factor_x, interval_x, "steps", 0, 1);
	pt.draw_axis_y_float(memdc, factor_y, interval_y, "MTF");

	HFONT font     = CreateFont(15, 0, 0, 0, FW_BLACK, 0,0,0, ANSI_CHARSET, 0,0,0,0, "����");
	HFONT old_font = (HFONT)SelectObject(memdc, font);
	TextOut(memdc, begin_x+w/10, 5, "MTF SCAN", 9);
	DeleteObject(font);
	font     = CreateFont(font_size, 0, 0, 0, FW_BLACK, 0,0,0, ANSI_CHARSET, 0,0,0,0, "����");
	old_font = (HFONT)SelectObject(memdc, font);
	HPEN pen, old_pen;
	pen = CreatePen(PS_SOLID, 1, COLOR_BLUE);
	old_pen = (HPEN)SelectObject(memdc, pen);

	int select_num = 0;
	for (int j=0; j<SLANTED_BW_NUM; j++){
		select_num = (int)((j-1)/4.0+1);
		if (select_num == 1 && !mysbw.use_field_a) continue;
		else if (select_num == 2 && !mysbw.use_field_b) continue;
		//log("1");
		for (int i=FS.select_roi; i<FS.select_roi+1; i++){
			//log("2");
			if (mysbw.roi_enable[i] == FALSE) continue;
			DeleteObject(pen);
			pen = CreatePen(PS_SOLID, 1, mycolor[j][i]);
			SelectObject(memdc, pen);
			MoveToEx(memdc, begin_x, full_h-begin_y, NULL);

			for (int m=0; m<FS.fs_index+1; m++){
			//	log("3");
				LineTo(memdc, begin_x+(int)(m*x_ratio), full_h-(begin_y+(int)(FS.sfr_val[i][j][m]*factor_y)));
			//	log("y=%d",(int)(s_aa::mysbw_sm.sfr_val[i][j][m]*factor_y));
			}
		}
	}

	DeleteObject(pen);
	DeleteObject(font);
	pt.release(hpainting, memdc);
}

//////////////////////////////////////////////////////////////////////////
//interface function

void draw_reg_mark_chart(HDC memdc)
{
	char buffer[100] = {0};
	HPEN pen		= CreatePen(PS_SOLID, 2, COLOR_GHOST_WHITE);
	HPEN old_pen = (HPEN)SelectObject(memdc, pen);

	//	HFONT hfont	 = CreateFont(50, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "����");  //13M camera
	HFONT hfont	 = CreateFont(24, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "����");  //2M camera
	HFONT old_font = (HFONT)SelectObject(memdc, hfont);
	SetTextColor(memdc, COLOR_BLUE);
	int select_num = 0;
	for (int i=0; i<SLANTED_BW_NUM; i++){
		select_num = (int)((i-1)/4.0+1);
		if (select_num == 1 && !mysbw.use_field_a) continue;
		/*else if (select_num == 2 && !mysbw.use_field_b) continue;*/
		ai_img::Output::draw_crossline(memdc, mysbw.search_rt[i].left+mysbw.search_w[select_num]/2, 
			mysbw.search_rt[i].top+mysbw.search_h[select_num]/2, mysbw.search_w[select_num], 
			mysbw.search_h[select_num]);
		Rectangle(memdc, mysbw.search_rt[i].left, mysbw.search_rt[i].top, mysbw.search_rt[i].right, mysbw.search_rt[i].bottom);
	//	log("***");
		sprintf(buffer, "#%d", i);
		TextOut(memdc, mysbw.search_rt[i].left, mysbw.search_rt[i].top, buffer, strlen(buffer));
	}

	DeleteObject(pen);
	pen		= CreatePen(PS_SOLID, 2, COLOR_YELLOW);
	old_pen = (HPEN)SelectObject(memdc, pen);
	for (int i=0; i<SLANTED_BW_NUM; i++){
		select_num = (int)((i-1)/4.0+1);
		if (select_num == 1 && !mysbw.use_field_a) continue;
		else if (select_num == 2 && !mysbw.use_field_b) continue;

		//		ai_img::Output::draw_crossline(memdc, (int)mysbw.oc_x[i]/*s.center_x[i]*/, (int)mysbw.oc_y[i]/*s.center_y[i]*/, 100, 100); //13M camera
		ai_img::Output::draw_crossline(memdc, (int)mysbw.oc_x[i]/*s.center_x[i]*/, (int)mysbw.oc_y[i]/*s.center_y[i]*/, 20, 20); //13M camera
		//ai_img::Output::draw_crossline(memdc, (int)mysbw.center_x[i], (int)mysbw.center_y[i], 20, 20); //13M camera
	}

	DeleteObject(pen);
	pen		= CreatePen(PS_SOLID, 2, COLOR_RED);
	old_pen = (HPEN)SelectObject(memdc, pen);
	for (int i=0; i<SLANTED_BW_NUM; i++){
		select_num = (int)((i-1)/4.0+1);
		if (select_num == 1 && !mysbw.use_field_a) continue;
		else if (select_num == 2 && !mysbw.use_field_b) continue;
		ai_img::Output::draw_crossline(memdc, (int)mysbw.center_x[i], (int)mysbw.center_y[i], 20, 20); //13M camera

	}
	DeleteObject(pen);
	RECT myrt;
	pen		= CreatePen(PS_SOLID, 2, COLOR_YELLOW);
	old_pen = (HPEN)SelectObject(memdc, pen);

	for (int j=0; j<SLANTED_BW_NUM; j++){
		select_num = (int)((j-1)/4.0+1);
		if (select_num == 1 && !mysbw.use_field_a) continue;
		else if (select_num == 2 && !mysbw.use_field_b) continue;
		else if (mysbw.find_target[j] == FALSE)	continue;

		for (int i=0; i<4; i++){
			if (mysbw.roi_enable[i] == FALSE) continue;
			DeleteObject(pen);
			pen		= CreatePen(PS_SOLID, 1, ((mysbw.sfr_result[j][i] == TRUE)?COLOR_GREEN:COLOR_RED));
			SelectObject(memdc, pen);

			Rectangle(memdc, mysbw.roi_rt[j][i].left, mysbw.roi_rt[j][i].top, mysbw.roi_rt[j][i].right, mysbw.roi_rt[j][i].bottom);
			SetTextColor(memdc, ((mysbw.sfr_result[j][i]==TRUE)?COLOR_GREEN:COLOR_RED));

		//	sprintf(buffer, "[%d,%d,1]%.1f", j, i, mysbw.sfr_val1[j][i]);
			sprintf(buffer, "%.1f", mysbw.sfr_val1[j][i]);
		//	log("sfr_val:%.2f",mysbw.sfr_val1[j][i]);
			if (i==0){
				TextOut(memdc, mysbw.roi_rt[j][i].left, mysbw.roi_rt[j][i].bottom+10, buffer, strlen(buffer));
			}
			else if (i==1){
				TextOut(memdc, mysbw.roi_rt[j][i].right-10, mysbw.roi_rt[j][i].bottom, buffer, strlen(buffer));
			}
			else if (i==2){
				TextOut(memdc, mysbw.roi_rt[j][i].right+2, mysbw.roi_rt[j][i].top, buffer, strlen(buffer));
			}
			else if (i==3){
				TextOut(memdc, mysbw.roi_rt[j][i].left-20, mysbw.roi_rt[j][i].bottom, buffer, strlen(buffer));
			}
			HBRUSH mybrush   = CreateSolidBrush(mycolor[j][i]);
			HBRUSH old_brush = (HBRUSH)SelectObject(memdc, (HBRUSH)mybrush);

			myrt.left   = mysbw.roi_rt[j][i].left;
			myrt.top    = mysbw.roi_rt[j][i].top;
			myrt.right  = myrt.left + mysbw.roi_w[0]/5;
			myrt.bottom = myrt.top +mysbw.roi_h[0]/5;
			FillRect(memdc, &myrt, mybrush);
			DeleteObject(mybrush);
			SelectObject(memdc, old_brush);
		}
	//	double buf_n;
	//	int x,y;
	//	x=(int)mysbw.oc_x[j]/1;
	//	y=(int)mysbw.oc_y[j]/1+10;
	//	buf_n=s_aa::bmp_buf_t[3*(y*IMG_WIDTH+x-10)]+s_aa::bmp_buf_t[3*(y*IMG_WIDTH+x+10)];
	//	y=(int)mysbw.oc_y[j]/1-10;
	//	buf_n=s_aa::bmp_buf_t[3*(y*IMG_WIDTH+x-10)]+s_aa::bmp_buf_t[3*(y*IMG_WIDTH+x+10)]+buf_n;
		sprintf(buffer, "(%.1f, %.1f)[%.1f]", mysbw.oc_x[j], mysbw.oc_y[j], (atan(mysbw.oc_slope[j][0])*180.0/PI));
// 		if (j == 2){
// 			TextOut(memdc, mysbw.search_rt[j].right, mysbw.search_rt[j].top+10, buffer, strlen(buffer));
// 		}
// 		else{
// 			TextOut(memdc, mysbw.search_rt[j].left, mysbw.search_rt[j].top-20, buffer, strlen(buffer));
// 		}
		TextOut(memdc, mysbw.search_rt[j].left, mysbw.search_rt[j].top-20, buffer, strlen(buffer));
	}
	DeleteObject(hfont);
// 	hfont	 = CreateFont(50, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "����");
// 	old_font = (HFONT)SelectObject(memdc, hfont);
// 	SetTextColor(memdc, (mysbw.test_result?COLOR_GREEN:COLOR_RED));
// 	TextOut(memdc, 10, 10, (mysbw.test_result?"PASS":"FAIL"), 4);
// 
// 	DeleteObject(hfont);
	DeleteObject(pen);
}

int init_reg_mark_chart_test(void)
{
	char path[MAX_PATH] = {0};

	sprintf(path, "%s\\recipe\\%s\\slanted_bw.ini", s_aa::dir, s_aa::recipe_name);
	ai_win::load_keys(path, "test", HWND_REG_MARK_CHART, config, sizeof(config)/sizeof(config[0]));

	return TRUE;
}

ai_img::Output myoutput;

int exec_reg_mark_chart_test(BYTE *img, BYTE *bmp, int w, int h)
{
	int a[100]={0}; 
	int err = FALSE;

	int roi_w = 0;
	int roi_h = 0;
	int center_x = 0;
	int center_y = 0;
	int center_x2 = 0;
	int center_y2 = 0;
	int search_w = 0;
	int search_h = 0;

	int roi_distance = 0;
	int pattern_size = 0;
	int my_edge_x[4] = {0};
	int my_edge_y[4] = {0};
	double mtfxp_val = 0.0;
	double mtfxp_freq = 0.0;

	double my_edge[4][500];
	int bw_order[4] = {0};

	int t = 0;
	BYTE *bmp_y = NULL;
	BYTE *bin = NULL;
	BYTE *p = NULL;
	set_roi();
//	log("y1=%d,y2=%d,y3=%d,y4=%d,y5=%d",mysbw.start_y[0],mysbw.start_y[1],mysbw.start_y[2],mysbw.start_y[3],mysbw.start_y[4]);
	int select_num = 0;
	double cross_x[2] = {0.0};
	double cross_y[2] = {0.0};

	//mysbw.test_result = TRUE;
	//mysbw.B_UVResult  = TRUE;
	//mysbw.A_UVResult  = TRUE;

	if (mysbw.binary_threhold<=0) mysbw.binary_threhold = 80;

	int one_edge_result = TRUE;
	int find_edge_result = TRUE;
	
	//HANDLE hand=creat_test_file();

	for (int index = 0; index<SLANTED_BW_NUM; index++){
		select_num = (int)((index-1)/4.0+1);
		if (select_num == 1 && !mysbw.use_field_a) 
		{
			continue;
		}
	/*	else if (select_num == 2 && !mysbw.use_field_b) continue;//?*/

		roi_w = mysbw.roi_w[select_num];
		roi_h = mysbw.roi_h[select_num];
		roi_distance = mysbw.roi_distance[select_num];
		pattern_size = mysbw.pattern_size[select_num];
		search_w = mysbw.search_w[select_num];
		search_h = mysbw.search_h[select_num];
		bmp_y = new BYTE[search_w*search_h];
		bin = new BYTE[search_w*search_h];

		s_aa::cimg.bmp24_to_y8(bmp, w, h, &mysbw.search_rt[index], bmp_y);
// 		for(int i=0;i<search_h;i++)
// 			for(int j=0;j<search_w;j++){
// 				bmp_y[i*search_w+j]=bmp[i*search_w*3+j*3+1];
// 			}
		for (int i=0; i<search_w*search_h; i++){
			bin[i] = ((bmp_y[i]>mysbw.binary_threhold)?ABS_WHITE:ABS_BLACK);
		}

		if (GET_CHECKER(HWND_REG_MARK_CHART, IDC_DISPLAY_IMG) && COMBO_GETCURSEL(HWND_REG_MARK_CHART, IDC_SELECT_ROI) == index){
			myoutput.clear_bmp_stream();
			myoutput.display(GetDlgItem(HWND_REG_MARK_CHART, IDC_DISPLAY), bin, search_w, search_h, 1);
		}
		/************************************************************************/
		/*  1. find qualified pattern
			2. search qualified pattern, get qualified pattern center x, y, cross edge x, y.
		*/
		/************************************************************************/
		mysbw.find_target[index] = slanted_bw::get_target_center(bin, search_w, search_h, pattern_size, pattern_size/4,
			center_x, center_y, my_edge_x, my_edge_y);

		mysbw.center_x[index] = mysbw.search_rt[index].left + center_x;
		mysbw.center_y[index] = mysbw.search_rt[index].top + search_h-1-center_y;
		p = NULL;

		if (center_x<0 || center_x>search_w){
			mysbw.find_target[index] = FALSE;
		}

		if (center_y<0 || center_y>search_h){
			mysbw.find_target[index] = FALSE;
		}

		if (center_y-1-roi_distance<0 || center_y-1+roi_distance>search_h){
			mysbw.find_target[index] = FALSE;
		}

		if (center_x-1-roi_distance<0 || center_x-1+roi_distance>search_w){
			mysbw.find_target[index] = FALSE;
		}
		if (mysbw.find_target[index]){
			p = bin+(center_y-1-roi_distance)*search_w+center_x-roi_distance;
			slanted_bw::get_pattern_center(p, search_w, search_h, roi_distance*2, center_x2,
				center_y2, my_edge_x, my_edge_y);

			if (center_x2<roi_distance/2 || center_x2>roi_distance*3/2){
				mysbw.find_target[index] = FALSE;
			}

			if (center_y2<roi_distance/2 || center_y2>roi_distance*3/2){
				mysbw.find_target[index] = FALSE;
			}

			if (mysbw.find_target[index] == TRUE){
				BYTE *roi_bin = new BYTE[roi_w*roi_h];
				for (int i=0; i<4; i++){

					mysbw.edge_x[index][i] = mysbw.search_rt[index].left+(center_x-roi_distance)+my_edge_x[i];
					mysbw.edge_y[index][i] = mysbw.search_rt[index].top+search_h-1-((center_y-1-roi_distance)+my_edge_y[i]);

					mysbw.roi_rt[index][i].left	  = mysbw.edge_x[index][i]-roi_w/2;
					mysbw.roi_rt[index][i].top	  = mysbw.edge_y[index][i]-roi_h/2;
					mysbw.roi_rt[index][i].right  = mysbw.edge_x[index][i]+roi_w/2;
					mysbw.roi_rt[index][i].bottom = mysbw.edge_y[index][i]+roi_h/2;

					if (mysbw.roi_rt[index][i].left<0  || mysbw.roi_rt[index][i].top<0 || 
						mysbw.roi_rt[index][i].right<0 || mysbw.roi_rt[index][i].bottom<0 ||
						mysbw.roi_rt[index][i].left> w || mysbw.roi_rt[index][i].top>h || 
						mysbw.roi_rt[index][i].right>w || mysbw.roi_rt[index][i].bottom>h ){
							mysbw.find_target[index] = FALSE;
							mysbw.sfr_val1[index][i] = 0.0;
							continue;
					}

					s_aa::cimg.bmp24_to_y8(bmp, w, h, &mysbw.roi_rt[index][i], roi_bin);
					char mystr[100]={0};
				//	sprintf(mystr, "d:\\%d_%d.bmp", index, i);
				//	ai_img::save_bmp8_image(mystr, roi_bin, roi_w, roi_h);
#ifdef USE_FINE_CROSS_SEARCH
					if (i==1 || i==3){
						one_edge_result = find_roi_v_edge(roi_bin, mysbw.roi_rt[index][i].left, mysbw.roi_rt[index][i].top,
							mysbw.roi_w[select_num], mysbw.roi_h[select_num], my_edge[i], mysbw.oc_slope[index][i], mysbw.oc_offset[index][i], bw_order[i]);
						for (int sx = 0; sx<roi_w; sx++){
							mysbw.edge_v_x[index][i][sx] = mysbw.roi_rt[index][i].left + sx;
							mysbw.edge_v_y[index][i][sx] = my_edge[i][sx];
						}

						if (one_edge_result == FALSE) find_edge_result = FALSE;
					}
					else{
						one_edge_result = find_roi_h_edge(roi_bin, mysbw.roi_rt[index][i].left, mysbw.roi_rt[index][i].top,
							mysbw.roi_w[select_num], mysbw.roi_h[select_num], my_edge[i], mysbw.oc_slope[index][i], mysbw.oc_offset[index][i], bw_order[i]);

						for (int sy = 0; sy<roi_h; sy++){
							mysbw.edge_h_x[index][i][sy] = my_edge[i][sy];
							mysbw.edge_h_y[index][i][sy] = mysbw.roi_rt[index][i].top + roi_h-1-sy;
						}
						if (one_edge_result == FALSE){
							find_edge_result = FALSE;
						//	log("find edge h fail:%d, %d", index, i);
						}
					}
#endif
					if (mysbw.roi_enable[i] == FALSE) {
						continue;
					}
					if (i ==1 || i==3){
						ai_img::roi_column_to_row(roi_bin, roi_w, roi_h);
						t = roi_w;
						roi_w = roi_h;
						roi_h = t;
					}
					err = sfr_proc(roi_bin, roi_w, roi_h, 1, &mysbw.freq_val1, &mysbw.sfr_val1[index][i], 
						mysbw.slope[index][i], mysbw.offset[index][i], mysbw.esf[index][i], 
						mysbw.lsf[index][i], mysbw.freq[index][i], mysbw.amp[index][i]);
					
					//save_reg_mark_report_test(hand, mysbw.freq[index][i], mysbw.amp[index][i],index,i);
			//		log("************sfr_val:%.2f",mysbw.sfr_val1[index][i]);
					mysbw.sfr_val2[index][i] = mysbw.sfr_val1[index][i];
					if (err == FALSE && mysbw.use_mtf50p){
						//log("mtf success");
						mtfp(mysbw.freq[index][i], mysbw.amp[index][i], mysbw.mtfxp, mtfxp_val, mtfxp_freq, roi_w*2);
						mysbw.sfr_val2[index][i] = mysbw.sfr_val1[index][i];
						mysbw.sfr_val1[index][i] = 100.0*mtfxp_freq;
					}


					//sfr test result
					
					// before uv test result
					// after uv test result
				}
				//sfr test result & before uv test result
				//if(mysbw.sfr_val1[index][AA_SELECT_ROI]<(double)mysbw.spec1[select_num]){
				//	mysbw.test_result = FALSE;
				//	mysbw.A_UVResult  = FALSE;
				//}

				delete [] roi_bin;
#ifdef USE_FINE_CROSS_SEARCH
				if (find_edge_result == TRUE){
					//get accuracy oc x,y
					slanted_bw::get_cross_xy(mysbw.oc_slope[index][0], mysbw.oc_offset[index][0], mysbw.oc_slope[index][1], 
						mysbw.oc_offset[index][1], cross_x[0], cross_y[0]);
					slanted_bw::get_cross_xy(mysbw.oc_slope[index][2], mysbw.oc_offset[index][2], mysbw.oc_slope[index][3], 
						mysbw.oc_offset[index][3], cross_x[1], cross_y[1]);

					mysbw.oc_x[index] = (cross_x[0]+cross_x[1])/2;
					mysbw.oc_y[index] = (cross_y[0]+cross_y[1])/2;
				}
				else{
					mysbw.oc_x[index] = mysbw.center_x[index];
					mysbw.oc_y[index] = mysbw.center_y[index];
				}
#else
				mysbw.oc_x[index] = mysbw.center_x[index];
				mysbw.oc_y[index] = mysbw.center_y[index];
#endif
				find_edge_result = TRUE;
			}
		}
		if (mysbw.find_target[index] == FALSE && index<5){
 			mysbw.oc_x[index] = mysbw.search_rt[index].left + search_w/2;
 			mysbw.oc_y[index] = mysbw.search_rt[index].top + search_h/2;
			/*log("find target:%d, i=%d",mysbw.find_target[index], index);*/

			//mysbw.test_result = FALSE;
			//mysbw.B_UVResult  = FALSE;
//			mysbw.A_UVResult  = FALSE;
/*			log("test result sfr3 %d",mysbw.test_result);*/
		}	
		delete [] bin;
		delete [] bmp_y;
	}
	// test result 2
	mysbw.eSfrDiff  = get_eSfrDiff();
	mysbw.eCoorDiff = get_eCoorDiff();
	//if (err != 0 || mysbw.eSfrDiff>mysbw.sfrDiff_spec
	//	||mysbw.eCoorDiff>mysbw.coorDiff_spec){
	//		mysbw.test_result = FALSE;
	//		mysbw.B_UVResult  = FALSE;
	////		mysbw.A_UVResult  = FALSE;
	//}

#ifdef SFR_METHOD_EX
	draw_sfr_curve(mysbw.roi_w[0]*2);
#else
	draw_sfr_curve(FFT_NUM/2);
#endif
//	log("draw fs curve.");
	draw_fs_curve2(AA_FS_MAX_CNT);
	return mysbw.test_result;

}

int get_eSfrDiff(void){
	double maxval;
	double minval;
	maxval = 0;
	minval = 100;
	for(int i=0;i<4;i++){
		if(maxval<mysbw.sfr_val1[i][AA_SELECT_ROI])
			maxval = mysbw.sfr_val1[i][AA_SELECT_ROI];
		if(minval>mysbw.sfr_val1[i][AA_SELECT_ROI])
			minval = mysbw.sfr_val1[i][AA_SELECT_ROI];
	}
//	log("eSfrMax=%.2f,eSfrMin=%.2",maxval,minval);
	return (maxval-minval);
}
int get_eCoorDiff(void){
	double maxval;
	double minval;
	double r[10];
	maxval = 0;
	minval = 1000;

	for(int i=0;i<4;i++){
		r[i]=sqrt((mysbw.oc_x[i+1]-mysbw.oc_x[0])*(mysbw.oc_x[i+1]-mysbw.oc_x[0])+(mysbw.oc_y[i+1]-mysbw.oc_y[0])*(mysbw.oc_y[i+1]-mysbw.oc_y[0]));
		if(maxval<r[i])
			maxval = r[i];
		if(minval>r[i])
			minval = r[i];
	}
//	log("eCoorMax=%.2f,eCoorMin=%.2",maxval,minval);
	return (maxval-minval);
}

int SfrResult(int model){
	int index = 4;
	double spec;
	int select_num = 0;
	mysbw.test_result		   = TRUE;
	mysbw.B_UVResult		   = TRUE;
	mysbw.A_UVResult		   = TRUE;
	for(index=0;index<5;index++){
		select_num = (int)((index-1)/4.0+1);
		if(mysbw.find_target[index] == FALSE){
			mysbw.test_result		   = FALSE;
			mysbw.B_UVResult		   = FALSE;
			mysbw.A_UVResult		   = FALSE;
			continue;
		}
		if(sfr_type == A_UV)
			spec = (double)mysbw.spec2[select_num];
		else
			spec = (double)mysbw.spec1[select_num];
		for(int i=0;i<4;i++){
			if (mysbw.roi_enable[i] == FALSE) continue;

			if (mysbw.sfr_val1[index][i]<spec){			
				mysbw.sfr_result[index][i] = FALSE;
				mysbw.test_result		   = FALSE;
				mysbw.B_UVResult		   = FALSE;
				mysbw.A_UVResult		   = FALSE;
			}
			else
				mysbw.sfr_result[index][i] = TRUE;
		}
	}
	if (abs(mysbw.tilt_x)>mysbw.tilt_limit
		||abs(mysbw.tilt_y)>mysbw.tilt_limit
		||abs(mysbw.oc_x[0]-IMG_WIDTH/2)>(double)mysbw.oc_limit
		||abs(mysbw.oc_y[0]-IMG_HEIGHT/2)>(double)mysbw.oc_limit
		){
			mysbw.test_result		   = FALSE;
			mysbw.B_UVResult		   = FALSE;
//			log("bUV_result:%d",mysbw.B_UVResult);
	}
	return mysbw.test_result;
}

HANDLE create_define_path_raw(const char *path){
	char new_path[MAX_PATH]={0};
	if(strcmp((path+strlen(path)-4),".RAW")){
		sprintf(new_path, "%s.RAW",path);
	}
	else{
		strcpy(new_path,path);
	}
	HANDLE hfile = CreateFile(new_path, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	return hfile;

}
HANDLE create_define_path_test(const char *path){
	char new_path[MAX_PATH]={0};
	if(strcmp((path+strlen(path)-4),".txt")){
		sprintf(new_path, "%s.txt",path);
	}
	else{
		strcpy(new_path,path);
	}
	HANDLE hfile = CreateFile(new_path, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	return hfile;

}
void save_reg_mark_report_c(BYTE *bmp, int img_w, int img_h, int type)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	char date_string[MAX_PATH] = {0};
	sprintf(date_string, "%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
// 	char sn[MAX_PATH] = {0};
// 	sprintf(sn, "%c%c%c%c%c%c%c%c%c%c%c%c", mysbw.sn[0],mysbw.sn[1],mysbw.sn[2],mysbw.sn[3],mysbw.sn[4],mysbw.sn[5],mysbw.sn[6],
// 		mysbw.sn[7],mysbw.sn[8],mysbw.sn[9],mysbw.sn[10],mysbw.sn[11]);
	char report_folder_path[MAX_PATH] = {0};
	char report_folder_name[MAX_PATH] = {0};
	char report_path[MAX_PATH] = {0};
	int is_report_exist;
	char file_name[MAX_PATH] = {0};
	switch(type){
	case (SAVE_OK_REG_PRE_REPORT):
		is_report_exist = create_report_file("okBeforeUV", report_folder_path, report_folder_name);
		sprintf(report_path, "%s\\okBeforeUV_report_%s.csv", report_folder_path, report_folder_name);
		break;
	case (SAVE_NG_REG_PRE_REPORT):
		is_report_exist = create_report_file("ngBeforeUV", report_folder_path, report_folder_name);
		sprintf(report_path, "%s\\ngBeforeUV_report_%s.csv", report_folder_path, report_folder_name);
		break;
	case (SAVE_OK_REG_REPORT):
		is_report_exist = create_report_file("ok", report_folder_path, report_folder_name);
		sprintf(report_path, "%s\\ok_report_%s.csv", report_folder_path, report_folder_name);
		break;
	case(SAVE_NG_REG_REPORT):
		is_report_exist = create_report_file("ng", report_folder_path, report_folder_name);
		sprintf(report_path, "%s\\ng_report_%s.csv", report_folder_path, report_folder_name);
		break;
	case (SAVE_REG_REPORT):
		is_report_exist = create_report_file("�ֶ�����", report_folder_path, report_folder_name);		
		sprintf(report_path, "%s\\�ֶ�����_report_%s.csv", report_folder_path, report_folder_name);
		break;
	case (SAVE_MES):
		is_report_exist = create_report_file_raw("Datalog", report_folder_path, report_folder_name);		
		sprintf(report_path, "%s\\%s_%s_%02d%02d%02d.RAW", report_folder_path, mysbw.sn, report_folder_name, st.wHour, st.wMinute, st.wSecond);
		break;
	}

	HANDLE report_handle;
	if(type==SAVE_MES)
		report_handle = create_define_path_raw(report_path);
	else
		report_handle = ai_win::create_define_path_csv(report_path);
	char str[500] = {0};

	if (is_report_exist == FALSE){
		if(type==SAVE_MES){
			SetFilePointer(report_handle, 0, NULL, FILE_BEGIN);
			sprintf(str, "####\r\nCamera1\r\n%d\r\nTV1.0\r\nSN%s\r\nAA\r\n", cy_id, mysbw.sn);
			ai_win::write_csv(report_handle, str);
			sprintf(str, "%02d/%02d/%04d\r\n%02d:%02d:%02d\r\n",st.wDay, st.wMonth,st.wYear ,st.wHour, st.wMinute, st.wSecond);
			ai_win::write_csv(report_handle, str);
			sprintf(str, "ST40\r\nTN%s\r\n", mysbw.sn);
			ai_win::write_csv(report_handle, str);
		}
		else{
			SetFilePointer(report_handle, 0, NULL, FILE_BEGIN);
			sprintf(str, "date, sn, test_result, dispensing_result, sfr_c, sfr_lu, sfr_ru, sfr_lb, sfr_rb, eSfrDiff, tilt_x, tilt_y, \
						 center_x, center_y, coor_lt_x, coor_lt_y, coor_rt_x, coor_rt_y, coor_lb_x, coor_lb_y, coor_rb_x, coor_rb_y, eCoorDiff\r\n");
			ai_win::write_csv(report_handle, str);
		}
	}
	SetFilePointer(report_handle, 0, NULL, FILE_END);
	if(type==SAVE_MES){
		sprintf(str, "sfr_1, %.2f,P\r\n sfr_2, %.2f,P\r\nsfr_3, %.2f,P\r\nsfr_4, %.2f,P\r\nsfr_5, %.2f,P\r\nPASS",
			mysbw.sfr_val1[0][AA_SELECT_ROI], mysbw.sfr_val1[1][AA_SELECT_ROI], mysbw.sfr_val1[2][AA_SELECT_ROI],
			mysbw.sfr_val1[3][AA_SELECT_ROI], mysbw.sfr_val1[4][AA_SELECT_ROI],mysbw.oc_x[0], mysbw.oc_y[0],
			mysbw.oc_x[1],  mysbw.oc_y[1], mysbw.oc_x[2],  mysbw.oc_y[2], mysbw.oc_x[3],  mysbw.oc_y[3], mysbw.oc_x[4],  mysbw.oc_y[4]);
		log("pre_result: %d,%d,test_result:%d",mysbw.B_UVResult, mysbw.oc_limit, mysbw.test_result);
		ai_win::write_csv(report_handle, str);
		CloseHandle(report_handle);
	}
	else{
		sprintf(str, "#%s, %s, %d, %d, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\r\n", 
			date_string, mysbw.sn, mysbw.test_result, mysbw.dispensing_result, 
			mysbw.sfr_val1[0][AA_SELECT_ROI], mysbw.sfr_val1[1][AA_SELECT_ROI], mysbw.sfr_val1[2][AA_SELECT_ROI],
			mysbw.sfr_val1[3][AA_SELECT_ROI], mysbw.sfr_val1[4][AA_SELECT_ROI], 
			mysbw.eSfrDiff, mysbw.tilt_x, mysbw.tilt_y, 
			mysbw.oc_x[0], mysbw.oc_y[0],
			mysbw.oc_x[1],  mysbw.oc_y[1], mysbw.oc_x[2],  mysbw.oc_y[2], mysbw.oc_x[3],  mysbw.oc_y[3], mysbw.oc_x[4],  mysbw.oc_y[4], 
			mysbw.eCoorDiff);
		log("pre_result: %d,%d,test_result:%d",mysbw.B_UVResult, mysbw.oc_limit, mysbw.test_result);

		ai_win::write_csv(report_handle, str);
		CloseHandle(report_handle);

		sprintf(file_name, "%s\\[%s]sfr_%s-org.bmp", report_folder_path, mysbw.sn, date_string);
		ai_img::save_bmp24_image(file_name, bmp, img_w, img_h);
		sprintf(file_name, "%s\\[%s]sfr_%s-result.bmp", report_folder_path, mysbw.sn, date_string);
		s_aa::myimg.save_dib_to_bmp(file_name, img_w, img_h);
	}
}


void save_reg_mark_report_test(int n,int ty)//save  MTF
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	char date_string[MAX_PATH] = {0};
	char str[500] = {0};
	sprintf(date_string, "%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	char report_folder_path[MAX_PATH] = {0};
	char report_folder_name[MAX_PATH] = {0};
	char report_path[MAX_PATH] = {0};
	int is_report_exist;
	
	if(ty==1){
		is_report_exist = create_report_file_test("MTF_SCAN", report_folder_path, report_folder_name);
		sprintf(report_path, "%s\\MTF_SCAN_report_%s.txt", report_folder_path, mysbw.sn);
		
	}
	else if(ty==3){
		is_report_exist = create_report_file_test("AA_BeforUV", report_folder_path, report_folder_name);
		sprintf(report_path, "%s\\AA_BeforUV_report_%s.txt", report_folder_path, mysbw.sn);
	}
	else if(ty==4){
		is_report_exist = create_report_file_test("AA_AfterUV", report_folder_path, report_folder_name);
		sprintf(report_path, "%s\\AA_AfterUV_report_%s.txt", report_folder_path, mysbw.sn);
	}
	else if(ty==5){
		is_report_exist = create_report_file_test("SCAN_���", report_folder_path, report_folder_name);
		sprintf(report_path, "%s\\SCAN_���_report_%s.txt", report_folder_path, mysbw.sn);
	}
	else{
		is_report_exist = create_report_file_test("MTF����", report_folder_path, report_folder_name);
		sprintf(report_path, "%s\\MTF����_report_%s.txt", report_folder_path, mysbw.sn);
	}
	HANDLE report_handle;

	report_handle = create_define_path_test(report_path);

	if (is_report_exist == FALSE){
		SetFilePointer(report_handle, 0, NULL, FILE_BEGIN);
		if(ty==1)
			sprintf(str, "## MTF SCAN ���� ## \r\ncy/px = 0.25\r\nƫ��X����	ƫ��Y����	Z������	����	����	����	����	����\r\n");
		else if(ty==3)
			sprintf(str, "## AA ��ɺ� UV ǰ ## \r\ncy/px = 0.25\r\nƫ��X����	ƫ��Y����	Z������	����	����	����	����	����\r\n");
		else if(ty==4)
			sprintf(str, "## AA ��ɺ� UV �� ## \r\ncy/px = 0.25\r\nƫ��X����	ƫ��Y����	Z������	����	����	����	����	����\r\n");
		else if(ty==5)
			sprintf(str, "## MTF SCAN ������� ## \r\n	����	����	����	����	����\r\n");
		else
			sprintf(str, "## MTF ���� ## \r\ncy/px	SFR\r\n");
		ai_win::write_csv(report_handle, str);
	}
	else SetFilePointer(report_handle, 0, NULL, FILE_END);
	//char str[500] = {0};
	//sprintf(str, "%d   %d\r\n",n,m);
	//ai_win::write_csv(report_handle, str);
	//
	SetFilePointer(report_handle, 0, NULL, FILE_END);
	if(ty==1){
		sprintf(str,"step %2d:	%.2f	%.2f	:	%.4f:	%.2f	%.2f	%.2f	%.2f	%.2f	\r\n",
				n,mysbw.oc_x[0],mysbw.oc_y[0],Pos2[2],(mysbw.sfr_val2[0][0]+mysbw.sfr_val2[0][2])/2,
				(mysbw.sfr_val2[1][0]+mysbw.sfr_val2[1][2])/2,(mysbw.sfr_val2[2][0]+mysbw.sfr_val2[2][2])/2,
				(mysbw.sfr_val2[3][0]+mysbw.sfr_val2[3][2])/2,(mysbw.sfr_val2[4][0]+mysbw.sfr_val2[4][2])/2);
		ai_win::write_csv(report_handle, str);
	}
	else if(ty==3){
		sprintf(str,"%.2f	%.2f	%.4f	%.2f	%.2f	%.2f	%.2f	%.2f	\r\n",
				mysbw.oc_x[0],mysbw.oc_y[0],Pos2[2],(mysbw.sfr_val2[0][0]+mysbw.sfr_val2[0][2])/2,
				(mysbw.sfr_val2[1][0]+mysbw.sfr_val2[1][2])/2,(mysbw.sfr_val2[2][0]+mysbw.sfr_val2[2][2])/2,
				(mysbw.sfr_val2[3][0]+mysbw.sfr_val2[3][2])/2,(mysbw.sfr_val2[4][0]+mysbw.sfr_val2[4][2])/2);
		ai_win::write_csv(report_handle, str);
	}
	else if(ty==4){
		sprintf(str,"%.2f	%.2f	%.4f	%.2f	%.2f	%.2f	%.2f	%.2f	\r\n",
				mysbw.oc_x[0],mysbw.oc_y[0],Pos2[2],(mysbw.sfr_val2[0][0]+mysbw.sfr_val2[0][2])/2,
				(mysbw.sfr_val2[1][0]+mysbw.sfr_val2[1][2])/2,(mysbw.sfr_val2[2][0]+mysbw.sfr_val2[2][2])/2,
				(mysbw.sfr_val2[3][0]+mysbw.sfr_val2[3][2])/2,(mysbw.sfr_val2[4][0]+mysbw.sfr_val2[4][2])/2);
		ai_win::write_csv(report_handle, str);
	}
	else if(ty==5){
		for(int i = 0; i < n; i++){
			sprintf(str,"step %2d:	%.2f	%.2f	%.2f	%.2f	%.2f\r\n",i,mysbw.sfr_val3[0][i],mysbw.sfr_val3[1][i],mysbw.sfr_val3[2][i],mysbw.sfr_val3[3][i],mysbw.sfr_val3[4][i]);
		ai_win::write_csv(report_handle, str);
		}
	}
	else{
		for(int i=0;i<300;i++)
		{
			sprintf(str, "%.2f   %.2f   %.2f   %.2f   %.2f   %.2f   %.2f   %.2f   %.2f   %.2f   %.2f\r\n",
				mysbw.freq[0][0][i],mysbw.amp[0][0][i],mysbw.amp[0][2][i],mysbw.amp[1][0][i],mysbw.amp[1][2][i],
				mysbw.amp[2][0][i],mysbw.amp[2][2][i],mysbw.amp[3][0][i],mysbw.amp[3][2][i],mysbw.amp[4][0][i],mysbw.amp[4][2][i]);
			ai_win::write_csv(report_handle, str);
			//Sleep(1);
			if(mysbw.freq[0][0][i]>=0.50) break;
		}
	}
	CloseHandle(report_handle);

}

void save_reg_mark_report_test2(int ty/*MES or LOCAL*/,int time_num/*time for save*/)//save MES and LOCAL MTF
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	char date_string[MAX_PATH] = {0};
	char str[500] = {0};
	sprintf(date_string, "%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	char report_folder_path[MAX_PATH] = {0};
	char report_folder_name[MAX_PATH] = {0};
	char report_path[MAX_PATH] = {0};
	int is_report_exist;
	
	if(ty==1){
		is_report_exist = create_report_file_test2("LocalDate" , report_folder_path, report_folder_name);
		sprintf(report_path, "%s\\%s_report_%s.txt", report_folder_path, mysbw.sn ,report_folder_name);
	//	
	//}
	//else if(ty==3){
	//	is_report_exist = create_report_file_test("AA_BeforUV", report_folder_path, report_folder_name);
	//	sprintf(report_path, "%s\\AA_BeforUV_report_%s_%02d.txt", report_folder_path, report_folder_name,aa_num);
	//}
	//else if(ty==4){
	//	is_report_exist = create_report_file_test("AA_AfterUV", report_folder_path, report_folder_name);
	//	sprintf(report_path, "%s\\AA_AfterUV_report_%s_%02d.txt", report_folder_path, report_folder_name,aa_num);
	//}
	//else if(ty==5){
	//	is_report_exist = create_report_file_test("SCAN_���", report_folder_path, report_folder_name);
	//	sprintf(report_path, "%s\\SCAN_���_report_%s_%02d.txt", report_folder_path, report_folder_name,aa_num);
	}
	else{
		is_report_exist = create_report_file_raw("MesData", report_folder_path, report_folder_name);		
		sprintf(report_path, "%s\\%s_%s_%02d%02d%02d.RAW", report_folder_path, mysbw.sn, report_folder_name, st.wHour, st.wMinute, st.wSecond);
	}

	HANDLE report_handle;

	if(ty==1)
		report_handle = create_define_path_test(report_path);
	else 
		report_handle = create_define_path_raw(report_path);

	if (ty == 1){
		SetFilePointer(report_handle, 0, NULL, FILE_END);
		if(time_num==1)
			sprintf(str, "## AA��ɺ� ����ǰ ##\r\n");
		else if(time_num==2)
			sprintf(str, "## AA��ɺ� ������ ##\r\n");
		else if(time_num==3)
			sprintf(str, "## UV��ɺ� �ɿ���צǰ ##\r\n");
		else if(time_num==4)
			sprintf(str, "## UV��ɺ� �ɿ���צ�� ##\r\n");
		ai_win::write_csv(report_handle, str);
	}
	else{
		SetFilePointer(report_handle, 0, NULL, FILE_END);
		sprintf(str, "####\r\nCamera1\r\n%d\r\nTV1.0\r\nSN%s\r\nAA\r\n", cy_id, mysbw.sn);
		ai_win::write_csv(report_handle, str);
		sprintf(str, "%02d/%02d/%04d\r\n%02d:%02d:%02d\r\n",st.wDay, st.wMonth,st.wYear ,st.wHour, st.wMinute, st.wSecond);
		ai_win::write_csv(report_handle, str);
		sprintf(str, "ST40\r\nTN%s\r\n", mysbw.sn);
		ai_win::write_csv(report_handle, str);
	}
	//char str[500] = {0};
	//sprintf(str, "%d   %d\r\n",n,m);
	//ai_win::write_csv(report_handle, str);
	//
	double val = 0;
	int n = 0;
	SetFilePointer(report_handle, 0, NULL, FILE_END);
	if(ty==1){
		sprintf(str,"ƫ��X���꣺%.2f	ƫ��Y���꣺%.2f		Z�����꣺%.4f\r\n",mysbw.oc_x[0],mysbw.oc_y[0],Pos2[2]);
		ai_win::write_csv(report_handle, str);
		for(int i = 0;i < 5; i++){
			val = 0;
			n = 0;
			switch(i){
			case 0:
				sprintf(str,"Center:");
				ai_win::write_csv(report_handle, str);
				break;
			case 1:
				sprintf(str,"Left-Top:");
				ai_win::write_csv(report_handle, str);
				break;
			case 2:
				sprintf(str,"Right-Top:");
				ai_win::write_csv(report_handle, str);
				break;
			case 3:
				sprintf(str,"Left-Down:");
				ai_win::write_csv(report_handle, str);
				break;
			case 4:
				sprintf(str,"Right-Down:");
				ai_win::write_csv(report_handle, str);
				break;
			}
			for(int j=0;j<4;j++){
				switch(j){
				case 0:
					if(mysbw.roi_enable[0]){
						sprintf(str,"bottom:%.2f	",mysbw.sfr_val1[i][j]);
						if(save_set::save_sfr_only) ai_win::write_csv(report_handle, str);
						val += mysbw.sfr_val1[i][j];
						n++;
					}
					break;
				case 1:
					if(mysbw.roi_enable[1]){
						sprintf(str,"right:%.2f	",mysbw.sfr_val1[i][j]);
						if(save_set::save_sfr_only) ai_win::write_csv(report_handle, str);
						val += mysbw.sfr_val1[i][j];
						n++;
					}
					break;
				case 2:
					if(mysbw.roi_enable[2]){
						sprintf(str,"top:%.2f	",mysbw.sfr_val1[i][j]);
						if(save_set::save_sfr_only) ai_win::write_csv(report_handle, str);
						val += mysbw.sfr_val1[i][j];
						n++;
					}
					break;
				case 3:
					if(mysbw.roi_enable[3]){
						sprintf(str,"left:%.2f	",mysbw.sfr_val1[i][j]);
						if(save_set::save_sfr_only) ai_win::write_csv(report_handle, str);
						val += mysbw.sfr_val1[i][j];
						n++;
					}
					break;
				}
			}
			if(save_set::save_avg_only){
				sprintf(str,"avg_sfr:%.2f",val/n);
				ai_win::write_csv(report_handle, str);
			}
			sprintf(str,"\r\n\r\n");
			ai_win::write_csv(report_handle, str);
		}

	}
	else {
		//���������ƣ�����ֵ�����ޣ����ޣ������� ״̬��P:�ɹ���F:ʧ�ܣ�
		sprintf(str,"oc_x,%.2f,%d,%d,,",mysbw.oc_x[0],AA_TARGET_CENTER_X-mysbw.oc_limit,AA_TARGET_CENTER_X+mysbw.oc_limit);
		ai_win::write_csv(report_handle, str);
		if(abs(mysbw.oc_x[0]-AA_TARGET_CENTER_X)<=mysbw.oc_limit)
			sprintf(str,"P\r\n");
		else
			sprintf(str,"F\r\n");
		ai_win::write_csv(report_handle, str);

		sprintf(str,"oc_y,%.2f,%d,%d,,",mysbw.oc_y[0],AA_TARGET_CENTER_Y-mysbw.oc_limit,AA_TARGET_CENTER_Y+mysbw.oc_limit);
		ai_win::write_csv(report_handle, str);
		if(abs(mysbw.oc_y[0]-AA_TARGET_CENTER_Y)<=mysbw.oc_limit)
			sprintf(str,"P\r\n");
		else
			sprintf(str,"F\r\n");
		ai_win::write_csv(report_handle, str);
		char str2[500];
		int spec;
		for(int i = 0;i < 5; i++){
			val = 0;
			n = 0;
			switch(i){
			case 0:
				sprintf(str2,"Center");
				spec = mysbw.spec1[0];
				break;
			case 1:
				sprintf(str2,"Left-Top");
				spec = mysbw.spec1[1];
				break;
			case 2:
				sprintf(str2,"Right-Top");
				spec = mysbw.spec1[1];
				break;
			case 3:
				sprintf(str2,"Left-Down");
				spec = mysbw.spec1[1];
				break;
			case 4:
				sprintf(str2,"Right-Down");
				spec = mysbw.spec1[1];
				break;
			}
			for(int j=0;j<4;j++){
				switch(j){
				case 0:
					if(mysbw.roi_enable[0]){
						sprintf(str,"%sbottom,%.2f,%d,100,,",str2,mysbw.sfr_val1[i][j],spec);
						if(save_set::save_sfr_only) ai_win::write_csv(report_handle, str);

						if(mysbw.sfr_val1[i][j]>=spec)
							sprintf(str,"P\r\n");
						else
							sprintf(str,"F\r\n");
						if(save_set::save_sfr_only) ai_win::write_csv(report_handle, str);
						val += mysbw.sfr_val1[i][j];
						n++;
					}
					break;
				case 1:
					if(mysbw.roi_enable[1]){
						sprintf(str,"%sright,%.2f,%d,100,,",str2,mysbw.sfr_val1[i][j],spec);
						if(save_set::save_sfr_only) ai_win::write_csv(report_handle, str);

						if(mysbw.sfr_val1[i][j]>=spec)
							sprintf(str,"P\r\n");
						else
							sprintf(str,"F\r\n");
						if(save_set::save_sfr_only) ai_win::write_csv(report_handle, str);
						val += mysbw.sfr_val1[i][j];
						n++;
					}
					break;
				case 2:
					if(mysbw.roi_enable[2]){
						sprintf(str,"%stop,%.2f,%d,100,,",str2,mysbw.sfr_val1[i][j],spec);
						if(save_set::save_sfr_only) ai_win::write_csv(report_handle, str);

						if(mysbw.sfr_val1[i][j]>=spec)
							sprintf(str,"P\r\n");
						else
							sprintf(str,"F\r\n");
						if(save_set::save_sfr_only) ai_win::write_csv(report_handle, str);
						val += mysbw.sfr_val1[i][j];
						n++;
					}
					break;
				case 3:
					if(mysbw.roi_enable[3]){
						sprintf(str,"%sleft,%.2f,%d,100,,",str2,mysbw.sfr_val1[i][j],spec);
						if(save_set::save_sfr_only) ai_win::write_csv(report_handle, str);

						if(mysbw.sfr_val1[i][j]>=spec)
							sprintf(str,"P\r\n");
						else
							sprintf(str,"F\r\n");
						if(save_set::save_sfr_only) ai_win::write_csv(report_handle, str);
						val += mysbw.sfr_val1[i][j];
						n++;
					}
					break;
				}
			}
			if(save_set::save_avg_only){
				sprintf(str,"%savg_sfr,%.2f,%d,100,,",str2,val/n,spec);
				ai_win::write_csv(report_handle, str);
				
				if((val/n)>=spec)
					sprintf(str,"P\r\n");
				else
					sprintf(str,"F\r\n");
				ai_win::write_csv(report_handle, str);

			}
		}
		if(mysbw.test_result)
			sprintf(str,"PASS");
		else
			sprintf(str,"FAIL");
		ai_win::write_csv(report_handle, str);
	}
	char file_name[MAX_PATH] = {0};
	if( ty == 1){
		if(save_set::saveimg){
			sprintf(file_name, "%s\\%s_%d_%s.bmp", report_folder_path, mysbw.sn, cy_id, date_string);
			ai_img::save_bmp24_image(file_name, s_aa::bmp_buf_t, IMG_WIDTH, IMG_HEIGHT);
		}
	}
	else {
		if(save_set::mes_saveimg){
			sprintf(file_name, "%s\\%s_%d_%s.bmp", report_folder_path, mysbw.sn, cy_id, date_string);
			ai_img::save_bmp24_image(file_name, s_aa::bmp_buf_t, IMG_WIDTH, IMG_HEIGHT);
		}
	}
	CloseHandle(report_handle);

}

void save_reg_mark_report(BYTE *bmp, int img_w, int img_h)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	char date_string[MAX_PATH] = {0};
	sprintf(date_string, "%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	char report_folder_path[MAX_PATH] = {0};
	char report_folder_name[MAX_PATH] = {0};
	int is_report_exist = create_report_file("AA", report_folder_path, report_folder_name);

	char report_path[MAX_PATH] = {0};
	sprintf(report_path, "%s\\AA_report_%s.csv", report_folder_path, report_folder_name);

	HANDLE report_handle = ai_win::create_define_path_csv(report_path);
	char str[500] = {0};

	if (is_report_exist == FALSE){
		SetFilePointer(report_handle, 0, NULL, FILE_BEGIN);
//		sprintf(str, "result, sfr_c, sfr_lu, sfr_ru, sfr_lb, sfr_rb\r\n");
		sprintf(str, "date, result, sfr_c, sfr_lu, sfr_ru, sfr_lb, sfr_rb, tilt_x, tilt_y, tilt_limit, \
					 center_x, center_y, oc_limit, tilt_x_pos, tilt_y_pos\r\n");
		ai_win::write_csv(report_handle, str);
	}
	else SetFilePointer(report_handle, 0, NULL, FILE_END);

// 	sprintf(str, "%d, %.2f, %.2f, %.2f, %.2f, %.2f\r\n", 
// 		mysbw.test_result, mysbw.sfr_val1[0][2], mysbw.sfr_val1[1][2], mysbw.sfr_val1[2][2],
// 		mysbw.sfr_val1[3][0], mysbw.sfr_val1[4][0]);
	sprintf(str, "#%s, %d, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %d, %d, %d\r\n", 
		date_string, mysbw.test_result, mysbw.sfr_val1[0][2], mysbw.sfr_val1[1][2], mysbw.sfr_val1[2][2],
		mysbw.sfr_val1[3][0], mysbw.sfr_val1[4][0], mysbw.tilt_x, mysbw.tilt_y, mysbw.tilt_limit,mysbw.oc_x[0]-IMG_WIDTH/2, mysbw.oc_y[0]-IMG_HEIGHT/2, mysbw.oc_limit
		,mysbw.tilt_x_pos,mysbw.tilt_y_pos);
	ai_win::write_csv(report_handle, str);
	CloseHandle(report_handle);

	char file_name[MAX_PATH] = {0};
	sprintf(file_name, "%s\\[%s]sfr_%s-org.bmp", report_folder_path, s_aa::sn_str, date_string);
	ai_img::save_bmp24_image(file_name, bmp, img_w, img_h);

	sprintf(file_name, "%s\\[%s]sfr_%s-result.bmp", report_folder_path, s_aa::sn_str, date_string);
	s_aa::myimg.save_dib_to_bmp(file_name, img_w, img_h);

	// 	sprintf(file_name, "%s\\[%s]sfr_%s-curve.bmp", report_folder_path, s_aa::sn_str, date_string);
	// 
	// 	ai_img::save_drawing_image(HWND_REG_MARK_SFRCURVE, file_name);
}
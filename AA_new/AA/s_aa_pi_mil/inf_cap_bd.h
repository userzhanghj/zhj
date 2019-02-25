#ifndef _INF_CAP_BD_H_
#define _INF_CAP_BD_H_
#include <Windows.h>
#include <stdio.h>
#include <math.h>
#include <direct.h>
#include <assert.h>
#include <CommCtrl.h>
#include "s_win.h"
#include "resource.h"

#define CAP_BD_CNT		2
#define REG_LIST_SIZE					10240

typedef struct _tagPara
{	
	BYTE slave_id;
	int bd_id;
	int i2c_mode;
	int i2c_type;
	int output_sequence;
	int output_format;
	int interface_type;
	int image_width;
	int image_height;
	int reset_level;
	int pwdn_level;
	int pclk_polarity;
	int vsync_polarity;
	int is_variable_osc;
	int power_ch_on[6];
	int gpio[4];
	float power_ch_vol[6];
	int power_channel_sequence[6];
	int power_channel_delay[6];
	char power_channel_def[6][20];
	float io_level;
	float mclock;
	DWORD pixel_count;
	int select_power_on;
	int select_mclk_on;
	int select_io_on;
	int select_register_on;
	int select_image_on;
	int select_cam_id;
}Para;

extern HWND hsubcap[CAP_BD_CNT];
extern Para para;

extern int import_reg_cmd(HWND hdlg, const char *ini_path, const char *section);
extern BOOL run_register(const char *ini_path, char *section);
extern int  parse_cmd(char *line_buf);

extern BOOL decode_input_image_data(void);
extern int  init_sensor(char *path);
extern int get_image_pixel_cnt(void);

extern BOOL CALLBACK webcam_dlg_proc(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern void log(const char *format, ...);

#endif
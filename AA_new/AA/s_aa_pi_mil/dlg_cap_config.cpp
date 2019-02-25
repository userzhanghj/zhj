#define _CRT_SECURE_NO_WARNINGS
#include "inf_cap_bd.h"
#include "interface.h"


char cam_id_name[][FOLDER_NAME_LENGTH]			= {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
	"11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
	"21", "22", "23", "24", "25"};
char bd_id_name[][FOLDER_NAME_LENGTH]			= {"0", "1", "2", "3"};
char power_sequence_name[][FOLDER_NAME_LENGTH]	= {"0", "1", "2", "3", "4", "5"};
char i2c_mode_name[][FOLDER_NAME_LENGTH]		= {"A08_D08", "A08_D16", "A16_D08", "A16_D16"};
char output_format_name[][FOLDER_NAME_LENGTH]	= {"YUV", "Bayer8",	"Bayer10", "Bayer12"};

char output_sequence_name[][FOLDER_NAME_LENGTH] = {"BGGR/YUYV", "RGGB/YUYV", "GBRG/UYVY", "GRBG/VYUY"};
char mipi_channel_name[][FOLDER_NAME_LENGTH]	= {"Parallel", "MIPI 1 lane", "MIPI 2 lane", "MIPI 3 lane", "MIPI 4 lane"};

class AIU3ConfigDlg:public ai_win::Dlg
{
public:
	virtual BOOL init(HWND hdlg, HWND hfocus, LPARAM linit);
	virtual void command(HWND hdlg, int id, HWND hctrl, UINT code_notify);
private:
	char sensor_path[MAX_PATH];
	int reg_list_index;
	void fill_combo_edit(HWND hdlg, int idd, char (*name)[FOLDER_NAME_LENGTH], int count);
};

void AIU3ConfigDlg::fill_combo_edit(HWND hdlg, int idd, char (*name)[FOLDER_NAME_LENGTH], int count)
{
	COMBO_RESET(hdlg, idd);
	for (int i=0; i<count; i++){
		COMBO_ADDSTR(hdlg, idd, name[i]);
	}
}


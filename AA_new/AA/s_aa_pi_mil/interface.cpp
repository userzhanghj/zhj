#define _CRT_SECURE_NO_WARNINGS
#include "interface.h"
#include "inf_motion_aa.h"
#include "inf_motion_pi.h"
#include "acccomm.client.tlh"

#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "s_win.lib")
#pragma comment(lib, "s_img.lib")
#pragma comment(lib, "s_chart.lib")
#pragma comment(lib, "s_sfr.lib")
#pragma comment(lib, "s_white.lib")
#pragma comment(lib, "s_math.lib")
#pragma comment(lib, "s_comm.lib")
#pragma comment(lib, "FreeImage.lib")
#pragma comment(lib, "DXMediaCap.lib")
#pragma comment(lib, "PS400dll.lib")

int aa_num=0;

int sfr_type = 0;

extern int NG_SIGNAL;
// extern int PLC_DB_INDEX;
// extern int PLC_DB_START;
// extern int PLC_DB_LENS;
extern SlantedBW mysbw;
extern int moving_down;
extern int trans_button;

double pi_z = 0;

int is_move=0;
int is_stop=0;
int do_close=1;
int do_init=0;

int cy_id;

int set_ok=0;
int move_home_ok=0;

SHM *shm = NULL;
extern HANDLE plc_mutex;
namespace s_aa
{
	HMENU  hmenu = NULL;
	HANDLE hshm	 = NULL;

	DLGINFO				dinfo;
	TestConfig			tconf;
	GuideLine			gline;
	MyImage				myimg;
	ai_img::Convert		cimg;
	Test				test;
	TestOption			test_option[TEST_ITEM_NUM];

	ImportTool			imtool[TOOL_CNT];
	ImportHW			imhw[HW_CNT];
	ImportHW			*pimhw = NULL;
	SlantedBW_SM		mysbw_sm;

	TCPIP_V				vision_v;
	ImgCapDev			*cap_dev; 
	ImgFile				img_file;
	Img_Mil				img_mil;


	Device_Info dev[23];
	char dir[MAX_PATH]			   = {0};
	char config_path[MAX_PATH]	   = {0};
	char recipe_name[100] = {0};

	char shm_name[MAX_PATH]	   = {"share_video"};
	char img_format_str[5][100] = {"BMP", "JPEG", "PNG", "TIFF", "GIF"};
	char sn_str[MAX_PATH] = {0};
	char plc_ip[20] = {0};

	HWND hwnd[100] = {NULL};
	int	 ram_int[RAM_INT_NUM] = {0};
	double ram_double[RAM_DOUBLE_NUM] = {0};
	BYTE img_buf[MAX_IMG_SIZE*2] = {0};
	BYTE raw8_buf[MAX_IMG_SIZE]  = {0};
	BYTE bmp_buf[MAX_IMG_SIZE*3] = {0};
	BYTE bmp_buf_delay[MAX_IMG_SIZE*3] = {0};
	BYTE bmp_buf2[MAX_IMG_SIZE*3] = {0};
	BYTE bmp_buf_t[MAX_IMG_SIZE*3] = {0};
	BYTE bmp_buf_mil[MAX_IMG_SIZE*3]= {0};
}

void log(const char *format, ...)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	HWND hlog = GetDlgItem(HWND_LOG, IDC_LOG);
	HWND hmlog = GetDlgItem(HWND_MAIN, IDC_LOG_EX);
	char temp[1024] = {0};
	char log_str[1024] = {0};
	va_list args;
	va_start(args,format);

	vsprintf(temp,format,args);
	sprintf(log_str, "[%02d:%02d:%02d]>%s", st.wHour, st.wMinute, st.wSecond, temp);//jishi
	int index =SendMessage(hlog, LB_ADDSTRING, 0, (LPARAM) log_str);
	SendMessage(hlog, LB_SETCURSEL, index, 0);
	index =SendMessage(hmlog, LB_ADDSTRING, 0, (LPARAM) log_str);
	SendMessage(hmlog, LB_SETCURSEL, index, 0);
	ai_win::save_log(log_str, "sys", "sys_log", 512);
}

void set_status(int status)
{
	APP_STATUS = status;
	InvalidateRect(GetDlgItem(HWND_MAIN, IDC_STATUS), NULL, TRUE);
	UpdateWindow(GetDlgItem(HWND_MAIN, IDC_STATUS));
}

int open_shm(HWND hdlg)
{
	NG_SIGNAL=0;

	s_aa::hshm = OpenFileMapping(FILE_MAP_READ|FILE_MAP_WRITE, FALSE, s_aa::shm_name);
	if (GetLastError() == ERROR_ALREADY_EXISTS){
		close_shm();
		s_aa::hshm = OpenFileMapping(FILE_MAP_READ|FILE_MAP_WRITE, FALSE, s_aa::shm_name);
		if (GetLastError() == ERROR_ALREADY_EXISTS){
			MessageBox(hdlg, "mapping already exists.", "error", 0);
			CloseHandle(s_aa::hshm);
			return FALSE;
		}
	}

	if (s_aa::hshm != NULL){
		shm = (SHM *)MapViewOfFile(s_aa::hshm, FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, 0);
		if (shm != NULL) log("Link:Input Image Buffer OK.");
		else{
			log("Link:Input Image Buffer Fail.");
			return FALSE;
		}
	}
	else return FALSE;

	return TRUE;
}

int close_shm()
{
	int result = UnmapViewOfFile(shm);
	if (result == TRUE){
		CloseHandle(s_aa::hshm);
	}

	return result;
}

void create_report_folder()
{
	char path[MAX_PATH] = {0};
	sprintf(path, "%s\\report", s_aa::dir);
	_mkdir(path);

}

int create_report_file(const char *test_item_str, char *target_report_folder_path, char *target_folder_name)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	//////////////////////////////////////////////////////////////////////////
	//look for the target folder and file
	char dir[MAX_PATH] = {0};
	ai_win::get_current_directory(dir);

	char new_path[MAX_PATH] = {0};
	char new_folder[MAX_PATH] = {0};
	char new_folder_path[MAX_PATH] = {0};

	sprintf(new_folder, test_item_str);
	sprintf(new_path, "%s\\report", dir);
	sprintf(new_folder_path, "%s\\%s", new_path, new_folder);  //..\report\laser

	if (!ai_win::search_directory(new_path, new_folder)) //search laser folder
	{
		_mkdir(new_folder_path);
	}

	sprintf(new_folder, "%04d%02d%02d", st.wYear, st.wMonth, st.wDay);
	sprintf(new_path, "%s", new_folder_path);
	sprintf(new_folder_path, "%s\\%s", new_path, new_folder);

	if (!ai_win::search_directory(new_path, new_folder)){
		_mkdir(new_folder_path);									//..\report\laser\year_month_day
	}

	char report_path[MAX_PATH] = {0};
	char report_name[MAX_PATH] = {0};
	sprintf(report_name, "%s_report_%s.csv", test_item_str, new_folder);
	sprintf(report_path, "%s\\%s", new_folder_path, report_name);

	strcpy(target_report_folder_path, new_folder_path);
	strcpy(target_folder_name, new_folder);
	if (!ai_win::search_file(new_folder_path, report_name)){
		return 0;
	}
	else{
		return 1;
	}
}

int create_report_file_test(const char *test_item_str, char *target_report_folder_path, char *target_folder_name)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	//////////////////////////////////////////////////////////////////////////
	//look for the target folder and file
	char dir[MAX_PATH] = {0};
	ai_win::get_current_directory(dir);

	char new_path[MAX_PATH] = {0};
	char new_folder[MAX_PATH] = {0};
	char new_folder_path[MAX_PATH] = {0};

	sprintf(new_folder, test_item_str);
	sprintf(new_path, "%s\\report", dir);
	sprintf(new_folder_path, "%s\\%s", new_path, new_folder);  //..\report\laser

	if (!ai_win::search_directory(new_path, new_folder)) //search laser folder
	{
		_mkdir(new_folder_path);
	}

	sprintf(new_folder, "%04d%02d%02d", st.wYear, st.wMonth, st.wDay);
	sprintf(new_path, "%s", new_folder_path);
	sprintf(new_folder_path, "%s\\%s", new_path, new_folder);

	if (!ai_win::search_directory(new_path, new_folder)){
		_mkdir(new_folder_path);									//..\report\laser\year_month_day
	}
	char report_path[MAX_PATH] = {0};
	char report_name[MAX_PATH] = {0};
	sprintf(report_name, "%s_report_%s_%d.txt", test_item_str,new_folder,aa_num);
	sprintf(report_path, "%s\\%s", new_folder_path, report_name);

	strcpy(target_report_folder_path, new_folder_path);
	strcpy(target_folder_name, new_folder);
	if (!ai_win::search_file(new_folder_path, report_name)){
		return 0;
	}
	else{
		return 1;
	}
}

int create_report_file_test2(const char *test_item_str, char *target_report_folder_path, char *target_folder_name)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	//////////////////////////////////////////////////////////////////////////
	//look for the target folder and file
	char dir[MAX_PATH] = {0};
	ai_win::get_current_directory(dir);

	char new_path[MAX_PATH] = {0};
	char new_folder[MAX_PATH] = {0};
	char new_folder_path[MAX_PATH] = {0};

	sprintf(new_folder, test_item_str);
	sprintf(new_path, "D:\\");
	sprintf(new_folder_path, "%s\\%s", new_path, new_folder);  //..\report\laser

	if (!ai_win::search_directory(new_path, new_folder)) //search laser folder
	{
		_mkdir(new_folder_path);
	}

	sprintf(new_folder, "%04d%02d%02d", st.wYear, st.wMonth, st.wDay);
	sprintf(new_path, "%s", new_folder_path);
	sprintf(new_folder_path, "%s\\%s", new_path, new_folder);

	if (!ai_win::search_directory(new_path, new_folder)){
		_mkdir(new_folder_path);									//..\report\laser\year_month_day
	}
	char report_path[MAX_PATH] = {0};
	char report_name[MAX_PATH] = {0};
	sprintf(report_name, "%s_report_%s_%d.txt", test_item_str,new_folder,aa_num);
	sprintf(report_path, "%s\\%s", new_folder_path, report_name);

	strcpy(target_report_folder_path, new_folder_path);
	strcpy(target_folder_name, new_folder);
	if (!ai_win::search_file(new_folder_path, report_name)){
		return 0;
	}
	else{
		return 1;
	}
}

int create_report_file_raw(const char *test_item_str, char *target_report_folder_path, char *target_folder_name)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	//////////////////////////////////////////////////////////////////////////
	//look for the target folder and file
	char dir[MAX_PATH] = {0};
	ai_win::get_current_directory(dir);

	char new_path[MAX_PATH] = {0};
	char new_folder[MAX_PATH] = {0};
	char new_folder_path[MAX_PATH] = {0};

	sprintf(new_folder, test_item_str);
	sprintf(new_path, "D:\\");
	sprintf(new_folder_path, "%s\\%s", new_path, new_folder);  //..\report\laser

	if (!ai_win::search_directory(new_path, new_folder)) //search laser folder
	{
		_mkdir(new_folder_path);
	}

	sprintf(new_folder, "%04d%02d%02d", st.wYear, st.wMonth, st.wDay);

	char report_path[MAX_PATH] = {0};
	char report_name[MAX_PATH] = {0};
	sprintf(report_name, "%s_%s_%02d%02d%02d.RAW", mysbw.sn, new_folder, st.wHour, st.wMinute, st.wSecond);
	sprintf(report_path, "%s\\%s", new_folder_path, report_name);

	strcpy(target_report_folder_path, new_folder_path);
	strcpy(target_folder_name, new_folder);
	if (!ai_win::search_file(new_folder_path, report_name)){
		return 0;
	}
	else{
		return 1;
	}
}

void update_scenery_title(char *img_name, char *img_size, char *img_type, char *img_fps)
{
	char wintxt_str[MAX_PATH] = {0};
	GetWindowText(HWND_APP, wintxt_str, sizeof(wintxt_str));
	char mystr[4][MAX_PATH] = {0};
	char sep[] = ", ";
	int index = 0;
	char *token = NULL;
	token = strtok(wintxt_str, sep);

	while (token != NULL){
		strcpy(mystr[index], token);
		index++;
		token = strtok(NULL, sep);
	}

	if (img_name != NULL){
		memset(mystr[0], 0, sizeof(mystr[0]));
		strcpy(mystr[0], img_name);
	}
	if (img_size != NULL){
		memset(mystr[1], 0, sizeof(mystr[0]));
		strcpy(mystr[1], img_size);
	}

	if (img_type != NULL){
		memset(mystr[2], 0, sizeof(mystr[0]));
		strcpy(mystr[2], img_type);
	}
	if (img_fps != NULL){
		memset(mystr[3], 0, sizeof(mystr[0]));
		strcpy(mystr[3], img_fps);
	}

	char mystr2[MAX_PATH] = {0};
	sprintf(mystr2, "%s %s %s %s", mystr[0], mystr[1], mystr[2], mystr[3]);
	SetWindowText(HWND_APP, mystr2);
}

void update_mdlg_title(HWND hdlg)
{
	char str[100] = {0};
	sprintf(str, "Saker AA Software [%s]", s_aa::recipe_name);
	SetWindowText(hdlg, str);
}

int connect_device(int id)
{
	int result = 0;
	DXSetVideoPara(s_aa::dev[id].dwChannelHandle, 
		s_aa::dev[id].dwVidStandard, 
		s_aa::dev[id].dwColorspace, 
		s_aa::dev[id].dwWidth, 
		s_aa::dev[id].dwHeight,
		s_aa::dev[id].dwFrameRate);
	if (DXDeviceRun(s_aa::dev[id].dwChannelHandle) != 0x00){
		log("run device [%d] fail.", id);
		result = FALSE;
	}
	else{
		IMG_DEVICE_INDEX = id;
		log("use img device index = %d", id);
		result = TRUE;
	}

	return result;
}

void disconnect_device(int id)
{
	DXStopPreview(s_aa::dev[id].dwChannelHandle);
	DXDeviceStop(s_aa::dev[id].dwChannelHandle);
}

DWORD CALLBACK plc_online_counting_thread(LPVOID temp)
{
	BYTE write_b = 0;
	BYTE zPos  = 0;
	int status = 0;
	int timeout = 200;
	while (1){
		while (IS_PLC_CONNECTED == FALSE) Sleep(100);
	//	WaitForSingleObject(s_aa::plc_mutex, INFINITE);
		zPos = (int)(LINE_Y_POSITION * 1.0)/(LINE_Y_RATIO);
		write_b++;
		write_b %=255;
		status = write_to_plc_db(PLC_DB_INDEX/*145*/, PLC_DB_START, PLC_DB_LENS, &write_b);
		
		status = write_to_plc_db(PLC_DB_INDEX/*145*/, PLC_DB_START+4, PLC_DB_LENS, &zPos);
	//	log("zPosition = %d",zPos);
//		write_to_plc_db(144/*145*/, 0, 1, &write_b);
		Sleep(100);
		if (status != 0x00){
			IS_PLC_CONNECTED = FALSE;
			log("write err, plc not connected.");
		}
		
// 		ReleaseMutex(s_aa::plc_mutex);
// 		log("write plc byte0.");
		Sleep(100);
	}

	return FALSE;
}

DWORD CALLBACK plc_sn_read_thread(LPVOID temp)
{
	BYTE read_b = 0;
	BYTE write_b,check_b,check_d,to_plc,read_b2;
	BYTE zPos  = 0;
	int status = 0;
	int timeout = 200;
	int err_cnt;
	while (1){
		while (IS_PLC_CONNECTED == FALSE) Sleep(100);
		status = read_from_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+3, PLC_DB_LENS_R, &read_b);
	//	log("read 144.3 status = %d",read_b);
		if(read_b == 9){
			log("recv 9.read sn.");
			read_b = 0;
			status = write_to_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+3,PLC_DB_LENS_R, &read_b);
			BYTE read;
			read=0;
			for(int i=0;i<16;i++){
				status = read_from_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+10+i, 1, &read);
			//	read = read%16;
				mysbw.sn[i]=read;
				log("read%d",read);
			}
			log("sn=%s",mysbw.sn);
			read_b = 1;
			status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &read_b);
			log("read sn down.");
			SetDlgItemText(HWND_MAIN,IDC_SN_DISPLAY,mysbw.sn);
		}

		status = read_from_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+66, PLC_DB_LENS_R, &read_b);

		if(read_b == 7){
			log("recv 7.check img.");
							ITEM_ACTIVE(HWND_MAIN,IDC_START);
				Sleep(500);
				log("check img:%d",SHOW_IMG);
				err_cnt=0;
				if(SHOW_IMG==FALSE){
					ITEM_ACTIVE(HWND_MAIN,IDC_STOP);
					Sleep(200);
					ITEM_ACTIVE(HWND_MAIN,IDC_START);
					Sleep(500);
				}
				while(SHOW_IMG==FALSE){
					Sleep(300);
					err_cnt++;
					if(err_cnt>5)
						break;
				}
				if(err_cnt>=5){
					ITEM_ACTIVE(HWND_MAIN,IDC_STOP);
					

					write_b = 4;	//no image
					check_d = write_b;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &write_b);
					if (status != 0x00){
						//Sleep(50);
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &write_b);
					}
					write_b = 4;	//active down
					check_b = write_b;			
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
					if (status != 0x00){
						//Sleep(50);
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
					}
					log("no img.");
					SetDlgItemText(HWND_MAIN, IDC_PROCESS_AA_MOTION, "NG");
					NG_SIGNAL=1;
					//Sleep(100);
					write_b = 1;					
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &write_b);//
					to_plc = 0;
					//Sleep(50);
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write open catch down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &write_b);
						//Sleep(50);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &read_b2);
						
					}
					if(to_plc >= 10)
						break;

					log("************** case 7 end 1 ***************");
					break;
				}
				else{
					write_b = 3;	//you tu	
					check_d = write_b;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &write_b);
					if (status != 0x00){
						//Sleep(50);
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &write_b);
					}
					write_b = 4;	//active down	
					check_b = write_b;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
					if (status != 0x00){
						//Sleep(50);
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
					}
					//ITEM_ACTIVE(HWND_MAIN,IDC_STOP);
					//Sleep(200);
					log("************** case 7 end 2 ***************");
					break;
				}

		}
		Sleep(20);
	}

	return FALSE;
}

DWORD CALLBACK plc_emergency_stop_thread(LPVOID temp)
{
	BYTE s = 0x00;
	int status = 0;
//	int read_err_cnt = 0;
	while(1){
		status=read_from_plc_db(144,3,1,&s);
		if (status != 0){
			
			IS_PLC_CONNECTED = FALSE;
			log("read err, no data in this address.");
			
		}
		else
			if(s==0){
				log("emergency stop");
			}
			else{
				Sleep(100);
			}
	}
	return FALSE;
}

			

DWORD CALLBACK  plc_online_check_thread(LPVOID temp)
{
	BYTE last_b = 0;
	BYTE read_b = 0;
	int err_cnt = 0;
	int status = 0;
	int read_err_cnt = 0;
	int timeout = 200;

	while (1){
		while (IS_PLC_CONNECTED == FALSE) Sleep(100);
//		WaitForSingleObject(s_aa::plc_mutex, INFINITE);
	//	Sleep(1000);
		
		status = read_from_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R, PLC_DB_LENS_R, &read_b);
		SetDlgItemInt(HWND_MAIN, IDC_LINK_PLC2, read_b, FALSE);
		if (status != 0){
			read_err_cnt++;
			if (read_err_cnt>10){
				IS_PLC_CONNECTED = FALSE;
				log("read err, plc not connected[online check].");
			//	break;
			}
		}
		else{
			read_err_cnt = 0;
			err_cnt++;
			if (read_b != last_b){
				err_cnt = 0;
			}

			last_b = read_b;
			if (err_cnt>100){
				IS_PLC_CONNECTED = FALSE;
				log("over err cnt=100, plc not connected.");
			}
			


		}
//		ReleaseMutex(s_aa::plc_mutex);
	//	log("read plc byte0.");
		Sleep(100);
	}

	return FALSE;
}

void FloatToByte(float floatNum, unsigned char* byteArry)////浮点数到十六进制转换2
{

	char* pchar=(char*)&floatNum;

	for(int i=0;i<sizeof(float);i++){
		*byteArry=*pchar;
		pchar++;
		byteArry++;
	}
}

DWORD CALLBACK plc_read_thread2(LPVOID temp)
{
	int status  = 0;
	BYTE read_b = 0;
	BYTE read_b2 = 0;

	BYTE write_b = 0;
	BYTE mes_id[4];

	int to_plc = 0;

	int err_cnt = 0;
	int timeout = 0;
	int now_result = 0;
	while (1){
 		while (IS_PLC_CONNECTED == FALSE)
			Sleep(100);
		while((ok_ng_test::open_okng_test)&&((!ok_ng_test::ok_test)||(!ok_ng_test::ng_test)))
			Sleep(100);
		status = read_from_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+3, PLC_DB_LENS_R, &read_b);
	//	move_home_ok
		if (status != 0x00){
			err_cnt++;
			if (err_cnt>10){
				IS_PLC_CONNECTED = FALSE;
				log("read err, plc not connected.[read db]");
			}
		}
		else{
			read_b2 = 0;
			SetDlgItemInt(HWND_MAIN, IDC_LINK_PLC3, read_b, FALSE);
			err_cnt = 0;
			SYSTEMTIME st1,st2;

			switch (read_b){
			case 0:
				Sleep(100);
				break;	
			case 7:// check img exist
				log("************** case 7 start ***************");
				write_b = 0;					
				status = write_to_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+3, PLC_DB_LENS_R, &write_b);//clear
				to_plc = 0;
				//Sleep(50);
				status = read_from_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+3, PLC_DB_LENS_R, &read_b2);
				while(to_plc < 10){
					if(write_b == read_b2)
					{
						log("----****---- clear plc down -----****----");
						break;
					}
					to_plc++;
					status = write_to_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+3, PLC_DB_LENS_R, &write_b);
					//Sleep(50);
					status = read_from_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+3, PLC_DB_LENS_R, &read_b2);
					
				}
				if(to_plc >= 10)
					break;

				write_b = 2;				
				status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+5, PLC_DB_LENS, &write_b);//running
				
				to_plc = 0;
				//Sleep(50);
				status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+5, PLC_DB_LENS, &read_b2);
				while(to_plc < 10){
					if(write_b == read_b2)
					{
						log("----****---- write running down -----****----");
						break;
					}
					to_plc++;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+5, PLC_DB_LENS, &write_b);
					//Sleep(50);
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+5, PLC_DB_LENS, &read_b2);
					
				}
				if(to_plc >= 10)
					break;

				ITEM_ACTIVE(HWND_MAIN,IDC_START);
				Sleep(500);
				log("check img:%d",SHOW_IMG);
				err_cnt=0;
				if(SHOW_IMG==FALSE){
					ITEM_ACTIVE(HWND_MAIN,IDC_STOP);
					Sleep(200);
					ITEM_ACTIVE(HWND_MAIN,IDC_START);
					Sleep(500);
				}
				while(SHOW_IMG==FALSE){
					Sleep(300);
					err_cnt++;
					if(err_cnt>5)
						break;
				}
				if(err_cnt>=5){
					ITEM_ACTIVE(HWND_MAIN,IDC_STOP);
					

					write_b = 4;	//no image
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &write_b);
					to_plc = 0;
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write no-img down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &write_b);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &read_b2);
					}
					if(to_plc >= 10)
						break;

					write_b = 4;	//active down		
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+5, PLC_DB_LENS, &write_b);
					to_plc = 0;
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+5, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write active-down down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+5, PLC_DB_LENS, &write_b);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+5, PLC_DB_LENS, &read_b2);
					}
					if(to_plc >= 10)
						break;

					log("no img.");
					SetDlgItemText(HWND_MAIN, IDC_PROCESS_AA_MOTION, "NG");
					NG_SIGNAL=1;

					log("************** case 7 end 1 ***************");
					break;
				}
				else{
					write_b = 3;	//you tu	
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &write_b);
					to_plc = 0;
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write img-ok down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &write_b);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &read_b2);
					}
					if(to_plc >= 10)
						break;

					write_b = 4;	//active down	
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+5, PLC_DB_LENS, &write_b);
					to_plc = 0;
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+5, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write active-down down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+5, PLC_DB_LENS, &write_b);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+5, PLC_DB_LENS, &read_b2);
					}
					if(to_plc >= 10)
						break;
					//ITEM_ACTIVE(HWND_MAIN,IDC_STOP);
					//Sleep(200);
					log("************** case 7 end 2 ***************");
					break;
				}

			case 10://read sn
				log("************** case 10 start ***************");
				write_b = 0;					
				status = write_to_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+3, PLC_DB_LENS_R, &write_b);//clear
				to_plc = 0;
				status = read_from_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+3, PLC_DB_LENS_R, &read_b2);
				while(to_plc < 10){
					if(write_b == read_b2)
					{
						log("----****---- clear plc down -----****----");
						break;
					}
					to_plc++;
					status = write_to_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+3, PLC_DB_LENS_R, &write_b);
					status = read_from_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+3, PLC_DB_LENS_R, &read_b2);
					
				}
				if(to_plc >= 10)
					break;

				write_b = 2;				
				status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+5, PLC_DB_LENS, &write_b);//running
				
				to_plc = 0;
				status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+5, PLC_DB_LENS, &read_b2);
				while(to_plc < 10){
					if(write_b == read_b2)
					{
						log("----****---- write running down -----****----");
						break;
					}
					to_plc++;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+5, PLC_DB_LENS, &write_b);
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+5, PLC_DB_LENS, &read_b2);
					
				}
				if(to_plc >= 10)
					break;
				//*****************************************************

				BYTE read;
				status = read_from_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+6, 2, &read);
				mysbw.sn_cnt=read;
				//read=0;
				// 				status = read_from_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+132, 1, &read);
				// 				mysbw.white_result=read;
				//status = read_from_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+133, 1, &read);
				//mysbw.dispensing_result=read;
				//status = read_from_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+134, 1, &read);
				//mysbw.dispensing_result=read;
				read=0;
				for(int i=0;i<mysbw.sn_cnt;i++){
					status = read_from_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+8+i, 1, &read);
					//	read = read%16;
					//	log("read_byte:%d",read);
					if(read == NULL) break;
					mysbw.sn[i]=read;
					log("sn[%d]=%c",i,mysbw.sn[i]);

				}
				SetDlgItemText(HWND_MAIN,IDC_SN_DISPLAY,mysbw.sn);
				Sleep(500);

				//*****************************************************************
				//MES
				status = read_from_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+118, 4, mes_id);
				cy_id = mes_id[3] + (mes_id[2]<<8) + (mes_id[1]<<16) + (mes_id[0]<<24);
				log("read from mes id:%d",cy_id);

				//load_mes(mysbw.sn);
				//log("send mes load;");

				write_b = 4;	//active down
				status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+5, PLC_DB_LENS, &write_b);
				to_plc = 0;
				status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+5, PLC_DB_LENS, &read_b2);
				while(to_plc < 10){
					if(write_b == read_b2)
					{
						log("----****---- write active-down down -----****----");
						break;
					}
					to_plc++;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+5, PLC_DB_LENS, &write_b);
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+5, PLC_DB_LENS, &read_b2);
				}
				if(to_plc >= 10)
					break;
				log("************** case 10 end  ***************");
				break;
			default:
				log("receive other cmd:%d", read_b);
				write_b = 0;
				status = write_to_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+3,PLC_DB_LENS_R, &write_b);
				write_b = 4;	//active down			
				status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+5, PLC_DB_LENS, &write_b);

				break;
			}

			if (status != 0x00){
				IS_PLC_CONNECTED = FALSE;
			}

		}
	}
}

DWORD CALLBACK plc_read_thread(LPVOID temp)
{
	int status  = 0;
	BYTE read_b = 0;
	BYTE read_b2 = 0;

	BYTE check_r1 = 0;
	BYTE check_r2 = 0;

	BYTE write_b = 0;

	BYTE check_a = 0;
	BYTE check_b = 0;
	BYTE check_c = 0;
	BYTE check_d = 0;

	int to_plc = 0;

	int err_cnt = 0;
	int timeout = 0;
	int now_result = 0;
	while (1){
 		while (IS_PLC_CONNECTED == FALSE)
			Sleep(100);
		while((ok_ng_test::open_okng_test)&&((!ok_ng_test::ok_test)||(!ok_ng_test::ng_test))){
			if(start_ok_test){
				ITEM_ACTIVE(HWND_MAIN,IDC_START);
				Sleep(2000);
				if(mysbw.test_result == TRUE){		
					ok_ng_test::ok_test = TRUE;
					SET_CHECKER	(HWND_OKNG_TEST, IDC_OKTEST_CHECK, ok_ng_test::ok_test);

					ITEM_ACTIVE(HWND_MAIN,IDC_STOP);
					set_status(APP_PASS);
					log("ok_test check pass.**********************************");
					
				}
				else{
					ITEM_ACTIVE(HWND_MAIN,IDC_STOP);
					log("ok_test check fail.*********************************");
				}
				start_ok_test = 0;
			}
			
			else if(start_ng_test){
				ITEM_ACTIVE(HWND_MAIN,IDC_START);
				Sleep(2000);
				if(mysbw.test_result == TRUE){		

					ITEM_ACTIVE(HWND_MAIN,IDC_STOP);
					Sleep(100);
					log("ng_test check fail.**********************************");
					
				}
				else{
					ok_ng_test::ng_test = TRUE;
					SET_CHECKER	(HWND_OKNG_TEST, IDC_NGTEST_CHECK, ok_ng_test::ng_test);

					ITEM_ACTIVE(HWND_MAIN,IDC_STOP);
					Sleep(100);
					set_status(APP_PASS);
					log("ng_test check pass.*********************************");
					
				}
				start_ng_test = 0;
			}

			SetDlgItemText(HWND_MAIN, IDC_REPORT_OKNGTEST, "OK/NG 测试未通过...");
			Sleep(100);
		}
		SetDlgItemText(HWND_MAIN, IDC_REPORT_OKNGTEST, "");
		check_r2 = check_r1;
		status = read_from_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+1, PLC_DB_LENS_R, &read_b);
	//	move_home_ok
		check_r1 = read_b;
		if (status != 0x00){
			err_cnt++;
			if (err_cnt>10){
				IS_PLC_CONNECTED = FALSE;
				log("read err, plc not connected.[read db]");
			}
		}
		else{
			read_b2 = 0;
			SetDlgItemInt(HWND_MAIN, IDC_LINK_PLC3, read_b, FALSE);
			err_cnt = 0;
			SYSTEMTIME st1,st2;
			write_b = move_home_ok;
			status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+2, PLC_DB_LENS, &write_b);
			

			/*status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &check_a);
			if(check_a!=check_b){
				status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &check_b);
				log("To PLC check 1 : %d  %d.....",check_a,check_b);
				continue;
			}

			status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &check_c);
			if(check_c!=check_d){
				status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &check_d);
				log("To PLC check 2 : %d  %d.....",check_c,check_d);
				continue;
			}*/
			if(check_r1 == check_r2 && check_b != 2){
				continue;
			}
			switch (read_b){
			case 0:
				Sleep(100);
				break;	

			case 1:
				//pick lens
				log("************** case 1 start ***************");

				GetLocalTime(&st1);
				//write_b = 0;					
				//status = write_to_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+1, PLC_DB_LENS_R, &write_b);//clear
				//
				write_b = 2;
				check_b = write_b;
				status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);//running

				to_plc = 0;
				status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
				while(to_plc < 10){
					if(write_b == read_b2)
					{
						log("----****---- write running down -----****----");
						break;
					}
					to_plc++;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
				}
				if(to_plc >= 10)
					break;

				write_b = 1;					
				status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &write_b);//open
				to_plc = 0;
				//Sleep(50);
				status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &read_b2);
				while(to_plc < 10){
					if(write_b == read_b2)
					{
						log("----****---- write open catch down -----****----");
						break;
					}
					to_plc++;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &write_b);
					//Sleep(50);
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &read_b2);
					
				}
				if(to_plc >= 10)
					break;

				BUTTON_VALUE = 207;
				timeout = 100;

				while(timeout-- > 0  && MANUAL_MOVING == ON) 
				{
					Sleep(50);
				}

				write_b = 2;					
				status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &write_b);//close
				to_plc = 0;
				//Sleep(50);
				status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &read_b2);
				while(to_plc < 10){
					if(write_b == read_b2)
					{
						log("----****---- write close catch down -----****----");
						break;
					}
					to_plc++;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &write_b);
					//Sleep(50);
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &read_b2);
					
				}
				if(to_plc >= 10)
					break;

				write_b = 4;
				check_b = write_b;
				status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);//active down

				to_plc = 0;
				status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
				while(to_plc < 10){
					if(write_b == read_b2)
					{
						log("----****---- write active-down down -----****----");
						break;
					}
					to_plc++;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
				}
				if(to_plc >= 10)
					break;
				log("************** case 1 end ***************");
				break;

			case 2: //load 
				log("************** case 2 start ***************");
				//write_b = 0;					
				//status = write_to_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+1, PLC_DB_LENS_R, &write_b);//clear
				//
				write_b = 2;
				check_b = write_b;
				status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);//running

				to_plc = 0;
				//Sleep(50);
				status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
				while(to_plc < 10){
					if(write_b == read_b2)
					{
						log("----****---- write running down -----****----");
						break;
					}
					to_plc++;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
					//Sleep(50);
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
					
				}
				if(to_plc >= 10)
					break;

				BUTTON_VALUE = 208;
				timeout = 200;
				while(timeout-- > 0  && MANUAL_MOVING == ON) Sleep(50);//load lens

				write_b = 4;
				check_b = write_b;
				status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);//active down

				to_plc = 0;
				status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
				while(to_plc < 10){
					if(write_b == read_b2)
					{
						log("----****---- write active-down down -----****----");
						break;
					}
					to_plc++;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
				}
				if(to_plc >= 10)
					break;

				GetLocalTime(&st2);
				if(st2.wSecond<st1.wSecond)
					mysbw.ct = st2.wSecond+60-st1.wSecond;
				else
					mysbw.ct = st2.wSecond-st1.wSecond;
				log("load time : %d",mysbw.ct);
				ai_win::set_dlg_item_int(HWND_MAIN,IDC_REPORT_CycleTime,mysbw.ct);
				log("************** case 2 end ***************");
				break;


			case 3: //aa
				log("************** case 3 start ***************");
				GetLocalTime(&st1);

				write_b = 2;
				check_b = write_b;

				to_plc = 0;
				//Sleep(50);
				status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
				while(to_plc < 10){
					if(write_b == read_b2)
					{
						log("----****---- write running down -----****----");
						break;
					}
					to_plc++;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
					//Sleep(50);
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
					
				}
				if(to_plc >= 10)
					break;

				sfr_type = B_UV;

				timeout = 300;
				log("receive AA cmd");
			
				//ITEM_ACTIVE(HWND_MAIN,IDC_START);
				//Sleep(500);
				log("check img:%d",SHOW_IMG);
				err_cnt=0;
				if(SHOW_IMG==FALSE){
					ITEM_ACTIVE(HWND_MAIN,IDC_STOP);
					Sleep(200);
					ITEM_ACTIVE(HWND_MAIN,IDC_START);
					Sleep(1000);
				}
				while(SHOW_IMG==FALSE){
					Sleep(100);
					err_cnt++;
					if(err_cnt>10)
						break;
				}
				if(err_cnt>=10){
					ITEM_ACTIVE(HWND_MAIN,IDC_STOP);
					Sleep(200);
					write_b = 2;	//aa  ng
					check_d = write_b;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &write_b);

					to_plc = 0;
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write AA-NG down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &write_b);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &read_b2);
					}
					if(to_plc >= 10)
						break;

					write_b = 4;	//active down
					check_b = write_b;			
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);

					to_plc = 0;
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write active-down down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
					}
					if(to_plc >= 10)
						break;

					log("no img.");
					SetDlgItemText(HWND_MAIN, IDC_PROCESS_AA_MOTION, "NG");
					NG_SIGNAL=1;
					
					write_b = 1;					
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &write_b);//
					to_plc = 0;
					//Sleep(50);
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write open catch down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &write_b);
						//Sleep(50);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &read_b2);
						
					}
					if(to_plc >= 10)
						break;
					log("************** case 3 end 1 ***************");
					Sleep(200);
					break;
				}
				EXECUTE_ERROR = 0;
				EXECUTE_CMD = TRUE;													
				timeout = 2000;

				BUTTON_VALUE = 206;//aa

				while(timeout-->0 && EXECUTE_CMD == TRUE){
					Sleep(20);
				}

				if(mysbw.B_UVResult != 1 && timeout > 0){
					if(mysbw.sfr_val1[0][0]<mysbw.spec1[0]){
						EXECUTE_ERROR = PI_move_to_point(dPos_p[point_num[3]]);
					}
					if(!EXECUTE_ERROR){
					
						ITEM_ACTIVE(HWND_MAIN,IDC_STOP);
						Sleep(100);
						ITEM_ACTIVE(HWND_MAIN,IDC_START);
						BUTTON_VALUE = 206;							
						timeout = 2000;
						EXECUTE_CMD = TRUE;
						while(timeout-->0 && EXECUTE_CMD == TRUE){
							Sleep(20);
						}
					}
					
				}		
				if(timeout<=0 && EXECUTE_ERROR == TRUE){
					log("aa timeout");
					write_b=2;
					check_d = write_b;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &write_b);
					to_plc = 0;
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write AA-NG down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &write_b);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &read_b2);
					}
					if(to_plc >= 10)
						break;
					
					ITEM_ACTIVE(HWND_MAIN,IDC_STOP);
					Sleep(100);
					SetDlgItemText(HWND_MAIN, IDC_PROCESS_AA_MOTION, "NG");
					write_b=4;
					check_b = write_b;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);// err

					to_plc = 0;
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write active-down down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
					}
					if(to_plc >= 10)
						break;

					write_b = 1;					
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &write_b);//open
					to_plc = 0;
					//Sleep(50);
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write open catch down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &write_b);
						//Sleep(50);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &read_b2);
						
					}
					if(to_plc >= 10)
						break;
					log("************** case 3 end 2 ***************");
					break;
				}
					
				save_reg_mark_report_test(1,3);
				log("%d--%d",save_set::mes_aa_boffset,save_set::aa_boffset);
				if(save_set::mes_aa_boffset)
					save_reg_mark_report_test2(2,1);
				if(save_set::aa_boffset)
					save_reg_mark_report_test2(1,1);

				if (mysbw.test_result == FALSE){
				//	log("NG Signal:%d, err:%d",NG_SIGNAL, EXECUTE_ERROR);
					SetDlgItemText(HWND_MAIN, IDC_PROCESS_AA_MOTION, "NG");	
					save_reg_mark_report_c(s_aa::bmp_buf, IMG_WIDTH, IMG_HEIGHT,SAVE_NG_REG_PRE_REPORT);
					log("ng pre report saved.");
					write_b = 2;	//fail
					check_d = write_b;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &write_b);

					to_plc = 0;
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write AA-NG down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &write_b);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &read_b2);
					}
					if(to_plc >= 10)
						break;
					
					ITEM_ACTIVE(HWND_MAIN,IDC_STOP);
					Sleep(100);
					//Sleep(200);
					write_b = 4;	//active down	
					check_b = write_b;		
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);

					to_plc = 0;
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write active-down down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
					}
					if(to_plc >= 10)
						break;

					write_b = 1;					
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &write_b);//open
					to_plc = 0;
					//Sleep(50);
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write open catch down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &write_b);
						//Sleep(50);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &read_b2);
						
					}
					if(to_plc >= 10)
						break;
					log("AA fail.");
					set_status(APP_FAIL);
				//	ITEM_ACTIVE(HWND_MAIN,IDC_STOP);
				}
				else{
					SetDlgItemText(HWND_MAIN, IDC_PROCESS_AA_MOTION, "OK");		
					set_status(APP_PASS);
					//save_reg_mark_report_c(s_aa::bmp_buf, IMG_WIDTH, IMG_HEIGHT,SAVE_OK_REG_PRE_REPORT);
					//save_reg_mark_report_c(s_aa::bmp_buf, IMG_WIDTH, IMG_HEIGHT,SAVE_MES		   );
					log("AA ok, report saved, UV start cuving.");
					BUTTON_VALUE = 205;							
					timeout = 500;
				//	Sleep(500);
					while(timeout-->0 && MANUAL_MOVING == ON){
						Sleep(20);
					}
					if(save_set::mes_aa_aoffset)
						save_reg_mark_report_test2(2,1);
					if(save_set::aa_aoffset)
						save_reg_mark_report_test2(1,2);

					write_b = 1; //finished.ok
					check_d = write_b;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &write_b);	
					to_plc = 0;
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write AA-OK down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &write_b);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &read_b2);
					}
					if(to_plc >= 10)
						break;

					write_b = 4;	//active down			
					check_b = write_b;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);

					to_plc = 0;
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write active-down down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
					}
					if(to_plc >= 10)
						break;
					
				}
				GetLocalTime(&st2);
				if(st2.wSecond<st1.wSecond)
					mysbw.ct = st2.wSecond+60-st1.wSecond;
				else
					mysbw.ct = st2.wSecond-st1.wSecond;
				log("AA time : %d",mysbw.ct);
				ai_win::set_dlg_item_int(HWND_MAIN,IDC_REPORT_CycleTime,mysbw.ct);
				log("************** case 3 end 3 ***************");
				GetLocalTime(&st1);
				break;

			case 4://CHECK after UV 
				log("************** case 4 start ***************");
				//read_b = 0;					
				//status = write_to_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+1, PLC_DB_LENS_R, &read_b);//clear

				write_b = 2;			
				check_b = write_b;		
				status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);//running
				
				to_plc = 0;
				//Sleep(50);
				status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
				while(to_plc < 10){
					if(write_b == read_b2)
					{
						log("----****---- write running down -----****----");
						break;
					}
					to_plc++;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
					//Sleep(50);
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
					
				}
				if(to_plc >= 10)
					break;

				sfr_type = A_UV;//控制清晰度判断的规格选择

				ITEM_ACTIVE(HWND_MAIN,IDC_START);
				Sleep(1000);//***********************************************************************
				save_reg_mark_report_test(1,4);
				if(save_set::mes_uv_bload)
					save_reg_mark_report_test2(2,1);
				if(save_set::uv_bload)
					save_reg_mark_report_test2(1,3);

				write_b = 1;					
				status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &write_b);//open
				to_plc = 0;
				//Sleep(50);
				status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &read_b2);
				while(to_plc < 10){
					if(write_b == read_b2)
					{
						log("----****---- write open catch down -----****----");
						break;
					}
					to_plc++;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &write_b);
					//Sleep(50);
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &read_b2);
					
				}
				if(to_plc >= 10)
					break;
				Sleep(200);

				save_reg_mark_report_test(2,4);
				
				if(save_set::mes_uv_bload)
					save_reg_mark_report_test2(2,1);
				if(save_set::uv_bload)
					save_reg_mark_report_test2(1,4);

				if(SHOW_IMG == FALSE)
				{
					SetDlgItemText(HWND_MAIN, IDC_PROCESS_CHECK2, "NG");		
					
					save_reg_mark_report_c(s_aa::bmp_buf, IMG_WIDTH, IMG_HEIGHT,SAVE_NG_REG_REPORT);
					ITEM_ACTIVE(HWND_MAIN,IDC_STOP);
					Sleep(100);
					set_status(APP_ERR);
					log("Show image error.********************************************");
					write_b = 2; //check ng
					check_d = write_b;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &write_b);
					to_plc = 0;
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write AA-NG down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &write_b);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &read_b2);
					}
					if(to_plc >= 10)
						break;

					write_b = 4;	//active down				
					check_b = write_b;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
					to_plc = 0;
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write active-down down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
					}
					if(to_plc >= 10)
						break;

					GetLocalTime(&st2);
					if(st2.wSecond<st1.wSecond)
						mysbw.ct = st2.wSecond+60-st1.wSecond;
					else
						mysbw.ct = st2.wSecond-st1.wSecond;
					log("AA ok->after UV time : %d",mysbw.ct);
					ai_win::set_dlg_item_int(HWND_MAIN,IDC_REPORT_CycleTime,mysbw.ct);
					log("************** case 4 end 1 ***************");
					break;
				}
				log("----------------------------test result : %d",mysbw.test_result);
				if(mysbw.test_result == TRUE){
					SetDlgItemText(HWND_MAIN, IDC_PROCESS_CHECK2, "OK");		
					
					save_reg_mark_report_c(s_aa::bmp_buf, IMG_WIDTH, IMG_HEIGHT,SAVE_OK_REG_REPORT);
					ITEM_ACTIVE(HWND_MAIN,IDC_STOP);
					Sleep(100);
					set_status(APP_PASS);
					log("SFR CHECK ok, report saved.**********************************");
					write_b = 1; //finished.ok
					check_d = write_b;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &write_b);	
					to_plc = 0;
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write AA-OK down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &write_b);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &read_b2);
					}
					if(to_plc >= 10)
						break;

					write_b = 4;	//active down
					check_b = write_b;				
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
					to_plc = 0;
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write active-down down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
					}
					if(to_plc >= 10)
						break;
				}
				else{
					SetDlgItemText(HWND_MAIN, IDC_PROCESS_CHECK2, "NG");		
					
					save_reg_mark_report_c(s_aa::bmp_buf, IMG_WIDTH, IMG_HEIGHT,SAVE_NG_REG_REPORT);
					ITEM_ACTIVE(HWND_MAIN,IDC_STOP);
					Sleep(100);
					set_status(APP_FAIL);
					log("SFR CHECK ng, report saved.*********************************");
					write_b = 2; //check ng
					check_d = write_b;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &write_b);
					to_plc = 0;
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write AA-NG down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &write_b);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+3, PLC_DB_LENS, &read_b2);
					}
					if(to_plc >= 10)
						break;

					write_b = 4;	//active down
					check_b = write_b;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
					to_plc = 0;
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write active-down down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
					}
					if(to_plc >= 10)
						break;
				}
				GetLocalTime(&st2);
				if(st2.wSecond<st1.wSecond)
					mysbw.ct = st2.wSecond+60-st1.wSecond;
				else
					mysbw.ct = st2.wSecond-st1.wSecond;
				log("AA ok->after UV time : %d",mysbw.ct);
				ai_win::set_dlg_item_int(HWND_MAIN,IDC_REPORT_CycleTime,mysbw.ct);
				log("************** case 4 end 2 ***************");
				break;
				
			case 5://home
				write_b = 0;					
				status = write_to_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+1, PLC_DB_LENS_R, &write_b);//clear
				to_plc = 0;
				//Sleep(50);
				status = read_from_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+1, PLC_DB_LENS_R, &read_b2);
				while(to_plc < 10){
					if(write_b == read_b2)
					{
						log("----****---- clear plc down -----****----");
						break;
					}
					to_plc++;
					status = write_to_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+1, PLC_DB_LENS_R, &write_b);
					//Sleep(50);
					status = read_from_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+1, PLC_DB_LENS_R, &read_b2);
					
				}
				if(to_plc >= 10)
					break;

				log("************** case 5 start ***************");
				write_b = 1;					
				status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &write_b);//
				to_plc = 0;
				//Sleep(50);
				status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &read_b2);
				while(to_plc < 10){
					if(write_b == read_b2)
					{
						log("----****---- write open catch down -----****----");
						break;
					}
					to_plc++;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &write_b);
					//Sleep(50);
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &read_b2);
					
				}
				if(to_plc >= 10)
					break;
				Sleep(200);

				write_b = 2;	
				check_b = write_b;				
				status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);//running
				
				to_plc = 0;
				//Sleep(50);
				status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
				while(to_plc < 10){
					if(write_b == read_b2)
					{
						log("----****---- write running down -----****----");
						break;
					}
					to_plc++;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
					//Sleep(50);
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
					
				}
				if(to_plc >= 10)
					break;

				move_home_ok=move_home();
				if(move_home_ok){
					log("move home down.");
					
					BUTTON_VALUE = 209;
					log("move org pos down.");

					write_b = 5;
					check_b = write_b;					
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
					to_plc = 0;
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write home down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
					}
					if(to_plc >= 10)
						break;
				}
				else{
					log("home error.");
					write_b = 3;
					check_b = write_b;					
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
					to_plc = 0;
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
					while(to_plc < 10){
						if(write_b == read_b2)
						{
							log("----****---- write home-error down -----****----");
							break;
						}
						to_plc++;
						status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
						status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
					}
					if(to_plc >= 10)
						break;
				}
				log("************** case 5 end ***************");
				break;

			case 6://org position
				log("************** case 6 start ***************");
				write_b = 0;					
				status = write_to_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+1, PLC_DB_LENS_R, &write_b);//clear
				to_plc = 0;
				//Sleep(50);
				status = read_from_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+1, PLC_DB_LENS_R, &read_b2);
				while(to_plc < 10){
					if(write_b == read_b2)
					{
						log("----****---- clear plc down -----****----");
						break;
					}
					to_plc++;
					status = write_to_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+1, PLC_DB_LENS_R, &write_b);
					//Sleep(50);
					status = read_from_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+1, PLC_DB_LENS_R, &read_b2);
					
				}
				if(to_plc >= 10)
					break;

				write_b = 1;					
				status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &write_b);//
				to_plc = 0;
				//Sleep(50);
				status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &read_b2);
				while(to_plc < 10){
					if(write_b == read_b2)
					{
						log("----****---- write open catch down -----****----");
						break;
					}
					to_plc++;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &write_b);
					//Sleep(50);
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+6, PLC_DB_LENS, &read_b2);
					
				}
				if(to_plc >= 10)
					break;
				write_b = 2;
				check_b = write_b;					
				status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);//running
				
				to_plc = 0;
				//Sleep(50);
				status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
				while(to_plc < 10){
					if(write_b == read_b2)
					{
						log("----****---- write running down -----****----");
						break;
					}
					to_plc++;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
					//Sleep(50);
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
					
				}
				if(to_plc >= 10)
					break;

				BUTTON_VALUE = 209;

				log("move org pos down.");

				//unload_mes(mysbw.sn,1,1,1);
				//log("send mes unload;");

				write_b = 6;
				check_b = write_b;					
				status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
				to_plc = 0;
				status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
				while(to_plc < 10){
					if(write_b == read_b2)
					{
						log("----****---- write wait-point down -----****----");
						break;
					}
					to_plc++;
					status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);
					status = read_from_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &read_b2);
				}
				if(to_plc >= 10)
					break;
				log("************** case 6 end ***************");
				break;

			case 7:// check img exist
				break;
			case 10://read sn
				
				break;


			case 8: //auto
				break;
			case 9:
				break;
			default:
				log("receive other cmd:%d", read_b);
				write_b = 0;
				status = write_to_plc_db(PLC_DB_INDEX_R, PLC_DB_START_R+1,PLC_DB_LENS_R, &write_b);
				write_b = 4;	//active down			
				status = write_to_plc_db(PLC_DB_INDEX, PLC_DB_START+1, PLC_DB_LENS, &write_b);

				break;
			}

			if (status != 0x00){
				IS_PLC_CONNECTED = FALSE;
			}

		}
	}
}

DWORD CALLBACK check_img_exist_thread(LPVOID){
	log("start check img thread.");
	int w=IMG_WIDTH;
	int h=IMG_HEIGHT;
	int sum_g1,sum_g2,sum;
	int i,j;
	SHOW_IMG=FALSE;
	sum_g1=sum_g2=sum=0;
	i=5;
	while(1){
		sum_g1=0;
		for(j=0;j<w-1;j++)					
			sum_g1+=s_aa::bmp_buf[(i*w+j)*3+1];
		if(sum_g1==sum_g2){
			SHOW_IMG=FALSE;
		//	log("no img.1=%d,2=%d",sum_g1,sum_g2);
		}
		else{
			SHOW_IMG=TRUE;
			sum=0;
		//	log("exist img.1=%d,2=%d",sum_g1,sum_g2);
		}
		sum_g2=sum_g1;
		Sleep(400);

	}
}

DWORD CALLBACK  pi_link_thread(LPVOID temp)
{
	while (1){
		if (ID < 0){
			SetDlgItemText(HWND_MAIN, IDC_LINK_PI, "Error...");
//			ai_win::read_string_key(path, "plc", "ip", s_aa::plc_ip, sizeof(s_aa::plc_ip));
//			disconnect_from_plc();
//			Sleep(100);
//
//			status = connect_to_plc(s_aa::plc_ip);
//			Sleep(200);
//
//			if (status == TRUE){
//				IS_PLC_CONNECTED = TRUE;
//				SetDlgItemText(HWND_MAIN, IDC_LINK_PLC, "Connected");
//				
//				log("plc link ok.");
//				
//// 				read_b = 0;
//// 				write_to_plc_db(PLC_DB_INDEX, PLC_DB_START, PLC_DB_LENS, &read_b);///
//// 				log("write to plc ready.");
//			}
//			else{
//				SetDlgItemText(HWND_MAIN, IDC_LINK_PLC, "Connecting..");
//				Sleep(500);
//			}
		}
		else{
			SetDlgItemText(HWND_MAIN, IDC_LINK_PI, "Connected");
		}
		Sleep(500);
	}

	return FALSE;
}


DWORD CALLBACK  plc_link_thread(LPVOID temp)
{
	char path[MAX_PATH] = {0};
	sprintf(path, "%s\\recipe\\config.ini", s_aa::dir);
	int status = 0;
	BYTE read_b = 0;
	plc_mutex = CreateMutex(NULL, FALSE, "plc_mutex");

	while (1){
		if (IS_PLC_CONNECTED == FALSE){
			SetDlgItemText(HWND_MAIN, IDC_LINK_PLC, "Connecting...");
			
			ai_win::read_string_key(path, "plc", "ip", s_aa::plc_ip, sizeof(s_aa::plc_ip));
			disconnect_from_plc();
			Sleep(100);

			status = connect_to_plc(s_aa::plc_ip);
			Sleep(200);

			if (status == TRUE){
				IS_PLC_CONNECTED = TRUE;
				SetDlgItemText(HWND_MAIN, IDC_LINK_PLC, "Connected");
				
				log("plc link ok.");
				
// 				read_b = 0;
// 				write_to_plc_db(PLC_DB_INDEX, PLC_DB_START, PLC_DB_LENS, &read_b);///
// 				log("write to plc ready.");
			}
			else{
				SetDlgItemText(HWND_MAIN, IDC_LINK_PLC, "Connecting..");
				Sleep(500);
			}
		}
		else{
			SetDlgItemText(HWND_MAIN, IDC_LINK_PLC, "Connected");
		}
		Sleep(500);
	}

	return FALSE;
}

 BOOL load_mes(char *sn)
 {
 	int result = FALSE;
 //	log("step1");
 	ACCCommClientLib::IACCCommPtr comm;
 	comm.CreateInstance(__uuidof(ACCCommClientLib::ACCComm));
 	//	strcpy(comm.Server, "127.0.0.1");
 	comm->Server = _bstr_t("192.168.10.1");
 	comm->Port = 3801;
 	comm->Program = _bstr_t("EOLT");
 	comm->Station = _bstr_t("70");
 	comm->Product = _bstr_t("A000001");
 	comm->LogFile = _bstr_t("d:\\acc.log");
 	comm->Open();
 //	log("step2");
 	if (comm->Connected){
 		log("MES_L:connected.");
 		ACCCommClientLib::IACCCmdPtr cmd = comm->CreateCmd(); 
 
 		cmd->Name = "Load";
 		cmd->Params["PartTypeID"] = "1";
 		cmd->Params["SN1"] = sn/*"FC0000002"*/;
 		cmd->Send();
 		log("MES_L:Return code:%d", cmd->RTN_Code);
 		int MES_LOAD_CODE = cmd->RTN_Code;
 		log("MES_L:Return message:%s", (LPCTSTR)cmd->RTN_Msg);
 		log("MES_L:SN:%s", sn);
 		if (cmd->RTN_Code == 0){
 			log("MES_L:CycleID:%d",			cmd->RTN_CycleID);
 			log("MES_L:StatusBits:%d",		cmd->RTN_StatusBits);
 			log("MES_L:IsRework:%s",		cmd->RTN_IsRework?"True":"False");
 			log("MES_L:UnitPartTypeID:%d",	cmd->RTN_UnitParttypeID);
 			log("MES_L:PanelState:%d",		(LPCTSTR)cmd->RTN_PanelState);
 			log("MES_L:OtherInfo:%s",		(LPCTSTR)cmd->RTN_OtherInfo);
 			log("MES_L:UserDefined:%s",		(LPCTSTR)cmd->RTN_Params["UserDefined"]);
 			result = cmd->RTN_IsRework;
 		}
 	}
 	else{
 		log("MES_L:Connect fail.");
 		result = -1;
 	}
 	comm->Close();
 //	log("step3");
 	return result;
 }

 BOOL unload_mes(char *sn, int s2_result, int s3_result, int s4_result)
 {
 	int result = FALSE;
 
 	int bit1 = (s2_result&0x01);
 	int bit2 = (s3_result&0x01);
 	int bit3 = (s4_result&0x01);
 	int bit0 = (bit1&bit2&bit3);
 	char statusbit[10] = {0};
 	char failbit[10] = {0};
 
 	sprintf(statusbit, "%d", (bit3<<3)+(bit2<<2)+(bit1<<1)+bit0);
 	sprintf(failbit, "%d", ((!bit3)<<3)+((!bit2)<<2)+((!bit1)<<1)+(!bit0));
 
 	ACCCommClientLib::IACCCommPtr comm;
 	comm.CreateInstance(__uuidof(ACCCommClientLib::ACCComm));
 
 	//	strcpy(comm.Server, "127.0.0.1");
 	comm->Server = _bstr_t("192.168.10.1");
 	comm->Port = 3801;
 	comm->Program = _bstr_t("EOLT");
 	comm->Station = _bstr_t("70");
 	comm->Product = _bstr_t("A000001");
 	comm->LogFile = _bstr_t("d:\\acc.log");
 	comm->Open();
 
 	if (comm->Connected){
 		log("MES_U:connected.");
 		ACCCommClientLib::IACCCmdPtr cmd = comm->CreateCmd(); 
 
 		cmd->Name = "Unload";
 		cmd->Params["PartTypeID"] = "1";
 		cmd->Params["SN1"] = sn/*"FC0000002"*/;
 		cmd->Params["StatusBits"] = statusbit/*"7"*/; // 第1位是总的结果，第2~N是子项的结果
 		cmd->Params["FailureBits"] = failbit/*"0"*/;
 		//cmd->Params["N_Real1"] = "3.5";
 		//cmd->Params["N_Real2"] = "6.12";
 		cmd->Send();
 		log("MES_U:status bit:%s", statusbit);
 		log("MES_U:fail bit:%s", failbit);
 		log("MES_U:Return code:%d", cmd->RTN_Code);
 		log("MES_U:Return message:%s", (LPCTSTR)cmd->RTN_Msg);
 		log("MES_U:SN:%s", sn);
 		int MES_UNLOAD_CODE = cmd->RTN_Code;
 		if (MES_UNLOAD_CODE == 0) result = TRUE;
 	}
 	else{
 		log("MES_U:Connect fail.");
 		result = -1;
 	}
 	comm->Close();
 
 	return result;
 }
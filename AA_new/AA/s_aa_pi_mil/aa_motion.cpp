#define _CRT_SECURE_NO_WARNINGS
#define EXTERN
#define DECLARE
#include "interface.h"
#include "inf_motion_aa.h"
#include "test_reg_mark_chart.h"

#define M_LINE_X						0
#define M_LINE_Y						1
#define M_LINE_Z						2
#define M_TILT_X						3
#define M_TILT_Y						4
#define M_TILT_Z						5

#define TIMER_MOTOR_DLG_JOG				1
#define MOTOR_NUM						6

HANDLE	Motors1_hMutex;
HANDLE	Motors2_hMutex;
HANDLE	Motors3_hMutex;
HANDLE	Err_hMutex;

char err_msg[200][MAX_PATH] ={0};


SlantedBW_SM FS;
extern SlantedBW mysbw;

extern DWORD WINAPI home_motors_thread(LPVOID temp);
extern DWORD WINAPI position_display_thread(LPVOID temp);
extern DWORD WINAPI button_process_thread(LPVOID temp);

#define		thread_no		21

LPCTSTR		motors1_mutex_name	= "MUTEX_MOTORS1";
LPCTSTR		motors2_mutex_name	= "MUTEX_MOTORS2";
LPCTSTR		motors3_mutex_name	= "MUTEX_MOTORS3";
LPCTSTR		err_mutex_name		= "MUTEX_ERR";

HANDLE		hThread[thread_no];
DWORD		dwThreadID[thread_no];

int motor_init_err = 1;
int select_motor = M_LINE_X;
int motor_flag = 0;

void load_err_msg(void)//motor return errcode
{
	strcpy(err_msg[100], "SM101_NOT_READY_ERR			");	
	strcpy(err_msg[101], "SM101_NOT_IN_POSITION_ERR		");	
	strcpy(err_msg[102], "SM101_ALARM_ERR				");	
	strcpy(err_msg[103], "SM101_P_LIMIT_ERR				");	
	strcpy(err_msg[104], "SM101_N_LIMIT_ERR				");	
	strcpy(err_msg[105], "SM101_HOME_SENSOR_STILL_ON_ERR");	
	strcpy(err_msg[106], "SM101_HOMING_FAILURE_ERR		");	
	strcpy(err_msg[107], "SM101_NOT_HOMED_YET_WARNING	");	
	strcpy(err_msg[108], "SM101_TRIGGER_NOT_READY_ERR	");	
	strcpy(err_msg[109], "SM101_MOTION_COMMAND_ERR		");	

	strcpy(err_msg[110], "SM102_NOT_READY_ERR			");	
	strcpy(err_msg[111], "SM102_NOT_IN_POSITION_ERR		");	
	strcpy(err_msg[112], "SM102_ALARM_ERR				");	
	strcpy(err_msg[113], "SM102_P_LIMIT_ERR				");	
	strcpy(err_msg[114], "SM102_N_LIMIT_ERR				");	
	strcpy(err_msg[115], "SM102_HOME_SENSOR_STILL_ON_ERR");	
	strcpy(err_msg[116], "SM102_HOMING_FAILURE_ERR		");	
	strcpy(err_msg[117], "SM102_NOT_HOMED_YET_WARNING	");	
	strcpy(err_msg[118], "SM102_TRIGGER_NOT_READY_ERR	");	
	strcpy(err_msg[119], "SM102_MOTION_COMMAND_ERR		");	

	strcpy(err_msg[120], "SM103_NOT_READY_ERR			");
	strcpy(err_msg[121], "SM103_NOT_IN_POSITION_ERR		");
	strcpy(err_msg[122], "SM103_ALARM_ERR				");
	strcpy(err_msg[123], "SM103_P_LIMIT_ERR				");
	strcpy(err_msg[124], "SM103_N_LIMIT_ERR				");
	strcpy(err_msg[125], "SM103_HOME_SENSOR_STILL_ON_ERR");
	strcpy(err_msg[126], "SM103_HOMING_FAILURE_ERR		");
	strcpy(err_msg[127], "SM103_NOT_HOMED_YET_WARNING	");
	strcpy(err_msg[128], "SM103_TRIGGER_NOT_READY_ERR	");
	strcpy(err_msg[129], "SM103_MOTION_COMMAND_ERR		");

	strcpy(err_msg[130], "SM104_NOT_READY_ERR			");
	strcpy(err_msg[131], "SM104_NOT_IN_POSITION_ERR		");
	strcpy(err_msg[132], "SM104_ALARM_ERR				");
	strcpy(err_msg[133], "SM104_P_LIMIT_ERR				");
	strcpy(err_msg[134], "SM104_N_LIMIT_ERR				");
	strcpy(err_msg[135], "SM104_HOME_SENSOR_STILL_ON_ERR");
	strcpy(err_msg[136], "SM104_HOMING_FAILURE_ERR		");
	strcpy(err_msg[137], "SM104_NOT_HOMED_YET_WARNING	");
	strcpy(err_msg[138], "SM104_TRIGGER_NOT_READY_ERR	");
	strcpy(err_msg[139], "SM104_MOTION_COMMAND_ERR		");

	strcpy(err_msg[140], "SM201_NOT_READY_ERR			");
	strcpy(err_msg[141], "SM201_NOT_IN_POSITION_ERR		");
	strcpy(err_msg[142], "SM201_ALARM_ERR				");
	strcpy(err_msg[143], "SM201_P_LIMIT_ERR				");
	strcpy(err_msg[144], "SM201_N_LIMIT_ERR				");
	strcpy(err_msg[145], "SM201_HOME_SENSOR_STILL_ON_ERR");
	strcpy(err_msg[146], "SM201_HOMING_FAILURE_ERR		");
	strcpy(err_msg[147], "SM201_NOT_HOMED_YET_WARNING	");
	strcpy(err_msg[148], "SM201_TRIGGER_NOT_READY_ERR	");
	strcpy(err_msg[149], "SM201_MOTION_COMMAND_ERR		");

	strcpy(err_msg[150], "SM202_NOT_READY_ERR			");
	strcpy(err_msg[151], "SM202_NOT_IN_POSITION_ERR		");
	strcpy(err_msg[152], "SM202_ALARM_ERR				");
	strcpy(err_msg[153], "SM202_P_LIMIT_ERR				");
	strcpy(err_msg[154], "SM202_N_LIMIT_ERR				");
	strcpy(err_msg[155], "SM202_HOME_SENSOR_STILL_ON_ERR");
	strcpy(err_msg[156], "SM202_HOMING_FAILURE_ERR		");
	strcpy(err_msg[157], "SM202_NOT_HOMED_YET_WARNING	");
	strcpy(err_msg[158], "SM202_TRIGGER_NOT_READY_ERR	");
	strcpy(err_msg[159], "SM202_MOTION_COMMAND_ERR		");

	strcpy(err_msg[160], "SM203_NOT_READY_ERR			");
	strcpy(err_msg[161], "SM203_NOT_IN_POSITION_ERR		");
	strcpy(err_msg[162], "SM203_ALARM_ERR				");
	strcpy(err_msg[163], "SM203_P_LIMIT_ERR				");
	strcpy(err_msg[164], "SM203_N_LIMIT_ERR				");
	strcpy(err_msg[165], "SM203_HOME_SENSOR_STILL_ON_ERR");
	strcpy(err_msg[166], "SM203_HOMING_FAILURE_ERR		");
	strcpy(err_msg[167], "SM203_NOT_HOMED_YET_WARNING	");
	strcpy(err_msg[168], "SM203_TRIGGER_NOT_READY_ERR	");
	strcpy(err_msg[169], "SM203_MOTION_COMMAND_ERR		");

	strcpy(err_msg[170], "SM204_NOT_READY_ERR			");
	strcpy(err_msg[171], "SM204_NOT_IN_POSITION_ERR		");
	strcpy(err_msg[172], "SM204_ALARM_ERR				");
	strcpy(err_msg[173], "SM204_P_LIMIT_ERR				");
	strcpy(err_msg[174], "SM204_N_LIMIT_ERR				");
	strcpy(err_msg[175], "SM204_HOME_SENSOR_STILL_ON_ERR");
	strcpy(err_msg[176], "SM204_HOMING_FAILURE_ERR		");
	strcpy(err_msg[177], "SM204_NOT_HOMED_YET_WARNING	");
	strcpy(err_msg[178], "SM204_TRIGGER_NOT_READY_ERR	");
	strcpy(err_msg[179], "SM204_MOTION_COMMAND_ERR		");
	strcpy(err_msg[180], "AA_MOVE_Z_ERR"				 );
}

//////////////////////////////////////////////////////////////////////////
//interface function
int aa_motion_init(void)
{
	short	pCardNum;
	BYTE	bCardID;

	BUTTON_VALUE = 0X00;
	

	log("====AA motion Init====");
	Err_hMutex		= CreateMutex(NULL, FALSE, err_mutex_name		); log("create Err_hMutex[%d]", GetLastError());
	Motors1_hMutex	= CreateMutex(NULL, FALSE, motors1_mutex_name	); log("create motors1_hMutex[%d]", GetLastError());
	Motors2_hMutex	= CreateMutex(NULL, FALSE, motors2_mutex_name	); log("create motors2_hMutex[%d]", GetLastError());
	Motors3_hMutex	= CreateMutex(NULL, FALSE, motors3_mutex_name	); log("create motors3_hMutex[%d]", GetLastError());     
	hThread[0]	= CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Tscreen,				NULL,0,&dwThreadID[0]);	

	QueryPerformanceFrequency(&Frequency);	// get hardware performance counts per second
	//==== open device PISO-PS400 =======================================================================
	int errcode = ps400_scan(&pCardNum, NULL);
	if (errcode != SUCCESS_NO_ERROR){
		log("Device PISO-PS400 open error ! ERROR_CODE2=%d\n",errcode);
		return errcode;
	}
	for(int i=0;i < pCardNum; i++ ){
		if( (errcode = ps400_get_cardinfo( i, &bCardID )) == SUCCESS_NO_ERROR ){
			if((errcode = ps400_open(bCardID)) != SUCCESS_NO_ERROR){
				log("ps400 Card-%d open failure, and error code=%d\n",bCardID, errcode);
				return errcode;
			}
		}
	}

	if((errcode = Motion_Motor_Init()) != SUCCESS_NO_ERROR){
		log("Device PISO-PS400 initialization error ! ERROR_CODE2=%d\n",errcode);
		ps400_close_all();
		return (errcode);
	}
	log(">Motor Card Init OK.\n");
	SM101_HOMED_FLAG=SM102_HOMED_FLAG=SM103_HOMED_FLAG=SM201_HOMED_FLAG=SM202_HOMED_FLAG=SM203_HOMED_FLAG=0;
	log("home flag=%d",HOME_FLAG);

	SM101_SERVO_ON;
	SM102_SERVO_ON;
	SM103_SERVO_ON;

	SM201_SERVO_ON;
	SM202_SERVO_ON;
	SM203_SERVO_ON;

	//---------------------------------------------------------------------------------------------
	hThread[1]	= CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Iokernel,				NULL,0,&dwThreadID[1]);	
	hThread[2]	= CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Readcntr,				NULL,0,&dwThreadID[2]);
//	hThread[3]	= CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Monitor,				NULL,0,&dwThreadID[3]);
//	hThread[4]	= CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Lamp,					NULL,0,&dwThreadID[4]);
//	hThread[5]	= CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ErrProcs,				NULL,0,&dwThreadID[5]);
//	hThread[6]	= CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)FocusCheckTask,		NULL,0,&dwThreadID[6]);
//	hThread[7]	= CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)AFTestTask,			NULL,0,&dwThreadID[7]);
//	hThread[8]	= CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ImageTestTask,		NULL,0,&dwThreadID[8]);
//	hThread[9]	= CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)CentralRotateTask,	NULL,0,&dwThreadID[9]);
//	hThread[10]	= CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ManualCommandServer,	NULL,0,&dwThreadID[10]);	
	hThread[11] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)TriggerSM101,			NULL,0,&dwThreadID[11]);	
	hThread[12] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)TriggerSM102,			NULL,0,&dwThreadID[12]);	
	hThread[13] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)TriggerSM103,			NULL,0,&dwThreadID[13]);	
//	hThread[14] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)TriggerSM104,			NULL,0,&dwThreadID[14]);	
	hThread[15] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)TriggerSM201,			NULL,0,&dwThreadID[15]);	
	hThread[16] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)TriggerSM202,			NULL,0,&dwThreadID[16]);	
	hThread[17]	= CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)TriggerSM203,			NULL,0,&dwThreadID[17]);	
//	hThread[18]	= CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)TriggerSM204,			NULL,0,&dwThreadID[18]);
	hThread[19] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)home_motors_thread,	NULL,0,&dwThreadID[19]);

	return 0;
}

class MotionDlg:public ai_win::Dlg
{
public:
	virtual BOOL init(HWND hdlg, HWND hfocus, LPARAM linit);
	virtual void command(HWND hdlg, int id, HWND hctrl, UINT code_notify);
	virtual void close(HWND hdlg);
	virtual void timer(HWND hdlg, unsigned int id);
	virtual LRESULT ctlcolor_dlg(HWND hdlg, HDC hdc, HWND hcolor, int mark/* =CTLCOLOR_DLG */);

	int read_shm(char *path, MOTION_SHM *shm);
	int save_shm(char *path, MOTION_SHM *shm);


private:
	char config_path[MAX_PATH];
	char status[MAX_PATH];
	HBRUSH dlg_brush, old_dlg_brush;

};

MotionDlg motion_dlg;

BOOL CALLBACK motion_aa_dlg_process(HWND hdlg, unsigned int message, 
	WPARAM wParam, LPARAM lParam)
{
	switch (message){
		HANDLE_DLGMSG(hdlg, WM_INITDIALOG,  motion_dlg.init);
		HANDLE_DLGMSG(hdlg, WM_COMMAND,		motion_dlg.command);
		HANDLE_DLGMSG(hdlg, WM_CLOSE,		motion_dlg.close);
		HANDLE_DLGMSG(hdlg, WM_TIMER,		motion_dlg.timer);
//		HANDLE_DLGMSG(hdlg, WM_CTLCOLORDLG, motion_dlg.ctlcolor_dlg);
	}


	return FALSE;
}



BOOL MotionDlg::init(HWND hdlg, HWND hfocus, LPARAM linit)
{
	log("***********************aa motion init.**********************");
	FS = s_aa::mysbw_sm;
	char motor_name[MOTOR_NUM][20] = {"Line X", "Line Y", "Line Z", "Tilt X", "Tilt Y", "Tilt Z"};
	int motor_id[MOTOR_NUM] = {IDC_MOTOR, IDC_MOTOR2, IDC_MOTOR3, IDC_MOTOR4, IDC_MOTOR5, IDC_MOTOR6};
	sprintf(config_path, "%s\\recipe\\%s\\sharemem.mem", s_aa::dir, s_aa::recipe_name);

	for (int i=0; i<MOTOR_NUM; i++){
		SetDlgItemText(hdlg, motor_id[i], motor_name[i]);
	}

	SET_CHECKER(hdlg, IDC_MOTOR, TRUE);
	SetDlgItemText(hdlg, IDC_MOTOR_SELECT, "Line X");
	select_motor = M_LINE_X;


	COMBO_ADDSTR(hdlg, IDC_SELECT_ROI, "Bottom"	);
	COMBO_ADDSTR(hdlg, IDC_SELECT_ROI, "Right"	);
	COMBO_ADDSTR(hdlg, IDC_SELECT_ROI, "Top"	);
	COMBO_ADDSTR(hdlg, IDC_SELECT_ROI, "Left"	);

	COMBO_ADDSTR(hdlg, IDC_TF1_BACK_OPTION, "Stay"		 );
	COMBO_ADDSTR(hdlg, IDC_TF1_BACK_OPTION, "Center Peak");
	COMBO_ADDSTR(hdlg, IDC_TF1_BACK_OPTION, "Start Pos"	 );
	COMBO_ADDSTR(hdlg, IDC_TF1_BACK_OPTION, "1st Peak"	 );

	int err = read_shm(config_path, &mshm);

	if (err != 0x00){
		log("open shm file error[%d]", err);
	}
	else{

		COMBO_SETCURSEL(hdlg, IDC_SELECT_ROI, AA_SELECT_ROI);
		FS.select_roi = AA_SELECT_ROI;
		SET_CHECKER(hdlg, IDC_MOVE_HOME_OFFSET, RUN_HOME_OFFSET);

		ai_win::set_dlg_item_int(hdlg, IDC_PLC_DB,		  PLC_DB_INDEX			);
		ai_win::set_dlg_item_int(hdlg, IDC_DB_START,	  PLC_DB_START			);
		ai_win::set_dlg_item_int(hdlg, IDC_DB_LENS,		  PLC_DB_LENS			);
		ai_win::set_dlg_item_int(hdlg, IDC_PLC_DB2,		  PLC_DB_INDEX_R	    );
		ai_win::set_dlg_item_int(hdlg, IDC_DB_START2,	  PLC_DB_START_R		);
		ai_win::set_dlg_item_int(hdlg, IDC_DB_LENS2,	  PLC_DB_LENS_R			);

		ai_win::set_dlg_item_int(hdlg, IDC_STEP_Z_DELAY,  AA_FS_DELAY			);
		ai_win::set_dlg_item_int(hdlg, IDC_SPEED_HOME,	  LINE_X_HOME_SPEED		);
		ai_win::set_dlg_item_int(hdlg, IDC_SPEED_JOG,	  LINE_X_JOG_SPEED		);
		ai_win::set_dlg_item_int(hdlg, IDC_SPEED_MOVE,	  LINE_X_MOVE_SPEED		);
		ai_win::set_dlg_item_int(hdlg, IDC_AD_HOME,		  LINE_X_HOME_ACCEL_TIME);
		ai_win::set_dlg_item_int(hdlg, IDC_AD_JOG,		  LINE_X_JOG_ACCEL_TIME	);
		ai_win::set_dlg_item_int(hdlg, IDC_AD_MOVE,		  LINE_X_MOVE_ACCEL_TIME);

		ai_win::set_dlg_item_float(hdlg, IDC_P1_X,		  SM201_POSITION_1,     3);
		ai_win::set_dlg_item_float(hdlg, IDC_P2_X,		  SM201_POSITION_2,     3);
		ai_win::set_dlg_item_float(hdlg, IDC_P3_X,		  SM201_POSITION_3,		3);
		ai_win::set_dlg_item_float(hdlg, IDC_P4_X,		  SM201_POSITION_4,		3);
		ai_win::set_dlg_item_float(hdlg, IDC_P5_X,		  SM201_POSITION_5,		3);
		ai_win::set_dlg_item_float(hdlg, IDC_P6_X,		  SM201_POSITION_6,		3);
		ai_win::set_dlg_item_float(hdlg, IDC_P7_X,		  SM201_POSITION_7,		3);

		ai_win::set_dlg_item_float(hdlg, IDC_P1_Y,		  SM202_POSITION_1,		3);
		ai_win::set_dlg_item_float(hdlg, IDC_P2_Y,		  SM202_POSITION_2,		3);
		ai_win::set_dlg_item_float(hdlg, IDC_P3_Y,		  SM202_POSITION_3,		3);
		ai_win::set_dlg_item_float(hdlg, IDC_P4_Y,		  SM202_POSITION_4,		3);
		ai_win::set_dlg_item_float(hdlg, IDC_P5_Y,		  SM202_POSITION_5,		3);
		ai_win::set_dlg_item_float(hdlg, IDC_P6_Y,		  SM202_POSITION_6,		3);
		ai_win::set_dlg_item_float(hdlg, IDC_P7_Y,		  SM202_POSITION_7,		3);

		ai_win::set_dlg_item_float(hdlg, IDC_P1_Z,		  SM203_POSITION_1,		3);
		ai_win::set_dlg_item_float(hdlg, IDC_P2_Z,		  SM203_POSITION_2,		3);
		ai_win::set_dlg_item_float(hdlg, IDC_P3_Z,		  SM203_POSITION_3,		3);
		ai_win::set_dlg_item_float(hdlg, IDC_P4_Z,		  SM203_POSITION_4,		3);
		ai_win::set_dlg_item_float(hdlg, IDC_P5_Z,		  SM203_POSITION_5,		3);
		ai_win::set_dlg_item_float(hdlg, IDC_P6_Z,		  SM203_POSITION_6,		3);
		ai_win::set_dlg_item_float(hdlg, IDC_P7_Z,		  SM203_POSITION_7,		3);

		ai_win::set_dlg_item_float(hdlg, IDC_TARGET_RELATED_DISTANCE, (double)(LINE_X_SHORT_MOVE_DISTANCE*1.0/LINE_X_RATIO), 4);
		ai_win::set_dlg_item_float(hdlg, IDC_HOME_OFFSET,			  (double)(LINE_X_HOME_OFFSET*1.0/LINE_X_RATIO),		 4);
		ai_win::set_dlg_item_float(hdlg, IDC_STEP_Z_DISTANCE,		  AA_FS_STEP_UNIT,										 4);
		ai_win::set_dlg_item_float(hdlg, IDC_FINAL_X_OFFSET,		  AA_FINAL_X_OFFSET,									 4);
		ai_win::set_dlg_item_float(hdlg, IDC_FINAL_Y_OFFSET,		  AA_FINAL_Y_OFFSET,									 4);
		ai_win::set_dlg_item_float(hdlg, IDC_FINAL_Z_OFFSET,		  AA_FINAL_Z_OFFSET,									 4);
		ai_win::set_dlg_item_float(hdlg, IDC_PEAK_DIFF_SPEC,		  AA_PEAK_DIFF_SPEC,									 1);
		ai_win::set_dlg_item_float(hdlg, IDC_FS_TILT_LIMIT,			  AA_FS_TILT_LIMIT,								         2);

		ai_win::set_dlg_item_int  (hdlg, IDC_STEP_CNT,				  AA_FS_MAX_CNT											  );
		ai_win::set_dlg_item_int  (hdlg, IDC_TARGET_CENTER_X,	      AA_TARGET_CENTER_X									  );
		ai_win::set_dlg_item_int  (hdlg, IDC_TARGET_CENTER_Y,	      AA_TARGET_CENTER_Y									  );
		ai_win::set_dlg_item_int  (hdlg, IDC_RETURN_PEAK_DELAY,		  AA_RETURN_PEAK_DELAY									  );
		ai_win::set_dlg_item_int  (hdlg, IDC_PEAK_MIN_GUIDE,	      AA_PEAK_MIN_GUIDE										  );
		ai_win::set_dlg_item_int  (hdlg, IDC_FS_BACK_OFFSET,		  AA_FS_BACK_OFFSET										  );
		

// 		ai_win::set_dlg_item_float(hdlg, IDC_LENS_STEP_UNIT,	      AA_LENS_TF_STEP_UNIT,									 4);
// 		ai_win::set_dlg_item_float(hdlg, IDC_LENS_LOAD_Z,		      AA_LENS_Z_LOADING_POS,								 4);
// 		ai_win::set_dlg_item_int  (hdlg, IDC_LENS_STEP_CNT,		      AA_LENS_TF_MAX_CNT									  );
// 		ai_win::set_dlg_item_int  (hdlg, IDC_LENS_STEP_Z_DELAY,       AA_LENS_TF_DELAY										  );
// 		ai_win::set_dlg_item_float(hdlg, IDC_LENS_LOAD_X,		      AA_LENS_X_LOADING_POS,								 4);
// 		ai_win::set_dlg_item_float(hdlg, IDC_LENS_LOAD_Y,		      AA_LENS_Y_LOADING_POS,								 4);
// 		ai_win::set_dlg_item_float(hdlg, IDC_LENS_Z_OFFSET,		      AA_LENS_Z_OFFSET,										 4);//save data
		ai_win::set_dlg_item_float(hdlg, IDC_LENS_EFL,			      AA_LENS_EFL,											 4);

		ai_win::set_dlg_item_float(hdlg, IDC_PIXEL_SIZE,			  PIXEL_SIZE,											 4);

		COMBO_SETCURSEL (hdlg, IDC_TF1_BACK_OPTION,			AA_TF1_BACK_OPTION			);
		SET_CHECKER		(hdlg, IDC_FS_AUTO_CALIBRATION,		AA_FS_AUTO_CALIBRATION		);
		SET_CHECKER		(hdlg, IDC_MIRROR_X,				IMG_MIRROR_X				);
		SET_CHECKER		(hdlg, IDC_MIRROR_Y,				IMG_MIRROR_Y				);
// 		SET_CHECKER		(hdlg, IDC_CHECK_TILT1,				CHOOSE_XY					);
// 		SET_CHECKER		(hdlg, IDC_CHECK_TILT2,				SYMBOL1						);
// 		SET_CHECKER		(hdlg, IDC_CHECK_TILT3,				SYMBOL2						);

		SetTimer(hdlg, TIMER_MOTOR_DLG_JOG, 100, NULL);

		DWORD dw;
		HANDLE hmonitor = CreateThread(NULL, 0, position_display_thread, NULL, 0, &dw);
		CloseHandle(hmonitor);

		HANDLE hbutton = CreateThread(NULL, 0, button_process_thread, NULL, 0, &dw);
		CloseHandle(hbutton);
	}

	motor_init_err = aa_motion_init();

	sprintf(status, "Init Motor %s[%d].", (motor_init_err != 0x00)?"Fail":"OK", motor_init_err);
	SetDlgItemText(hdlg, IDC_STATUS2, status);
	load_err_msg();


	return TRUE;
}

void MotionDlg::command(HWND hdlg, int id, HWND hctrl, UINT code_notify)
{
	float float_val = 0.0f;
	double double_val = 0.0;
	int int_val = 0;
	int err = 0;
	switch (id)
	{
	case IDC_MOVE_TO_CENTER:
		BUTTON_VALUE = 200;
		break;

	case IDC_FOCUS_SCAN:
		log("Start focus scan.");
		sfr_type = B_UV;
		BUTTON_VALUE = 201;
		break;

	case IDC_SEARCH_Z:
		log("Search Z.");
		BUTTON_VALUE = 202; 
		break;

	case IDC_CALI_TILT_ANGLE:
		log("Cali tilt angle.");
		BUTTON_VALUE = 203; 
		break;

	case IDC_FS_CHECK:
		BUTTON_VALUE = 204;
		break;

	case IDC_MOVE_OFFSET:
		log("AA Move offset.");
		BUTTON_VALUE = 205;
		break;

	case IDC_FULL_AA:
		log("aa.");
		BUTTON_VALUE = 206;
		break;

	case IDC_PICK_LENS:
		log("pick lens.");
		BUTTON_VALUE = 207;
		break;

	case IDC_LOAD_LENS:
		log("pick lens.");
		BUTTON_VALUE = 208;
		break;

	case IDC_UNLOAD:
		BUTTON_VALUE = 209;
		break;
	

	case IDC_CALC_TO_CENTER_DISTANCE:
		BUTTON_VALUE = 223;
		break;

	case IDC_HOME_ALL:
		BUTTON_VALUE = 221;
		break;

	case IDC_SAVE_CONFIG:
		{
			err = save_shm(config_path, &mshm);
			log("save shm file[%d]", err);
		}
		break;

	case IDC_MOTOR:
		if (GET_CHECKER(hdlg, id)){
			SetDlgItemText(hdlg, IDC_MOTOR_SELECT, "Line X");
			select_motor = M_LINE_X;
			ai_win::set_dlg_item_int(hdlg, IDC_SPEED_HOME,	LINE_X_HOME_SPEED	  );
			ai_win::set_dlg_item_int(hdlg, IDC_SPEED_JOG,	LINE_X_JOG_SPEED	  );
			ai_win::set_dlg_item_int(hdlg, IDC_SPEED_MOVE,	LINE_X_MOVE_SPEED	  );
			ai_win::set_dlg_item_int(hdlg, IDC_AD_HOME,		LINE_X_HOME_ACCEL_TIME);
			ai_win::set_dlg_item_int(hdlg, IDC_AD_JOG,		LINE_X_JOG_ACCEL_TIME );
			ai_win::set_dlg_item_int(hdlg, IDC_AD_MOVE,		LINE_X_MOVE_ACCEL_TIME);

			ai_win::set_dlg_item_float(hdlg, IDC_TARGET_RELATED_DISTANCE, (double)(LINE_X_SHORT_MOVE_DISTANCE*1.0/LINE_X_RATIO), 4);
			ai_win::set_dlg_item_float(hdlg, IDC_HOME_OFFSET,			  (double)(LINE_X_HOME_OFFSET*1.0/LINE_X_RATIO),		 4);
		}
		break;

	case IDC_MOTOR2:
		if (GET_CHECKER(hdlg, id)){
			SetDlgItemText(hdlg, IDC_MOTOR_SELECT, "Line Y");
			select_motor = M_LINE_Y;
			ai_win::set_dlg_item_int(hdlg, IDC_SPEED_HOME,	LINE_Y_HOME_SPEED	  );
			ai_win::set_dlg_item_int(hdlg, IDC_SPEED_JOG,	LINE_Y_JOG_SPEED	  );
			ai_win::set_dlg_item_int(hdlg, IDC_SPEED_MOVE,	LINE_Y_MOVE_SPEED	  );
			ai_win::set_dlg_item_int(hdlg, IDC_AD_HOME,		LINE_Y_HOME_ACCEL_TIME);
			ai_win::set_dlg_item_int(hdlg, IDC_AD_JOG,		LINE_Y_JOG_ACCEL_TIME );
			ai_win::set_dlg_item_int(hdlg, IDC_AD_MOVE,		LINE_Y_MOVE_ACCEL_TIME);

			ai_win::set_dlg_item_float(hdlg, IDC_TARGET_RELATED_DISTANCE, (double)(LINE_Y_SHORT_MOVE_DISTANCE*1.0/LINE_Y_RATIO), 4);
			ai_win::set_dlg_item_float(hdlg, IDC_HOME_OFFSET,			  (double)(LINE_Y_HOME_OFFSET*1.0/LINE_Y_RATIO),		 4);
		}
		break;

	case IDC_MOTOR3:
		if (GET_CHECKER(hdlg, id)){
			SetDlgItemText(hdlg, IDC_MOTOR_SELECT, "Line Z");
			select_motor = M_LINE_Z;
			ai_win::set_dlg_item_int(hdlg, IDC_SPEED_HOME,	LINE_Z_HOME_SPEED	  );
			ai_win::set_dlg_item_int(hdlg, IDC_SPEED_JOG,	LINE_Z_JOG_SPEED	  );
			ai_win::set_dlg_item_int(hdlg, IDC_SPEED_MOVE,	LINE_Z_MOVE_SPEED	  );
			ai_win::set_dlg_item_int(hdlg, IDC_AD_HOME,		LINE_Z_HOME_ACCEL_TIME);
			ai_win::set_dlg_item_int(hdlg, IDC_AD_JOG,		LINE_Z_JOG_ACCEL_TIME );
			ai_win::set_dlg_item_int(hdlg, IDC_AD_MOVE,		LINE_Z_MOVE_ACCEL_TIME);

			ai_win::set_dlg_item_float(hdlg, IDC_TARGET_RELATED_DISTANCE, (double)(LINE_Z_SHORT_MOVE_DISTANCE*1.0/LINE_Z_RATIO), 4);
			ai_win::set_dlg_item_float(hdlg, IDC_HOME_OFFSET,			  (double)(LINE_Z_HOME_OFFSET*1.0/LINE_Z_RATIO),		 4);
		}

		break;

	case IDC_MOTOR4:
		if (GET_CHECKER(hdlg, id)){
			SetDlgItemText(hdlg, IDC_MOTOR_SELECT, "Tilt X");
			select_motor = M_TILT_X;
			ai_win::set_dlg_item_int(hdlg, IDC_SPEED_HOME,	TILT_X_HOME_SPEED	  );
			ai_win::set_dlg_item_int(hdlg, IDC_SPEED_JOG,	TILT_X_JOG_SPEED	  );
			ai_win::set_dlg_item_int(hdlg, IDC_SPEED_MOVE,	TILT_X_MOVE_SPEED	  );
			ai_win::set_dlg_item_int(hdlg, IDC_AD_HOME,		TILT_X_HOME_ACCEL_TIME);
			ai_win::set_dlg_item_int(hdlg, IDC_AD_JOG,		TILT_X_JOG_ACCEL_TIME );
			ai_win::set_dlg_item_int(hdlg, IDC_AD_MOVE,		TILT_X_MOVE_ACCEL_TIME);

			ai_win::set_dlg_item_float(hdlg, IDC_TARGET_RELATED_DISTANCE, (double)(TILT_X_SHORT_MOVE_DISTANCE*1.0/TILT_X_RATIO), 4);
			ai_win::set_dlg_item_float(hdlg, IDC_HOME_OFFSET,			  (double)(TILT_X_HOME_OFFSET*1.0/TILT_X_RATIO),		 4);
		}
		break;

	case IDC_MOTOR5:
		if (GET_CHECKER(hdlg, id)){
			SetDlgItemText(hdlg, IDC_MOTOR_SELECT, "Tilt Y");
			select_motor = M_TILT_Y;
			ai_win::set_dlg_item_int(hdlg, IDC_SPEED_HOME,	TILT_Y_HOME_SPEED	  );
			ai_win::set_dlg_item_int(hdlg, IDC_SPEED_JOG,	TILT_Y_JOG_SPEED	  );
			ai_win::set_dlg_item_int(hdlg, IDC_SPEED_MOVE,	TILT_Y_MOVE_SPEED	  );
			ai_win::set_dlg_item_int(hdlg, IDC_AD_HOME,		TILT_Y_HOME_ACCEL_TIME);
			ai_win::set_dlg_item_int(hdlg, IDC_AD_JOG,		TILT_Y_JOG_ACCEL_TIME );
			ai_win::set_dlg_item_int(hdlg, IDC_AD_MOVE,		TILT_Y_MOVE_ACCEL_TIME);

			ai_win::set_dlg_item_float(hdlg, IDC_TARGET_RELATED_DISTANCE, (double)(TILT_Y_SHORT_MOVE_DISTANCE*1.0/TILT_Y_RATIO), 4);
			ai_win::set_dlg_item_float(hdlg, IDC_HOME_OFFSET,			  (double)(TILT_Y_HOME_OFFSET*1.0/TILT_Y_RATIO),		 4);
		}
		break;

	case IDC_MOTOR6:
		if (GET_CHECKER(hdlg, id)){
			SetDlgItemText(hdlg, IDC_MOTOR_SELECT, "Tilt Z");
			select_motor = M_TILT_Z;
			ai_win::set_dlg_item_int(hdlg, IDC_SPEED_HOME,	TILT_Z_HOME_SPEED	  );
			ai_win::set_dlg_item_int(hdlg, IDC_SPEED_JOG,	TILT_Z_JOG_SPEED	  );
			ai_win::set_dlg_item_int(hdlg, IDC_SPEED_MOVE,	TILT_Z_MOVE_SPEED	  );
			ai_win::set_dlg_item_int(hdlg, IDC_AD_HOME,		TILT_Z_HOME_ACCEL_TIME);
			ai_win::set_dlg_item_int(hdlg, IDC_AD_JOG,		TILT_Z_JOG_ACCEL_TIME );
			ai_win::set_dlg_item_int(hdlg, IDC_AD_MOVE,		TILT_Z_MOVE_ACCEL_TIME);

			ai_win::set_dlg_item_float(hdlg, IDC_TARGET_RELATED_DISTANCE, (double)(TILT_Z_SHORT_MOVE_DISTANCE*1.0/TILT_Z_RATIO), 4);
			ai_win::set_dlg_item_float(hdlg, IDC_HOME_OFFSET,			  (double)(TILT_Z_HOME_OFFSET*1.0/TILT_Z_RATIO),		 4);
		}
		break;

	case IDC_MOTOR_SERVO_ON:
		switch (select_motor){
		case M_LINE_X: BUTTON_VALUE = LINE_X_BUTTON_INDEX; break;
		case M_LINE_Y: BUTTON_VALUE = LINE_Y_BUTTON_INDEX; break;
		case M_LINE_Z: BUTTON_VALUE = LINE_Z_BUTTON_INDEX; break;
		case M_TILT_X: BUTTON_VALUE = TILT_X_BUTTON_INDEX; break;
		case M_TILT_Y: BUTTON_VALUE = TILT_Y_BUTTON_INDEX; break;
		case M_TILT_Z: BUTTON_VALUE = TILT_Z_BUTTON_INDEX; break;
		}
		break;

	case IDC_MOTOR_SERVO_OFF:
		switch (select_motor){
		case M_LINE_X: BUTTON_VALUE = LINE_X_BUTTON_INDEX+1; break;
		case M_LINE_Y: BUTTON_VALUE = LINE_Y_BUTTON_INDEX+1; break;
		case M_LINE_Z: BUTTON_VALUE = LINE_Z_BUTTON_INDEX+1; break;
		case M_TILT_X: BUTTON_VALUE = TILT_X_BUTTON_INDEX+1; break;
		case M_TILT_Y: BUTTON_VALUE = TILT_Y_BUTTON_INDEX+1; break;	
		case M_TILT_Z: BUTTON_VALUE = TILT_Z_BUTTON_INDEX+1; break;
		}
		break;

	case IDC_MOTOR_HOME:
		switch (select_motor){
		case M_LINE_X: BUTTON_VALUE = LINE_X_BUTTON_INDEX+3; motor_flag=2; break;
		case M_LINE_Y: BUTTON_VALUE = LINE_Y_BUTTON_INDEX+3; motor_flag=2; break;
		case M_LINE_Z: BUTTON_VALUE = LINE_Z_BUTTON_INDEX+3; motor_flag=2; break;
		case M_TILT_X: BUTTON_VALUE = TILT_X_BUTTON_INDEX+3; motor_flag=2; break;
		case M_TILT_Y: BUTTON_VALUE = TILT_Y_BUTTON_INDEX+3; motor_flag=2; break;	
		case M_TILT_Z: BUTTON_VALUE = TILT_Z_BUTTON_INDEX+3; motor_flag=2; break;
		}
		break;

	case IDC_MOTOR_STOP:
		switch (select_motor){
		case M_LINE_X: BUTTON_VALUE = LINE_X_BUTTON_INDEX+6; break;
		case M_LINE_Y: BUTTON_VALUE = LINE_Y_BUTTON_INDEX+6; break;
		case M_LINE_Z: BUTTON_VALUE = LINE_Z_BUTTON_INDEX+6; break;
		case M_TILT_X: BUTTON_VALUE = TILT_X_BUTTON_INDEX+6; break;
		case M_TILT_Y: BUTTON_VALUE = TILT_Y_BUTTON_INDEX+6; break;	
		case M_TILT_Z: BUTTON_VALUE = TILT_Z_BUTTON_INDEX+6; break;
		}
		break;

	case IDC_MOVE_JOG_PLUS:  break;
	case IDC_MOVE_JOG_MINUS: break;

	case IDC_MOVE_POS_PLUS:
		switch (select_motor){
		case M_LINE_X: BUTTON_VALUE = LINE_X_BUTTON_INDEX+7; motor_flag=2; break;
		case M_LINE_Y: BUTTON_VALUE = LINE_Y_BUTTON_INDEX+7; motor_flag=2; break;
		case M_LINE_Z: BUTTON_VALUE = LINE_Z_BUTTON_INDEX+7; motor_flag=2; break;
		case M_TILT_X: BUTTON_VALUE = TILT_X_BUTTON_INDEX+7; motor_flag=2; break;
		case M_TILT_Y: BUTTON_VALUE = TILT_Y_BUTTON_INDEX+7; motor_flag=2; break;
		case M_TILT_Z: BUTTON_VALUE = TILT_Z_BUTTON_INDEX+7; motor_flag=2; break;
		}
		break;

	case IDC_MOVE_POS_MINUS:
		switch (select_motor){
		case M_LINE_X: BUTTON_VALUE = LINE_X_BUTTON_INDEX+8; motor_flag=2; break;
		case M_LINE_Y: BUTTON_VALUE = LINE_Y_BUTTON_INDEX+8; motor_flag=2; break;
		case M_LINE_Z: BUTTON_VALUE = LINE_Z_BUTTON_INDEX+8; motor_flag=2; break;
		case M_TILT_X: BUTTON_VALUE = TILT_X_BUTTON_INDEX+8; motor_flag=2; break;
		case M_TILT_Y: BUTTON_VALUE = TILT_Y_BUTTON_INDEX+8; motor_flag=2; break;
		case M_TILT_Z: BUTTON_VALUE = TILT_Z_BUTTON_INDEX+8; motor_flag=2; break;	
		}
		break;

	case IDC_HOME_OFFSET_SET:
		switch (select_motor){
		case M_LINE_X:
			LINE_X_HOME_OFFSET = LINE_X_POSITION;
			ai_win::set_dlg_item_float(hdlg, IDC_HOME_OFFSET, (double)(LINE_X_HOME_OFFSET*1.0/LINE_X_RATIO), 4);
			break;

		case M_LINE_Y:
			LINE_Y_HOME_OFFSET = LINE_Y_POSITION;
			ai_win::set_dlg_item_float(hdlg, IDC_HOME_OFFSET, (double)(LINE_Y_HOME_OFFSET*1.0/LINE_Y_RATIO), 4);
			break;

		case M_LINE_Z:
			LINE_Z_HOME_OFFSET = LINE_Z_POSITION;
			ai_win::set_dlg_item_float(hdlg, IDC_HOME_OFFSET, (double)(LINE_Z_HOME_OFFSET*1.0/LINE_Z_RATIO), 4);
			break;

		case M_TILT_X:
			TILT_X_HOME_OFFSET = TILT_X_POSITION;
			ai_win::set_dlg_item_float(hdlg, IDC_HOME_OFFSET, (double)(TILT_X_HOME_OFFSET*1.0/TILT_X_RATIO), 4);
			break;

		case M_TILT_Y:
			TILT_Y_HOME_OFFSET = TILT_Y_POSITION;
			ai_win::set_dlg_item_float(hdlg, IDC_HOME_OFFSET, (double)(TILT_Y_HOME_OFFSET*1.0/TILT_Y_RATIO), 4);
			break;
			
		case M_TILT_Z:
			TILT_Z_HOME_OFFSET = TILT_Z_POSITION;
			ai_win::set_dlg_item_float(hdlg, IDC_HOME_OFFSET, (double)(TILT_Z_HOME_OFFSET*1.0/TILT_Z_RATIO), 4);
			break;
		}
		break;

	case IDC_MOVE_TO_HOME_OFFSET:
		switch (select_motor){
		case M_LINE_X: BUTTON_VALUE = 210; break;
		case M_LINE_Y: BUTTON_VALUE = 211; break;
		case M_LINE_Z: BUTTON_VALUE = 212; break;
		case M_TILT_X: BUTTON_VALUE = 213; break;
		case M_TILT_Y: BUTTON_VALUE = 214; break;
		case M_TILT_Z: BUTTON_VALUE = 215; break;
		}
		break;

	case IDC_MOVE_LOAD_POSITION1:
		log("recv move to p1");
		BUTTON_VALUE = 310; break;
		
	case IDC_MOVE_LOAD_POSITION2:
		log("recv move to p2");
		BUTTON_VALUE = 311; break;

	case IDC_MOVE_LOAD_POSITION3:
		log("recv move to p3");
		BUTTON_VALUE = 312; break;

	case IDC_MOVE_LOAD_POSITION4:
		log("recv move to p4");
		BUTTON_VALUE = 313; break;

	case IDC_MOVE_LOAD_POSITION5:
		log("recv move to p5");
		BUTTON_VALUE = 314; break;

	case IDC_MOVE_LOAD_POSITION6:
		log("recv move to p6");
		BUTTON_VALUE = 315; break;

	case IDC_TARGET_RELATED_DISTANCE:
		double_val = ai_win::get_dlg_item_float(hdlg, id);

		switch (select_motor){
		case M_LINE_X: LINE_X_SHORT_MOVE_DISTANCE = (int)(double_val*LINE_X_RATIO+0.5); break;
		case M_LINE_Y: LINE_Y_SHORT_MOVE_DISTANCE = (int)(double_val*LINE_Y_RATIO+0.5); break;
		case M_LINE_Z: LINE_Z_SHORT_MOVE_DISTANCE = (int)(double_val*LINE_Z_RATIO+0.5); break;
		case M_TILT_X: TILT_X_SHORT_MOVE_DISTANCE = (int)(double_val*TILT_X_RATIO+0.5); break;
		case M_TILT_Y: TILT_Y_SHORT_MOVE_DISTANCE = (int)(double_val*TILT_Y_RATIO+0.5); break;
		case M_TILT_Z: TILT_Z_SHORT_MOVE_DISTANCE = (int)(double_val*TILT_Z_RATIO+0.5); break;
		}
		break;

	case IDC_CLEAR_MSG:
		SetDlgItemText(hdlg, IDC_STATUS2, "");
		break;

	case IDC_STOP_ALL:
		HomeMachine_StopFlag = TRUE;
		Sleep(1000);
		SM101_motor_stop();
		SM102_motor_stop();
		SM103_motor_stop();
		SM201_motor_stop();
		SM202_motor_stop();
		SM203_motor_stop();
		EMERGENCY_STOP_FLAG = TRUE;

		SetDlgItemText(HWND_AA_MOTION, IDC_STATUS2, "Stop All Motors");
		ITEM_ENABLED(HWND_AA_MOTION, IDC_HOME_ALL);
		break;

	case IDC_RESET:
		EMERGENCY_STOP_FLAG = FALSE;
		break;

	case IDC_MOVE_HOME_OFFSET:
		RUN_HOME_OFFSET = GET_CHECKER(hdlg, id);
		break;	//////////////////////////////////////////////////////////////////////////

	case IDC_TF1_BACK_OPTION:
		AA_TF1_BACK_OPTION = COMBO_GETCURSEL(hdlg, id);
		break;

	case IDC_SELECT_ROI:
		AA_SELECT_ROI = COMBO_GETCURSEL(hdlg, id);
		FS.select_roi = AA_SELECT_ROI;
		break;

// 	case IDC_MOTION_X_ANGLE:
// 		AA_MOTION_X_ANGLE = ai_win::get_dlg_item_float(hdlg, id);
// 		break;
// 
// 	case IDC_MOTION_Y_ANGLE:
// 		AA_MOTION_Y_ANGLE = ai_win::get_dlg_item_float(hdlg, id);
// 		break;

	case IDC_PIXEL_SIZE:
		PIXEL_SIZE = ai_win::get_dlg_item_float(hdlg, id);
		break;

	case IDC_TARGET_CENTER_X:
		AA_TARGET_CENTER_X = ai_win::get_dlg_item_int(hdlg, id);
		break;

	case IDC_TARGET_CENTER_Y:
		AA_TARGET_CENTER_Y = ai_win::get_dlg_item_int(hdlg, id);
		break;

	case IDC_STEP_Z_DELAY:
		{
			int mydelay = GetDlgItemInt(hdlg, id, NULL, FALSE);
			if (mydelay<0) mydelay = 5;
			AA_FS_DELAY = mydelay;
			log("AA_FS_DELAY=%d", AA_FS_DELAY);
		}
		break;

// 	case IDC_STEP2_Z_DELAY:
// 		{
// 			int mydelay = GetDlgItemInt(hdlg, id, NULL, FALSE);
// 			if (mydelay<0) mydelay = 5;
// 			AA_TF2_DELAY = mydelay;
// 			log("aa_tf2_delay=%d", AA_TF2_DELAY);
// 		}
// 		break;

// 	case IDC_USE_TF2: AA_USE_TF2 = GET_CHECKER(hdlg, id); 	break;
// 	case IDC_USE_TF3: AA_USE_TF3 = GET_CHECKER(hdlg, id);	break;
	case IDC_CHOOSE_XY:	CHOOSE_XY    = GET_CHECKER(hdlg,id);    break;
	case IDC_SYMBOL1:	SYMBOL1	     = GET_CHECKER(hdlg,id);    break;
	case IDC_SYMBOL2:	SYMBOL2      = GET_CHECKER(hdlg,id);    break; 
	case IDC_MIRROR_X:  IMG_MIRROR_X = GET_CHECKER(hdlg,id);	break;
	case IDC_MIRROR_Y:  IMG_MIRROR_Y = GET_CHECKER(hdlg,id);    break;
	case IDC_FS_AUTO_CALIBRATION: AA_FS_AUTO_CALIBRATION = GET_CHECKER(hdlg, id); break;
//	case IDC_TF2_AUTO_CALIBRATION: AA_TF2_AUTO_CALIBRATION = GET_CHECKER(hdlg, id);	break;

// 	case IDC_USB_CAM_INIT:
// 		{
// 			int result = s_aa::usb_cam.init(WEB_CAM_NUM);
// 			if (result){
// 				result = s_aa::usb_cam.run();
// 			}
// 			log("init usb cam result:%d", result);
// 		}
// 		break;
// 
// 	case IDC_USB_CAM_CAP:
// 		WEB_CAM_TF_TRIGGER_ON = TRUE;
// 		break;
// 
// 	case IDC_USB_CAM_STOP:
// 		{
// 			int result = s_aa::usb_cam.stop();
// 			result = s_aa::usb_cam.destroy();
// 
// 		}
// 		break;
	}

	switch (code_notify){
	case BN_CLICKED:
		switch (id){
		case IDC_MOVE_JOG_PLUS:
		case IDC_MOVE_JOG_MINUS:
			switch (select_motor){
			case M_LINE_X: BUTTON_VALUE = LINE_X_BUTTON_INDEX+6; break;
			case M_LINE_Y: BUTTON_VALUE = LINE_Y_BUTTON_INDEX+6; break;
			case M_LINE_Z: BUTTON_VALUE = LINE_Z_BUTTON_INDEX+6; break;
			case M_TILT_X: BUTTON_VALUE = TILT_X_BUTTON_INDEX+6; break;
			case M_TILT_Y: BUTTON_VALUE = TILT_Y_BUTTON_INDEX+6; break;
			case M_TILT_Z: BUTTON_VALUE = TILT_Z_BUTTON_INDEX+6; break;
			}
			break;
		}
		break;

	case EN_CHANGE:
		switch (id){
		case IDC_SPEED_HOME:
			int_val = GetDlgItemInt(hdlg, id, NULL, FALSE);
			switch (select_motor){
			case M_LINE_X: LINE_X_HOME_SPEED = int_val; break;
			case M_LINE_Y: LINE_Y_HOME_SPEED = int_val; break;
			case M_LINE_Z: LINE_Z_HOME_SPEED = int_val; break;
			case M_TILT_X: TILT_X_HOME_SPEED = int_val; break;
			case M_TILT_Y: TILT_Y_HOME_SPEED = int_val; break;	
			case M_TILT_Z: TILT_Z_HOME_SPEED = int_val; break;
			}
			break;

		case IDC_SPEED_JOG:
			int_val = GetDlgItemInt(hdlg, id, NULL, FALSE);
			switch (select_motor){
			case M_LINE_X: LINE_X_JOG_SPEED = int_val; break;
			case M_LINE_Y: LINE_Y_JOG_SPEED = int_val; break;
			case M_LINE_Z: LINE_Z_JOG_SPEED = int_val; break;
			case M_TILT_X: TILT_X_JOG_SPEED = int_val; break;
			case M_TILT_Y: TILT_Y_JOG_SPEED = int_val; break;
			case M_TILT_Z: TILT_Z_JOG_SPEED = int_val; break;
			}
			break;

		case IDC_SPEED_MOVE:
			int_val = GetDlgItemInt(hdlg, id, NULL, FALSE);
			switch (select_motor){
			case M_LINE_X: LINE_X_MOVE_SPEED = int_val; break;
			case M_LINE_Y: LINE_Y_MOVE_SPEED = int_val; break;
			case M_LINE_Z: LINE_Z_MOVE_SPEED = int_val; break;
			case M_TILT_X: TILT_X_MOVE_SPEED = int_val; break;
			case M_TILT_Y: TILT_Y_MOVE_SPEED = int_val; break;
			case M_TILT_Z: TILT_Z_MOVE_SPEED = int_val; break;
			}
			break;

		case IDC_AD_HOME:
			int_val = GetDlgItemInt(hdlg, id, NULL, FALSE);
			switch (select_motor){
			case M_LINE_X: LINE_X_HOME_ACCEL_TIME = int_val; break;
			case M_LINE_Y: LINE_Y_HOME_ACCEL_TIME = int_val; break;
			case M_LINE_Z: LINE_Z_HOME_ACCEL_TIME = int_val; break;
			case M_TILT_X: TILT_X_HOME_ACCEL_TIME = int_val; break;
			case M_TILT_Y: TILT_Y_HOME_ACCEL_TIME = int_val; break;	
			case M_TILT_Z: TILT_Z_HOME_ACCEL_TIME = int_val; break;
			}
			break;

		case IDC_AD_JOG:
			int_val = GetDlgItemInt(hdlg, id, NULL, FALSE);
			switch (select_motor){
			case M_LINE_X: LINE_X_JOG_ACCEL_TIME = int_val; break;
			case M_LINE_Y: LINE_Y_JOG_ACCEL_TIME = int_val; break;
			case M_LINE_Z: LINE_Z_JOG_ACCEL_TIME = int_val; break;
			case M_TILT_X: TILT_X_JOG_ACCEL_TIME = int_val; break;
			case M_TILT_Y: TILT_Y_JOG_ACCEL_TIME = int_val; break;	
			case M_TILT_Z: TILT_Z_JOG_ACCEL_TIME = int_val; break;
			}
			break;

		case IDC_AD_MOVE:
			int_val = GetDlgItemInt(hdlg, id, NULL, FALSE);
			switch (select_motor){
			case M_LINE_X: LINE_X_MOVE_ACCEL_TIME = int_val; break;
			case M_LINE_Y: LINE_Y_MOVE_ACCEL_TIME = int_val; break;
			case M_LINE_Z: LINE_Z_MOVE_ACCEL_TIME = int_val; break;
			case M_TILT_X: TILT_X_MOVE_ACCEL_TIME = int_val; break;
			case M_TILT_Y: TILT_Y_MOVE_ACCEL_TIME = int_val; break;	
			case M_TILT_Z: TILT_Z_MOVE_ACCEL_TIME = int_val; break;
			}
			break;
		case IDC_STEP_CNT:				AA_FS_MAX_CNT		   = ai_win::get_dlg_item_int(hdlg, id);	break;
		case IDC_FS_BACK_OFFSET:		AA_FS_BACK_OFFSET		= ai_win::get_dlg_item_int(hdlg, id);	break;
		case IDC_RETURN_PEAK_DELAY:		AA_RETURN_PEAK_DELAY   = ai_win::get_dlg_item_int(hdlg, id);	break;
		case IDC_PEAK_MIN_GUIDE:		AA_PEAK_MIN_GUIDE	   = ai_win::get_dlg_item_int(hdlg, id);	break;

		case IDC_STEP_Z_DISTANCE:		AA_FS_STEP_UNIT	   = ai_win::get_dlg_item_float(hdlg, id);	break;
		case IDC_PEAK_DIFF_SPEC:		AA_PEAK_DIFF_SPEC	   = ai_win::get_dlg_item_float(hdlg, id);	break;
		case IDC_FINAL_X_OFFSET:		AA_FINAL_X_OFFSET	   = ai_win::get_dlg_item_float(hdlg, id);	break;
		case IDC_FINAL_Z_OFFSET:		AA_FINAL_Z_OFFSET	   = ai_win::get_dlg_item_float(hdlg, id);	break;
		case IDC_FINAL_Y_OFFSET:		AA_FINAL_Y_OFFSET	   = ai_win::get_dlg_item_float(hdlg, id);	break;
		case IDC_FS_TILT_LIMIT:			AA_FS_TILT_LIMIT	   = ai_win::get_dlg_item_float(hdlg, id);	break;			

		case IDC_LENS_EFL:				AA_LENS_EFL			   = ai_win::get_dlg_item_float(hdlg, id);  break;
		case IDC_PLC_DB:				PLC_DB_INDEX		   = ai_win::get_dlg_item_int(hdlg, id);	break;
		case IDC_DB_START:				PLC_DB_START		   = ai_win::get_dlg_item_int(hdlg, id);	break;
		case IDC_DB_LENS:				PLC_DB_LENS			   = ai_win::get_dlg_item_int(hdlg, id);	break;
		case IDC_PLC_DB2:				PLC_DB_INDEX_R		   = ai_win::get_dlg_item_int(hdlg, id);	break;
		case IDC_DB_START2:				PLC_DB_START_R		   = ai_win::get_dlg_item_int(hdlg, id);	break;
		case IDC_DB_LENS2:				PLC_DB_LENS_R		   = ai_win::get_dlg_item_int(hdlg, id);	break;

		case IDC_P1_X:					SM201_POSITION_1	   = ai_win::get_dlg_item_float(hdlg,id);	break;			
		case IDC_P2_X:					SM201_POSITION_2	   = ai_win::get_dlg_item_float(hdlg,id);	break;
		case IDC_P3_X:					SM201_POSITION_3	   = ai_win::get_dlg_item_float(hdlg,id);	break;
		case IDC_P4_X:					SM201_POSITION_4	   = ai_win::get_dlg_item_float(hdlg,id);	break;
		case IDC_P5_X:					SM201_POSITION_5	   = ai_win::get_dlg_item_float(hdlg,id);	break;
		case IDC_P6_X:					SM201_POSITION_6	   = ai_win::get_dlg_item_float(hdlg,id);	break;

		case IDC_P1_Y:					SM202_POSITION_1	   = ai_win::get_dlg_item_float(hdlg,id);	break;	
		case IDC_P2_Y:					SM202_POSITION_2	   = ai_win::get_dlg_item_float(hdlg,id);	break;
		case IDC_P3_Y:					SM202_POSITION_3	   = ai_win::get_dlg_item_float(hdlg,id);	break;	
		case IDC_P4_Y:					SM202_POSITION_4	   = ai_win::get_dlg_item_float(hdlg,id);	break;
		case IDC_P5_Y:					SM202_POSITION_5	   = ai_win::get_dlg_item_float(hdlg,id);	break;
		case IDC_P6_Y:					SM202_POSITION_6	   = ai_win::get_dlg_item_float(hdlg,id);	break;

		case IDC_P1_Z:					SM203_POSITION_1	   = ai_win::get_dlg_item_float(hdlg,id);	break;
		case IDC_P2_Z:					SM203_POSITION_2	   = ai_win::get_dlg_item_float(hdlg,id);	break;
		case IDC_P3_Z:					SM203_POSITION_3	   = ai_win::get_dlg_item_float(hdlg,id);	break;	
		case IDC_P4_Z:					SM203_POSITION_4	   = ai_win::get_dlg_item_float(hdlg,id);	break;
		case IDC_P5_Z:					SM203_POSITION_5	   = ai_win::get_dlg_item_float(hdlg,id);	break;
		case IDC_P6_Z:					SM203_POSITION_6	   = ai_win::get_dlg_item_float(hdlg,id);	break;
			//	case IDC_P1_X:					SM201_POSITION_1	   = ai_win::get_dlg_item_float(hdlg,id);	break;

		}

		break;
	}
}

void MotionDlg::timer(HWND hdlg, unsigned int id)
{
	int err = 0;
	switch (id){
	case TIMER_MOTOR_DLG_JOG:
		{
			int jog_up_button   = ::SendMessage(GetDlgItem(hdlg, IDC_MOVE_JOG_PLUS),  BM_GETSTATE, 0, 0);
			int jog_down_button = ::SendMessage(GetDlgItem(hdlg, IDC_MOVE_JOG_MINUS), BM_GETSTATE, 0, 0);
			// 			log("jog_up_button=%x", jog_up_button);
			// 			BST_CHECKED;

			if (jog_up_button == 0x26c){
				switch (select_motor){
				case M_LINE_X:	BUTTON_VALUE = LINE_X_BUTTON_INDEX+4; break;
				case M_LINE_Y:	BUTTON_VALUE = LINE_Y_BUTTON_INDEX+4; break;
				case M_LINE_Z:	BUTTON_VALUE = LINE_Z_BUTTON_INDEX+4; break;
				case M_TILT_X:	BUTTON_VALUE = TILT_X_BUTTON_INDEX+4; break;
				case M_TILT_Y:	BUTTON_VALUE = TILT_Y_BUTTON_INDEX+4; break;
				case M_TILT_Z:  BUTTON_VALUE = TILT_Z_BUTTON_INDEX+4; break;
				}

				log("jog cw active!");
			}

			if (jog_down_button == 0x26c ){
				switch (select_motor){
				case M_LINE_X:	BUTTON_VALUE = LINE_X_BUTTON_INDEX+5; break;
				case M_LINE_Y:	BUTTON_VALUE = LINE_Y_BUTTON_INDEX+5; break;
				case M_LINE_Z:	BUTTON_VALUE = LINE_Z_BUTTON_INDEX+5; break;
				case M_TILT_X:	BUTTON_VALUE = TILT_X_BUTTON_INDEX+5; break;
				case M_TILT_Y:	BUTTON_VALUE = TILT_Y_BUTTON_INDEX+5; break;
				case M_TILT_Z:  BUTTON_VALUE = TILT_Z_BUTTON_INDEX+5; break;
				}
				log("jog ccw active!");
			}
		}
		break;
	}
}

void MotionDlg::close(HWND hdlg)
{
	EndDialog(hdlg, FALSE);
}

LRESULT MotionDlg::ctlcolor_dlg(HWND hdlg, HDC hdc, HWND hcolor, int mark)
{
	DeleteObject(dlg_brush);
	SetBkMode(hdc, TRANSPARENT);
	dlg_brush = CreateSolidBrush(RGB(0x66, 0x66, 0x66));
	old_dlg_brush = (HBRUSH)SelectObject(hdc, (HBRUSH)dlg_brush);

	return (LRESULT)dlg_brush;
}

int MotionDlg::read_shm(char *path, MOTION_SHM *shm)
{
	FILE *file_ptr;
	int counter = 0x00;
	char buf1[40];
	int i;
	float float_val;

	if ((file_ptr=fopen(path, "rt")) == NULL){
		log("read shm file fail.");
		return 0x01;
	}


	//=========================================================================================
	// discard the first line which is title for this data ====================================
	if(fgets(buf1,sizeof(buf1)-1,file_ptr)==NULL){ fclose(file_ptr); return(0x02);}

	// read SERVO MOTOR parameters 
	for(i=0;i<MOTORS_PARA_NUM;i++)
	{
		fscanf(file_ptr,"%d",&(shm->SERVO_MOTOR_PARAMETERS[i]));     
		counter++;
	}
	if(fgets(buf1,sizeof(buf1)-1,file_ptr)==NULL){ fclose(file_ptr); return(0x02);}   // report file reading error

	//=========================================================================================
	// discard the first line which is title for this data ====================================
	if(fgets(buf1,sizeof(buf1)-1,file_ptr)==NULL){ fclose(file_ptr); return(0x05);}
	// read general RAM parameter 
	for(i=0;i<INT_RAM_NUM;i++){
		fscanf(file_ptr,"%d",&(shm->RAM_INT[i]));  
		counter++;
	}
	if(fgets(buf1,sizeof(buf1)-1,file_ptr)==NULL){ fclose(file_ptr); return(0x05);}   // report file reading error

	//=========================================================================================
	// discard the first line which is title for this data ====================================
	if(fgets(buf1,sizeof(buf1)-1,file_ptr)==NULL){ fclose(file_ptr); return(0x05);}
	// read general RAM parameter 
	for(i=0;i<DOUBLE_RAM_NUM;i++){
		fscanf(file_ptr,"%f",&float_val); 
		shm->RAM_DOUBLE[i] = (double)float_val;
		counter++;
	}
	if(fgets(buf1,sizeof(buf1)-1,file_ptr)==NULL){ fclose(file_ptr); return(0x05);}   // report file reading error

	//=========================================================================================
	// discard the first line which is title for this data ====================================
	if(fgets(buf1,sizeof(buf1)-1,file_ptr)==NULL){ fclose(file_ptr); return(0x04);}
	// read Timer 
	for(i=0;i<TIMER_RAM_NUM;i++){
		fscanf(file_ptr,"%d",&(shm->TIMER[i])); 
		counter++;
	}
	if(fgets(buf1,sizeof(buf1)-1,file_ptr)==NULL){ fclose(file_ptr); return(0x04);}   // report file reading error

	fclose(file_ptr);

	return(0x00);
}

int MotionDlg::save_shm(char *path, MOTION_SHM *shm)
{
	FILE *file_ptr;
	int i, counter;

	if (strlen(path)<=0){
		//		MessageBox(NULL, "Model name not exist.", "Error", 0);
		return -1;
	}

	counter=0x00;
	if((file_ptr=fopen(path,"wt"))==NULL){ 
		//		MessageBox(NULL, "Open Sharing Memory File Error.", "Error", 0);
		return -2; 
	}

	fprintf(file_ptr,"SERVO MOTOR PARAMETRS:%s","\x0D\x0A\x00");   
	for(i=0;i<MOTORS_PARA_NUM;i++){
		fprintf(file_ptr," %d",shm->SERVO_MOTOR_PARAMETERS[i]);  // save servo motor parameters 
		counter++;
	}
	fprintf(file_ptr,"%s","\x0D\x0A\x00"); 

	fprintf(file_ptr,"INT RAM:%s","\x0D\x0A\x00");   
	for(i=0;i<INT_RAM_NUM;i++){
		fprintf(file_ptr," %d",shm->RAM_INT[i]);  // save servo motor parameters 
		counter++;
	}
	fprintf(file_ptr,"%s","\x0D\x0A\x00"); 

	fprintf(file_ptr,"DOUBLE RAM:%s","\x0D\x0A\x00");   
	for(i=0;i<DOUBLE_RAM_NUM;i++){
		fprintf(file_ptr," %f",shm->RAM_DOUBLE[i]);  // save servo motor parameters 
		counter++;
	}
	fprintf(file_ptr,"%s","\x0D\x0A\x00"); 

	fprintf(file_ptr,"TIMER:%s","\x0D\x0A\x00");   
	for(i=0;i<TIMER_RAM_NUM;i++){
		fprintf(file_ptr," %d",shm->TIMER[i]);  // save servo motor parameters 
		counter++;
	}
	fprintf(file_ptr,"%s","\x0D\x0A\x00"); 

	fclose(file_ptr);

	return counter;
}

DWORD WINAPI position_display_thread(LPVOID temp)
{
	double motor_pos[MOTOR_NUM] = {0.0};

	while (1){
		motor_pos[0] = ((double)(LINE_X_POSITION * 1.0)/(LINE_X_RATIO	));
		motor_pos[1] = ((double)(LINE_Y_POSITION * 1.0)/(LINE_Y_RATIO	));
		motor_pos[2] = ((double)(LINE_Z_POSITION * 1.0)/(LINE_Z_RATIO	));

		motor_pos[3] = ((double)(TILT_X_POSITION * 1.0)/(TILT_X_RATIO	));
	//	log("tilt_x position:%f",motor_pos[3]);
		motor_pos[4] = ((double)(TILT_Y_POSITION * 1.0)/(TILT_Y_RATIO	));
		motor_pos[5] = ((double)(TILT_Z_POSITION * 1.0)/(TILT_Z_RATIO	));

		switch (select_motor){
		case M_LINE_X:	
			ai_win::set_dlg_item_float(HWND_AA_MOTION, IDC_SIGNAL_POSITION, motor_pos[0], 4); 
			ai_win::set_dlg_item_int(HWND_AA_MOTION, IDC_SIGNAL_POSITION2, LINE_X_POSITION);
				//SM201 STATUS DISPLAY ============================================================

			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_HOME,			(((LINE_X_STATUS & 0x01)==0x01)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_LIMIT_PLUS,	(((LINE_X_STATUS & 0x02)==0x02)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_LIMIT_MINUS,	(((LINE_X_STATUS & 0x04)==0x04)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_ALARM,		(((LINE_X_STATUS & 0x08)==0x08)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_SERVO_ON,		(((LINE_X_STATUS & 0x10)==0x10)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_INPOS,		(((LINE_X_STATUS & 0x20)==0x20)?TRUE:FALSE));
			break;

		case M_LINE_Y:	
			ai_win::set_dlg_item_float(HWND_AA_MOTION, IDC_SIGNAL_POSITION, motor_pos[1], 4); 
			ai_win::set_dlg_item_int(HWND_AA_MOTION, IDC_SIGNAL_POSITION2, LINE_Y_POSITION);

			//===== SM203 STATUS DISPLAY ============================================================

			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_HOME,			(((LINE_Y_STATUS & 0x01)==0x01)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_LIMIT_PLUS,	(((LINE_Y_STATUS & 0x02)==0x02)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_LIMIT_MINUS,	(((LINE_Y_STATUS & 0x04)==0x04)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_ALARM,		(((LINE_Y_STATUS & 0x08)==0x08)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_SERVO_ON,		(((LINE_Y_STATUS & 0x10)==0x10)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_INPOS,		(((LINE_Y_STATUS & 0x20)==0x20)?TRUE:FALSE));
			break;

		case M_LINE_Z:	
			ai_win::set_dlg_item_float(HWND_AA_MOTION, IDC_SIGNAL_POSITION, motor_pos[2], 4); 
			ai_win::set_dlg_item_int(HWND_AA_MOTION, IDC_SIGNAL_POSITION2, LINE_Z_POSITION);
			//===== SM102 STATUS DISPLAY ============================================================
	//		log("tilt_x position:%f",motor_pos[2]);
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_HOME,			(((LINE_Z_STATUS & 0x01)==0x01)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_LIMIT_PLUS,	(((LINE_Z_STATUS & 0x02)==0x02)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_LIMIT_MINUS,	(((LINE_Z_STATUS & 0x04)==0x04)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_ALARM,		(((LINE_Z_STATUS & 0x08)==0x08)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_SERVO_ON,		(((LINE_Z_STATUS & 0x10)==0x10)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_INPOS,		(((LINE_Z_STATUS & 0x20)==0x20)?TRUE:FALSE));

			break;

		case M_TILT_X:	
			ai_win::set_dlg_item_float(HWND_AA_MOTION, IDC_SIGNAL_POSITION, motor_pos[3], 4); 
			ai_win::set_dlg_item_int(HWND_AA_MOTION, IDC_SIGNAL_POSITION2, TILT_X_POSITION);
			mysbw.tilt_x_pos = TILT_X_POSITION;
			//===== SM101 STATUS DISPLAY ============================================================
		//	log("tilt_y position:%f",motor_pos[3]);
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_HOME,			(((TILT_X_STATUS & 0x01)==0x01)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_LIMIT_PLUS,	(((TILT_X_STATUS & 0x02)==0x02)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_LIMIT_MINUS,	(((TILT_X_STATUS & 0x04)==0x04)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_ALARM,		(((TILT_X_STATUS & 0x08)==0x08)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_SERVO_ON,		(((TILT_X_STATUS & 0x10)==0x10)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_INPOS,		(((TILT_X_STATUS & 0x20)==0x20)?TRUE:FALSE));
			break;

		case M_TILT_Y:	
			ai_win::set_dlg_item_float(HWND_AA_MOTION, IDC_SIGNAL_POSITION, motor_pos[4], 4); 
			ai_win::set_dlg_item_int(HWND_AA_MOTION, IDC_SIGNAL_POSITION2, TILT_Y_POSITION);
			mysbw.tilt_y_pos = TILT_Y_POSITION;
			//===== SM104 STATUS DISPLAY ============================================================

			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_HOME,			(((TILT_Y_STATUS & 0x01)==0x01)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_LIMIT_PLUS,	(((TILT_Y_STATUS & 0x02)==0x02)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_LIMIT_MINUS,	(((TILT_Y_STATUS & 0x04)==0x04)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_ALARM,		(((TILT_Y_STATUS & 0x08)==0x08)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_SERVO_ON,		(((TILT_Y_STATUS & 0x10)==0x10)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_INPOS,		(((TILT_Y_STATUS & 0x20)==0x20)?TRUE:FALSE));
			break;

		case M_TILT_Z:	
			ai_win::set_dlg_item_float(HWND_AA_MOTION, IDC_SIGNAL_POSITION, motor_pos[5], 4); 
			ai_win::set_dlg_item_int(HWND_AA_MOTION, IDC_SIGNAL_POSITION2, TILT_Z_POSITION);
			//===== SM104 STATUS DISPLAY ============================================================

			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_HOME,			(((TILT_Z_STATUS & 0x01)==0x01)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_LIMIT_PLUS,	(((TILT_Z_STATUS & 0x02)==0x02)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_LIMIT_MINUS,	(((TILT_Z_STATUS & 0x04)==0x04)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_ALARM,		(((TILT_Z_STATUS & 0x08)==0x08)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_SERVO_ON,		(((TILT_Z_STATUS & 0x10)==0x10)?TRUE:FALSE));
			SET_CHECKER(HWND_AA_MOTION, IDC_CHECK_INPOS,		(((TILT_Z_STATUS & 0x20)==0x20)?TRUE:FALSE));
			break;
		}
		ai_win::set_dlg_item_float(HWND_MAIN, IDC_REPORT_CENTER_X,		 mysbw.oc_x[0], 2);
		ai_win::set_dlg_item_float(HWND_MAIN, IDC_REPORT_CENTER_Y,		 mysbw.oc_y[0], 2);
		ai_win::set_dlg_item_float(HWND_MAIN, IDC_REPORT_eCoorDiff,		 mysbw.eCoorDiff,  2);
		ai_win::set_dlg_item_float(HWND_MAIN, IDC_REPORT_eSfrDiff,		 mysbw.eSfrDiff,  2);
		ai_win::set_dlg_item_float(HWND_MAIN, IDC_REPORT_SFR_C,			 mysbw.sfr_val1[0][AA_SELECT_ROI], 2);
		ai_win::set_dlg_item_float(HWND_MAIN, IDC_REPORT_SFR_LT,		 mysbw.sfr_val1[1][AA_SELECT_ROI], 2);
		ai_win::set_dlg_item_float(HWND_MAIN, IDC_REPORT_SFR_RT,		 mysbw.sfr_val1[2][AA_SELECT_ROI], 2);
		ai_win::set_dlg_item_float(HWND_MAIN, IDC_REPORT_SFR_LB,		 mysbw.sfr_val1[3][AA_SELECT_ROI], 2);
		ai_win::set_dlg_item_float(HWND_MAIN, IDC_REPORT_SFR_RB,		 mysbw.sfr_val1[4][AA_SELECT_ROI], 2);
		
		Sleep(100);
	}

	return 0;
}

void motor_button_waiting(int wait_time)
{
	Sleep(50);
	ITEM_GRAYED(HWND_AA_MOTION, IDC_MOTOR_SERVO_ON);
	ITEM_GRAYED(HWND_AA_MOTION, IDC_MOTOR_SERVO_OFF);
	ITEM_GRAYED(HWND_AA_MOTION, IDC_MOTOR_HOME);
	ITEM_GRAYED(HWND_AA_MOTION, IDC_MOTOR_RESET);
	ITEM_GRAYED(HWND_AA_MOTION, IDC_MOTOR_STOP);

	ITEM_GRAYED(HWND_AA_MOTION, IDC_SPEED_HOME);
	ITEM_GRAYED(HWND_AA_MOTION, IDC_SPEED_MOVE);
	ITEM_GRAYED(HWND_AA_MOTION, IDC_SPEED_JOG);
	ITEM_GRAYED(HWND_AA_MOTION, IDC_AD_HOME);
	ITEM_GRAYED(HWND_AA_MOTION, IDC_AD_JOG);
	ITEM_GRAYED(HWND_AA_MOTION, IDC_AD_MOVE);

	ITEM_GRAYED(HWND_AA_MOTION, IDC_MOVE_JOG_MINUS);
	ITEM_GRAYED(HWND_AA_MOTION, IDC_MOVE_JOG_PLUS);
	ITEM_GRAYED(HWND_AA_MOTION, IDC_MOVE_JOG_MINUS);
	ITEM_GRAYED(HWND_AA_MOTION, IDC_MOVE_JOG_PLUS);

	ITEM_GRAYED(HWND_AA_MOTION, IDC_HOME_ALL);
	//	ITEM_GRAYED(HWND_AA_MOTION, IDC_STOP_ALL);
	ITEM_GRAYED(HWND_AA_MOTION, IDC_SAVE);

	int timeout = 10*wait_time;
	while ((MANUAL_MOVING == ON) &&(timeout-- >= 0x00)) Sleep(100);
	Sleep(100);
	ITEM_ENABLED(HWND_AA_MOTION, IDC_MOTOR_SERVO_ON);
	ITEM_ENABLED(HWND_AA_MOTION, IDC_MOTOR_SERVO_OFF);
	ITEM_ENABLED(HWND_AA_MOTION, IDC_MOTOR_HOME);
	ITEM_ENABLED(HWND_AA_MOTION, IDC_MOTOR_RESET);
	ITEM_ENABLED(HWND_AA_MOTION, IDC_MOTOR_STOP);

	ITEM_ENABLED(HWND_AA_MOTION, IDC_SPEED_HOME);
	ITEM_ENABLED(HWND_AA_MOTION, IDC_SPEED_MOVE);
	ITEM_ENABLED(HWND_AA_MOTION, IDC_SPEED_JOG);
	ITEM_ENABLED(HWND_AA_MOTION, IDC_AD_HOME);
	ITEM_ENABLED(HWND_AA_MOTION, IDC_AD_JOG);
	ITEM_ENABLED(HWND_AA_MOTION, IDC_AD_MOVE);

	ITEM_ENABLED(HWND_AA_MOTION, IDC_MOVE_JOG_MINUS);
	ITEM_ENABLED(HWND_AA_MOTION, IDC_MOVE_JOG_PLUS);
	ITEM_ENABLED(HWND_AA_MOTION, IDC_MOVE_JOG_MINUS);
	ITEM_ENABLED(HWND_AA_MOTION, IDC_MOVE_JOG_PLUS);

	ITEM_ENABLED(HWND_AA_MOTION, IDC_HOME_ALL);
	//	ITEM_ENABLED(HWND_AA_MOTION, IDC_STOP_ALL);
	ITEM_ENABLED(HWND_AA_MOTION, IDC_SAVE);

	if (MANUAL_ERROR != 0x00){
		char buf[100] = {0};
		sprintf(buf, "Err[%d]:%s", MANUAL_ERROR, err_msg[MANUAL_ERROR]);
		SetDlgItemText(HWND_AA_MOTION, IDC_STATUS2, buf);
	}
}

DWORD WINAPI button_process_thread(LPVOID temp)
{
	while (1){
		if (motor_flag){
			motor_button_waiting(motor_flag);
			motor_flag = 0;
		}

		Sleep(50);
	}

	return 0x00;
}

int home_all_motor(void)
{
	if (SM101_SERVO_CHECK == OFF) SM101_SERVO_ON;
	if (SM102_SERVO_CHECK == OFF) SM102_SERVO_ON;
	if (SM103_SERVO_CHECK == OFF) SM103_SERVO_ON;
	if (SM201_SERVO_CHECK == OFF) SM201_SERVO_ON;
	if (SM202_SERVO_CHECK == OFF) SM202_SERVO_ON;
	if (SM203_SERVO_CHECK == OFF) SM203_SERVO_ON;

	int err ;
	if((err = SM202_encoder_reset())!=0x00) return err;
	if((err = SM201_encoder_reset())!=0x00) return err;
	if((err = SM203_encoder_reset())!=0x00) return err;
	if((err = SM101_encoder_reset())!=0x00) return err;
	if((err = SM102_encoder_reset())!=0x00) return err;
	if((err = SM103_encoder_reset())!=0x00) return err;
	if (err != 0x00) return err;

#ifdef HOME_ONE_BY_ONE
	if((err = SM101_encoder_reset())!=0x00) return err;
	if((err = SM102_encoder_reset())!=0x00) return err;
	if((err = SM201_encoder_reset())!=0x00) return err;
	if((err = SM203_encoder_reset())!=0x00) return err;


	if (RUN_HOME_OFFSET){
		if((err = SM101_move_to_home_position())!=0x00) return err;
		if((err = SM102_move_to_home_position())!=0x00) return err;
		if((err = SM104_move_to_home_position())!=0x00) return err;
		if((err = SM201_move_to_home_position())!=0x00) return err;
		if((err = SM203_move_to_home_position())!=0x00) return err;
	}

#else

//	SM202_TRIGGER = SM202_MOVE_TO_HOME_POSITION;
	int timeout = 10;
	SetDlgItemText(HWND_AA_MOTION, IDC_STATUS2, "Motor Homing...");
	while ((SM101_TRIGGER != OFF ||	SM102_TRIGGER != OFF || SM103_TRIGGER != OFF ||
		    SM201_TRIGGER != OFF || SM202_TRIGGER != OFF ||	SM203_TRIGGER != OFF) && 
			timeout-->=0){
		Sleep(10);
	}

	
	if (timeout<=0x00){
		if (SM101_TRIGGER != OFF)	   return SM101_TRIGGER_NOT_READY_ERR;
		else if (SM102_TRIGGER != OFF) return SM102_TRIGGER_NOT_READY_ERR;
		else if (SM103_TRIGGER != OFF) return SM103_TRIGGER_NOT_READY_ERR;
		else if (SM201_TRIGGER != OFF) return SM201_TRIGGER_NOT_READY_ERR;
		else if (SM202_TRIGGER != OFF) return SM202_TRIGGER_NOT_READY_ERR;
		else if (SM203_TRIGGER != OFF) return SM203_TRIGGER_NOT_READY_ERR;

	}
	log("Start to trigger all motors:");

	SM202_TRIGGER = SM202_HOMING;// aa line z home************
	log("sm 202 home.");
	timeout = 5000;
	while (SM202_TRIGGER != OFF && timeout-->=0){
			Sleep(10);
	}
	if (timeout>0x00){
		SM202_TRIGGER = SM202_MOVE_TO_HOME_POSITION;// aa line z move org
		timeout = 2000;
		while (SM202_TRIGGER != OFF && timeout-->=0){
			Sleep(10);
		}
	}
	else{
		log("home z time out.");
		return -1;
	}

	if (timeout>0x00){
		log("start move other motors.");
		SM101_TRIGGER = SM101_HOMING;
		SM102_TRIGGER = SM102_HOMING;
		SM103_TRIGGER = SM103_HOMING;
		SM201_TRIGGER = SM201_HOMING;
		//SM202_TRIGGER = SM202_HOMING;
		SM203_TRIGGER = SM203_HOMING;

		timeout = 2000;

		while ((SM101_TRIGGER != OFF || SM102_TRIGGER != OFF || SM103_TRIGGER != OFF ||
			SM201_TRIGGER != OFF || SM203_TRIGGER != OFF) && timeout-->=0){
				Sleep(10);
		}
	}
	else {
		log("home line z org time out.");
		return -1;
	}

	if (timeout<=0x00){
		if (SM101_TRIGGER != OFF)	   return SM101_TRIGGER_NOT_READY_ERR;
		else if (SM102_TRIGGER != OFF) return SM102_TRIGGER_NOT_READY_ERR;
		else if (SM103_TRIGGER != OFF) return SM103_TRIGGER_NOT_READY_ERR;
		else if (SM201_TRIGGER != OFF) return SM201_TRIGGER_NOT_READY_ERR;
		else if (SM202_TRIGGER != OFF) return SM202_TRIGGER_NOT_READY_ERR;
		else if (SM203_TRIGGER != OFF) return SM203_TRIGGER_NOT_READY_ERR;
	}

	if (RUN_HOME_OFFSET){
		log("Start to move to home offset:");
		SM101_TRIGGER = SM101_MOVE_TO_HOME_POSITION;
		SM102_TRIGGER = SM102_MOVE_TO_HOME_POSITION;
		SM103_TRIGGER = SM103_MOVE_TO_HOME_POSITION;

		SM201_TRIGGER = SM201_MOVE_TO_HOME_POSITION;
		SM202_TRIGGER = SM202_MOVE_TO_HOME_POSITION;
		SM203_TRIGGER = SM203_MOVE_TO_HOME_POSITION;
		timeout = 50000/10;

		while ((SM101_TRIGGER != OFF || SM102_TRIGGER != OFF || SM103_TRIGGER != OFF ||
			    SM201_TRIGGER != OFF || SM202_TRIGGER != OFF ||	SM203_TRIGGER != OFF) &&
				timeout-->=0){
			Sleep(10);
		}

		if (timeout<=0x00){
			if (SM101_TRIGGER != OFF)	   return SM101_TRIGGER_NOT_READY_ERR;
			else if (SM102_TRIGGER != OFF) return SM102_TRIGGER_NOT_READY_ERR;
			else if (SM103_TRIGGER != OFF) return SM103_TRIGGER_NOT_READY_ERR;
			else if (SM201_TRIGGER != OFF) return SM201_TRIGGER_NOT_READY_ERR;
			else if (SM203_TRIGGER != OFF) return SM203_TRIGGER_NOT_READY_ERR;
			else if (SM202_TRIGGER != OFF) return SM202_TRIGGER_NOT_READY_ERR;
		}
	}
#endif

	SetDlgItemText(HWND_AA_MOTION, IDC_STATUS2, "Motor Home Done.");
	return 0x00;
}

DWORD WINAPI home_motors_thread(LPVOID temp)
{
	int home_result = FALSE;
	char buf[100] = {0};
	while (1){
		while (EXECUTE_HOME == FALSE) Sleep(10);
		if ((home_result = home_all_motor()) != 0x00){
			sprintf(buf, "Err[%d]:%s", home_result, err_msg[home_result]);
			SetDlgItemText(HWND_AA_MOTION, IDC_STATUS2, buf);
			EXECUTE_ERROR = TRUE;
			log("***Home Error***");
		}
		EXECUTE_CMD = FALSE;
		EXECUTE_HOME = FALSE;
		log("execute home = %d",EXECUTE_HOME);
		ITEM_ENABLED(HWND_AA_MOTION, IDC_HOME_ALL);
		Sleep(10);
	}

	return FALSE;
}



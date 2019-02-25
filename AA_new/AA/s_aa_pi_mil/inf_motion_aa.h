#include <Windows.h>
#include <stdio.h>

#include <math.h>


#include "mem_def.h"
#include "PS400.h"

#include "err_def.h"
#include "declare.h"
#include "define.h"
#include "motor_io.h"
#include "fun_num.h"


extern MOTION_SHM mshm;

extern float my_a;
extern float my_b;
extern float my_c;

//==== SM101 ===================================================================== tilt x
#define SM101_RangeSet			16000
#define SM101_Multiple			(8000000.0/SM101_RangeSet)
#define SM101_DegreeRatio		(100000.0/38)		// 1mm					//2000
#define SM101_SpeedRatio		(SM101_DegreeRatio/SM101_Multiple)

//==== SM102 ===================================================================== tilt y
#define SM102_RangeSet			16000
#define SM102_Multiple			(8000000.0/SM102_RangeSet)
#define SM102_DegreeRatio		(100000.0/28)		// 1600 pulse = 0.5mm  //2000
#define SM102_SpeedRatio		(SM102_DegreeRatio/SM102_Multiple)

//==== SM103 ===================================================================== tilt z
#define SM103_RangeSet			16000
#define SM103_Multiple			(8000000.0/SM103_RangeSet)
#define SM103_DegreeRatio		(5000.0/18)		// 10000 pulses = 2mm		//2000
#define SM103_SpeedRatio		(SM103_DegreeRatio/SM103_Multiple)

//==== SM104 ===================================================================== no use
#define SM104_RangeSet			16000
#define SM104_Multiple			(8000000.0/SM104_RangeSet)
#define SM104_DegreeRatio		10000	// 1600x(335/1)pulse = 360 pulse //240
#define SM104_SpeedRatio		(SM104_DegreeRatio/SM104_Multiple)

//==== SM201 ===================================================================== LINE X
#define SM201_RangeSet			16000
#define SM201_Multiple			(8000000.0/SM201_RangeSet)
#define SM201_PositionRatio		20000.0		// 10000 pulse = 1mm	//10000
#define SM201_SpeedRatio		(SM201_PositionRatio/SM201_Multiple)

//==== SM202 =====================================================================	line y
#define SM202_RangeSet			16000
#define SM202_Multiple			(8000000.0/SM202_RangeSet)
#define SM202_PositionRatio		20000.0		// 10000 pulse = 10mm
#define SM202_SpeedRatio		(SM202_PositionRatio/SM202_Multiple)

//==== SM203 ===================================================================== line z
#define SM203_RangeSet			16000
#define SM203_Multiple			(8000000.0/SM203_RangeSet)
#define SM203_PositionRatio		20000.0		// 10000 pulse = 1mm
#define SM203_SpeedRatio		(SM203_PositionRatio/SM203_Multiple)

//==== SM204 ===================================================================== NOT USED
#define SM204_RangeSet			16000
#define SM204_Multiple			(8000000.0/SM204_RangeSet)
#define SM204_PositionRatio		((25600*(44/22))/360.0)	// 25600 x (44/22) pulse = 360.0 degree
#define SM204_SpeedRatio		(SM204_PositionRatio/SM204_Multiple)
// resolution	= 25600 pulse
// pulley ratio = 22:44


//================================================================================
#define	MotionCardBase1									mshm.Ps400_AddrBase[0]			
#define	MotionCardBase2									mshm.Ps400_AddrBase[1]			
#define	MotionCardBase3									mshm.Ps400_AddrBase[2]			

//======================== RAM Int ====================================
//=== define Task step counter -> reserve RAM_INT[0] ~ RAM_INT[19] 
#define  SYSTEM_TASK									mshm.RAM_INT[0]
#define  FOCUS_ADJUST_TASK								mshm.RAM_INT[1]
#define  AF_TEST_TASK									mshm.RAM_INT[2]
#define  IMAGE_TEST_TASK								mshm.RAM_INT[3]
#define  CENTRAL_ROTATE_TASK							mshm.RAM_INT[4]
#define  MONITOR_TASK									mshm.RAM_INT[5]

//=== define ERROR flag -> RAM_INT[20] ~ RAM_INT[39]===================
#define  SYSTEM_ERR										mshm.RAM_INT[20]
#define  FOCUS_ADJUST_TASK_ERR							mshm.RAM_INT[21]
#define  AF_TEST_TASK_ERR								mshm.RAM_INT[22]
#define  IMAGE_TEST_TASK_ERR							mshm.RAM_INT[23]
#define  CENTRAL_ROTATE_TASK_ERR						mshm.RAM_INT[24]
#define  MONITOR_TASK_ERR								mshm.RAM_INT[25]

//=== define TASK status -> RAM_INT[40] ~ RAM_INT[59]==================
#define  FOCUS_TEST_TASK_STATUS							mshm.RAM_INT[40]
#define  AF_TSET_TASK_STATUS							mshm.RAM_INT[41]
#define  IMAGE_TEST_TASK_STATUS							mshm.RAM_INT[42]

//=== General Purpose flag ============================================
#define  MEMORY_OPEN_FLAG								mshm.RAM_INT[60]
#define  PROCESS_EXIT_FLAG								mshm.RAM_INT[61]
#define  EV_AUTO										mshm.RAM_INT[62]	// 
#define  ERROR_FLAG										mshm.RAM_INT[63]
#define  ERROR_CODE2									mshm.RAM_INT[64]
#define  MANUAL_MOVING									mshm.RAM_INT[65]
#define  MANUAL_ERROR									mshm.RAM_INT[66]
#define  EMERGENCY_STOP_FLAG							mshm.RAM_INT[67]
#define  INITIAL_STATUS									mshm.RAM_INT[68]
#define  BUTTON_VALUE									mshm.RAM_INT[69]
#define  MC_STATE										mshm.RAM_INT[70]
#define  LOAD_CELL_LIMIT_FLAG							mshm.RAM_INT[71]
#define  INIT_FLAG										mshm.RAM_INT[72]
#define  OPERATION_LEVEL								mshm.RAM_INT[73]
#define  TESTRUN_FLAG									mshm.RAM_INT[74]
#define  P32C32_TOTAL_DEVICE_NUMBER						mshm.RAM_INT[75]
#define  PS400_TOTAL_DEVICE_NUMBER						mshm.RAM_INT[76]
#define  BUZZER_DISABLE									mshm.RAM_INT[77]
#define  IO_DISPLAY_FLAG								mshm.RAM_INT[78]

#define  START_BUTTON_FLAG								mshm.RAM_INT[80]
#define  STOP_BUTTON_FLAG								mshm.RAM_INT[81]
#define  STEP_BUTTON_FLAG								mshm.RAM_INT[82]
#define  ERR_RESET_FLAG									mshm.RAM_INT[83]
#define  HOME_BUTTON_FLAG								mshm.RAM_INT[84]
#define  RUNNING_START_BUTTON_FLAG						mshm.RAM_INT[85]
#define  RUNNING_STOP_BUTTON_FLAG						mshm.RAM_INT[86]
#define  RUNNING_SKIP_BUTTON_FLAG						mshm.RAM_INT[87]
#define  RUN_HOME_OFFSET								mshm.RAM_INT[88]

#define  RUN_STATUS_LAMP								mshm.RAM_INT[90]
#define  INITIAL_STATUS_LAMP							mshm.RAM_INT[91]
#define  MANUAL_STATUS_LAMP								mshm.RAM_INT[92]
#define  STOP_STATUS_LAMP								mshm.RAM_INT[93]
#define  WARNING_STATUS_LAMP							mshm.RAM_INT[94]
#define  ALARM_STATUS_LAMP								mshm.RAM_INT[95]

//=====================================================================
#define  SM101_HOMED_FLAG								mshm.RAM_INT[100]
#define  SM102_HOMED_FLAG								mshm.RAM_INT[101]
#define  SM103_HOMED_FLAG								mshm.RAM_INT[102]
#define  SM104_HOMED_FLAG								mshm.RAM_INT[103]
#define  SM201_HOMED_FLAG								mshm.RAM_INT[104]
#define  SM202_HOMED_FLAG								mshm.RAM_INT[105]
#define  SM203_HOMED_FLAG								mshm.RAM_INT[106]
#define  SM204_HOMED_FLAG								mshm.RAM_INT[107]
#define  HOME_FLAG										mshm.RAM_INT[108]

//=====================================================================
#define  SM101_TRIGGER									mshm.RAM_INT[110]
#define  SM102_TRIGGER									mshm.RAM_INT[111]
#define  SM103_TRIGGER									mshm.RAM_INT[112]
#define  SM104_TRIGGER									mshm.RAM_INT[113]
#define  SM201_TRIGGER									mshm.RAM_INT[114]
#define  SM202_TRIGGER									mshm.RAM_INT[115]
#define  SM203_TRIGGER									mshm.RAM_INT[116]
#define  SM204_TRIGGER									mshm.RAM_INT[117]

#define  SM101_MOTION_ERR								mshm.RAM_INT[120]
#define  SM102_MOTION_ERR								mshm.RAM_INT[121]
#define  SM103_MOTION_ERR								mshm.RAM_INT[122]
#define  SM104_MOTION_ERR								mshm.RAM_INT[123]
#define  SM201_MOTION_ERR								mshm.RAM_INT[124]
#define  SM202_MOTION_ERR								mshm.RAM_INT[125]
#define  SM203_MOTION_ERR								mshm.RAM_INT[126]
#define  SM204_MOTION_ERR								mshm.RAM_INT[127]

#define	 SITE1_TEST_ENABLE_FLAG							mshm.RAM_INT[130]	 
#define	 SITE2_TEST_ENABLE_FLAG							mshm.RAM_INT[131] 
#define	 SITE3_TEST_ENABLE_FLAG							mshm.RAM_INT[132] 
#define	 SITE4_TEST_ENABLE_FLAG							mshm.RAM_INT[133]

#define	 TOTAL_DISPENSE_POINTS							mshm.RAM_INT[140] 
#define	 DISPENSING_TIME1								mshm.RAM_INT[141]	// Unit: ms 
#define	 DISPENSING_TIME2								mshm.RAM_INT[142]	// Unit: ms 
#define	 DISPENSING_TIME3								mshm.RAM_INT[143]	// Unit: ms 
#define	 DISPENSING_TIME4								mshm.RAM_INT[144]	// Unit: ms 
#define	 DISPENSING_TIME5								mshm.RAM_INT[145]	// Unit: ms 
#define  UV_METHOD										mshm.RAM_INT[146]	// Fixed mode = 0; Points mode = 1; Rotate mode = 2;
#define  UV_CURING_TIME									mshm.RAM_INT[147]	// Unit: ms
#define  CLEAN_TIME										mshm.RAM_INT[148]	// Unit: ms
#define  TOTAL_CLEAN_CYCLE								mshm.RAM_INT[149]	

#define	 LASER_PROCESSING_FLAG							mshm.RAM_INT[150]	// ON processing = 1, OFF processing = 0; 
#define	 UV_CURING_PROCESSING_FLAG						mshm.RAM_INT[151]	// ON processing = 1, OFF processing = 0; 
#define	 CLEAN_PROCESSING_FLAG							mshm.RAM_INT[152]	// ON processing = 1, OFF processing = 0; 
#define  HomeMachine_StopFlag							mshm.RAM_INT[153]
#define  EmergencyStopFlag								mshm.RAM_INT[154]
#define  DispensePointIndex								mshm.RAM_INT[155]
#define  LoadingSiteFlag								mshm.RAM_INT[156]
#define  FocusAdjustSiteFlag							mshm.RAM_INT[157]
#define  AFTestSiteFlag									mshm.RAM_INT[158]
#define  ImageSiteFlag									mshm.RAM_INT[159]

#define  UVCuringTimeCount								mshm.RAM_INT[160]
#define  DispenserTimeCount								mshm.RAM_INT[161]

#define  FocusUnitCommand								mshm.RAM_INT[162]
#define  AFTestUnitCommand								mshm.RAM_INT[163]
#define  ImageTestUnitCommand							mshm.RAM_INT[164]
#define  IndexTableRunningFlag							mshm.RAM_INT[165]
#define  MAIN_MENU_FLAG									mshm.RAM_INT[166]
#define  HOME_DIALOG_FLAG								mshm.RAM_INT[167]
#define  DISPENSE_Z_MOVING_DOWN_FLAG					mshm.RAM_INT[168]
#define  DispenserCleaningCount							mshm.RAM_INT[169]
#define  ManualOperationCommand							mshm.RAM_INT[170]
#define  DispenseProcedureZaxisMoveUpHeight				mshm.RAM_INT[171]
#define  OnePointDispensingTime							mshm.RAM_INT[172]	// unit = 1ms 
#define  FocusAdjustUnitEnableFlag						mshm.RAM_INT[173]	// ON = enable, OFF = disable 
#define  ImageTestUnitEnableFlag						mshm.RAM_INT[174]	// ON = enable, OFF = disable 
#define  DispenserUnitEnableFlag						mshm.RAM_INT[175]	// ON = enable, OFF = disable 

#define AUTO_FOCUS_COMMAND								mshm.RAM_INT[176]	//communication with auto focus program					

#define  DISPENSER_R_DISABLE_FLAG						mshm.RAM_INT[200]	// 1: ON = disable Dispenser-R 
#define  DISPENSER_UV_R_DISABLE_FLAG					mshm.RAM_INT[201]	// 1£ºON = disable UV-R 
#define  FOCUS_Z_PROTECT_DISABLE_FLAG					mshm.RAM_INT[202]
#define  DISPENSE_Z_PROTECT_DISABLE_FLAG				mshm.RAM_INT[203]
#define  INDEX_TABLE_MOVING_FLAG						mshm.RAM_INT[204]

#define  ParaA_DeviceMemoryFlag							mshm.RAM_INT[210]
#define  ParaB_DeviceMemoryFlag							mshm.RAM_INT[211]
#define  ParaC_DeviceMemoryFlag							mshm.RAM_INT[212]
#define  ParaD_DeviceMemoryFlag							mshm.RAM_INT[213]
#define  ParaR_DeviceMemoryFlag							mshm.RAM_INT[214]

#define  ParaB_Site2TestResult							mshm.RAM_INT[215]
#define  ParaB_Site2TestErrorCode						mshm.RAM_INT[216]

#define  ParaC_Site2TestResult							mshm.RAM_INT[217]
#define  ParaC_Site2TestErrorCode						mshm.RAM_INT[218]
#define  ParaC_Site3TestResult							mshm.RAM_INT[219]
#define  ParaC_Site3TestErrorCode						mshm.RAM_INT[220]

#define  ParaD_Site2TestResult							mshm.RAM_INT[221]
#define  ParaD_Site2TestErrorCode						mshm.RAM_INT[222]
#define  ParaD_Site3TestResult							mshm.RAM_INT[223]
#define  ParaD_Site3TestErrorCode						mshm.RAM_INT[224]
#define  ParaD_Site4TestResult							mshm.RAM_INT[225]
#define  ParaD_Site4TestErrorCode						mshm.RAM_INT[226]

#define  ParaR_Site2TestResult							mshm.RAM_INT[227]
#define  ParaR_Site2TestErrorCode						mshm.RAM_INT[228]
#define  ParaR_Site3TestResult							mshm.RAM_INT[229]
#define  ParaR_Site3TestErrorCode						mshm.RAM_INT[230]
#define  ParaR_Site4TestResult							mshm.RAM_INT[231]
#define  ParaR_Site4TestErrorCode						mshm.RAM_INT[232]
#define  ParaR_ResultDisplayTrigger						mshm.RAM_INT[233]

#define  IS_TEST_OVERIDE								mshm.RAM_INT[240]
#define  SOCKET_PROTECT_ENABLE							mshm.RAM_INT[241]

#define  AA_FS_MAX_CNT									mshm.RAM_INT[250]
#define  AA_TF2_MAX_CNT									mshm.RAM_INT[251]
#define  AA_TF1_BACK_OPTION								mshm.RAM_INT[252]
#define  AA_TARGET_CENTER_X								mshm.RAM_INT[253]
#define  AA_TARGET_CENTER_Y								mshm.RAM_INT[254]
#define  AA_SELECT_ROI									mshm.RAM_INT[255]
#define  AA_FS_DELAY									mshm.RAM_INT[256]
#define  AA_USE_TF2										mshm.RAM_INT[257]
#define  AA_TF2_DELAY									mshm.RAM_INT[258]
#define  AA_FS_AUTO_CALIBRATION						mshm.RAM_INT[259]
#define  AA_TF2_AUTO_CALIBRATION						mshm.RAM_INT[260]
#define  AA_USE_TF3										mshm.RAM_INT[261]
#define  AA_RETURN_PEAK_DELAY							mshm.RAM_INT[262]
#define  AA_PEAK_MIN_GUIDE								mshm.RAM_INT[263]
#define  AA_FS_BACK_OFFSET							mshm.RAM_INT[264]
#define  AA_TF3_START_OFFSET							mshm.RAM_INT[265]
#define  AA_LENS_TF_MAX_CNT								mshm.RAM_INT[266]
#define  AA_LENS_TF_DELAY								mshm.RAM_INT[267]
#define  PLC_DB_INDEX									mshm.RAM_INT[268]
#define  PLC_DB_START									mshm.RAM_INT[269]
#define  PLC_DB_LENS									mshm.RAM_INT[270]
#define  PLC_DB_INDEX_R									mshm.RAM_INT[271]
#define  PLC_DB_START_R									mshm.RAM_INT[272]
#define  PLC_DB_LENS_R									mshm.RAM_INT[273]
#define  IMG_MIRROR_X									mshm.RAM_INT[274]		
#define  IMG_MIRROR_Y									mshm.RAM_INT[275]
#define  CHOOSE_XY									mshm.RAM_INT[276]
#define  SYMBOL1									mshm.RAM_INT[277]
#define  SYMBOL2									mshm.RAM_INT[278]
#define  W_PLC_DB_INDEX									mshm.RAM_INT[279]
#define  W_PLC_DB_START									mshm.RAM_INT[280]
#define  W_PLC_DB_LENS									mshm.RAM_INT[281]
#define  W_PLC_DB_INDEX_R								mshm.RAM_INT[282]
#define  W_PLC_DB_START_R								mshm.RAM_INT[283]
#define  W_PLC_DB_LENS_R								mshm.RAM_INT[284]



//======================== RAM Float ====================================
#define  FOCUS_MODULE_CONE_BACKRUSH						mshm.RAM_DOUBLE[10]	// Unit: degree
#define  FOCUS_MODULE_PITCH								mshm.RAM_DOUBLE[11]	// Unit: mm
#define  FOCUS_MODULE_CONE_UP_DISTANCE					mshm.RAM_DOUBLE[12]	// Unit: mm

#define  DISPENSE_MODULE_LENS_RADIUS					mshm.RAM_DOUBLE[20]	// Unit: mm
#define  DISPENSE_MODULE_LENS_DEPTH						mshm.RAM_DOUBLE[21]	// Unit: mm
#define  DISPENSE_MODULE_OFFSET_TO_LENS_EDGE			mshm.RAM_DOUBLE[22]	// Unit: mm
#define	 DISPENSE_MODULE_R_OFFSET						mshm.RAM_DOUBLE[23]	// Degree
#define	 DISPENSE_MODULE_X_AXIS_LASER_TO_GLASS_OFFSET	mshm.RAM_DOUBLE[24]	// Unit: mm
#define	 DISPENSE_MODULE_Y_AXIS_LASER_TO_GLASS_OFFSET	mshm.RAM_DOUBLE[25]	// Unit: mm
#define	 DISPENSE_MODULE_Z_AXIS_LASER_TO_GLASS_OFFSET	mshm.RAM_DOUBLE[26]	// Unit: mm
#define	 DISPENSE_MODULE_X_AXIS_VISION_TO_GLASS_OFFSET	mshm.RAM_DOUBLE[27]	// Unit: mm
#define	 DISPENSE_MODULE_Y_AXIS_VISION_TO_GLASS_OFFSET	mshm.RAM_DOUBLE[28]	// Unit: mm
#define	 DISPENSE_MODULE_Z_AXIS_VISION_TO_GLASS_OFFSET	mshm.RAM_DOUBLE[29]	// Unit: mm
#define  DISPENSE_MODULE_Z_AXIS_LC_TO_GLASS_OFFSET		mshm.RAM_DOUBLE[30] // Unit: mm

#define AA_FS_STEP_UNIT									mshm.RAM_DOUBLE[50]
#define AA_TF2_STEP_UNIT								mshm.RAM_DOUBLE[51]
// #define AA_MOTION_X_ANGLE								mshm.RAM_DOUBLE[52]
// #define AA_MOTION_Y_ANGLE								mshm.RAM_DOUBLE[53]
#define PIXEL_SIZE										mshm.RAM_DOUBLE[54]
#define AA_MOVE_X_MM									mshm.RAM_DOUBLE[55]
#define AA_MOVE_Y_MM									mshm.RAM_DOUBLE[56]
#define AA_Z_LOADING_POS								mshm.RAM_DOUBLE[57]
#define AA_FINAL_X_OFFSET								mshm.RAM_DOUBLE[58]
#define AA_FINAL_Z_OFFSET								mshm.RAM_DOUBLE[59]
#define AA_PEAK_DIFF_SPEC								mshm.RAM_DOUBLE[60]
#define AA_FINAL_Y_OFFSET								mshm.RAM_DOUBLE[61]
#define AA_FS_TILT_LIMIT								mshm.RAM_DOUBLE[62]
#define AA_Z_LIMIT_PLUS_POS								mshm.RAM_DOUBLE[63]
#define AA_SCAN_STEP									mshm.RAM_DOUBLE[64]

#define AA_FINAL_MOVE_DISTANCE							mshm.RAM_DOUBLE[65]

#define AA_LENS_TF_STEP_UNIT							mshm.RAM_DOUBLE[66]
#define AA_LENS_Z_LOADING_POS							mshm.RAM_DOUBLE[67]
#define AA_LENS_TILT_X									mshm.RAM_DOUBLE[68]
#define AA_LENS_TILT_Y									mshm.RAM_DOUBLE[69]
#define AA_LENS_X_LOADING_POS							mshm.RAM_DOUBLE[70]
#define AA_LENS_Y_LOADING_POS							mshm.RAM_DOUBLE[71]
#define AA_LENS_Z_OFFSET								mshm.RAM_DOUBLE[72]
#define AA_LENS_EFL										mshm.RAM_DOUBLE[73]

//==== index definition for AC Servo Motor parameters ==============================
//---- SM101 -----------------------------------------------------------------------
#define	 SM101_AXIS_HOME_SPEED							mshm.SERVO_MOTOR_PARAMETERS[0]	// Unit: 0.01mm/s
#define	 SM101_AXIS_HOME_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[1]  // Unit: ms
#define	 SM101_AXIS_JOG_SPEED							mshm.SERVO_MOTOR_PARAMETERS[2]	// Unit: 0.01mm/s 
#define	 SM101_AXIS_JOG_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[3]  // Unit: ms
#define	 SM101_AXIS_MOVE_SPEED							mshm.SERVO_MOTOR_PARAMETERS[4]	// Unit: 0.01mm/s 
#define	 SM101_AXIS_MOVE_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[5]  // Unit: ms

#define	 SM101_SHORT_MOVE_DISTANCE						mshm.SERVO_MOTOR_PARAMETERS[10]	// Pulses
#define	 SM101_HOME_OFFSET								mshm.SERVO_MOTOR_PARAMETERS[11]	// Pulses
#define	 SM101_POSITION_1								mshm.RAM_DOUBLE[111] // Pulses
#define	 SM101_POSITION_2								mshm.RAM_DOUBLE[112] // Pulses
#define	 SM101_POSITION_3								mshm.RAM_DOUBLE[113] // Pulses
#define	 SM101_POSITION_4								mshm.RAM_DOUBLE[114] // Pulses
#define	 SM101_POSITION_5								mshm.RAM_DOUBLE[115] // Pulses
		
//---- SM102 -----------------------------------------------------------------------
#define	 SM102_AXIS_HOME_SPEED							mshm.SERVO_MOTOR_PARAMETERS[30]	// Unit: 0.01mm/s 
#define	 SM102_AXIS_HOME_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[31] // Unit: ms
#define	 SM102_AXIS_JOG_SPEED							mshm.SERVO_MOTOR_PARAMETERS[32]	// Unit: 0.01mm/s 
#define	 SM102_AXIS_JOG_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[33] // Unit: ms
#define	 SM102_AXIS_MOVE_SPEED							mshm.SERVO_MOTOR_PARAMETERS[34]	// Unit: 0.01mm/s 
#define	 SM102_AXIS_MOVE_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[35] // Unit: ms

#define	 SM102_SHORT_MOVE_DISTANCE						mshm.SERVO_MOTOR_PARAMETERS[40] // Pulses
#define	 SM102_HOME_OFFSET								mshm.SERVO_MOTOR_PARAMETERS[41]	// Pulses
#define	 SM102_POSITION_1								mshm.RAM_DOUBLE[121] // Pulses
#define	 SM102_POSITION_2								mshm.RAM_DOUBLE[122] // Pulses
#define	 SM102_POSITION_3								mshm.RAM_DOUBLE[123] // Pulses
#define	 SM102_POSITION_4								mshm.RAM_DOUBLE[124] // Pulses
#define	 SM102_POSITION_5								mshm.RAM_DOUBLE[125] // Pulses

//---- SM103 -----------------------------------------------------------------------
#define	 SM103_AXIS_HOME_SPEED							mshm.SERVO_MOTOR_PARAMETERS[60]	// Unit: 0.01mm/s 
#define	 SM103_AXIS_HOME_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[61] // Unit: ms
#define	 SM103_AXIS_JOG_SPEED							mshm.SERVO_MOTOR_PARAMETERS[62]	// Unit: 0.01mm/s 
#define	 SM103_AXIS_JOG_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[63] // Unit: ms
#define	 SM103_AXIS_MOVE_SPEED							mshm.SERVO_MOTOR_PARAMETERS[64]	// Unit: 0.01mm/s 
#define	 SM103_AXIS_MOVE_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[65] // Unit: ms

#define	 SM103_SHORT_MOVE_DISTANCE						mshm.SERVO_MOTOR_PARAMETERS[70]	// Pulses
#define	 SM103_HOME_OFFSET								mshm.SERVO_MOTOR_PARAMETERS[71]	// Pulses
#define	 SM103_POSITION_1								mshm.RAM_DOUBLE[131] // Pulses
#define	 SM103_POSITION_2								mshm.RAM_DOUBLE[132] // Pulses
#define	 SM103_POSITION_3								mshm.RAM_DOUBLE[133] // Pulses
#define	 SM103_POSITION_4								mshm.RAM_DOUBLE[134] // Pulses
#define	 SM103_POSITION_5								mshm.RAM_DOUBLE[135] // Pulses

//---- SM104 -----------------------------------------------------------------------
#define	 SM104_AXIS_HOME_SPEED							mshm.SERVO_MOTOR_PARAMETERS[90]	// Unit: 0.01mm/s 
#define	 SM104_AXIS_HOME_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[91] // Unit: ms
#define	 SM104_AXIS_JOG_SPEED							mshm.SERVO_MOTOR_PARAMETERS[92]	// Unit: 0.01mm/s 
#define	 SM104_AXIS_JOG_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[93] // Unit: ms
#define	 SM104_AXIS_MOVE_SPEED							mshm.SERVO_MOTOR_PARAMETERS[94]	// Unit: 0.01mm/s 
#define	 SM104_AXIS_MOVE_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[95] // Unit: ms

#define	 SM104_SHORT_MOVE_DISTANCE						mshm.SERVO_MOTOR_PARAMETERS[100]// Pulses
#define	 SM104_HOME_OFFSET								mshm.SERVO_MOTOR_PARAMETERS[101]// Pulses
#define	 SM104_POSITION_1								mshm.RAM_DOUBLE[141] // Pulses
#define	 SM104_POSITION_2								mshm.RAM_DOUBLE[142] // Pulses
#define	 SM104_POSITION_3								mshm.RAM_DOUBLE[143] // Pulses
#define	 SM104_POSITION_4								mshm.RAM_DOUBLE[144] // Pulses
#define	 SM104_POSITION_5								mshm.RAM_DOUBLE[145] // Pulses

//---- SM201 -----------------------------------------------------------------------
#define	 SM201_AXIS_HOME_SPEED							mshm.SERVO_MOTOR_PARAMETERS[120]// Unit: 0.01mm/s 
#define	 SM201_AXIS_HOME_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[121]// Unit: ms
#define	 SM201_AXIS_JOG_SPEED							mshm.SERVO_MOTOR_PARAMETERS[122]// Unit: 0.01mm/s 
#define	 SM201_AXIS_JOG_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[123]// Unit: ms
#define	 SM201_AXIS_MOVE_SPEED							mshm.SERVO_MOTOR_PARAMETERS[124]// Unit: 0.01mm/s 
#define	 SM201_AXIS_MOVE_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[125]// Unit: ms

#define	 SM201_SHORT_MOVE_DISTANCE						mshm.SERVO_MOTOR_PARAMETERS[130]// Pulses
#define	 SM201_HOME_OFFSET								mshm.SERVO_MOTOR_PARAMETERS[131]// Pulses
#define	 SM201_POSITION_1								mshm.RAM_DOUBLE[151]// Pulses
#define	 SM201_POSITION_2								mshm.RAM_DOUBLE[152]// Pulses
#define	 SM201_POSITION_3								mshm.RAM_DOUBLE[153]// Pulses
#define	 SM201_POSITION_4								mshm.RAM_DOUBLE[154]// Pulses
#define	 SM201_POSITION_5								mshm.RAM_DOUBLE[155]// Pulses
#define	 SM201_POSITION_6								mshm.RAM_DOUBLE[156]// Pulses
#define	 SM201_POSITION_7								mshm.RAM_DOUBLE[157]// Pulses
#define	 SM201_POSITION_8								mshm.RAM_DOUBLE[158]// Pulses

#define	 SM201_DISPENSE_X_POSITION_1					mshm.SERVO_MOTOR_PARAMETERS[140]// Pulses
#define	 SM201_DISPENSE_X_POSITION_2					mshm.SERVO_MOTOR_PARAMETERS[141]// Pulses
#define	 SM201_DISPENSE_X_POSITION_3					mshm.SERVO_MOTOR_PARAMETERS[142]// Pulses
#define	 SM201_DISPENSE_X_POSITION_4					mshm.SERVO_MOTOR_PARAMETERS[143]// Pulses
#define	 SM201_DISPENSE_X_POSITION_5					mshm.SERVO_MOTOR_PARAMETERS[144]// Pulses

//---- SM202 -----------------------------------------------------------------------
#define	 SM202_AXIS_HOME_SPEED							mshm.SERVO_MOTOR_PARAMETERS[150]// Unit: 0.01mm/s 
#define	 SM202_AXIS_HOME_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[151]// Unit: ms
#define	 SM202_AXIS_JOG_SPEED							mshm.SERVO_MOTOR_PARAMETERS[152]// Unit: 0.01mm/s 
#define	 SM202_AXIS_JOG_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[153]// Unit: ms
#define	 SM202_AXIS_MOVE_SPEED							mshm.SERVO_MOTOR_PARAMETERS[154]// Unit: 0.01mm/s 
#define	 SM202_AXIS_MOVE_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[155]// Unit: ms

#define	 SM202_SHORT_MOVE_DISTANCE						mshm.SERVO_MOTOR_PARAMETERS[160]// Pulses
#define	 SM202_HOME_OFFSET								mshm.SERVO_MOTOR_PARAMETERS[161]// Pulses
#define	 SM202_POSITION_1								mshm.RAM_DOUBLE[161]// Pulses
#define	 SM202_POSITION_2								mshm.RAM_DOUBLE[162]// Pulses
#define	 SM202_POSITION_3								mshm.RAM_DOUBLE[163]// Pulses
#define	 SM202_POSITION_4								mshm.RAM_DOUBLE[164]// Pulses
#define	 SM202_POSITION_5								mshm.RAM_DOUBLE[165]// Pulses
#define	 SM202_POSITION_6								mshm.RAM_DOUBLE[166]// Pulses
#define	 SM202_POSITION_7								mshm.RAM_DOUBLE[167]// Pulses
#define	 SM202_POSITION_8								mshm.RAM_DOUBLE[168]// Pulses

#define	 SM202_DISPENSE_Y_POSITION_1					mshm.SERVO_MOTOR_PARAMETERS[170]// Pulses
#define	 SM202_DISPENSE_Y_POSITION_2					mshm.SERVO_MOTOR_PARAMETERS[171]// Pulses
#define	 SM202_DISPENSE_Y_POSITION_3					mshm.SERVO_MOTOR_PARAMETERS[172]// Pulses
#define	 SM202_DISPENSE_Y_POSITION_4					mshm.SERVO_MOTOR_PARAMETERS[173]// Pulses
#define	 SM202_DISPENSE_Y_POSITION_5					mshm.SERVO_MOTOR_PARAMETERS[174]// Pulses

//---- SM203 -----------------------------------------------------------------------
#define	 SM203_AXIS_HOME_SPEED							mshm.SERVO_MOTOR_PARAMETERS[180]// Unit: 0.01mm/s 
#define	 SM203_AXIS_HOME_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[181]// Unit: ms
#define	 SM203_AXIS_JOG_SPEED							mshm.SERVO_MOTOR_PARAMETERS[182]// Unit: 0.01mm/s 
#define	 SM203_AXIS_JOG_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[183]// Unit: ms
#define	 SM203_AXIS_MOVE_SPEED							mshm.SERVO_MOTOR_PARAMETERS[184]// Unit: 0.01mm/s 
#define	 SM203_AXIS_MOVE_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[185]// Unit: ms

#define	 SM203_SHORT_MOVE_DISTANCE						mshm.SERVO_MOTOR_PARAMETERS[190]// Pulses
#define	 SM203_HOME_OFFSET								mshm.SERVO_MOTOR_PARAMETERS[191]// Pulses
#define	 SM203_POSITION_1								mshm.RAM_DOUBLE[171]// Pulses
#define	 SM203_POSITION_2								mshm.RAM_DOUBLE[172]// Pulses
#define	 SM203_POSITION_3								mshm.RAM_DOUBLE[173]// Pulses
#define	 SM203_POSITION_4								mshm.RAM_DOUBLE[174]// Pulses
#define	 SM203_POSITION_5								mshm.RAM_DOUBLE[175]// Pulses
#define	 SM203_POSITION_6								mshm.RAM_DOUBLE[176]// Pulses
#define	 SM203_POSITION_7								mshm.RAM_DOUBLE[177]// Pulses
#define	 SM203_POSITION_8								mshm.RAM_DOUBLE[178]// Pulses

#define	 SM203_DISPENSE_Z_POSITION_1					mshm.SERVO_MOTOR_PARAMETERS[200]// Pulses
#define	 SM203_DISPENSE_Z_POSITION_2					mshm.SERVO_MOTOR_PARAMETERS[201]// Pulses
#define	 SM203_DISPENSE_Z_POSITION_3					mshm.SERVO_MOTOR_PARAMETERS[202]// Pulses
#define	 SM203_DISPENSE_Z_POSITION_4					mshm.SERVO_MOTOR_PARAMETERS[203]// Pulses
#define	 SM203_DISPENSE_Z_POSITION_5					mshm.SERVO_MOTOR_PARAMETERS[204]// Pulses

//---- SM204 -----------------------------------------------------------------------
#define	 SM204_AXIS_HOME_SPEED							mshm.SERVO_MOTOR_PARAMETERS[210]// Unit: 0.01deg/s 
#define	 SM204_AXIS_HOME_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[211]// Unit: ms
#define	 SM204_AXIS_JOG_SPEED							mshm.SERVO_MOTOR_PARAMETERS[212]// Unit: 0.01deg/s 
#define	 SM204_AXIS_JOG_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[213]// Unit: ms
#define	 SM204_AXIS_MOVE_SPEED							mshm.SERVO_MOTOR_PARAMETERS[214]// Unit: 0.01deg/s 
#define	 SM204_AXIS_MOVE_ACCEL_TIME						mshm.SERVO_MOTOR_PARAMETERS[215]// Unit: ms
														
#define	 SM204_SHORT_MOVE_DISTANCE						mshm.SERVO_MOTOR_PARAMETERS[220]// Pulses
#define	 SM204_HOME_OFFSET								mshm.SERVO_MOTOR_PARAMETERS[221]// Pulses
#define	 SM204_POSITION_1								mshm.RAM_DOUBLE[181]// Pulses
#define	 SM204_POSITION_2								mshm.RAM_DOUBLE[182]// Pulses
#define	 SM204_POSITION_3								mshm.RAM_DOUBLE[183]// Pulses
#define	 SM204_POSITION_4								mshm.RAM_DOUBLE[184]// Pulses
#define	 SM204_POSITION_5								mshm.RAM_DOUBLE[185]// Pulses
#define	 SM204_POSITION_6								mshm.RAM_DOUBLE[186]// Pulses
#define	 SM204_POSITION_7								mshm.RAM_DOUBLE[187]// Pulses
#define	 SM204_POSITION_8								mshm.RAM_DOUBLE[188]// Pulses

#define	 SM204_DISPENSE_R_POSITION_1					mshm.SERVO_MOTOR_PARAMETERS[230]// Pulses
#define	 SM204_DISPENSE_R_POSITION_2					mshm.SERVO_MOTOR_PARAMETERS[231]// Pulses
#define	 SM204_DISPENSE_R_POSITION_3					mshm.SERVO_MOTOR_PARAMETERS[232]// Pulses
#define	 SM204_DISPENSE_R_POSITION_4					mshm.SERVO_MOTOR_PARAMETERS[233]// Pulses
#define	 SM204_DISPENSE_R_POSITION_5					mshm.SERVO_MOTOR_PARAMETERS[234]// Pulses
														
//--- Servo Motor Position ---------------------------------------------------------
#define SM101_POSITION									mshm.SM101_ENCODER		// Pulses
#define SM102_POSITION									mshm.SM102_ENCODER		// Pulses
#define SM103_POSITION									mshm.SM103_ENCODER		// Pulses
#define SM104_POSITION									mshm.SM104_ENCODER		// Pulses
														
#define SM201_POSITION									mshm.SM201_ENCODER		// Pulses
#define SM202_POSITION									mshm.SM202_ENCODER		// Pulses
#define SM203_POSITION									mshm.SM203_ENCODER		// Pulses
#define SM204_POSITION									mshm.SM204_ENCODER		// Pulses

// ====================================================================================================
#define TIMER0											mshm.TIMER[0]

//////////////////////////////////////////////////////////////////////////
#define LINE_X_HOME_SPEED								SM201_AXIS_HOME_SPEED
#define LINE_X_HOME_ACCEL_TIME							SM201_AXIS_HOME_ACCEL_TIME
#define LINE_X_JOG_SPEED								SM201_AXIS_JOG_SPEED
#define LINE_X_JOG_ACCEL_TIME							SM201_AXIS_JOG_ACCEL_TIME
#define LINE_X_MOVE_SPEED								SM201_AXIS_MOVE_SPEED
#define LINE_X_MOVE_ACCEL_TIME							SM201_AXIS_MOVE_ACCEL_TIME
#define LINE_X_SHORT_MOVE_DISTANCE						SM201_SHORT_MOVE_DISTANCE
#define LINE_X_HOME_OFFSET								SM201_HOME_OFFSET
#define LINE_X_POSITION									SM201_POSITION
#define LINE_X_RATIO									SM201_PositionRatio
#define LINE_X_BUTTON_INDEX								150
#define LINE_X_MOVE_TO_HOME_POS							SM201_move_to_home_position()
#define LINE_X_STATUS									(mshm.SM201_MOTOR_STATUS)

#define TILT_Z_HOME_SPEED								SM103_AXIS_HOME_SPEED
#define TILT_Z_HOME_ACCEL_TIME							SM103_AXIS_HOME_ACCEL_TIME
#define TILT_Z_JOG_SPEED								SM103_AXIS_JOG_SPEED
#define TILT_Z_JOG_ACCEL_TIME							SM103_AXIS_JOG_ACCEL_TIME
#define TILT_Z_MOVE_SPEED								SM103_AXIS_MOVE_SPEED
#define TILT_Z_MOVE_ACCEL_TIME							SM103_AXIS_MOVE_ACCEL_TIME
#define TILT_Z_SHORT_MOVE_DISTANCE						SM103_SHORT_MOVE_DISTANCE
#define TILT_Z_HOME_OFFSET								SM103_HOME_OFFSET
#define TILT_Z_POSITION									SM103_POSITION
#define TILT_Z_RATIO									SM103_DegreeRatio
#define TILT_Z_BUTTON_INDEX								130
#define TILT_Z_MOVE_TO_HOME_POS							SM103_move_to_home_position()
#define TILT_Z_STATUS									(mshm.SM103_MOTOR_STATUS)

#define TILT_X_HOME_SPEED								SM101_AXIS_HOME_SPEED
#define TILT_X_HOME_ACCEL_TIME							SM101_AXIS_HOME_ACCEL_TIME
#define TILT_X_JOG_SPEED								SM101_AXIS_JOG_SPEED
#define TILT_X_JOG_ACCEL_TIME							SM101_AXIS_JOG_ACCEL_TIME
#define TILT_X_MOVE_SPEED								SM101_AXIS_MOVE_SPEED
#define TILT_X_MOVE_ACCEL_TIME							SM101_AXIS_MOVE_ACCEL_TIME
#define TILT_X_SHORT_MOVE_DISTANCE						SM101_SHORT_MOVE_DISTANCE
#define TILT_X_HOME_OFFSET								SM101_HOME_OFFSET
#define TILT_X_POSITION									SM101_POSITION
#define TILT_X_RATIO									SM101_DegreeRatio
#define TILT_X_BUTTON_INDEX								110
#define TILT_X_MOVE_TO_HOME_POS							SM101_move_to_home_position()
#define TILT_X_STATUS									(mshm.SM101_MOTOR_STATUS)

#define TILT_Y_HOME_SPEED								SM102_AXIS_HOME_SPEED
#define TILT_Y_HOME_ACCEL_TIME							SM102_AXIS_HOME_ACCEL_TIME
#define TILT_Y_JOG_SPEED								SM102_AXIS_JOG_SPEED
#define TILT_Y_JOG_ACCEL_TIME							SM102_AXIS_JOG_ACCEL_TIME
#define TILT_Y_MOVE_SPEED								SM102_AXIS_MOVE_SPEED
#define TILT_Y_MOVE_ACCEL_TIME							SM102_AXIS_MOVE_ACCEL_TIME
#define TILT_Y_SHORT_MOVE_DISTANCE						SM102_SHORT_MOVE_DISTANCE
#define TILT_Y_HOME_OFFSET								SM102_HOME_OFFSET
#define TILT_Y_POSITION									SM102_POSITION
#define TILT_Y_RATIO									SM102_DegreeRatio
#define TILT_Y_BUTTON_INDEX								120
#define TILT_Y_MOVE_TO_HOME_POS							SM102_move_to_home_position()
#define TILT_Y_STATUS									(mshm.SM102_MOTOR_STATUS)

#define LINE_Z_HOME_SPEED								SM203_AXIS_HOME_SPEED
#define LINE_Z_HOME_ACCEL_TIME							SM203_AXIS_HOME_ACCEL_TIME
#define LINE_Z_JOG_SPEED								SM203_AXIS_JOG_SPEED
#define LINE_Z_JOG_ACCEL_TIME							SM203_AXIS_JOG_ACCEL_TIME
#define LINE_Z_MOVE_SPEED								SM203_AXIS_MOVE_SPEED
#define LINE_Z_MOVE_ACCEL_TIME							SM203_AXIS_MOVE_ACCEL_TIME
#define LINE_Z_SHORT_MOVE_DISTANCE						SM203_SHORT_MOVE_DISTANCE
#define LINE_Z_HOME_OFFSET								SM203_HOME_OFFSET
#define LINE_Z_POSITION									SM203_POSITION
#define LINE_Z_RATIO									SM203_PositionRatio
#define LINE_Z_BUTTON_INDEX								170
#define LINE_Z_MOVE_TO_HOME_POS							SM203_move_to_home_position()
#define LINE_Z_STATUS									(mshm.SM203_MOTOR_STATUS)

#define LINE_Y_HOME_SPEED								SM202_AXIS_HOME_SPEED
#define LINE_Y_HOME_ACCEL_TIME							SM202_AXIS_HOME_ACCEL_TIME
#define LINE_Y_JOG_SPEED								SM202_AXIS_JOG_SPEED
#define LINE_Y_JOG_ACCEL_TIME							SM202_AXIS_JOG_ACCEL_TIME
#define LINE_Y_MOVE_SPEED								SM202_AXIS_MOVE_SPEED
#define LINE_Y_MOVE_ACCEL_TIME							SM202_AXIS_MOVE_ACCEL_TIME
#define LINE_Y_SHORT_MOVE_DISTANCE						SM202_SHORT_MOVE_DISTANCE
#define LINE_Y_HOME_OFFSET								SM202_HOME_OFFSET
#define LINE_Y_POSITION									SM202_POSITION
#define LINE_Y_RATIO									SM202_PositionRatio
#define LINE_Y_BUTTON_INDEX								160
#define LINE_Y_MOVE_TO_HOME_POS							SM202_move_to_home_position()
#define LINE_Y_STATUS									(mshm.SM202_MOTOR_STATUS)

extern int aa_tilt_process(void);
extern int move_step_z(double step_val);
extern int PI_move_step_xy(double value_x,double value_y);
extern int PI_move_step_x(double value);
extern int PI_move_step_y(double value);
extern int PI_move_step_z(double value);
extern int PI_move_step_xt(double value);
extern int PI_move_step_yt(double value);
extern int PI_move_step_yxt(double value,double value2);
extern int PI_move_to_point(double posArray[6]);
extern int search_z_process(void);
extern int focus_scan(void);
extern int use_tf2(void);
extern int aa_move_center(void);
extern int aa_move_offset(void);
extern int aa_move_offset2(void);
extern int fs_check(void);
extern int calculate_tilt_x_y(double step_unit);
extern int calibrate_tilt_angle(void);
extern int get_motion_move_center_offset(double x, double y, 
										double tx, double ty, 
										double A, double B, double &lx, double &ly);
extern int move_to_target_center(void);


extern void log(const char *format, ...);




#ifndef _S_DEFINE_H_
#define _S_DEFINE_H_
#include "shm.h"

#define IMG_DEV_FILE						0
#define IMG_DEV_360E						1
#define IMG_DEV_WEBCAM						2
#define IMG_DEV_MIL							3
				

//#define WEB_CAM_NUM						6 
#define TOOL_CNT							10
#define TEST_CNT							10
#define HW_CNT								20

#define FIXED_TEST_CNT						1


#define IMG_SOURCE_FILE						0
#define IMG_SOURCE_EXTERN_DEVICE			1
#define IMG_SOURCE_INTERNAL_DEVICE			2

#define MYTOOLBAR_HEIGHT					70

#define EXTERN_IMG_DEVICE_POWER_UP			1
#define EXTERN_IMG_DEVICE_GET_IMG			3
#define EXTERN_IMG_DEVICE_STOP				5

#define TEST_ITEM_NUM						4
#define TI_NONE							   -1
#define TI_DISPLAY_IMAGE					0
#define TI_STD_CHART						1
#define TI_REG_MARK_CHART					2
#define TI_WHITE							3

#define SAVE_OK_REG_PRE_REPORT				0
#define SAVE_NG_REG_PRE_REPORT				1
#define SAVE_OK_REG_REPORT					2
#define SAVE_NG_REG_REPORT					3
#define SAVE_REG_REPORT						4
#define SAVE_MES							5

#define HWND_APP							(s_aa::hwnd[0])
#define HWND_MAIN							(s_aa::hwnd[1])
#define HWND_STATUS							(s_aa::hwnd[2])
#define HWND_LOG							(s_aa::hwnd[3])
#define HWND_TEST_CONFIG					(s_aa::hwnd[4])

#define HWND_RECIPE							(s_aa::hwnd[8])
#define HWND_HISTOGRAM						(s_aa::hwnd[9])
#define HWND_MEASURE						(s_aa::hwnd[10])
#define HWND_GUIDELINE						(s_aa::hwnd[11])
#define HWND_IMG_INFO						(s_aa::hwnd[12])
#define HWND_HW_CONFIG						(s_aa::hwnd[13])
#define HWND_V5U							(s_aa::hwnd[14])

#define HWND_STD_CHART						(s_aa::hwnd[20])
#define HWND_STD_CURVE						(s_aa::hwnd[21])
#define HWND_REG_MARK_CHART					(s_aa::hwnd[22])
#define HWND_REG_MARK_SFRCURVE				(s_aa::hwnd[23])
#define HWND_WHITE							(s_aa::hwnd[24])
#define HWND_AA_MOTION						(s_aa::hwnd[25])
#define HWND_REG_MARK_TF					(s_aa::hwnd[26])
#define HWND_REG_MARK_CHART2				(s_aa::hwnd[27])
#define HWND_PI_MOTION						(s_aa::hwnd[28])
#define HWND_USER							(s_aa::hwnd[29])
#define HWND_SAVE_SET						(s_aa::hwnd[30])
#define HWND_OKNG_TEST						(s_aa::hwnd[31])

#define HWND_TOOL(n)						(s_aa::hwnd[100+((n>TOOL_CNT)?TOOL_CNT:n)])
#define HWND_HW_INDEX						(100+TOOL_CNT)
#define HWND_HW(n)							(s_aa::hwnd[HWND_HW_INDEX+((n>HW_CNT)?HW_CNT:n)])

#define HANDSHAKE_GET_SHM					(s_aa::ram_int[0])
#define IMG_CAPTURE_READY					(s_aa::ram_int[1])
#define EXTERN_INPUT_IMG_SOURCE_OCCUPIED	(s_aa::ram_int[2])
#define SELECT_IMG_SOURCE					(s_aa::ram_int[3])

#define IMG_DEVICE_CNT						(s_aa::ram_int[4])
#define IMG_DEVICE_INDEX					(s_aa::ram_int[5])
#define IMG_DEVICE_RUN						(s_aa::ram_int[6])

#define IS_IMAGE_SCENERY_OCCUPIED			(s_aa::ram_int[6])
#define SELECT_INTERNAL_DEVICE				(s_aa::ram_int[8])
#define DEVICE_START_INIT					(s_aa::ram_int[9])

#define TEST_START							(s_aa::ram_int[10])
#define TEST_NEXT							(s_aa::ram_int[11])
#define TEST_STOP							(s_aa::ram_int[12])

#define IMG_DEVICE_INITED					(s_aa::ram_int[13])
#define WEB_CAM_INITED						(s_aa::ram_int[14])

#define APP_STATUS							(s_aa::ram_int[18])
#define IMPORT_TEST_MODULE_CNT				(s_aa::ram_int[19])

#define ENABLE_IMG_ZOOM_IN					(s_aa::ram_int[20])
#define MOUSE_START_X						(s_aa::ram_int[21])
#define MOUSE_START_Y						(s_aa::ram_int[22])
#define MOUSE_END_X							(s_aa::ram_int[23])
#define MOUSE_END_Y							(s_aa::ram_int[24])

#define DRAW_ZOOM_IN_AREA_DONE				(s_aa::ram_int[25])
#define DRAW_MOVING_MOUSE					(s_aa::ram_int[26])

#define ENABLE_MEASURE						(s_aa::ram_int[30])
#define MEASURE_START_X						(s_aa::ram_int[31])
#define MEASURE_START_Y						(s_aa::ram_int[32])
#define MEASURE_END_X						(s_aa::ram_int[33])
#define MEASURE_END_Y						(s_aa::ram_int[34])
#define MEASURE_DRAW_RECT_DONE				(s_aa::ram_int[35])

#define IS_HISTOGRAM_DLG_SHOW				(s_aa::ram_int[36])

#define MOUSE_X								(s_aa::ram_int[40])
#define MOUSE_Y								(s_aa::ram_int[41])
#define MOUSE_LDOWN							(s_aa::ram_int[42])
#define MOUSE_LDOWN_MOVE					(s_aa::ram_int[43])
#define MOUSE_LUP							(s_aa::ram_int[44])
#define MOUSE_LDBCLICK						(s_aa::ram_int[45])

#define VISION_COMMAND						(s_aa::ram_int[50])
#define IS_PLC_CONNECTED					(s_aa::ram_int[51])

#define WEB_CAM_NUM							(s_aa::ram_int[52])
#define EXECUTE_HOME						(s_aa::ram_int[53])

#define WEB_CAM_TF_TRIGGER_ON				(s_aa::ram_int[54])

#define IS_LOGIN_SUCCEED					(s_aa::ram_int[55])

#define IMG_RATIO_X							(s_aa::ram_double[25])
#define IMG_RATIO_Y							(s_aa::ram_double[26])

#define WEB_CAM_WIDTH						(s_aa::ram_int[94])
#define WEB_CAM_HEIGHT						(s_aa::ram_int[95])

#define DISPLAY_IMG_START_X					(s_aa::ram_int[96])
#define DISPLAY_IMG_START_Y					(s_aa::ram_int[97])
#define DISPLAY_IMG_WIDTH					(s_aa::ram_int[98])
#define DISPLAY_IMG_HEIGHT					(s_aa::ram_int[99])

#define IMG_WIDTH							(s_aa::ram_int[100])
#define IMG_HEIGHT							(s_aa::ram_int[101])
#define IMG_DEPTH							(s_aa::ram_int[102])
#define IMG_FORMAT							(s_aa::ram_int[103])
#define IMG_PIXEL_ORDER						(s_aa::ram_int[104])
#define IMG_RAW_FORMAT						(s_aa::ram_int[105])
#define IMG_SELECT_DEMOSAIC					(s_aa::ram_int[106])
#define IMG_RAW_HEADER						(s_aa::ram_int[107])
#define IMG_DUMMY_CNT						(s_aa::ram_int[108])
#define IMG_AVERAGE_CNT						(s_aa::ram_int[109])
#define IMG_DISPLAY_MODE					(s_aa::ram_int[110])

#define HW_NUM								(s_aa::ram_int[111])
#define TESTER_ID							(s_aa::ram_int[112])

#define COMM_CONNECTED						(s_aa::ram_int[112])
#define COMM_PORT							(s_aa::ram_int[113])
#define COMM_THREAD_ON						(s_aa::ram_int[114])
#define EXECUTE_CMD							(s_aa::ram_int[115])
#define EXECUTE_ERROR						(s_aa::ram_int[116])
#define SHOW_IMG							(s_aa::ram_int[117])

#define PROCESS_TEST						(s_aa::ram_int[120])

#define ERROR_ON							(s_aa::ram_int[121])
#define ERROR_CODE							(s_aa::ram_int[122])

#define START_DISPLAY_IMAGE					(s_aa::ram_int[130])
#define START_STD_CHART_TEST				(s_aa::ram_int[131])
#define START_REG_MARK_CHART_TEST			(s_aa::ram_int[132])
#define START_WHITE_TEST					(s_aa::ram_int[133])

#define RESULT_LOAD_TOOL(n)					(s_aa::ram_int[200+((n>TOOL_CNT)?TOOL_CNT:n)])
#define RESULT_LOAD_HW_INDEX				(200+TOOL_CNT)
#define RESULT_LOAD_HW(n)					(s_aa::ram_int[RESULT_LOAD_HW_INDEX+((n>HW_CNT)?HW_CNT:n)])

#define HANDSHAKE_TRIGGER_SIGNAL			(shm->RAM_INT[20])
#define HANDSHAKE_RECEIVE_SIGNAL			(shm->RAM_INT[21])
#define HANDSHAKE_REQUEST_IMG_SIGNAL		(shm->RAM_INT[23])
#define HANDSHAKE_LINKED					(shm->RAM_INT[24])

#define EXT_CAM_IMG_WIDTH					(shm->RAM_INT[100])
#define EXT_CAM_IMG_HEIGHT					(shm->RAM_INT[101])
#define EXT_CAM_IMG_DEPTH					(shm->RAM_INT[102])
#define EXT_CAM_IMG_FORMAT					(shm->RAM_INT[103])
#define EXT_CAM_IMG_PIXEL_ORDER				(shm->RAM_INT[104])
#define EXT_CAM_IMG_RAW_FORMAT				(shm->RAM_INT[105])
#define EXT_CAM_IMG_SELECT_DEMOSAIC			(shm->RAM_INT[106])
#define EXT_CAM_IMG_RAW_HEADER				(shm->RAM_INT[107])
															  
											
#define LOOP_CNT							100
#define STD_FOCUS_CURVE_RANGE				100

#define ID_MENU_TOOL(n)						(50000+((n>TOOL_CNT)?TOOL_CNT:n))
#define ID_MENU_HW_INDEX					(50000+TOOL_CNT)
#define ID_MENU_HW(n)						(ID_MENU_HW_INDEX+((n>HW_CNT)?HW_CNT:n))


#define SBINDEX_VERSION						1
#define SBINDEX_LINK						2
#define SBINDEX_IMAGE_INFO					3
#define SBINDEX_FRAMERATE					4
#define SBINDEX_TEXT						5


#define ERR_CREATE_TEST_EVENT				0
#define ERR_TEST_CAN_NOT_STOP				1
#define ERR_CREATE_TEST_MUTEX				2
#define ERR_WAIT_TEST_MUTEX					3
#define ERR_INIT_IMG_DEVICE					4
#define ERR_TEST_OCCUPIED					5
#define ERR_NO_IMG_DEVICE					6
#define ERR_OPEN_DEVICE						7
#define ERR_RUN_DEVICE						8

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#endif
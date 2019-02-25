//=========================================================================================  
//	Project:		U100-F
//	Program:		defio.h
//	Description:	Digital Input/Output point address definition and mapping
//	date:			2016-10-27
//===========================================================================================

#define inpb 			PISODIO_InputByte
#define outpb 			PISODIO_OutputByte

//==================================================================================  
#define ON				0x01
#define OFF				0x00
#define Normal			==0x00)?ON:OFF) 
#define Inverse			==0x00)?OFF:ON)

//======================================================================================
// General definition for Digital input / Digital output
//======================================================================================
//INPUT0
#define INPUT0	inpb((WORD)PEXP32C32_BoardNo, (WORD)0)

#define S01		(((INPUT0 & 0x01)Normal      //DI_0
#define	S02		(((INPUT0 & 0x02)Normal      //DI_1
#define	S03		(((INPUT0 & 0x04)Normal      //DI_2
#define	S04 	(((INPUT0 & 0x08)Normal	     //DI_3
#define	S05		(((INPUT0 & 0x10)Normal      //DI_4
#define	S06		(((INPUT0 & 0x20)Normal      //DI_5
#define S07		(((INPUT0 & 0x40)Normal      //DI_6
#define S08		(((INPUT0 & 0x80)Normal      //DI_7

//INPUT1
#define INPUT1	inpb((WORD)PEXP32C32_BoardNo, (WORD)1)

#define S09 	(((INPUT1 & 0x01)Normal      //DI_8  
#define S10		(((INPUT1 & 0x02)Normal      //DI_9
#define S11		(((INPUT1 & 0x04)Normal      //DI_10
#define S12		(((INPUT1 & 0x08)Normal      //DI_11
#define S13		(((INPUT1 & 0x10)Normal      //DI_12
#define S14		(((INPUT1 & 0x20)Normal      //DI_13
#define S15		(((INPUT1 & 0x40)Normal      //DI_14
#define S16		(((INPUT1 & 0x80)Normal      //DI_15

//INPUT2
#define INPUT2	inpb((WORD)PEXP32C32_BoardNo, (WORD)2)

#define S17		(((INPUT2 & 0x01)Normal      // DI_16
#define	S18		(((INPUT2 & 0x02)Normal      // DI_17
#define	S19		(((INPUT2 & 0x04)Normal      // DI_18
#define	S20 	(((INPUT2 & 0x08)Normal      // DI_19
#define	S21		(((INPUT2 & 0x10)Normal      // DI_20
#define	S22		(((INPUT2 & 0x20)Normal      // DI_21
#define S23		(((INPUT2 & 0x40)Normal      // DI_22
#define S24		(((INPUT2 & 0x80)Normal      // DI_23

//INPUT3
#define INPUT3	inpb((WORD)PEXP32C32_BoardNo, (WORD)3)
#define S25 	(((INPUT3 & 0x01)Normal      // DI_24 
#define S26		(((INPUT3 & 0x02)Normal      // DI_25
#define S27		(((INPUT3 & 0x04)Normal      // DI_26
#define S28		(((INPUT3 & 0x08)Normal      // DI_27
#define S29		(((INPUT3 & 0x10)Normal      // DI_28
#define S30		(((INPUT3 & 0x20)Normal      // DI_29
#define S31		(((INPUT3 & 0x40)Normal      // DI_30
#define S32		(((INPUT3 & 0x80)Normal      // DI_31

//==========================================================================================
// DEFINE INPUT
#define SITE1_CONTACT_CY_UP_SENSOR							S01
#define SITE1_CONTACT_CY_DOWN_SENSOR						S02
#define SITE2_CONTACT_CY_UP_SENSOR							S03
#define SITE2_CONTACT_CY_DOWN_SENSOR						S04
#define SITE3_CONTACT_CY_UP_SENSOR							S05
#define SITE3_CONTACT_CY_DOWN_SENSOR						S06
#define SITE4_CONTACT_CY_UP_SENSOR							S07
#define SITE4_CONTACT_CY_DOWN_SENSOR						S08

#define SITE2_TEST_TARGET_UP_SENSOR							S11
#define SITE2_TEST_TARGET_DOWN_SENSOR						S12
#define SITE3_CHART_IN_SENSOR								S11
#define SITE3_CHART_OUT_SENSOR								S12
#define SITE4_RIGHT_SENSOR									S13
#define SITE4_LEFT_SENSOR									S14
#define SITE4_TEST_TARGET_UP_SENSOR							S15
#define SITE4_TEST_TARGET_DOWN_SENSOR						S16
//-----------------------------------------------------
#define HW_START_BUTTON										S17
#define HW_SKIP_BUTTON										S18
#define HW_STOP_BUTTON										S18
#define HW_HOME_BUTTON										S20
#define SAFETY_OPERATION_SENSOR								S21
#define RESERVED______S22									S22
#define AIR_SUPPLY_SENSOR									S23
#define RESERVED______S24									S24

#define SOCKET_PROECT_DETECT								S25
#define BARCODE_SCAN_DEVICE_DETECT_SENSOR					S26
#define REAR_DOOR_LOCK_SENSOR								S27
#define LEFT_DOOR_LOCK_SENSOR								S28
#define RIGHT_DOOR_LOCK_SENSOR								S29
#define HW_EMERGENCY_STOP_BUTTON							(S30^1)
#define RESERVED______S31									S31
#define SAFETY_DOOR_SENSOR									S32

//OUTPUT0===========================================================================
#define OUTPUT0			outpb((WORD)PEXP32C32_BoardNo, (WORD)0, (WORD)(shm->OUTPUT0_STATUS
#define	Y01_ON			OUTPUT0 |= 0x01))	
#define	Y01_OFF			OUTPUT0 &= 0xfe))    
#define	Y01_TOGGLE		OUTPUT0 ^= 0x01))	

#define Y02_ON			OUTPUT0 |= 0x02))	
#define Y02_OFF			OUTPUT0 &= 0xfd))
#define	Y02_TOGGLE		OUTPUT0 ^= 0x02))	

#define Y03_ON			OUTPUT0 |= 0x04))	
#define Y03_OFF			OUTPUT0 &= 0xfb))
#define	Y03_TOGGLE		OUTPUT0 ^= 0x04))	

#define Y04_ON			OUTPUT0 |= 0x08))	
#define Y04_OFF			OUTPUT0 &= 0xf7))
#define	Y04_TOGGLE		OUTPUT0 ^= 0x08))	

#define Y05_ON			OUTPUT0 |= 0x10))	
#define Y05_OFF			OUTPUT0 &= 0xef))
#define	Y05_TOGGLE		OUTPUT0 ^= 0x10))	

#define Y06_ON			OUTPUT0 |= 0x20))	
#define Y06_OFF			OUTPUT0 &= 0xdf))
#define	Y06_TOGGLE		OUTPUT0 ^= 0x20))	

#define Y07_ON			OUTPUT0 |= 0x40))	
#define Y07_OFF			OUTPUT0 &= 0xbf))
#define	Y07_TOGGLE		OUTPUT0 ^= 0x40))	

#define Y08_ON			OUTPUT0 |= 0x80))	
#define Y08_OFF			OUTPUT0 &= 0x7f))
#define	Y08_TOGGLE		OUTPUT0 ^= 0x80))	

//OUTPUT1===========================================================================
#define OUTPUT1			outpb((WORD)0/*PEXP32C32_BoardNo*/, (WORD)1, (WORD)(shm->OUTPUT1_STATUS

#define	Y09_ON			OUTPUT1 |= 0x01))	
#define	Y09_OFF			OUTPUT1 &= 0xfe)) 
#define	Y09_TOGGLE		OUTPUT1 ^= 0x01))	

#define	Y10_ON			OUTPUT1 |= 0x02))	
#define	Y10_OFF			OUTPUT1 &= 0xfd)) 
#define	Y10_TOGGLE		OUTPUT1 ^= 0x02))	

#define	Y11_ON			OUTPUT1 |= 0x04))	
#define	Y11_OFF			OUTPUT1 &= 0xfb)) 
#define	Y11_TOGGLE		OUTPUT1 ^= 0x04))	

#define	Y12_ON			OUTPUT1 |= 0x08))	
#define	Y12_OFF			OUTPUT1 &= 0xf7)) 
#define	Y12_TOGGLE		OUTPUT1 ^= 0x08))	

#define	Y13_ON			OUTPUT1 |= 0x10))	
#define	Y13_OFF			OUTPUT1 &= 0xef)) 
#define	Y13_TOGGLE		OUTPUT1 ^= 0x10))	

#define	Y14_ON			OUTPUT1 |= 0x20))	
#define	Y14_OFF			OUTPUT1 &= 0xdf)) 
#define	Y14_TOGGLE		OUTPUT1 ^= 0x20))	

#define	Y15_ON			OUTPUT1 |= 0x40))	
#define	Y15_OFF			OUTPUT1 &= 0xbf))	
#define	Y15_TOGGLE		OUTPUT1 ^= 0x40))	

#define	Y16_ON			OUTPUT1 |= 0x80))	
#define	Y16_OFF			OUTPUT1 &= 0x7f))	
#define	Y16_TOGGLE		OUTPUT1 ^= 0x80))	

//OUTPUT2 ==========================================================================
#define OUTPUT2			outpb((WORD)0/*PEXP32C32_BoardNo*/, (WORD)2, (WORD)(shm->OUTPUT2_STATUS

#define Y17_ON			OUTPUT2 |= 0x01))   
#define Y17_OFF			OUTPUT2 &= 0xfe))   
#define	Y17_TOGGLE		OUTPUT2 ^= 0x01))	

#define Y18_ON			OUTPUT2 |= 0x02))   
#define Y18_OFF			OUTPUT2 &= 0xfd))
#define	Y18_TOGGLE		OUTPUT2 ^= 0x02))	

#define Y19_ON			OUTPUT2 |= 0x04))   
#define Y19_OFF			OUTPUT2 &= 0xfb))
#define	Y19_TOGGLE		OUTPUT2 ^= 0x04))	

#define Y20_ON			OUTPUT2 |= 0x08))   
#define Y20_OFF			OUTPUT2 &= 0xf7))
#define	Y20_TOGGLE		OUTPUT2 ^= 0x08))	

#define Y21_ON			OUTPUT2 |= 0x10))   
#define Y21_OFF			OUTPUT2 &= 0xef))
#define	Y21_TOGGLE		OUTPUT2 ^= 0x10))	

#define Y22_ON			OUTPUT2 |= 0x20))   
#define Y22_OFF			OUTPUT2 &= 0xdf))
#define	Y22_TOGGLE		OUTPUT2 ^= 0x20))	

#define Y23_ON			OUTPUT2 |= 0x40))   
#define Y23_OFF			OUTPUT2 &= 0xbf))
#define	Y23_TOGGLE		OUTPUT2 ^= 0x40))	

#define Y24_ON			OUTPUT2 |= 0x80))   
#define Y24_OFF			OUTPUT2 &= 0x7f))
#define	Y24_TOGGLE		OUTPUT2 ^= 0x80))	

//OUTPUT3 ==========================================================================
#define OUTPUT3			outpb((WORD)0/*PEXP32C32_BoardNo*/, (WORD)3, (WORD)(shm->OUTPUT3_STATUS

#define Y25_ON			OUTPUT3 |= 0x01))    
#define Y25_OFF			OUTPUT3 &= 0xfe))
#define	Y25_TOGGLE		OUTPUT3 ^= 0x01))	

#define Y26_ON			OUTPUT3 |= 0x02))    
#define Y26_OFF			OUTPUT3 &= 0xfd))
#define	Y26_TOGGLE		OUTPUT3 ^= 0x02))	

#define Y27_ON			OUTPUT3 |= 0x04))    
#define Y27_OFF			OUTPUT3 &= 0xfb))
#define	Y27_TOGGLE		OUTPUT3 ^= 0x04))	

#define Y28_ON			OUTPUT3 |= 0x08))    
#define Y28_OFF			OUTPUT3 &= 0xf7))
#define	Y28_TOGGLE		OUTPUT3 ^= 0x08))	

#define Y29_ON			OUTPUT3 |= 0x10))    
#define Y29_OFF			OUTPUT3 &= 0xef))
#define	Y29_TOGGLE		OUTPUT3 ^= 0x10))	

#define Y30_ON			OUTPUT3 |= 0x20))    
#define Y30_OFF			OUTPUT3 &= 0xdf))
#define	Y30_TOGGLE		OUTPUT3 ^= 0x20))	

#define	Y31_ON			OUTPUT3 |= 0x40))	
#define	Y31_OFF			OUTPUT3 &= 0xbf)) 
#define	Y31_TOGGLE		OUTPUT3 ^= 0x40))	

#define	Y32_ON			OUTPUT3 |= 0x80))	
#define	Y32_OFF			OUTPUT3 &= 0x7f)) 
#define	Y32_TOGGLE		OUTPUT3 ^= 0x80))	

//========================================================================================
// DEFINE OUTPUT CHECK

#define Y01_CHECK		(((shm->OUTPUT0_STATUS & 0x01)==0x01) ? ON:OFF)
#define Y02_CHECK		(((shm->OUTPUT0_STATUS & 0x02)==0x02) ? ON:OFF)
#define Y03_CHECK		(((shm->OUTPUT0_STATUS & 0x04)==0x04) ? ON:OFF)
#define Y04_CHECK		(((shm->OUTPUT0_STATUS & 0x08)==0x08) ? ON:OFF)
#define Y05_CHECK		(((shm->OUTPUT0_STATUS & 0x10)==0x10) ? ON:OFF)
#define Y06_CHECK		(((shm->OUTPUT0_STATUS & 0x20)==0x20) ? ON:OFF)
#define Y07_CHECK		(((shm->OUTPUT0_STATUS & 0x40)==0x40) ? ON:OFF)
#define Y08_CHECK		(((shm->OUTPUT0_STATUS & 0x80)==0x80) ? ON:OFF)

#define Y09_CHECK		(((shm->OUTPUT1_STATUS & 0x01)==0x01) ? ON:OFF)
#define Y10_CHECK		(((shm->OUTPUT1_STATUS & 0x02)==0x02) ? ON:OFF)
#define Y11_CHECK		(((shm->OUTPUT1_STATUS & 0x04)==0x04) ? ON:OFF)
#define Y12_CHECK		(((shm->OUTPUT1_STATUS & 0x08)==0x08) ? ON:OFF)
#define Y13_CHECK		(((shm->OUTPUT1_STATUS & 0x10)==0x10) ? ON:OFF)
#define Y14_CHECK		(((shm->OUTPUT1_STATUS & 0x20)==0x20) ? ON:OFF)
#define Y15_CHECK		(((shm->OUTPUT1_STATUS & 0x40)==0x40) ? ON:OFF)
#define Y16_CHECK		(((shm->OUTPUT1_STATUS & 0x80)==0x80) ? ON:OFF)
//--------------------------------------------------------------------------
#define Y17_CHECK		(((shm->OUTPUT2_STATUS & 0x01)==0x01) ? ON:OFF)
#define Y18_CHECK		(((shm->OUTPUT2_STATUS & 0x02)==0x02) ? ON:OFF)
#define Y19_CHECK		(((shm->OUTPUT2_STATUS & 0x04)==0x04) ? ON:OFF)
#define Y20_CHECK		(((shm->OUTPUT2_STATUS & 0x08)==0x08) ? ON:OFF)
#define Y21_CHECK		(((shm->OUTPUT2_STATUS & 0x10)==0x10) ? ON:OFF)
#define Y22_CHECK		(((shm->OUTPUT2_STATUS & 0x20)==0x20) ? ON:OFF)
#define Y23_CHECK		(((shm->OUTPUT2_STATUS & 0x40)==0x40) ? ON:OFF)
#define Y24_CHECK		(((shm->OUTPUT2_STATUS & 0x80)==0x80) ? ON:OFF)

#define Y25_CHECK		(((shm->OUTPUT3_STATUS & 0x01)==0x01) ? ON:OFF)
#define Y26_CHECK		(((shm->OUTPUT3_STATUS & 0x02)==0x02) ? ON:OFF)
#define Y27_CHECK		(((shm->OUTPUT3_STATUS & 0x04)==0x04) ? ON:OFF)
#define Y28_CHECK		(((shm->OUTPUT3_STATUS & 0x08)==0x08) ? ON:OFF)
#define Y29_CHECK		(((shm->OUTPUT3_STATUS & 0x10)==0x10) ? ON:OFF)
#define Y30_CHECK		(((shm->OUTPUT3_STATUS & 0x20)==0x20) ? ON:OFF)
#define Y31_CHECK		(((shm->OUTPUT3_STATUS & 0x40)==0x40) ? ON:OFF)
#define Y32_CHECK		(((shm->OUTPUT3_STATUS & 0x80)==0x80) ? ON:OFF)

//========================================================================================
// DEFINE OUTPUT

#define	SITE1_CONTACT_CY_DOWN						Y01_OFF
#define	SITE1_CONTACT_CY_UP							Y01_ON

#define	SITE2_CONTACT_CY_DOWN						Y02_OFF
#define	SITE2_CONTACT_CY_UP							Y02_ON

#define	SITE3_CONTACT_CY_DOWN						Y03_OFF
#define	SITE3_CONTACT_CY_UP							Y03_ON

#define	SITE4_CONTACT_CY_DOWN						Y04_OFF
#define	SITE4_CONTACT_CY_UP							Y04_ON

#define	SITE2_TEST_TARGET_MOVE_DOWN_OFF				Y05_OFF
#define	SITE2_TEST_TARGET_MOVE_DOWN_ON				Y05_ON

#define SITE2_TEST_TARGET_MOVE_UP_ON				Y06_ON
#define SITE2_TEST_TARGET_MOVE_UP_OFF				Y06_OFF

#define	SITE4_TEST_TARGET_MOVE_DOWN_OFF				Y07_OFF
#define	SITE4_TEST_TARGET_MOVE_DOWN_ON				Y07_ON

#define	SITE4_TEST_TARGET_MOVE_UP_OFF				Y08_OFF
#define	SITE4_TEST_TARGET_MOVE_UP_ON				Y08_ON

#define RED_LIGHT_ON								Y09_ON
#define RED_LIGHT_OFF								Y09_OFF

#define YELLOW_LIGHT_ON								Y10_ON
#define YELLOW_LIGHT_OFF							Y10_OFF

#define GREEN_LIGHT_ON								Y11_ON
#define GREEN_LIGHT_OFF								Y11_OFF

#define BUZZER_ON									Y12_ON
#define BUZZER_OFF									Y12_OFF

#define TEST_PASS_SIGNAL_ON							Y13_ON				
#define TEST_PASS_SIGNAL_OFF						Y13_OFF	

#define TEST_FAIL_SIGNAL_ON							Y14_ON
#define TEST_FAIL_SIGNAL_OFF						Y14_OFF

#define	RESERVED_Y15_OFF___________					Y15_OFF
#define	RESERVED_Y15_ON____________					Y15_ON

#define BARCODE_TRIGGER_ON							Y16_ON
#define BARCODE_TRIGGER_OFF							Y16_OFF

#define START_BUTTON_LAMP_ON						Y17_ON
#define START_BUTTON_LAMP_OFF						Y17_OFF

#define STOP_BUTTON_LAMP_ON							Y18_ON
#define STOP_BUTTON_LAMP_OFF						Y18_OFF

#define HOME_BUTTON_LAMP_ON							Y20_ON
#define HOME_BUTTON_LAMP_OFF						Y20_OFF

#define	AIR_SUPPLY_VALVE_OFF						Y21_OFF
#define	AIR_SUPPLY_VALVE_ON							Y21_ON
#define SM101_BRAKE_OFF								Y22_ON	
#define SM101_BRAKE_ON								Y22_OFF

#define SM101_BRAKE_CHECK							(Y22_CHECK^0x01)	

#define SM104_ALARM_RESET_ON						Y23_ON
#define SM104_ALARM_RESET_OFF						Y23_OFF

#define SM101_ALARM_RESET_ON						Y24_ON		//RESERVED
#define SM101_ALARM_RESET_OFF						Y24_OFF		//RESERVED



#define COLOR_TEMP_SWITCH1_ON						Y25_ON
#define COLOR_TEMP_SWITCH1_OFF						Y25_OFF

#define COLOR_TEMP_SWITCH2_ON						Y26_ON
#define COLOR_TEMP_SWITCH2_OFF						Y26_OFF

#define REAR_DOOR_LOCK_ON							Y27_ON
#define REAR_DOOR_LOCK_OFF							Y27_OFF

#define LEFT_DOOR_LOCK_ON							Y28_ON
#define LEFT_DOOR_LOCK_OFF							Y28_OFF

#define RIGHT_DOOR_LOCK_ON							Y29_ON
#define RIGHT_DOOR_LOCK_OFF							Y29_OFF

#define SM102_ALARM_RESET_ON						Y30_ON		//RESERVED
#define SM102_ALARM_RESET_OFF						Y30_OFF		//RESERVED

#define SM103_ALARM_RESET_ON						Y30_ON		//RESERVED
#define SM103_ALARM_RESET_OFF						Y30_OFF		//RESERVED






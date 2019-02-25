#include "interface.h"
#include "inf_motion_aa.h"
#include "inf_motion_pi.h"
#include "test_reg_mark_chart.h"

/*按钮反馈*/
extern MOTION_SHM mshm;
extern SlantedBW mysbw;
extern int moving_down;
extern int trans_button;


DWORD Tscreen(LPVOID lpvThreadParm)
{
	int cnt = 0;
	int err = 0;
	int timeout = 0;
	while (1){
		Sleep(200);
		MANUAL_MOVING = ON;
		number = BUTTON_VALUE;
		if (number == 0x00) continue;
		else{
			BUTTON_VALUE = 0x00;
		}
		
		HomeMachine_StopFlag = FALSE;
		SetDlgItemText(HWND_AA_MOTION, IDC_STATUS2, "");
		log("init:button_val:%d", number);
		switch (number){
//MOTOR 101
		case 110: SM101_SERVO_ON;  MANUAL_ERROR = OFF;    break;
		case 111: SM101_SERVO_OFF; MANUAL_ERROR = OFF;    break;
		case 113: MANUAL_ERROR = SM101_encoder_reset();   break;	
		case 114: MANUAL_ERROR = SM101_jog_cw();		  break;
		case 115: MANUAL_ERROR = SM101_jog_ccw();		  break;
		case 116: MANUAL_ERROR = OFF; SM101_motor_stop(); break;
		case 117: MANUAL_ERROR = SM101_relative_short_move(1*SM101_SHORT_MOVE_DISTANCE);  break;
		case 118: MANUAL_ERROR = SM101_relative_short_move(-1*SM101_SHORT_MOVE_DISTANCE); break;
//MOTOR 102
		case 120: SM102_SERVO_ON;  MANUAL_ERROR = OFF;	  break;
		case 121: SM102_SERVO_OFF; MANUAL_ERROR = OFF;	  break;
		case 123: MANUAL_ERROR = SM102_encoder_reset();	  break;	
		case 124: MANUAL_ERROR = SM102_jog_cw();		  break;
		case 125: MANUAL_ERROR = SM102_jog_ccw();		  break;
		case 126: MANUAL_ERROR = OFF; SM102_motor_stop(); break;
		case 127: MANUAL_ERROR = SM102_relative_short_move(1*SM102_SHORT_MOVE_DISTANCE);  break;
		case 128: MANUAL_ERROR = SM102_relative_short_move(-1*SM102_SHORT_MOVE_DISTANCE); break;
//MOTOR 103
		case 130: SM103_SERVO_ON; MANUAL_ERROR = OFF;	  break;
		case 131: SM103_SERVO_OFF; MANUAL_ERROR = OFF;	  break;
		case 133: MANUAL_ERROR = SM103_encoder_reset();	  break;
		case 134: MANUAL_ERROR = SM103_jog_cw();		  break;
		case 135: MANUAL_ERROR = SM103_jog_ccw();		  break;
		case 136: SM103_motor_stop();MANUAL_ERROR = OFF;  break;
		case 137: MANUAL_ERROR = SM103_relative_short_move(1*SM103_SHORT_MOVE_DISTANCE);  break;
		case 138: MANUAL_ERROR = SM103_relative_short_move(-1*SM103_SHORT_MOVE_DISTANCE); break;
//MOTOR 104
// 		case 140: SM104_SERVO_ON; MANUAL_ERROR = OFF;	  break;
// 		case 141: SM104_SERVO_OFF; MANUAL_ERROR = OFF;	  break;
// 		case 143: MANUAL_ERROR = SM104_encoder_reset();	  break;
// 		case 144: MANUAL_ERROR = SM104_jog_cw();		  break;
// 		case 145: MANUAL_ERROR = SM104_jog_ccw();		  break;
// 		case 146: SM104_motor_stop();MANUAL_ERROR = OFF;  break;
// 		case 147: MANUAL_ERROR = SM104_relative_short_move(1*SM104_SHORT_MOVE_DISTANCE);  break;
// 		case 148: MANUAL_ERROR = SM104_relative_short_move(-1*SM104_SHORT_MOVE_DISTANCE); break;  not use this axis
//MOTOR 201
		case 150: SM201_SERVO_ON;  MANUAL_ERROR = OFF;	  break;
		case 151: SM201_SERVO_OFF; MANUAL_ERROR = OFF; 	  break;
		case 153: MANUAL_ERROR = SM201_encoder_reset();   break;			
		case 154: MANUAL_ERROR = SM201_jog_cw();		  break;
		case 155: MANUAL_ERROR = SM201_jog_ccw();		  break;
		case 156: SM201_motor_stop(); MANUAL_ERROR = OFF; break;
		case 157: MANUAL_ERROR = SM201_relative_short_move(1*SM201_SHORT_MOVE_DISTANCE);  break;
		case 158: MANUAL_ERROR = SM201_relative_short_move(-1*SM201_SHORT_MOVE_DISTANCE); break;
//MOTOR 202
		case 160: SM202_SERVO_ON;  MANUAL_ERROR = OFF;	  break;
		case 161: SM202_SERVO_OFF; MANUAL_ERROR = OFF; 	  break;
		case 163: log("163");MANUAL_ERROR = SM202_encoder_reset();   break;			
		case 164: MANUAL_ERROR = SM202_jog_cw();		  break;
		case 165: MANUAL_ERROR = SM202_jog_ccw();		  break;
		case 166: SM202_motor_stop(); MANUAL_ERROR = OFF; break;
		case 167: MANUAL_ERROR = SM202_relative_short_move(1*SM202_SHORT_MOVE_DISTANCE);  break;
		case 168: MANUAL_ERROR = SM202_relative_short_move(-1*SM202_SHORT_MOVE_DISTANCE); break;
//MOTOR 203
		case 170: SM203_SERVO_ON;  MANUAL_ERROR = OFF;	  break;
		case 171: SM203_SERVO_OFF; MANUAL_ERROR = OFF;	  break;
		case 173: MANUAL_ERROR = SM203_encoder_reset();	  break;	
		case 174: MANUAL_ERROR = SM203_jog_cw();		  break;
		case 175: MANUAL_ERROR = SM203_jog_ccw();		  break;
		case 176: SM203_motor_stop(); MANUAL_ERROR = OFF; break;
		case 177: MANUAL_ERROR = SM203_relative_short_move(1*SM203_SHORT_MOVE_DISTANCE);  break;
		case 178: MANUAL_ERROR = SM203_relative_short_move(-1*SM203_SHORT_MOVE_DISTANCE); break;
//MOTOR 204
// 		case 180: SM204_SERVO_ON; MANUAL_ERROR = OFF;	  break;
// 		case 181: SM204_SERVO_OFF; MANUAL_ERROR = OFF;	  break;
// 		case 183: MANUAL_ERROR = SM204_encoder_reset();	  break;
// 		case 184: MANUAL_ERROR = SM204_jog_cw();		  break;
// 		case 185: MANUAL_ERROR = SM204_jog_ccw();		  break;
// 		case 186: SM204_motor_stop();MANUAL_ERROR = OFF;  break;
// 		case 187: MANUAL_ERROR = SM204_relative_short_move(+1*SM204_SHORT_MOVE_DISTANCE); break;
// 		case 188: MANUAL_ERROR = SM204_relative_short_move(-1*SM204_SHORT_MOVE_DISTANCE); break;  not use this axis


//AA分解动作
		case 200: MANUAL_ERROR = aa_move_center(); break;// **************************
		case 201: MANUAL_ERROR = focus_scan(); break;
		case 202: MANUAL_ERROR = search_z_process(); break;
		case 203: MANUAL_ERROR = calibrate_tilt_angle(); break;
		case 204: 
			MANUAL_ERROR = fs_check(); 
			if(!MANUAL_ERROR)
				SetDlgItemText(HWND_MAIN,IDC_PROCESS_CHECK1,"OK");
			else
				SetDlgItemText(HWND_MAIN,IDC_PROCESS_CHECK1,"NG");
			break;
		case 205: MANUAL_ERROR = aa_move_offset(); break;
		case 206: 		
			SYSTEMTIME st1,st2;
			//GetLocalTime(&st1);
			log("===Start AA===");
			timeout = 100;
			SetDlgItemText(HWND_MAIN, IDC_PROCESS_AA_MOTION,	  "...");
			SetDlgItemText(HWND_MAIN, IDC_PROCESS_CHECK1,		  "...");
			SetDlgItemText(HWND_MAIN, IDC_PROCESS_CHECK2,		  "...");

			MANUAL_ERROR = search_z_process();
			if(SHOW_IMG == FALSE){
				log("show image error, break 206!");
				break;
			}
			if(!MANUAL_ERROR){ 
				MANUAL_ERROR = aa_move_center(); 
				if(!MANUAL_ERROR){
					log("move_center err");
				}
				MANUAL_ERROR = aa_move_center(); 

				if(abs(mysbw.oc_x[0]-AA_TARGET_CENTER_X)>1||abs(mysbw.oc_y[0]-AA_TARGET_CENTER_Y)>1){
								Sleep(200);
								MANUAL_ERROR += aa_move_center(); 

							}
				if(!MANUAL_ERROR){
					MANUAL_ERROR = focus_scan();
					MANUAL_ERROR += aa_move_center();
					if(abs(mysbw.oc_x[0]-AA_TARGET_CENTER_X)>1||abs(mysbw.oc_y[0]-AA_TARGET_CENTER_Y)>1){
								Sleep(200);
								MANUAL_ERROR += aa_move_center(); 
							}
					if(!MANUAL_ERROR){
						
						MANUAL_ERROR = fs_check();
						MANUAL_ERROR += aa_move_center();
						if(abs(mysbw.oc_x[0]-AA_TARGET_CENTER_X)>1||abs(mysbw.oc_y[0]-AA_TARGET_CENTER_Y)>1){
								Sleep(200);
								MANUAL_ERROR += aa_move_center(); 

							}
						if(!MANUAL_ERROR){
						//	MANUAL_ERROR = aa_move_offset();
							cnt = 5;
							while(cnt-->0 && (abs(mysbw.oc_x[0]-AA_TARGET_CENTER_X)>1||abs(mysbw.oc_y[0]-AA_TARGET_CENTER_Y)>1)){
								Sleep(200);
								MANUAL_ERROR = aa_move_center(); 
							}

							if(!MANUAL_ERROR){
								log("aa ok.");
								SetDlgItemText(HWND_MAIN,IDC_PROCESS_AA_MOTION,"OK");
							}							
						}
					}
				}
			}

			if (MANUAL_ERROR != 0x00){
				SetDlgItemText(HWND_MAIN,IDC_PROCESS_AA_MOTION,"NG");
				log("***AA Final Error***");
				EXECUTE_ERROR = TRUE;
			}
			EXECUTE_CMD = FALSE;
			//GetLocalTime(&st2);
			//if(st2.wSecond<st1.wSecond)
			//	mysbw.ct = st2.wSecond+60-st1.wSecond;
			//else
			//	mysbw.ct = st2.wSecond-st1.wSecond;
			//log("AA time : %d",mysbw.ct);
			//ai_win::set_dlg_item_int(HWND_MAIN,IDC_REPORT_CycleTime,mysbw.ct);
			log("===End AA===");
			break;

		case 207:
			//pick lens
			err = PI_move_to_point(dPos_p[point_num[0]]);
			if(err)	log("move to lens pick position err.");

			err = PI_move_to_point(dPos_p[point_num[1]]);
			if(err)	log("move to lens pick position err.");

			break;

		case 208://load lens

			err = PI_move_to_point(dPos_p[point_num[0]]);
			if(err)	log("move step1 err");
		
			err = PI_move_to_point(dPos_p[point_num[2]]);
			if(err)	log("move step2 err");
		
			err = PI_move_to_point(dPos_p[point_num[3]]);
			if(err)	log("move step3 err");
		

			break;

		case 209://UNLOAD
			err = move_safe();
			if(err)	log("move safe err");
			err = PI_move_to_point(dPos_p[point_num[2]]);
			if(err)	log("move step2 err");
			err = PI_move_to_point(dPos_p[point_num[0]]);
			if(err)	log("move step3 err");
	
			break;

		case 210: //move to line x home pos
			if ((err = LINE_X_MOVE_TO_HOME_POS)!= 0x00)	log("Line X(SM201) move to home pos fail.");
			break;
			
		case 211:
			if ((err = LINE_Y_MOVE_TO_HOME_POS)!= 0x00)	log("Line Y(SM103) move to home pos fail.");
			break;

		case 212:
			if ((err = LINE_Z_MOVE_TO_HOME_POS)!= 0x00)	log("Line Z(SM101) move to home pos fail.");
			break;

		case 213:
			if ((err = TILT_X_MOVE_TO_HOME_POS)!= 0x00)	log("Tilt X(SM102) move to home pos fail.");
			break;

		case 214:
			if ((err = TILT_Y_MOVE_TO_HOME_POS)!= 0x00)	log("Tilt Y(SM203) move to home pos fail.");
			break;

		case 215:
			if ((err = TILT_Z_MOVE_TO_HOME_POS)!= 0x00)	log("Tilt Z(SM202) move to home pos fail.");
			break;

		case 216:
			if(HOME_FLAG == FALSE){
				log("motor have not home, aa fail.");
				set_status(APP_NOHOME);
				break;
			}
			//==============================
			MANUAL_ERROR = SM202_move_to_load_position3();
			while(MANUAL_ERROR==1) Sleep(50);
			MANUAL_ERROR = 1;
			MANUAL_ERROR = SM203_move_to_load_position3();
			while(MANUAL_ERROR==1) Sleep(50);
			MANUAL_ERROR = 1;
			MANUAL_ERROR = SM201_move_to_load_position3();
			while(MANUAL_ERROR==1) Sleep(50);
			if(MANUAL_ERROR)
				log("down,MANUAL_ERROR=%d",MANUAL_ERROR);

			SM103_TRIGGER = SM103_MOVE_TO_HOME_POSITION;
			SM101_TRIGGER = SM101_MOVE_TO_HOME_POSITION;
			SM102_TRIGGER = SM102_MOVE_TO_HOME_POSITION;//tilt reset
			timeout = 2000;
			while ((SM103_TRIGGER != 0 || SM101_TRIGGER != 0 || SM102_TRIGGER != 0 ) && timeout-->0){
					Sleep(10);
			}
			if(timeout<0){
				EXECUTE_ERROR = TRUE;
				break;
			}//==============================
	
			log("===Start AA2===");
			SetDlgItemText(HWND_MAIN, IDC_PROCESS_AA_MOTION,	  "...");
			SetDlgItemText(HWND_MAIN, IDC_PROCESS_CHECK1,		  "...");
			SetDlgItemText(HWND_MAIN, IDC_PROCESS_CHECK2,		  "...");

	//		MANUAL_ERROR = move_step_z(-15*AA_FS_STEP_UNIT);
			MANUAL_ERROR = search_z_process();
			if(!MANUAL_ERROR){ 
				MANUAL_ERROR = aa_move_center(); 
				if(!MANUAL_ERROR){
					log("move_center err");
				}
				if(!MANUAL_ERROR){
					MANUAL_ERROR = focus_scan();
					MANUAL_ERROR = aa_move_center(); 
					if(!MANUAL_ERROR){
						MANUAL_ERROR = fs_check();
						MANUAL_ERROR = aa_move_center(); 
						if(!MANUAL_ERROR){
							while(abs(mysbw.oc_x[0]-AA_TARGET_CENTER_X)>1||abs(mysbw.oc_y[0]-AA_TARGET_CENTER_Y)>1){
								Sleep(200);
								MANUAL_ERROR = aa_move_center(); 
							}
							if(!MANUAL_ERROR){
								log("aa ok.");
								SetDlgItemText(HWND_MAIN,IDC_PROCESS_AA_MOTION,"OK");
							}							
						}
					}
				}
			}
			if (MANUAL_ERROR != 0x00){
				SetDlgItemText(HWND_MAIN,IDC_PROCESS_AA_MOTION,"NG");
				log("***AA Final Error***");
				EXECUTE_ERROR = TRUE;
			}
			EXECUTE_CMD = FALSE;
			SYSTEMTIME st3;	
			//GetLocalTime(&st3);
			//if(st3.wSecond<st1.wSecond)
			//	mysbw.ct = st3.wSecond+60-st1.wSecond;
			//else
			//	mysbw.ct = st3.wSecond-st1.wSecond;
			//ai_win::set_dlg_item_int(HWND_MAIN,IDC_REPORT_CycleTime,mysbw.ct);
			//log("")
			log("===End AA2===");
			break;


		case 220:
			SM202_TRIGGER;//************************

			break;

		case 221:
			EMERGENCY_STOP_FLAG = FALSE;
			HomeMachine_StopFlag = FALSE;
			EXECUTE_HOME = TRUE;
			timeout = 5000;
			while (EXECUTE_HOME == TRUE && timeout-->0){
				Sleep(10);
			}
			if (timeout<0){
				log("execute home time out.");
				EXECUTE_ERROR = TRUE;
			//	ITEM_GRAYED(HWND_AA_MOTION, IDC_HOME_ALL);
			}
/*			else log("Current still homing.");*/
			break;

		case 222:
			if ((err = SM202_move_to_loading_position()) != 0){
				log("***sm202 move loading Error***");
				EXECUTE_ERROR = TRUE;
			}
			EXECUTE_CMD = FALSE;
			break;

		case 223:
			{
				double move_x = 0;
				double move_y = 0;
				double cl_ct_x = mysbw.oc_x[0];
				double cl_ct_y = mysbw.oc_y[0];
				double img_center_x = AA_TARGET_CENTER_X/*2052*//* IMG_WIDTH/2*/;
				double img_center_y = AA_TARGET_CENTER_Y/*1531*//*IMG_HEIGHT/2*/;
				
				log("ct_x:%f, ct_y:%f, tar_x:%f, tar_y:%f", cl_ct_x, cl_ct_y, img_center_x, img_center_y);
//				log("motion_ang_x:%f, motion_ang_y:%f", AA_MOTION_X_ANGLE, AA_MOTION_Y_ANGLE);

#ifdef DIA_ALIGNMENT
				get_motion_move_center_offset(cl_ct_x, cl_ct_y, img_center_x, img_center_y,
					AA_MOTION_X_ANGLE, AA_MOTION_Y_ANGLE,
					move_x, move_y);
#else
				move_x = img_center_x-cl_ct_x;
				move_y = img_center_y-cl_ct_y;

#endif
				log("move pixel x=%f, y=%f", move_x, move_y);
				log("img_center_x=%f, y=%f ,cl_ct_x=%f,y=%f", img_center_x, img_center_y,cl_ct_x,cl_ct_y);

				//ai_win::set_dlg_item_float(HWND_AA_MOTION, IDC_CALC_DIFF_CENTER_X, move_x, 4);
				//ai_win::set_dlg_item_float(HWND_AA_MOTION, IDC_CALC_DIFF_CENTER_Y, move_y, 4);
				//ai_win::set_dlg_item_float(HWND_AA_MOTION, IDC_REPORT_CENTER_X, move_x, 4);
				//ai_win::set_dlg_item_float(HWND_AA_MOTION, IDC_REPORT_CENTER_Y, move_y, 4);

				AA_MOVE_X_MM = move_x*PIXEL_SIZE;
				AA_MOVE_Y_MM = move_y*PIXEL_SIZE;
// 
// 				ai_win::set_dlg_item_float(HWND_AA_MOTION, IDC_MOVE_X_MM, move_x*PIXEL_SIZE, 4);
// 				ai_win::set_dlg_item_float(HWND_AA_MOTION, IDC_MOVE_Y_MM, move_y*PIXEL_SIZE, 4);
			}
			break;

			
		}

		MANUAL_MOVING = FALSE;
		log("recv: button_val =%d", number);
	}


	return FALSE;
}
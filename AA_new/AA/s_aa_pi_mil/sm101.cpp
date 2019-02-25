#include "interface.h"
#include "inf_motion_aa.h"

void read_SM101_position(void)
{
	long position;
	WaitForSingleObject(SM101_mutex,INFINITE);
	ps400_get_cmdcounter(SM101_card, SM101_axis, &position);
	SM101_POSITION = (position);
	ReleaseMutex(SM101_mutex);
	return;
}

int SM101_home_search(void)
{
	long	START_V, DRIVE_V, ACC;
	int		errcode, timeout;
	BYTE	status;
	WORD	StopStatus;

	if(SM101_SERVO_ALARM==ON )	return(SM101_ALARM_ERR);
	if(SM101_SERVO_READY==OFF)	return(SM101_NOT_READY_ERR);
	if(SM101_SERVO_CHECK==OFF)	return(SM101_NOT_READY_ERR);
	if(EMERGENCY_STOP_FLAG == TRUE) return(MACHINE_EMERGENCY_STOP_WARNING);

	DRIVE_V = (long)(SM101_AXIS_HOME_SPEED); // unit = pulses per second PPS
	ACC		= (long)(SM101_AXIS_HOME_ACCEL_TIME);
	START_V = (int)(sqrt((double)DRIVE_V));	

	if (START_V<500) START_V = 500;

	//=== Home PHASE-1 =======================================================

	log("before set home");
	if((errcode = ps400_set_home_cfg(SM101_card, SM101_axis, 
		HOME_LOGIC_ACTIVE_LOW, 
		NHOME_LOGIC_ACTIVE_LOW, 
		INDEX_LOGIC_ACTIVE_LOW, 
		(AUTO_HOME_STEP1_FORWARD | AUTO_HOME_STEP2_FORWARD |
		 AUTO_HOME_STEP3_DISABLE | AUTO_HOME_STEP4_DISABLE), 0)) != SUCCESS_NO_ERROR) return(SM101_MOTION_COMMAND_ERR);
	log("after set home");

	if((errcode = ps400_home_start( SM101_card, SM101_axis, 
									START_V, ACC, ACC, DRIVE_V, DRIVE_V,
									DISABLE_BLOCK_OPEARTION)) != SUCCESS_NO_ERROR) return(SM101_MOTION_COMMAND_ERR);
	log("sv=%d,v=%d,acc=%d,err=%d",START_V,DRIVE_V,ACC,errcode);
	timeout=5000;//50s
	status = OFF;
	while((status != MOTION_DONE)&&(timeout-- >= 0x00)){
		if(EmergencyStopFlag==ON){
			SM101_motor_stop();
			return(MACHINE_EMERGENCY_STOP_WARNING);
		}
		if(HomeMachine_StopFlag==ON){ 
			SM101_motor_stop(); 
			return(0x00); 
		}
		ps400_motion_done(SM101_card, SM101_axis, &status, &StopStatus);
		Sleep(10);
	}
	if(timeout <= 0x00){
		SM101_motor_stop();
		return(SM101_HOMING_FAILURE_ERR);
	}
	else if(StopStatus != DRIVE_FINISH_WITH_AUTO_HOME){
		return(SM101_HOMING_FAILURE_ERR);
	}
	else{
		HOME_FLAG = TRUE;
		SM101_HOMED_FLAG = ON;
		log("sm101 home flag=%d",SM101_HOMED_FLAG);
	}
	return(0);
}
int SM101_motor_stop(void)
{
	WaitForSingleObject(SM101_mutex, INFINITE);
	ps400_stop_move(SM101_card, SM101_axis, STOP_SUDDEN);
	ReleaseMutex(SM101_mutex);
	return(0);
}
int SM101_encoder_reset()
{
	int  errcode;
	long position;

	if((errcode=SM101_home_search()) != 0x00) return(errcode);
	log("err=%d",errcode);
	if(HomeMachine_StopFlag==ON){ SM101_motor_stop(); return(0x00); }

	Sleep(300);
	WaitForSingleObject(SM101_mutex,INFINITE);

	ps400_set_cmdcounter(SM101_card, SM101_axis, 0x00); 
	ps400_get_cmdcounter(SM101_card, SM101_axis, &position);
	SM101_POSITION = (position);
	ReleaseMutex(SM101_mutex);
// 	SM101_HOMED_FLAG = OFF;//复位后需重新归位
// 	log("sm101 home flag=%d",SM101_HOMED_FLAG);
	return(0x00);
}
int SM101_jog_cw(void)
{
	long START_V, DRIVE_V, ACC;
	int  errcode;

	if(SM101_SERVO_ALARM==ON)	return(SM101_ALARM_ERR);
	if(SM101_SERVO_READY==OFF)	return(SM101_NOT_READY_ERR);
	if(SM101_SERVO_CHECK==OFF)	return(SM101_NOT_READY_ERR);


	DRIVE_V = (long)(SM101_AXIS_JOG_SPEED); // unit = pulses per second PPS
	ACC		= (long)(SM101_AXIS_JOG_ACCEL_TIME);
	START_V = (int)(sqrt((double)DRIVE_V));
	if (START_V<500) START_V = 500;
	DWORD dwrt = WaitForSingleObject(SM101_mutex, LONG_MAX);
	if((errcode=ps400_velocity_move(SM101_card, SM101_axis, 
		START_V, DRIVE_V, ACC, 
		MOVE_DIRECTION_FORWARD)) != ERROR_SUCCESS){
			ReleaseMutex(SM101_mutex);
			return(SM101_MOTION_COMMAND_ERR);
	}
	int rt = ReleaseMutex(SM101_mutex);
	return(0x00);
}
int SM101_jog_ccw(void)
{
	long START_V, DRIVE_V, ACC;
	int  errcode;

	if(SM101_SERVO_ALARM==ON )	return(SM101_ALARM_ERR);
	if(SM101_SERVO_READY==OFF)	return(SM101_NOT_READY_ERR);
	if(SM101_SERVO_CHECK==OFF)	return(SM101_NOT_READY_ERR);


	DRIVE_V = (long)(SM101_AXIS_JOG_SPEED); // unit = pulses per second PPS
	ACC		= (long)(SM101_AXIS_JOG_ACCEL_TIME);
	START_V = (int)(sqrt((double)DRIVE_V));
	if (START_V<500) START_V = 500;
	WaitForSingleObject(SM101_mutex,INFINITE);
	if((errcode=ps400_velocity_move(SM101_card, SM101_axis, 
		START_V, DRIVE_V, ACC, 
		MOVE_DIRECTION_REVERSE)) != ERROR_SUCCESS){
			ReleaseMutex(SM101_mutex);
			return(SM101_MOTION_COMMAND_ERR);
	}
	ReleaseMutex(SM101_mutex);
	return(0x00);
}  
int SM101_relative_movement(int target, int speed, int acceleration, int deceleration)//diandong
{
	long	START_V, DRIVE_V, ACC, DEC;
	long	distance;
	int		timeout, errcode;
	BYTE	status;
	WORD	StopStatus;

	if(SM101_SERVO_ALARM==ON )	return(SM101_ALARM_ERR);
	if(SM101_SERVO_READY==OFF)	return(SM101_NOT_READY_ERR);
	if(SM101_SERVO_CHECK==OFF)	return(SM101_NOT_READY_ERR);
	if(SM101_LMT_N		==ON )	return(SM101_N_LIMIT_ERR);
	if(SM101_LMT_P		==ON )	return(SM101_P_LIMIT_ERR);
	if(EMERGENCY_STOP_FLAG == TRUE) return(MACHINE_EMERGENCY_STOP_WARNING);

	read_SM101_position();//获取当前位
	distance = (target - SM101_POSITION); //target 目标位
	if(abs(distance) < 1) return(0);//ok

	START_V	= (int)(sqrt((double)speed));	// initial speed
	DRIVE_V	= speed;				// driving speed
	ACC		= acceleration;			// Acceleration 
	DEC		= deceleration;			// Deceleration 
	if (START_V<500) START_V = 500;
	WaitForSingleObject(SM101_mutex,INFINITE);
	if((errcode=ps400_t_move(SM101_card, SM101_axis, //风险，distance过大,需增加判断distance
		START_V, DRIVE_V, ACC, DEC, distance, 8)) != SUCCESS_NO_ERROR){
			ReleaseMutex(SM101_mutex);
			return(SM101_MOTION_COMMAND_ERR);
	}
	ReleaseMutex(SM101_mutex);

	timeout=2000;//20s
	ps400_motion_done(SM101_card, SM101_axis, &status, &StopStatus);
	while((status != MOTION_DONE)&&(timeout-- >= 0x00)){
		if((SM101_LMT_N==ON)||(SM101_LMT_P==ON)){
			SM101_motor_stop();
			if(SM101_LMT_P==ON) return(SM101_P_LIMIT_ERR);
			return(SM101_N_LIMIT_ERR);
		}
		if(EmergencyStopFlag==ON){
			SM101_motor_stop();
			return(MACHINE_EMERGENCY_STOP_WARNING);
		}
		ps400_motion_done(SM101_card, SM101_axis, &status, &StopStatus);
		Sleep(10);
	}//急停,判断极限,过程从开始运动到脉冲发送完毕

	timeout = 1000; //10s
	while((abs(target-SM101_POSITION) > 4)&&(timeout-- >= 0x00)){
		if((SM101_LMT_N==ON)||(SM101_LMT_P==ON)){
			SM101_motor_stop();
			if(SM101_LMT_P==ON) return(SM101_P_LIMIT_ERR);
			return(SM101_N_LIMIT_ERR);
		}
		if(EmergencyStopFlag==ON){
			SM101_motor_stop();
			return(MACHINE_EMERGENCY_STOP_WARNING);
		}
		read_SM101_position();
		Sleep(10);
	}//判断是否运动到目标值
	if(timeout < 0x00) return(SM101_NOT_IN_POSITION_ERR);
	return(0);
}
int SM101_move_to_home_position(void)
{
	int accel, speed, target, y;

	if(SM101_HOMED_FLAG	==OFF)	return(SM101_NOT_HOMED_YET_WARNING);

	target	= SM101_HOME_OFFSET;
	speed	= (long)(SM101_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM101_AXIS_MOVE_ACCEL_TIME);

	if((y=SM101_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM101_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM101_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM101_move_to_lens_loading_position(void)
{
	int accel, speed, target, y;

	if(SM101_HOMED_FLAG	==OFF)	return(SM101_NOT_HOMED_YET_WARNING);

	target	= AA_LENS_Y_LOADING_POS*SM101_DegreeRatio;
	speed	= (long)(SM101_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM101_AXIS_MOVE_ACCEL_TIME);

	if((y=SM101_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM101_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM101_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM101_relative_short_move(int MoveDistance)
{
	int accel, speed, target, y;

	read_SM101_position();
	target	= SM101_POSITION + MoveDistance; 
	speed	= (long)(SM101_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM101_AXIS_MOVE_ACCEL_TIME);

	if(abs(SM101_SHORT_MOVE_DISTANCE) < 1) return(0);
	if((y=SM101_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	return(0x00);
}

int SM101_move_to_load_position1(void)
{
	int accel, speed, target, y;

	if(SM101_HOMED_FLAG	==OFF)	return(SM101_NOT_HOMED_YET_WARNING);

	target	= (int)(SM101_POSITION_1*SM101_DegreeRatio);
	speed	= (long)(SM101_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM101_AXIS_MOVE_ACCEL_TIME);

	if((y=SM101_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM101_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM101_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM101_move_to_load_position2(void)
{
	int accel, speed, target, y;

	if(SM101_HOMED_FLAG	==OFF)	return(SM101_NOT_HOMED_YET_WARNING);

	target	= SM101_POSITION_2*SM101_POSITION_2;
	speed	= (long)(SM101_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM101_AXIS_MOVE_ACCEL_TIME);

	if((y=SM101_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM101_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM101_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM101_move_to_load_position3(void)
{
	int accel, speed, target, y;

	if(SM101_HOMED_FLAG	==OFF)	return(SM101_NOT_HOMED_YET_WARNING);

	target	= SM101_POSITION_2 + SM101_HOME_OFFSET; 
	speed	= (long)(SM101_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM101_AXIS_MOVE_ACCEL_TIME);

	if((y=SM101_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM101_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM101_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM101_move_to_load_position4(void)//SM101_focus_x_offset_move_from_cone_position(void)
{
	int accel, speed, target, y;

	if(SM101_HOMED_FLAG	==OFF)	return(SM101_NOT_HOMED_YET_WARNING);

	read_SM101_position();
	target	= SM101_POSITION_3 + SM101_HOME_OFFSET; 
	speed	= (long)(SM101_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM101_AXIS_MOVE_ACCEL_TIME);

	if((y=SM101_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM101_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM101_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM101_move_to_load_position5(void)//SM101_focus_x_move_to_laser_position
{
	int accel, speed, target, y;

	if(SM101_HOMED_FLAG	==OFF)	return(SM101_NOT_HOMED_YET_WARNING);

	target	= SM101_POSITION_4;
	speed	= (long)(SM101_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM101_AXIS_MOVE_ACCEL_TIME);

	if((y=SM101_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM101_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM101_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM101_move_to_load_position6(void)//SM101_focus_x_move_to_cone_position
{
	int accel, speed, target, y;

	if(SM101_HOMED_FLAG	==OFF)	return(SM101_NOT_HOMED_YET_WARNING);

	target	= SM101_POSITION_5; 
	speed	= (long)(SM101_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM101_AXIS_MOVE_ACCEL_TIME);

	if((y=SM101_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM101_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM101_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM101_move_to_load_position7()//SM101_move_to_target_distance
{
	int accel, speed, target, y;
//	log("sm101 active.");
//	if(SM101_HOMED_FLAG	==OFF)	return(SM101_NOT_HOMED_YET_WARNING);

	target	= (int)(AA_FINAL_MOVE_DISTANCE*SM101_DegreeRatio)*my_b; 
	log("SM101 target:%d, a=%.2f", target, my_b);
	speed	= (long)(SM101_AXIS_MOVE_SPEED)*my_b; // unit = pulses per second PPS
	accel	= (long)(SM101_AXIS_MOVE_ACCEL_TIME)*my_b;

	if((y=SM101_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM101_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM101_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);



}

int SM101_initialization(void)//SM101_focus_x_initialization
{
	int ret;

	if((SM101_HOMED_FLAG==ON)||(SM101_LMT_P==ON)){
		if((ret=SM101_move_to_home_position())	!= 0) return(ret);
	}
	else{
		if((ret=SM101_encoder_reset())	!= 0) return(ret);
	}
	return(0x00);
}




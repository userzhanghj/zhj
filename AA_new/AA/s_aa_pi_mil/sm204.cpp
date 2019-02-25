#include "inf_motion_aa.h"

int SM204_home_search(void)
{
	long	START_V, DRIVE_V, ACC;
	int		errcode, timeout;
	BYTE	status;
	WORD	StopStatus;

	if(SM204_SERVO_ALARM==ON )	return(SM204_ALARM_ERR);
	if(SM204_SERVO_READY==OFF)	return(SM204_NOT_READY_ERR);
	if(SM204_SERVO_CHECK==OFF)	return(SM204_NOT_READY_ERR);
	if(EMERGENCY_STOP_FLAG == TRUE) return(MACHINE_EMERGENCY_STOP_WARNING);


	DRIVE_V = (long)(SM204_AXIS_HOME_SPEED); // unit = pulses per second PPS
	ACC		= (long)(SM204_AXIS_HOME_ACCEL_TIME);
	START_V = (int)(sqrt((double)DRIVE_V));
	if (START_V<500) START_V = 500;
//	SM204_HOMED_FLAG = OFF;

	//=== Home PHASE-1 =======================================================
	/*if(SM204_HOME==OFF)*/{
		if((errcode = ps400_set_home_cfg(SM204_card, SM204_axis, 
			HOME_LOGIC_ACTIVE_LOW, 
			NHOME_LOGIC_ACTIVE_LOW, 
			INDEX_LOGIC_ACTIVE_LOW, 
			(AUTO_HOME_STEP1_FORWARD | AUTO_HOME_STEP2_FORWARD 
			| AUTO_HOME_STEP3_DISABLE | AUTO_HOME_STEP4_DISABLE), 0)) != SUCCESS_NO_ERROR) return(SM204_MOTION_COMMAND_ERR);

		if((errcode = ps400_home_start( SM204_card, SM204_axis, 
			START_V, ACC, ACC, DRIVE_V, DRIVE_V,
			DISABLE_BLOCK_OPEARTION)) != SUCCESS_NO_ERROR) return(SM204_MOTION_COMMAND_ERR);
		timeout=5000;
		status = OFF;
		while((status != MOTION_DONE)&&(timeout-- >= 0x00)){
			if(EmergencyStopFlag==ON){
				SM204_motor_stop();
				return(MACHINE_EMERGENCY_STOP_WARNING);
			}
			if(HomeMachine_StopFlag==ON){ 
				SM204_motor_stop(); 
				return(0x00); 
			}
			ps400_motion_done(SM204_card, SM204_axis, &status, &StopStatus);
			Sleep(10);
		}
		if(timeout <= 0x00){
			SM204_motor_stop();
			return(SM204_HOMING_FAILURE_ERR);
		}
		else if(StopStatus != DRIVE_FINISH_WITH_AUTO_HOME){
			return(SM204_HOMING_FAILURE_ERR);
		}
		else{
			HOME_FLAG = TRUE;
			SM204_HOMED_FLAG = ON;
			log("sm204 home flag=%d",SM204_HOMED_FLAG);
		}
	}

	return(0);
}
int SM204_motor_stop(void)
{
	WaitForSingleObject(SM204_mutex,INFINITE);
	ps400_stop_move(SM204_card, SM204_axis, STOP_SUDDEN);
	ReleaseMutex(SM204_mutex);
	return(0);
}
int SM204_encoder_reset()
{
	int  errcode;
	long position;

	if((errcode=SM204_home_search()) != 0x00) return(errcode);
	if(HomeMachine_StopFlag==ON){ SM204_motor_stop(); return(0x00); }

	Sleep(300);
	WaitForSingleObject(SM204_mutex,INFINITE);
	ps400_set_cmdcounter(SM204_card, SM204_axis, 0x00); 
	ps400_get_cmdcounter(SM204_card, SM204_axis, &position);
	SM204_POSITION = (position);
	ReleaseMutex(SM204_mutex);
// 	SM204_HOMED_FLAG = OFF;
// 	log("sm204 home flag=%d",SM204_HOMED_FLAG);
	return(0x00);
}
int SM204_jog_cw(void)
{
	long START_V, DRIVE_V, ACC;
	int  errcode;

	if(SM204_SERVO_ALARM==ON)	return(SM204_ALARM_ERR);
	if(SM204_SERVO_READY==OFF)	return(SM204_NOT_READY_ERR);
	if(SM204_SERVO_CHECK==OFF)	return(SM204_NOT_READY_ERR);


	DRIVE_V = (long)(SM204_AXIS_JOG_SPEED); // unit = pulses per second PPS
	ACC		= (long)(SM204_AXIS_JOG_ACCEL_TIME);
	START_V = (int)(sqrt((double)DRIVE_V));
	if (START_V<500) START_V = 500;
	WaitForSingleObject(SM204_mutex,INFINITE);
	if((errcode=ps400_velocity_move(SM204_card, SM204_axis, 
		START_V, DRIVE_V, ACC, 
		MOVE_DIRECTION_FORWARD)) != ERROR_SUCCESS){
			ReleaseMutex(SM204_mutex);
			return(SM204_MOTION_COMMAND_ERR);
	}
	ReleaseMutex(SM204_mutex);
	return(0x00);
}
int SM204_jog_ccw(void)
{
	long START_V, DRIVE_V, ACC;
	int  errcode;

	if(SM204_SERVO_ALARM==ON )	return(SM204_ALARM_ERR);
	if(SM204_SERVO_READY==OFF)	return(SM204_NOT_READY_ERR);
	if(SM204_SERVO_CHECK==OFF)	return(SM204_NOT_READY_ERR);


	DRIVE_V = (long)(SM204_AXIS_JOG_SPEED); // unit = pulses per second PPS
	ACC		= (long)(SM204_AXIS_JOG_ACCEL_TIME);
	START_V = (int)(sqrt((double)DRIVE_V));
	if (START_V<500) START_V = 500;
	WaitForSingleObject(SM204_mutex,INFINITE);
	if((errcode=ps400_velocity_move(SM204_card, SM204_axis, 
		START_V, DRIVE_V, ACC, 
		MOVE_DIRECTION_REVERSE)) != ERROR_SUCCESS){
			ReleaseMutex(SM204_mutex);
			return(SM204_MOTION_COMMAND_ERR);
	}
	ReleaseMutex(SM204_mutex);
	return(0x00);
}  
int SM204_relative_movement(int target, int speed, int acceleration, int deceleration)
{
	long	START_V, DRIVE_V, ACC, DEC;
	long	distance;
	int		timeout, errcode;
	BYTE	status;
	WORD	StopStatus;

	if(SM204_SERVO_ALARM==ON )	return(SM204_ALARM_ERR);
	if(SM204_SERVO_READY==OFF)	return(SM204_NOT_READY_ERR);
	if(SM204_SERVO_CHECK==OFF)	return(SM204_NOT_READY_ERR);
	if(SM204_LMT_N		==ON )	return(SM204_N_LIMIT_ERR);
	if(SM204_LMT_P		==ON )	return(SM204_P_LIMIT_ERR);
	if(EMERGENCY_STOP_FLAG == TRUE) return(MACHINE_EMERGENCY_STOP_WARNING);

	read_SM204_position();
	distance = (target - SM204_POSITION); 
	if(abs(distance) < 1) return(0);

	START_V	= (int)(sqrt((double)speed));	// initial speed
	DRIVE_V	= speed;				// driving speed
	ACC		= acceleration;			// Acceleration 
	DEC		= deceleration;			// Deceleration 
	if (START_V<500) START_V = 500;
	WaitForSingleObject(SM204_mutex,INFINITE);
	if((errcode=ps400_t_move(SM204_card, SM204_axis, 
		START_V, DRIVE_V, ACC, DEC, distance, 8)) != SUCCESS_NO_ERROR){
			ReleaseMutex(SM204_mutex);
			return(SM204_MOTION_COMMAND_ERR);
	}
	ReleaseMutex(SM204_mutex);
	timeout=3000;
	ps400_motion_done(SM204_card, SM204_axis, &status, &StopStatus);
	while((status != MOTION_DONE)&&(timeout-- >= 0x00)){
		if((SM204_LMT_N==ON)||(SM204_LMT_P==ON)){
			SM204_motor_stop();
			if(SM204_LMT_P==ON) return(SM204_P_LIMIT_ERR);
			return(SM204_N_LIMIT_ERR);
		}
		if(EmergencyStopFlag==ON){
			SM204_motor_stop();
			return(MACHINE_EMERGENCY_STOP_WARNING);
		}
		ps400_motion_done(SM204_card, SM204_axis, &status, &StopStatus);
		Sleep(10);
	}
	timeout = 1000/10; 
	read_SM204_position();
	while((abs(target-SM204_POSITION) > 4)&&(timeout-- >= 0x00)){
		if((SM204_LMT_N==ON)||(SM204_LMT_P==ON)){
			SM204_motor_stop();
			if(SM204_LMT_P==ON) return(SM204_P_LIMIT_ERR);
			return(SM204_N_LIMIT_ERR);
		}
		if(EmergencyStopFlag==ON){
			SM204_motor_stop();
			return(MACHINE_EMERGENCY_STOP_WARNING);
		}
		read_SM204_position();
		Sleep(10);
	}
	if(timeout < 0x00) return(SM204_NOT_IN_POSITION_ERR);
	return(0);
}
int SM204_move_to_home_position(void)
{
	int accel, speed, target, y;

	if(SM204_HOMED_FLAG	==OFF)	return(SM204_NOT_HOMED_YET_WARNING);

	target	= SM204_HOME_OFFSET;
	speed	= (long)(SM204_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM204_AXIS_MOVE_ACCEL_TIME);

	if((y=SM204_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM204_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM204_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM204_move_to_loading_position(void)
{
	int accel, speed, target, y;

	if(SM204_HOMED_FLAG	==OFF)	return(SM204_NOT_HOMED_YET_WARNING);

	target	= AA_Z_LOADING_POS*SM204_PositionRatio;
	speed	= (long)(SM204_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM204_AXIS_MOVE_ACCEL_TIME);

	if((y=SM204_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM204_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM204_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM204_relative_short_move(int MoveDistance)
{
	int accel, speed, target, y;

	read_SM204_position();
	target	= SM204_POSITION + MoveDistance; 
	speed	= (long)(SM204_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM204_AXIS_MOVE_ACCEL_TIME);

	if(abs(SM204_SHORT_MOVE_DISTANCE) < 1) return(0);
	if((y=SM204_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	return(0x00);
}
int SM204_move_to_load_position1(void)//SM204_dispense_x_move_to_vision_position
{
	int accel, speed, target, y;

	if(SM204_HOMED_FLAG	==OFF)	return(SM204_NOT_HOMED_YET_WARNING);

	target	= SM204_POSITION_1; 
	speed	= (long)(SM204_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM204_AXIS_MOVE_ACCEL_TIME);

	if((y=SM204_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM204_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM204_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM204_move_to_load_position2(void)//SM204_dispense_x_move_vision_center
{
	int accel, speed, target, y;

	if(SM204_HOMED_FLAG	==OFF)	return(SM204_NOT_HOMED_YET_WARNING);

	target	= SM204_POSITION_2 + SM204_HOME_OFFSET; 
	speed	= (long)(SM204_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM204_AXIS_MOVE_ACCEL_TIME);

	if((y=SM204_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM204_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM204_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}












int SM204_move_to_target_distance()
{
	int accel, speed, target, y;

	//	if(SM204_HOMED_FLAG	==OFF)	return(SM204_NOT_HOMED_YET_WARNING);
	//	log("SM204 active.");
	target	= (int)(AA_FINAL_MOVE_DISTANCE * SM204_PositionRatio)*my_a; 
	log("SM204 target:%d, a=%.2f", target, my_a);
	speed	= (long)(SM204_AXIS_MOVE_SPEED)*my_a; // unit = pulses per second PPS
	accel	= (long)(SM204_AXIS_MOVE_ACCEL_TIME)*my_a;

	if((y=SM204_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM204_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM204_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}


//----------------------------------------------------------------------
int SM204_initialization(void)
{
	int ret;

	if((SM204_HOMED_FLAG==ON)||(SM204_LMT_P==ON)){
		if((ret=SM204_move_to_home_position())	!= 0) return(ret);
	}
	else{
		if((ret=SM204_encoder_reset())	!= 0) return(ret);
	}
	return(0x00);
}

void read_SM204_position(void)
{
	long position;

	WaitForSingleObject(SM204_mutex,INFINITE);
	ps400_get_cmdcounter(SM204_card, SM204_axis, &position);
	SM204_POSITION = (position);
	ReleaseMutex(SM204_mutex);
	return;
}

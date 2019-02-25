#include "interface.h"
#include "inf_motion_aa.h"

int SM102_home_search(void)
{
	long	START_V, DRIVE_V, ACC;
	int		errcode, timeout;
	BYTE	status;
	WORD	StopStatus;

	if(SM102_SERVO_ALARM==ON )	return(SM102_ALARM_ERR);
	if(SM102_SERVO_READY==OFF)	return(SM102_NOT_READY_ERR);
	if(SM102_SERVO_CHECK==OFF)	return(SM102_NOT_READY_ERR);
	if(EMERGENCY_STOP_FLAG == TRUE) return(MACHINE_EMERGENCY_STOP_WARNING);

	DRIVE_V = (long)(SM102_AXIS_HOME_SPEED); // unit = pulses per second PPS
	ACC		= (long)(SM102_AXIS_HOME_ACCEL_TIME);
	START_V = (int)(sqrt((double)DRIVE_V));

	if (START_V<500) START_V = 500;

//	SM102_HOMED_FLAG = OFF;

	//=== Home PHASE-1 =======================================================
	/*if(SM102_HOME==OFF)*/{
		log("before set home");
		if((errcode = ps400_set_home_cfg(SM102_card, SM102_axis, 
			HOME_LOGIC_ACTIVE_LOW, 
			NHOME_LOGIC_ACTIVE_LOW, 
			INDEX_LOGIC_ACTIVE_LOW, 
			(AUTO_HOME_STEP1_FORWARD | AUTO_HOME_STEP2_FORWARD |
			AUTO_HOME_STEP3_DISABLE | AUTO_HOME_STEP4_DISABLE), 0)) != SUCCESS_NO_ERROR) return(SM102_MOTION_COMMAND_ERR);
		log("after set home");

		if((errcode = ps400_home_start( SM102_card, SM102_axis, 
			START_V, ACC, ACC, DRIVE_V, DRIVE_V,
			DISABLE_BLOCK_OPEARTION)) != SUCCESS_NO_ERROR) return(SM102_MOTION_COMMAND_ERR);
		timeout=5000;
		status = OFF;
		while((status != MOTION_DONE)&&(timeout-- >= 0x00)){
			if(EmergencyStopFlag==ON){
				SM102_motor_stop();
				return(MACHINE_EMERGENCY_STOP_WARNING);
			}
			if(HomeMachine_StopFlag==ON){ 
				SM102_motor_stop(); 
				return(0x00); 
			}
			ps400_motion_done(SM102_card, SM102_axis, &status, &StopStatus);
			Sleep(10);
		}
		if(timeout <= 0x00){
			SM102_motor_stop();
			return(SM102_HOMING_FAILURE_ERR);
		}
		else if(StopStatus != DRIVE_FINISH_WITH_AUTO_HOME){
			return(SM102_HOMING_FAILURE_ERR);
		}
		else{
			HOME_FLAG = TRUE;
			SM102_HOMED_FLAG = ON;
			log("sm102 home flag=%d",SM102_HOMED_FLAG);
		}
	}

	return(0);
}
int SM102_motor_stop(void)
{
	WaitForSingleObject(SM102_mutex, INFINITE);
	ps400_stop_move(SM102_card, SM102_axis, STOP_SUDDEN);
	ReleaseMutex(SM102_mutex);
	return(0);
}
int SM102_encoder_reset()
{
	int  errcode;
	long position;

	if((errcode=SM102_home_search()) != 0x00) return(errcode);
	if(HomeMachine_StopFlag==ON){ SM102_motor_stop(); return(0x00); }

	Sleep(300);
	WaitForSingleObject(SM102_mutex,INFINITE);

	ps400_set_cmdcounter(SM102_card, SM102_axis, 0x00); 
	ps400_get_cmdcounter(SM102_card, SM102_axis, &position);
	SM102_POSITION = (position);
	ReleaseMutex(SM102_mutex);
// 	SM102_HOMED_FLAG = OFF;
// 	log("sm102 home flag=%d",SM102_HOMED_FLAG);
	return(0x00);
}
int SM102_jog_cw(void)
{
	long START_V, DRIVE_V, ACC;
	int  errcode;

	if(SM102_SERVO_ALARM==ON)	return(SM102_ALARM_ERR);
	if(SM102_SERVO_READY==OFF)	return(SM102_NOT_READY_ERR);
	if(SM102_SERVO_CHECK==OFF)	return(SM102_NOT_READY_ERR);


	DRIVE_V = (long)(SM102_AXIS_JOG_SPEED); // unit = pulses per second PPS
	ACC		= (long)(SM102_AXIS_JOG_ACCEL_TIME);
	START_V = (int)(sqrt((double)DRIVE_V));
	if (START_V<500) START_V = 500;
	DWORD dwrt = WaitForSingleObject(SM102_mutex, LONG_MAX);
	if((errcode=ps400_velocity_move(SM102_card, SM102_axis, 
		START_V, DRIVE_V, ACC, 
		MOVE_DIRECTION_FORWARD)) != ERROR_SUCCESS){
			ReleaseMutex(SM102_mutex);
			return(SM102_MOTION_COMMAND_ERR);
	}
	int rt = ReleaseMutex(SM102_mutex);
	return(0x00);
}
int SM102_jog_ccw(void)
{
	long START_V, DRIVE_V, ACC;
	int  errcode;

	if(SM102_SERVO_ALARM==ON )	return(SM102_ALARM_ERR);
	if(SM102_SERVO_READY==OFF)	return(SM102_NOT_READY_ERR);
	if(SM102_SERVO_CHECK==OFF)	return(SM102_NOT_READY_ERR);


	DRIVE_V = (long)(SM102_AXIS_JOG_SPEED); // unit = pulses per second PPS
	ACC		= (long)(SM102_AXIS_JOG_ACCEL_TIME);
	START_V = (int)(sqrt((double)DRIVE_V));
	if (START_V<500) START_V = 500;
	WaitForSingleObject(SM102_mutex,INFINITE);
	if((errcode=ps400_velocity_move(SM102_card, SM102_axis, 
		START_V, DRIVE_V, ACC, 
		MOVE_DIRECTION_REVERSE)) != ERROR_SUCCESS){
			ReleaseMutex(SM102_mutex);
			return(SM102_MOTION_COMMAND_ERR);
	}
	ReleaseMutex(SM102_mutex);
	return(0x00);
}  
int SM102_relative_movement(int target, int speed, int acceleration, int deceleration)
{
	long	START_V, DRIVE_V, ACC, DEC;
	long	distance;
	int		timeout, errcode;
	BYTE	status;
	WORD	StopStatus;

	if(SM102_SERVO_ALARM==ON )	return(SM102_ALARM_ERR);
	if(SM102_SERVO_READY==OFF)	return(SM102_NOT_READY_ERR);
	if(SM102_SERVO_CHECK==OFF)	return(SM102_NOT_READY_ERR);
	if(SM102_LMT_N		==ON )	return(SM102_N_LIMIT_ERR);
	if(SM102_LMT_P		==ON )	return(SM102_P_LIMIT_ERR);
	if(EMERGENCY_STOP_FLAG == TRUE) return(MACHINE_EMERGENCY_STOP_WARNING);

	read_SM102_position();
	distance = (target - SM102_POSITION); 
	if(abs(distance) < 1) return(0);

	START_V	= (int)(sqrt((double)speed));	// initial speed
	DRIVE_V	= speed;				// driving speed
	ACC		= acceleration;			// Acceleration 
	DEC		= deceleration;			// Deceleration 
	if (START_V<500) START_V = 500;
	WaitForSingleObject(SM102_mutex,INFINITE);
	if((errcode=ps400_t_move(SM102_card, SM102_axis, 
		START_V, DRIVE_V, ACC, DEC, distance, 8)) != SUCCESS_NO_ERROR){
			ReleaseMutex(SM102_mutex);
			return(SM102_MOTION_COMMAND_ERR);
	}
	ReleaseMutex(SM102_mutex);
	timeout=2000;
	ps400_motion_done(SM102_card, SM102_axis, &status, &StopStatus);
	while((status != MOTION_DONE)&&(timeout-- >= 0x00)){
		if((SM102_LMT_N==ON)||(SM102_LMT_P==ON)){
			SM102_motor_stop();
			if(SM102_LMT_P==ON) return(SM102_P_LIMIT_ERR);
			return(SM102_N_LIMIT_ERR);
		}
		if(EmergencyStopFlag==ON){
			SM102_motor_stop();
			return(MACHINE_EMERGENCY_STOP_WARNING);
		}
		ps400_motion_done(SM102_card, SM102_axis, &status, &StopStatus);
		Sleep(10);
	}
	timeout = 1000/10; 
	while((abs(target-SM102_POSITION) > 4)&&(timeout-- >= 0x00)){
		if((SM102_LMT_N==ON)||(SM102_LMT_P==ON)){
			SM102_motor_stop();
			if(SM102_LMT_P==ON) return(SM102_P_LIMIT_ERR);
			return(SM102_N_LIMIT_ERR);
		}
		if(EmergencyStopFlag==ON){
			SM102_motor_stop();
			return(MACHINE_EMERGENCY_STOP_WARNING);
		}
		read_SM102_position();
		Sleep(10);
	}
	if(timeout < 0x00) return(SM102_NOT_IN_POSITION_ERR);
	return(0);
}
int SM102_move_to_home_position(void)
{
	int accel, speed, target, y;

	if(SM102_HOMED_FLAG	==OFF)	return(SM102_NOT_HOMED_YET_WARNING);

	target	= SM102_HOME_OFFSET;
	speed	= (long)(SM102_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM102_AXIS_MOVE_ACCEL_TIME);

	if((y=SM102_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM102_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM102_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM102_move_to_lens_loading_position(void)
{
	int accel, speed, target, y;

	if(SM102_HOMED_FLAG	==OFF)	return(SM102_NOT_HOMED_YET_WARNING);

	target	= (long int)AA_LENS_Y_LOADING_POS*SM102_DegreeRatio;
	speed	= (long int)(SM102_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long int)(SM102_AXIS_MOVE_ACCEL_TIME);

	if((y=SM102_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM102_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM102_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM102_relative_short_move(int MoveDistance)
{
	int accel, speed, target, y;

	read_SM102_position();
	target	= SM102_POSITION + MoveDistance; 
	speed	= (long)(SM102_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM102_AXIS_MOVE_ACCEL_TIME);

	if(abs(SM102_SHORT_MOVE_DISTANCE) < 1) return(0);
	if((y=SM102_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	return(0x00);
}

int SM102_move_to_load_position1(void)
{
	int accel, speed, target, y;

	if(SM102_HOMED_FLAG	==OFF)	return(SM102_NOT_HOMED_YET_WARNING);

	target	= (int)(SM102_POSITION_1*SM102_DegreeRatio);
	speed	= (long)(SM102_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM102_AXIS_MOVE_ACCEL_TIME);

	if((y=SM102_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM102_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM102_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM102_move_to_load_position2(void)
{
	int accel, speed, target, y;

	if(SM102_HOMED_FLAG	==OFF)	return(SM102_NOT_HOMED_YET_WARNING);

	target	= SM102_POSITION_1;
	speed	= (long)(SM102_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM102_AXIS_MOVE_ACCEL_TIME);

	if((y=SM102_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM102_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM102_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM102_move_to_load_position3(void)
{
	int accel, speed, target, y;

	if(SM102_HOMED_FLAG	==OFF)	return(SM102_NOT_HOMED_YET_WARNING);

	target	= SM102_POSITION_2 + SM102_HOME_OFFSET; 
	speed	= (long)(SM102_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM102_AXIS_MOVE_ACCEL_TIME);

	if((y=SM102_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM102_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM102_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM102_move_to_load_position4(void)//SM102_focus_x_offset_move_from_cone_position(void)
{
	int accel, speed, target, y;

	if(SM102_HOMED_FLAG	==OFF)	return(SM102_NOT_HOMED_YET_WARNING);

	read_SM102_position();
	target	= SM102_POSITION_3 + SM102_HOME_OFFSET; 
	speed	= (long)(SM102_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM102_AXIS_MOVE_ACCEL_TIME);

	if((y=SM102_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM102_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM102_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM102_move_to_load_position5(void)//SM102_focus_x_move_to_laser_position
{
	int accel, speed, target, y;

	if(SM102_HOMED_FLAG	==OFF)	return(SM102_NOT_HOMED_YET_WARNING);

	target	= SM102_POSITION_4;
	speed	= (long)(SM102_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM102_AXIS_MOVE_ACCEL_TIME);

	if((y=SM102_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM102_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM102_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM102_move_to_load_position6(void)//SM102_focus_x_move_to_cone_position
{
	int accel, speed, target, y;

	if(SM102_HOMED_FLAG	==OFF)	return(SM102_NOT_HOMED_YET_WARNING);

	target	= SM102_POSITION_5; 
	speed	= (long)(SM102_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM102_AXIS_MOVE_ACCEL_TIME);

	if((y=SM102_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM102_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM102_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM102_move_to_load_position7()//SM102_move_to_target_distance
{
	int accel, speed, target, y;
	//	log("sm101 active.");
	//	if(SM102_HOMED_FLAG	==OFF)	return(SM102_NOT_HOMED_YET_WARNING);

	target	= (int)(AA_FINAL_MOVE_DISTANCE*SM102_DegreeRatio)*my_b; 
	log("SM102 target:%d, a=%.2f", target, my_b);
	speed	= (long int)(SM102_AXIS_MOVE_SPEED)*my_b; // unit = pulses per second PPS
	accel	= (long int)(SM102_AXIS_MOVE_ACCEL_TIME)*my_b;

	if((y=SM102_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM102_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM102_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);



}

int SM102_initialization(void)//SM102_focus_x_initialization
{
	int ret;

	if((SM102_HOMED_FLAG==ON)||(SM102_LMT_P==ON)){
		if((ret=SM102_move_to_home_position())	!= 0) return(ret);
	}
	else{
		if((ret=SM102_encoder_reset())	!= 0) return(ret);
	}
	return(0x00);
}

void read_SM102_position(void)
{
	long position;
	//	log("read sm101:");
	WaitForSingleObject(SM102_mutex,INFINITE);
	ps400_get_cmdcounter(SM102_card, SM102_axis, &position);
	SM102_POSITION = (position);
	ReleaseMutex(SM102_mutex);
	//	log("read end[%d]", GetLastError());
	return;
}

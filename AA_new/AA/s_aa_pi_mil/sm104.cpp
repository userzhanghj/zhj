#include "interface.h"
#include "inf_motion_aa.h"

int SM104_home_search(void)
{
	long	START_V, DRIVE_V, ACC;
	int		errcode, timeout;
	BYTE	status;
	WORD	StopStatus;

	if(SM104_SERVO_ALARM==ON )	return(SM104_ALARM_ERR);
	if(SM104_SERVO_READY==OFF)	return(SM104_NOT_READY_ERR);
	if(SM104_SERVO_CHECK==OFF)	return(SM104_NOT_READY_ERR);
	if(EMERGENCY_STOP_FLAG == TRUE) return(MACHINE_EMERGENCY_STOP_WARNING);

	DRIVE_V = (long)(SM104_AXIS_HOME_SPEED); // unit = pulses per second PPS
	ACC		= (long)(SM104_AXIS_HOME_ACCEL_TIME);
	START_V = (int)(sqrt((double)DRIVE_V));

	if (START_V<500) START_V = 500;

//	SM104_HOMED_FLAG = OFF;

	//=== Home PHASE-1 =======================================================
	/*if(SM104_HOME==OFF)*/{
		log("before set home");
		if((errcode = ps400_set_home_cfg(SM104_card, SM104_axis, 
			HOME_LOGIC_ACTIVE_HIGH, 
			NHOME_LOGIC_ACTIVE_HIGH, 
			INDEX_LOGIC_ACTIVE_LOW, 
			(AUTO_HOME_STEP1_FORWARD | AUTO_HOME_STEP2_FORWARD |
			AUTO_HOME_STEP3_DISABLE | AUTO_HOME_STEP4_DISABLE), 0)) != SUCCESS_NO_ERROR) return(SM104_MOTION_COMMAND_ERR);
		log("after set home");

		if((errcode = ps400_home_start( SM104_card, SM104_axis, 
			START_V, ACC, ACC, DRIVE_V, DRIVE_V,
			DISABLE_BLOCK_OPEARTION)) != SUCCESS_NO_ERROR) return(SM104_MOTION_COMMAND_ERR);
		timeout=5000;
		status = OFF;
		while((status != MOTION_DONE)&&(timeout-- >= 0x00)){
			if(EmergencyStopFlag==ON){
				SM104_motor_stop();
				return(MACHINE_EMERGENCY_STOP_WARNING);
			}
			if(HomeMachine_StopFlag==ON){ 
				SM104_motor_stop(); 
				return(0x00); 
			}
			ps400_motion_done(SM104_card, SM104_axis, &status, &StopStatus);
			Sleep(10);
		}
		if(timeout <= 0x00){
			SM104_motor_stop();
			return(SM104_HOMING_FAILURE_ERR);
		}
		else if(StopStatus != DRIVE_FINISH_WITH_AUTO_HOME){
			return(SM104_HOMING_FAILURE_ERR);
		}
		else{
			HOME_FLAG = TRUE;
			SM104_HOMED_FLAG = ON;
			log("sm104 home flag=%d",SM104_HOMED_FLAG);
		}
			
	}

	return(0);
}
int SM104_motor_stop(void)
{
	WaitForSingleObject(SM104_mutex, INFINITE);
	ps400_stop_move(SM104_card, SM104_axis, STOP_SUDDEN);
	ReleaseMutex(SM104_mutex);
	return(0);
}
int SM104_encoder_reset()
{
	int  errcode;
	long position;

	if((errcode=SM104_home_search()) != 0x00) return(errcode);
	if(HomeMachine_StopFlag==ON){ SM104_motor_stop(); return(0x00); }

	Sleep(300);
	WaitForSingleObject(SM104_mutex,INFINITE);

	ps400_set_cmdcounter(SM104_card, SM104_axis, 0x00); 
	ps400_get_cmdcounter(SM104_card, SM104_axis, &position);
	SM104_POSITION = (position);
	ReleaseMutex(SM104_mutex);
// 	SM104_HOMED_FLAG = OFF;
// 	log("sm104 home flag=%d",SM104_HOMED_FLAG);
	return(0x00);
}
int SM104_jog_cw(void)
{
	long START_V, DRIVE_V, ACC;
	int  errcode;

	if(SM104_SERVO_ALARM==ON)	return(SM104_ALARM_ERR);
	if(SM104_SERVO_READY==OFF)	return(SM104_NOT_READY_ERR);
	if(SM104_SERVO_CHECK==OFF)	return(SM104_NOT_READY_ERR);


	DRIVE_V = (long)(SM104_AXIS_JOG_SPEED); // unit = pulses per second PPS
	ACC		= (long)(SM104_AXIS_JOG_ACCEL_TIME);
	START_V = (int)(sqrt((double)DRIVE_V));
	if (START_V<500) START_V = 500;
	DWORD dwrt = WaitForSingleObject(SM104_mutex, LONG_MAX);
	if((errcode=ps400_velocity_move(SM104_card, SM104_axis, 
		START_V, DRIVE_V, ACC, 
		MOVE_DIRECTION_FORWARD)) != ERROR_SUCCESS){
			ReleaseMutex(SM104_mutex);
			return(SM104_MOTION_COMMAND_ERR);
	}
	int rt = ReleaseMutex(SM104_mutex);
	return(0x00);
}
int SM104_jog_ccw(void)
{
	long START_V, DRIVE_V, ACC;
	int  errcode;

	if(SM104_SERVO_ALARM==ON )	return(SM104_ALARM_ERR);
	if(SM104_SERVO_READY==OFF)	return(SM104_NOT_READY_ERR);
	if(SM104_SERVO_CHECK==OFF)	return(SM104_NOT_READY_ERR);


	DRIVE_V = (long)(SM104_AXIS_JOG_SPEED); // unit = pulses per second PPS
	ACC		= (long)(SM104_AXIS_JOG_ACCEL_TIME);
	START_V = (int)(sqrt((double)DRIVE_V));
	if (START_V<500) START_V = 500;
	WaitForSingleObject(SM104_mutex,INFINITE);
	if((errcode=ps400_velocity_move(SM104_card, SM104_axis, 
		START_V, DRIVE_V, ACC, 
		MOVE_DIRECTION_REVERSE)) != ERROR_SUCCESS){
			ReleaseMutex(SM104_mutex);
			return(SM104_MOTION_COMMAND_ERR);
	}
	ReleaseMutex(SM104_mutex);
	return(0x00);
}  
int SM104_relative_movement(int target, int speed, int acceleration, int deceleration)
{
	long	START_V, DRIVE_V, ACC, DEC;
	long	distance;
	int		timeout, errcode;
	BYTE	status;
	WORD	StopStatus;

	if(SM104_SERVO_ALARM==ON )	return(SM104_ALARM_ERR);
	if(SM104_SERVO_READY==OFF)	return(SM104_NOT_READY_ERR);
	if(SM104_SERVO_CHECK==OFF)	return(SM104_NOT_READY_ERR);
	if(SM104_LMT_N		==ON )	return(SM104_N_LIMIT_ERR);
	if(SM104_LMT_P		==ON )	return(SM104_P_LIMIT_ERR);
	if(EMERGENCY_STOP_FLAG == TRUE) return(MACHINE_EMERGENCY_STOP_WARNING);

	read_SM104_position();
	distance = (target - SM104_POSITION); 
	if(abs(distance) < 1) return(0);

	START_V	= (int)(sqrt((double)speed));	// initial speed
	DRIVE_V	= speed;				// driving speed
	ACC		= acceleration;			// Acceleration 
	DEC		= deceleration;			// Deceleration 
	if (START_V<500) START_V = 500;
	WaitForSingleObject(SM104_mutex,INFINITE);
	if((errcode=ps400_t_move(SM104_card, SM104_axis, 
		START_V, DRIVE_V, ACC, DEC, distance, 8)) != SUCCESS_NO_ERROR){
			ReleaseMutex(SM104_mutex);
			return(SM104_MOTION_COMMAND_ERR);
	}
	ReleaseMutex(SM104_mutex);
	timeout=2000;
	ps400_motion_done(SM104_card, SM104_axis, &status, &StopStatus);
	while((status != MOTION_DONE)&&(timeout-- >= 0x00)){
		if((SM104_LMT_N==ON)||(SM104_LMT_P==ON)){
			SM104_motor_stop();
			if(SM104_LMT_P==ON) return(SM104_P_LIMIT_ERR);
			return(SM104_N_LIMIT_ERR);
		}
		if(EmergencyStopFlag==ON){
			SM104_motor_stop();
			return(MACHINE_EMERGENCY_STOP_WARNING);
		}
		ps400_motion_done(SM104_card, SM104_axis, &status, &StopStatus);
		Sleep(10);
	}
	timeout = 1000/10; 
	while((abs(target-SM104_POSITION) > 4)&&(timeout-- >= 0x00)){
		if((SM104_LMT_N==ON)||(SM104_LMT_P==ON)){
			SM104_motor_stop();
			if(SM104_LMT_P==ON) return(SM104_P_LIMIT_ERR);
			return(SM104_N_LIMIT_ERR);
		}
		if(EmergencyStopFlag==ON){
			SM104_motor_stop();
			return(MACHINE_EMERGENCY_STOP_WARNING);
		}
		read_SM104_position();
		Sleep(10);
	}
	if(timeout < 0x00) return(SM104_NOT_IN_POSITION_ERR);
	return(0);
}
int SM104_move_to_home_position(void)
{
	int accel, speed, target, y;

	if(SM104_HOMED_FLAG	==OFF)	return(SM104_NOT_HOMED_YET_WARNING);

	target	= SM104_HOME_OFFSET;
	speed	= (long)(SM104_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM104_AXIS_MOVE_ACCEL_TIME);

	if((y=SM104_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM104_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM104_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM104_move_to_lens_loading_position(void)
{
	int accel, speed, target, y;

	if(SM104_HOMED_FLAG	==OFF)	return(SM104_NOT_HOMED_YET_WARNING);

	target	= AA_LENS_Y_LOADING_POS*SM104_DegreeRatio;
	speed	= (long)(SM104_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM104_AXIS_MOVE_ACCEL_TIME);

	if((y=SM104_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM104_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM104_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM104_relative_short_move(int MoveDistance)
{
	int accel, speed, target, y;

	read_SM104_position();
	target	= SM104_POSITION + MoveDistance; 
	speed	= (long)(SM104_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM104_AXIS_MOVE_ACCEL_TIME);

	if(abs(SM104_SHORT_MOVE_DISTANCE) < 1) return(0);
	if((y=SM104_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	return(0x00);
}

int SM104_move_to_load_position1(void)
{
	int accel, speed, target, y;

	if(SM104_HOMED_FLAG	==OFF)	return(SM104_NOT_HOMED_YET_WARNING);

	target	= (int)(AA_Z_LOADING_POS*SM104_DegreeRatio);
	speed	= (long)(SM104_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM104_AXIS_MOVE_ACCEL_TIME);

	if((y=SM104_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM104_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM104_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM104_move_to_load_position2(void)
{
	int accel, speed, target, y;

	if(SM104_HOMED_FLAG	==OFF)	return(SM104_NOT_HOMED_YET_WARNING);

	target	= SM104_POSITION_1;
	speed	= (long)(SM104_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM104_AXIS_MOVE_ACCEL_TIME);

	if((y=SM104_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM104_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM104_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM104_move_to_load_position3(void)
{
	int accel, speed, target, y;

	if(SM104_HOMED_FLAG	==OFF)	return(SM104_NOT_HOMED_YET_WARNING);

	target	= SM104_POSITION_2 + SM104_HOME_OFFSET; 
	speed	= (long)(SM104_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM104_AXIS_MOVE_ACCEL_TIME);

	if((y=SM104_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM104_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM104_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM104_move_to_load_position4(void)//SM104_focus_x_offset_move_from_cone_position(void)
{
	int accel, speed, target, y;

	if(SM104_HOMED_FLAG	==OFF)	return(SM104_NOT_HOMED_YET_WARNING);

	read_SM104_position();
	target	= SM104_POSITION_3 + SM104_HOME_OFFSET; 
	speed	= (long)(SM104_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM104_AXIS_MOVE_ACCEL_TIME);

	if((y=SM104_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM104_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM104_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM104_move_to_load_position5(void)//SM104_focus_x_move_to_laser_position
{
	int accel, speed, target, y;

	if(SM104_HOMED_FLAG	==OFF)	return(SM104_NOT_HOMED_YET_WARNING);

	target	= SM104_POSITION_4;
	speed	= (long)(SM104_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM104_AXIS_MOVE_ACCEL_TIME);

	if((y=SM104_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM104_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM104_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM104_move_to_load_position6(void)//SM104_focus_x_move_to_cone_position
{
	int accel, speed, target, y;

	if(SM104_HOMED_FLAG	==OFF)	return(SM104_NOT_HOMED_YET_WARNING);

	target	= SM104_POSITION_5; 
	speed	= (long)(SM104_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM104_AXIS_MOVE_ACCEL_TIME);

	if((y=SM104_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM104_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM104_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM104_move_to_load_position7()//SM104_move_to_target_distance
{
	int accel, speed, target, y;
	//	log("sm101 active.");
	//	if(SM104_HOMED_FLAG	==OFF)	return(SM104_NOT_HOMED_YET_WARNING);

	target	= (int)(AA_FINAL_MOVE_DISTANCE*SM104_DegreeRatio)*my_b; 
	log("SM104 target:%d, a=%.2f", target, my_b);
	speed	= (long)(SM104_AXIS_MOVE_SPEED)*my_b; // unit = pulses per second PPS
	accel	= (long)(SM104_AXIS_MOVE_ACCEL_TIME)*my_b;

	if((y=SM104_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM104_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM104_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);



}

int SM104_initialization(void)//SM104_focus_x_initialization
{
	int ret;

	if((SM104_HOMED_FLAG==ON)||(SM104_LMT_P==ON)){
		if((ret=SM104_move_to_home_position())	!= 0) return(ret);
	}
	else{
		if((ret=SM104_encoder_reset())	!= 0) return(ret);
	}
	return(0x00);
}

void read_SM104_position(void)
{
	long position;
	//	log("read sm101:");
	WaitForSingleObject(SM104_mutex,INFINITE);
	ps400_get_cmdcounter(SM104_card, SM104_axis, &position);
	SM104_POSITION = (position);
	ReleaseMutex(SM104_mutex);
	//	log("read end[%d]", GetLastError());
	return;
}


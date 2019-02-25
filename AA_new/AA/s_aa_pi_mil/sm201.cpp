#include "inf_motion_aa.h"

int SM201_home_search(void)
{
	long	START_V, DRIVE_V, ACC;
	int		errcode, timeout;
	BYTE	status;
	WORD	StopStatus;

	if(SM201_SERVO_ALARM==ON )	return(SM201_ALARM_ERR);
	if(SM201_SERVO_READY==OFF)	return(SM201_NOT_READY_ERR);
	if(SM201_SERVO_CHECK==OFF)	return(SM201_NOT_READY_ERR);
	if(EMERGENCY_STOP_FLAG == TRUE) return(MACHINE_EMERGENCY_STOP_WARNING);


	DRIVE_V = (long)(SM201_AXIS_HOME_SPEED); // unit = pulses per second PPS
	ACC		= (long)(SM201_AXIS_HOME_ACCEL_TIME);
	START_V = (int)(sqrt((double)DRIVE_V));
	if (START_V<500) START_V = 500;
//	SM201_HOMED_FLAG = OFF;

	//=== Home PHASE-1 =======================================================
	/*if(SM201_HOME==OFF)*/{
		if((errcode = ps400_set_home_cfg(SM201_card, SM201_axis, 
			HOME_LOGIC_ACTIVE_LOW, 
			NHOME_LOGIC_ACTIVE_LOW, 
			INDEX_LOGIC_ACTIVE_LOW, 
			(AUTO_HOME_STEP1_FORWARD | AUTO_HOME_STEP2_FORWARD 
			| AUTO_HOME_STEP3_DISABLE | AUTO_HOME_STEP4_DISABLE), 0)) != SUCCESS_NO_ERROR) return(SM201_MOTION_COMMAND_ERR);

		if((errcode = ps400_home_start( SM201_card, SM201_axis, 
			START_V, ACC, ACC, DRIVE_V, DRIVE_V,
			DISABLE_BLOCK_OPEARTION)) != SUCCESS_NO_ERROR) return(SM201_MOTION_COMMAND_ERR);
		timeout=5000;
		status = OFF;
		while((status != MOTION_DONE)&&(timeout-- >= 0x00)){
			if(EmergencyStopFlag==ON){
				SM201_motor_stop();
				return(MACHINE_EMERGENCY_STOP_WARNING);
			}
			if(HomeMachine_StopFlag==ON){ 
				SM201_motor_stop(); 
				return(0x00); 
			}
			ps400_motion_done(SM201_card, SM201_axis, &status, &StopStatus);
			Sleep(10);
		}
		if(timeout <= 0x00){
			SM201_motor_stop();
			return(SM201_HOMING_FAILURE_ERR);
		}
		else if(StopStatus != DRIVE_FINISH_WITH_AUTO_HOME){
			return(SM201_HOMING_FAILURE_ERR);
		}
		else{
			HOME_FLAG = TRUE;
			SM201_HOMED_FLAG = ON;
			log("sm201 home flag=%d",SM201_HOMED_FLAG);
		}
			
	}

	return(0);
}
int SM201_motor_stop(void)
{
	WaitForSingleObject(SM201_mutex,INFINITE);
	ps400_stop_move(SM201_card, SM201_axis, STOP_SUDDEN);
	ReleaseMutex(SM201_mutex);
	return(0);
}
int SM201_encoder_reset()
{
	int  errcode;
	long position;

	if((errcode=SM201_home_search()) != 0x00) return(errcode);
	if(HomeMachine_StopFlag==ON){ SM201_motor_stop(); return(0x00); }

	Sleep(300);
	WaitForSingleObject(SM201_mutex,INFINITE);
	ps400_set_cmdcounter(SM201_card, SM201_axis, 0x00); 
	ps400_get_cmdcounter(SM201_card, SM201_axis, &position);
	SM201_POSITION = (position);
	ReleaseMutex(SM201_mutex);
// 	SM201_HOMED_FLAG = OFF;
// 	log("sm201 home flag=%d",SM201_HOMED_FLAG);
	return(0x00);
}
int SM201_jog_cw(void)
{
	long START_V, DRIVE_V, ACC;
	int  errcode;
	log("sm201_a:%d, r:%d, c:%d", SM201_SERVO_ALARM, SM201_SERVO_READY, SM201_SERVO_CHECK);

	if(SM201_SERVO_ALARM==ON)	return(SM201_ALARM_ERR);
	if(SM201_SERVO_READY==OFF)	return(SM201_NOT_READY_ERR);
	if(SM201_SERVO_CHECK==OFF)	return(SM201_NOT_READY_ERR);


	DRIVE_V = (long)(SM201_AXIS_JOG_SPEED); // unit = pulses per second PPS
	ACC		= (long)(SM201_AXIS_JOG_ACCEL_TIME);
	START_V = (int)(sqrt((double)DRIVE_V));
	if (START_V<500) START_V = 500;
	WaitForSingleObject(SM201_mutex,INFINITE);
	if((errcode=ps400_velocity_move(SM201_card, SM201_axis, 
		START_V, DRIVE_V, ACC, 
		MOVE_DIRECTION_FORWARD)) != ERROR_SUCCESS){
			ReleaseMutex(SM201_mutex);
			return(SM201_MOTION_COMMAND_ERR);
	}
	ReleaseMutex(SM201_mutex);
	return(0x00);
}
int SM201_jog_ccw(void)
{
	long START_V, DRIVE_V, ACC;
	int  errcode;
	log("sm201_a:%d, r:%d, c:%d", SM201_SERVO_ALARM, SM201_SERVO_READY, SM201_SERVO_CHECK);

	if(SM201_SERVO_ALARM==ON )	return(SM201_ALARM_ERR);
	if(SM201_SERVO_READY==OFF)	return(SM201_NOT_READY_ERR);
	if(SM201_SERVO_CHECK==OFF)	return(SM201_NOT_READY_ERR);


	DRIVE_V = (long)(SM201_AXIS_JOG_SPEED); // unit = pulses per second PPS
	ACC		= (long)(SM201_AXIS_JOG_ACCEL_TIME);
	START_V = (int)(sqrt((double)DRIVE_V));
	if (START_V<500) START_V = 500;
	WaitForSingleObject(SM201_mutex,INFINITE);
	if((errcode=ps400_velocity_move(SM201_card, SM201_axis, 
		START_V, DRIVE_V, ACC, 
		MOVE_DIRECTION_REVERSE)) != ERROR_SUCCESS){
			ReleaseMutex(SM201_mutex);
			return(SM201_MOTION_COMMAND_ERR);
	}
	ReleaseMutex(SM201_mutex);
	return(0x00);
}  
int SM201_relative_movement(int target, int speed, int acceleration, int deceleration)
{
	long	START_V, DRIVE_V, ACC, DEC;
	long	distance;
	int		timeout, errcode;
	BYTE	status;
	WORD	StopStatus;

	if(SM201_SERVO_ALARM==ON )	return(SM201_ALARM_ERR);
	if(SM201_SERVO_READY==OFF)	return(SM201_NOT_READY_ERR);
	if(SM201_SERVO_CHECK==OFF)	return(SM201_NOT_READY_ERR);
	if(SM201_LMT_N		==ON )	return(SM201_N_LIMIT_ERR);
	if(SM201_LMT_P		==ON )	return(SM201_P_LIMIT_ERR);
	if(EMERGENCY_STOP_FLAG == TRUE) return(MACHINE_EMERGENCY_STOP_WARNING);

	read_SM201_position();
	distance = (target - SM201_POSITION); 
	if(abs(distance) < 1) return(0);

	START_V	= (int)(sqrt((double)speed));	// initial speed
	DRIVE_V	= speed;				// driving speed
	ACC		= acceleration;			// Acceleration 
	DEC		= deceleration;			// Deceleration 
	if (START_V<500) START_V = 500;
	WaitForSingleObject(SM201_mutex,INFINITE);
	if((errcode=ps400_t_move(SM201_card, SM201_axis, 
		START_V, DRIVE_V, ACC, DEC, distance, 8)) != SUCCESS_NO_ERROR){
			ReleaseMutex(SM201_mutex);
			return(SM201_MOTION_COMMAND_ERR);
	}
	ReleaseMutex(SM201_mutex);
	timeout=3000;
	ps400_motion_done(SM201_card, SM201_axis, &status, &StopStatus);
	while((status != MOTION_DONE)&&(timeout-- >= 0x00)){
		if((SM201_LMT_N==ON)||(SM201_LMT_P==ON)){
			SM201_motor_stop();
			if(SM201_LMT_P==ON) return(SM201_P_LIMIT_ERR);
			return(SM201_N_LIMIT_ERR);
		}
		if(EmergencyStopFlag==ON){
			SM201_motor_stop();
			return(MACHINE_EMERGENCY_STOP_WARNING);
		}
		ps400_motion_done(SM201_card, SM201_axis, &status, &StopStatus);
		Sleep(10);
	}
	timeout = 1000/10; 
	read_SM201_position();
	while((abs(target-SM201_POSITION) > 4)&&(timeout-- >= 0x00)){
		if((SM201_LMT_N==ON)||(SM201_LMT_P==ON)){
			SM201_motor_stop();
			if(SM201_LMT_P==ON) return(SM201_P_LIMIT_ERR);
			return(SM201_N_LIMIT_ERR);
		}
		if(EmergencyStopFlag==ON){
			SM201_motor_stop();
			return(MACHINE_EMERGENCY_STOP_WARNING);
		}
		read_SM201_position();
		Sleep(10);
	}
	if(timeout < 0x00) return(SM201_NOT_IN_POSITION_ERR);
	return(0);
}
int SM201_move_to_home_position(void)
{
	int accel, speed, target, y;

	if(SM201_HOMED_FLAG	==OFF)	return(SM201_NOT_HOMED_YET_WARNING);

	target	= SM201_HOME_OFFSET;
	speed	= (long)(SM201_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM201_AXIS_MOVE_ACCEL_TIME);

	if((y=SM201_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM201_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM201_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM201_move_to_loading_position(void)
{
	int accel, speed, target, y;

	if(SM201_HOMED_FLAG	==OFF)	return(SM201_NOT_HOMED_YET_WARNING);

	target	= AA_Z_LOADING_POS*SM201_PositionRatio;
	speed	= (long)(SM201_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM201_AXIS_MOVE_ACCEL_TIME);

	if((y=SM201_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM201_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM201_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM201_relative_short_move(int MoveDistance)
{
	int accel, speed, target, y;

	read_SM201_position();
	target	= SM201_POSITION + MoveDistance; 
	speed	= (long)(SM201_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM201_AXIS_MOVE_ACCEL_TIME);

	if(abs(SM201_SHORT_MOVE_DISTANCE) < 1) return(0);
	if((y=SM201_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	return(0x00);
}
int SM201_move_to_load_position1(void)//SM201_dispense_x_move_to_vision_position
{
	int accel, speed, target, y;
	INT timeout = 100;

	if(SM201_HOMED_FLAG	==OFF)	return(SM201_NOT_HOMED_YET_WARNING);

	target	= SM201_POSITION_1*SM201_PositionRatio; 
	speed	= (long)(SM201_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM201_AXIS_MOVE_ACCEL_TIME);

	if((y=SM201_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM201_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM201_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	log("start move to P1 x.");

	int cnt;
	for(cnt=300;cnt>0;cnt--){	
		log("p1=%d,p=%d",(int)(SM201_POSITION_1*SM201_PositionRatio),SM201_POSITION);
		if((int)(SM201_POSITION_1*SM201_PositionRatio-SM201_POSITION)<4) break;
		Sleep(50);
	}
	if(cnt>0) return(0x00);
	else return(0x01);
}

int SM201_move_to_load_position2(void)//SM201_dispense_x_move_vision_center
{
	int accel, speed, target, y;

	if(SM201_HOMED_FLAG	==OFF)	return(SM201_NOT_HOMED_YET_WARNING);

	target	= SM201_POSITION_2*SM201_PositionRatio; 
	speed	= (long)(SM201_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM201_AXIS_MOVE_ACCEL_TIME);

	if((y=SM201_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM201_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM201_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	int cnt;
	for(cnt=300;cnt>0;cnt--){	
		log("p1=%d,p=%d",(int)(SM201_POSITION_2*SM201_PositionRatio),SM201_POSITION);
		if((int)(SM201_POSITION_2*SM201_PositionRatio-SM201_POSITION)<4) break;
		Sleep(50);
	}
	if(cnt>0) return(0x00);
	else return(0x01);
}
int SM201_move_to_load_position3(void)//SM201_dispense_x_move_vision_center
{
	int accel, speed, target, y;

	if(SM201_HOMED_FLAG	==OFF)	return(SM201_NOT_HOMED_YET_WARNING);

	target	= SM201_POSITION_3*SM201_PositionRatio; 
	speed	= (long)(SM201_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM201_AXIS_MOVE_ACCEL_TIME);
	log("target=%d",target);
	if((y=SM201_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM201_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM201_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	int cnt;
	for(cnt=300;cnt>0;cnt--){	
		log("p1=%d,p=%d",(int)(SM201_POSITION_3*SM201_PositionRatio),SM201_POSITION);
		if((int)(SM201_POSITION_3*SM201_PositionRatio-SM201_POSITION)<4) break;
		Sleep(50);
	}
	if(cnt>0) return(0x00);
	else return(0x01);
}
int SM201_move_to_load_position4(void)//SM201_dispense_x_move_vision_center
{
	int accel, speed, target, y;

	if(SM201_HOMED_FLAG	==OFF)	return(SM201_NOT_HOMED_YET_WARNING);

	target	= SM201_POSITION_4*SM201_PositionRatio; 
	speed	= (long)(SM201_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM201_AXIS_MOVE_ACCEL_TIME);
	log("target=%d",target);
	if((y=SM201_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM201_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM201_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	int cnt;
	for(cnt=300;cnt>0;cnt--){	
		log("p4=%d,p=%d",(int)(SM201_POSITION_4*SM201_PositionRatio),SM201_POSITION);
		if((int)(SM201_POSITION_4*SM201_PositionRatio-SM201_POSITION)<4) break;
		Sleep(50);
	}
	if(cnt>0) return(0x00);
	else return(0x01);
}

int SM201_move_to_load_position5(void)//SM201_dispense_x_move_vision_center
{
	int accel, speed, target, y;

	if(SM201_HOMED_FLAG	==OFF)	return(SM201_NOT_HOMED_YET_WARNING);

	target	= SM201_POSITION_5*SM201_PositionRatio; 
	speed	= (long)(SM201_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM201_AXIS_MOVE_ACCEL_TIME);
	log("target=%d",target);
	if((y=SM201_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM201_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM201_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	int cnt;
	for(cnt=300;cnt>0;cnt--){	
		log("p5=%d,p=%d",(int)(SM201_POSITION_5*SM201_PositionRatio),SM201_POSITION);
		if((int)(SM201_POSITION_5*SM201_PositionRatio-SM201_POSITION)<4) break;
		Sleep(50);
	}
	if(cnt>0) return(0x00);
	else return(0x01);
}
int SM201_move_to_load_position6(void)//SM201_dispense_x_move_vision_center
{
	int accel, speed, target, y;

	if(SM201_HOMED_FLAG	==OFF)	return(SM201_NOT_HOMED_YET_WARNING);

	target	= SM201_POSITION_6*SM201_PositionRatio; 
	speed	= (long)(SM201_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM201_AXIS_MOVE_ACCEL_TIME);
	log("target=%d",target);
	if((y=SM201_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM201_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM201_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	int cnt;
	for(cnt=300;cnt>0;cnt--){	
		log("p6=%d,p=%d",(int)(SM201_POSITION_6*SM201_PositionRatio),SM201_POSITION);
		if((int)(SM201_POSITION_6*SM201_PositionRatio-SM201_POSITION)<4) break;
		Sleep(50);
	}
	if(cnt>0) return(0x00);
	else return(0x01);
}








int SM201_move_to_target_distance()
{
	int accel, speed, target, y;

//	if(SM201_HOMED_FLAG	==OFF)	return(SM201_NOT_HOMED_YET_WARNING);
//	log("SM201 active.");
	target	= (int)(AA_FINAL_MOVE_DISTANCE * SM201_PositionRatio)*my_a; 
	log("SM201 target:%d, a=%.2f", target, my_a);
	speed	= (long)(SM201_AXIS_MOVE_SPEED)*my_a; // unit = pulses per second PPS
	accel	= (long)(SM201_AXIS_MOVE_ACCEL_TIME)*my_a;

	if((y=SM201_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM201_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM201_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}


//----------------------------------------------------------------------
int SM201_initialization(void)
{
	int ret;

	if((SM201_HOMED_FLAG==ON)||(SM201_LMT_P==ON)){
		if((ret=SM201_move_to_home_position())	!= 0) return(ret);
	}
	else{
		if((ret=SM201_encoder_reset())	!= 0) return(ret);
	}
	return(0x00);
}

void read_SM201_position(void)
{
	long position;

	WaitForSingleObject(SM201_mutex,INFINITE);
	ps400_get_cmdcounter(SM201_card, SM201_axis, &position);
	SM201_POSITION = (position);
	ReleaseMutex(SM201_mutex);
	return;
}

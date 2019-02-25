#include "inf_motion_aa.h"

int SM202_home_search(void)
{
	long	START_V, DRIVE_V, ACC;
	int		errcode, timeout;
	BYTE	status;
	WORD	StopStatus;

	if(SM202_SERVO_ALARM==ON )	return(SM202_ALARM_ERR);
	if(SM202_SERVO_READY==OFF)	return(SM202_NOT_READY_ERR);
	if(SM202_SERVO_CHECK==OFF)	return(SM202_NOT_READY_ERR);
	if(EMERGENCY_STOP_FLAG == TRUE) return(MACHINE_EMERGENCY_STOP_WARNING);


	DRIVE_V = (long)(SM202_AXIS_HOME_SPEED); // unit = pulses per second PPS
	ACC		= (long)(SM202_AXIS_HOME_ACCEL_TIME);
	START_V = (int)(sqrt((double)DRIVE_V));
	if (START_V<2000) START_V = 2000;
//	SM202_HOMED_FLAG = OFF;

	//=== Home PHASE-1 =======================================================
	/*if(SM202_HOME==OFF)*/{
		if((errcode = ps400_set_home_cfg(SM202_card, SM202_axis, 
			HOME_LOGIC_ACTIVE_LOW, 
			NHOME_LOGIC_ACTIVE_LOW, 
			INDEX_LOGIC_ACTIVE_LOW, 
			(AUTO_HOME_STEP1_REVERSE | AUTO_HOME_STEP2_REVERSE 
			| AUTO_HOME_STEP3_DISABLE | AUTO_HOME_STEP4_DISABLE), 0)) != SUCCESS_NO_ERROR) return(SM202_MOTION_COMMAND_ERR);
	//	log("sv=%d,dv=%d,a=%d",START_V,DRIVE_V,ACC);
		if((errcode = ps400_home_start( SM202_card, SM202_axis, 
			START_V, ACC, ACC, DRIVE_V, DRIVE_V,
			DISABLE_BLOCK_OPEARTION)) != SUCCESS_NO_ERROR) return(SM202_MOTION_COMMAND_ERR);
		timeout=5000;
		status = OFF;
		while((status != MOTION_DONE)&&(timeout-- >= 0x00)){
			if(EmergencyStopFlag==ON){
				SM202_motor_stop();
				return(MACHINE_EMERGENCY_STOP_WARNING);
			}
			if(HomeMachine_StopFlag==ON){ 
				SM202_motor_stop(); 
				return(0x00); 
			}
			ps400_motion_done(SM202_card, SM202_axis, &status, &StopStatus);
			Sleep(10);
		}
		if(timeout <= 0x00){
			SM202_motor_stop();
			return(SM202_HOMING_FAILURE_ERR);
		}
		else if(StopStatus != DRIVE_FINISH_WITH_AUTO_HOME){
			return(SM202_HOMING_FAILURE_ERR);
		}
		else{
			HOME_FLAG = TRUE;
			SM202_HOMED_FLAG = ON;
			log("on sm202 home flag=%d",SM202_HOMED_FLAG);
		}
	}

	return(0);
}
int SM202_motor_stop(void)
{
	WaitForSingleObject(SM202_mutex,INFINITE);
	ps400_stop_move(SM202_card, SM202_axis, STOP_SUDDEN);
	ReleaseMutex(SM202_mutex);
	return(0);
}
int SM202_encoder_reset()
{

	int  errcode;
	long position;

	if((errcode=SM202_home_search()) != 0x00) return(errcode);
	if(HomeMachine_StopFlag==ON){ SM202_motor_stop(); return(0x00); }

	Sleep(300);
	WaitForSingleObject(SM202_mutex,INFINITE);
	ps400_set_cmdcounter(SM202_card, SM202_axis, 0x00); 
	ps400_get_cmdcounter(SM202_card, SM202_axis, &position);
	SM202_POSITION = (position);
	ReleaseMutex(SM202_mutex);
// 	SM202_HOMED_FLAG = OFF;
// 	log("xxxxxxxxxx off sm202 home flag=%d xxxxxxxxx",SM202_HOMED_FLAG);
	return(0x00);
}
int SM202_jog_cw(void)
{
	long START_V, DRIVE_V, ACC;
	int  errcode;

	if(SM202_SERVO_ALARM==ON)	return(SM202_ALARM_ERR);
	if(SM202_SERVO_READY==OFF)	return(SM202_NOT_READY_ERR);
	if(SM202_SERVO_CHECK==OFF)	return(SM202_NOT_READY_ERR);


	DRIVE_V = (long)(SM202_AXIS_JOG_SPEED); // unit = pulses per second PPS
	ACC		= (long)(SM202_AXIS_JOG_ACCEL_TIME);
	START_V = (int)(sqrt((double)DRIVE_V));
	if (START_V<2000) START_V = 2000;
	WaitForSingleObject(SM202_mutex,INFINITE);
	if((errcode=ps400_velocity_move(SM202_card, SM202_axis, 
		START_V, DRIVE_V, ACC, 
		MOVE_DIRECTION_FORWARD)) != ERROR_SUCCESS){
			ReleaseMutex(SM202_mutex);
			return(SM202_MOTION_COMMAND_ERR);
	}
	ReleaseMutex(SM202_mutex);
	return(0x00);
}
int SM202_jog_ccw(void)
{
	long START_V, DRIVE_V, ACC;
	int  errcode;

	if(SM202_SERVO_ALARM==ON )	return(SM202_ALARM_ERR);
	if(SM202_SERVO_READY==OFF)	return(SM202_NOT_READY_ERR);
	if(SM202_SERVO_CHECK==OFF)	return(SM202_NOT_READY_ERR);


	DRIVE_V = (long)(SM202_AXIS_JOG_SPEED); // unit = pulses per second PPS
	ACC		= (long)(SM202_AXIS_JOG_ACCEL_TIME);
	START_V = (int)(sqrt((double)DRIVE_V));
	if (START_V<2000) START_V = 2000;
	WaitForSingleObject(SM202_mutex,INFINITE);
	if((errcode=ps400_velocity_move(SM202_card, SM202_axis, 
		START_V, DRIVE_V, ACC, 
		MOVE_DIRECTION_REVERSE)) != ERROR_SUCCESS){
			ReleaseMutex(SM202_mutex);
			return(SM202_MOTION_COMMAND_ERR);
	}
	ReleaseMutex(SM202_mutex);
	return(0x00);
}  
int SM202_relative_movement(int target, int speed, int acceleration, int deceleration)
{
	long	START_V, DRIVE_V, ACC, DEC;
	long	distance;
	int		timeout, errcode;
	BYTE	status;
	WORD	StopStatus;

	if(SM202_SERVO_ALARM==ON )	return(SM202_ALARM_ERR);
	if(SM202_SERVO_READY==OFF)	return(SM202_NOT_READY_ERR);
	if(SM202_SERVO_CHECK==OFF)	return(SM202_NOT_READY_ERR);
	if(SM202_LMT_N		==ON )	return(SM202_N_LIMIT_ERR);
	if(SM202_LMT_P		==ON )	return(SM202_P_LIMIT_ERR);
	if(EMERGENCY_STOP_FLAG == TRUE) return(MACHINE_EMERGENCY_STOP_WARNING);

	read_SM202_position();
	distance = (target - SM202_POSITION); 
	if(abs(distance) < 1) return(0);

	START_V	= (int)(sqrt((double)speed));	// initial speed
	DRIVE_V	= speed;				// driving speed
	ACC		= acceleration;			// Acceleration 
	DEC		= deceleration;			// Deceleration 
	if (START_V<2000) START_V = 2000;
	WaitForSingleObject(SM202_mutex,INFINITE);
	if((errcode=ps400_t_move(SM202_card, SM202_axis, 
		START_V, DRIVE_V, ACC, DEC, distance, 8)) != SUCCESS_NO_ERROR){
			ReleaseMutex(SM202_mutex);
			return(SM202_MOTION_COMMAND_ERR);
	}
	ReleaseMutex(SM202_mutex);
	timeout=3000;
	ps400_motion_done(SM202_card, SM202_axis, &status, &StopStatus);
	while((status != MOTION_DONE)&&(timeout-- >= 0x00)){
		if((SM202_LMT_N==ON)||(SM202_LMT_P==ON)){
			SM202_motor_stop();
			if(SM202_LMT_P==ON) return(SM202_P_LIMIT_ERR);
			return(SM202_N_LIMIT_ERR);
		}
		if(EmergencyStopFlag==ON){
			SM202_motor_stop();
			return(MACHINE_EMERGENCY_STOP_WARNING);
		}
		ps400_motion_done(SM202_card, SM202_axis, &status, &StopStatus);
		Sleep(10);
	}
	timeout = 1000/10; 
	read_SM202_position();
	while((abs(target-SM202_POSITION) > 4)&&(timeout-- >= 0x00)){
		if((SM202_LMT_N==ON)||(SM202_LMT_P==ON)){
			SM202_motor_stop();
			if(SM202_LMT_P==ON) return(SM202_P_LIMIT_ERR);
			return(SM202_N_LIMIT_ERR);
		}
		if(EmergencyStopFlag==ON){
			SM202_motor_stop();
			return(MACHINE_EMERGENCY_STOP_WARNING);
		}
		read_SM202_position();
		Sleep(10);
	}
	if(timeout < 0x00) return(SM202_NOT_IN_POSITION_ERR);
	return(0);
}
int SM202_move_to_home_position(void)
{
	int accel, speed, target, y;

	if(SM202_HOMED_FLAG	==OFF)	return(SM202_NOT_HOMED_YET_WARNING);

	target	= SM202_HOME_OFFSET;
	speed	= (long)(SM202_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM202_AXIS_MOVE_ACCEL_TIME);

	if((y=SM202_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM202_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM202_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM202_move_to_loading_position(void)
{
	int accel, speed, target, y;

	if(SM202_HOMED_FLAG	==OFF)	return(SM202_NOT_HOMED_YET_WARNING);

	target	= AA_Z_LOADING_POS*SM202_PositionRatio;
	speed	= (long)(SM202_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM202_AXIS_MOVE_ACCEL_TIME);

	if((y=SM202_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM202_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM202_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}

int SM202_relative_short_move(int MoveDistance)
{
	int accel, speed, target, y;

	read_SM202_position();
	target	= (SM202_POSITION + MoveDistance); 
	speed	= (long)(SM202_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM202_AXIS_MOVE_ACCEL_TIME);

	if(abs(SM202_SHORT_MOVE_DISTANCE) < 1) return(0);
	if((y=SM202_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	return(0x00);
}
int SM202_move_to_load_position1(void)//SM202_dispense_x_move_to_vision_position
{
	int accel, speed, target, y;

	if(SM202_HOMED_FLAG	==OFF)	return(SM202_NOT_HOMED_YET_WARNING);

	target	= SM202_POSITION_1*SM202_PositionRatio/*-SM202_POSITION*/; 
	speed	= (long)(SM202_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM202_AXIS_MOVE_ACCEL_TIME);
	log("dv=%d,a=%d",speed,accel);
	if((y=SM202_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM202_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM202_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	log("move to P1 y.");
	int cnt;
	for(cnt=300;cnt>0;cnt--){	
		log("p1=%d,p=%d",(int)(SM202_POSITION_1*SM202_PositionRatio),SM202_POSITION);
		if((int)(SM202_POSITION_1*SM202_PositionRatio-SM202_POSITION)<4) break;
		Sleep(50);
	}
	if(cnt>0) return(0x00);
	else return(0x01);
}

int SM202_move_to_load_position2(void)//SM202_dispense_x_move_vision_center
{
	int accel, speed, target, y;

	if(SM202_HOMED_FLAG	==OFF)	return(SM202_NOT_HOMED_YET_WARNING);

	target	= SM202_POSITION_2*SM202_PositionRatio;
	log("target=%d",target);
	speed	= (long)(SM202_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM202_AXIS_MOVE_ACCEL_TIME);

	if((y=SM202_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM202_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM202_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	int cnt;
	for(cnt=300;cnt>0;cnt--){	
		log("p1=%d,p=%d",(int)(SM202_POSITION_2*SM202_PositionRatio),SM202_POSITION);
		if((int)(SM202_POSITION_2*SM202_PositionRatio-SM202_POSITION)<4) break;
		Sleep(50);
	}
	if(cnt>0) return(0x00);
	else return(0x01);
}
int SM202_move_to_load_position3(void)//SM202_dispense_x_move_vision_center
{
	int accel, speed, target, y;

	if(SM202_HOMED_FLAG	==OFF)	return(SM202_NOT_HOMED_YET_WARNING);

	target	= SM202_POSITION_3*SM202_PositionRatio;
	log("target=%d",target);
	speed	= (long)(SM202_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM202_AXIS_MOVE_ACCEL_TIME);

	if((y=SM202_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM202_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM202_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	int cnt;
	for(cnt=300;cnt>0;cnt--){	
		log("p3=%d,p=%d",(int)(SM202_POSITION_3*SM202_PositionRatio),SM202_POSITION);
		if((int)(SM202_POSITION_3*SM202_PositionRatio-SM202_POSITION)<4) break;
		Sleep(50);
	}
	if(cnt>0) return(0x00);
	else return(0x01);
}
int SM202_move_to_load_position4(void)//SM202_dispense_x_move_vision_center
{
	int accel, speed, target, y;

	if(SM202_HOMED_FLAG	==OFF)	return(SM202_NOT_HOMED_YET_WARNING);

	target	= SM202_POSITION_4*SM202_PositionRatio;
	log("target=%d",target);
	speed	= (long)(SM202_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM202_AXIS_MOVE_ACCEL_TIME);

	if((y=SM202_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM202_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM202_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	int cnt;
	for(cnt=300;cnt>0;cnt--){	
		log("p4=%d,p=%d",(int)(SM202_POSITION_4*SM202_PositionRatio),SM202_POSITION);
		if((int)(SM202_POSITION_4*SM202_PositionRatio-SM202_POSITION)<4) break;
		Sleep(50);
	}
	if(cnt>0) return(0x00);
	else return(0x01);
}

int SM202_move_to_load_position5(void)//SM202_dispense_x_move_vision_center
{
	int accel, speed, target, y;

	if(SM202_HOMED_FLAG	==OFF)	return(SM202_NOT_HOMED_YET_WARNING);

	target	= SM202_POSITION_5*SM202_PositionRatio; 
	speed	= (long)(SM202_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM202_AXIS_MOVE_ACCEL_TIME);
	log("target=%d",target);
	if((y=SM202_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM202_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM202_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	int cnt;
	for(cnt=300;cnt>0;cnt--){	
		log("p5=%d,p=%d",(int)(SM202_POSITION_5*SM202_PositionRatio),SM202_POSITION);
		if((int)(SM202_POSITION_5*SM202_PositionRatio-SM202_POSITION)<4) break;
		Sleep(50);
	}
	if(cnt>0) return(0x00);
	else return(0x01);
}
int SM202_move_to_load_position6(void)//SM202_dispense_x_move_vision_center
{
	int accel, speed, target, y;

	if(SM202_HOMED_FLAG	==OFF)	return(SM202_NOT_HOMED_YET_WARNING);

	target	= SM202_POSITION_6*SM202_PositionRatio; 
	speed	= (long)(SM202_AXIS_MOVE_SPEED); // unit = pulses per second PPS
	accel	= (long)(SM202_AXIS_MOVE_ACCEL_TIME);
	log("target=%d",target);
	if((y=SM202_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM202_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM202_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	int cnt;
	for(cnt=300;cnt>0;cnt--){	
		log("p6=%d,p=%d",(int)(SM202_POSITION_6*SM202_PositionRatio),SM202_POSITION);
		if((int)(SM202_POSITION_6*SM202_PositionRatio-SM202_POSITION)<4) break;
		Sleep(50);
	}
	if(cnt>0) return(0x00);
	else return(0x01);
}










int SM202_move_to_target_distance()
{
	int accel, speed, target, y;

	//	if(SM202_HOMED_FLAG	==OFF)	return(SM202_NOT_HOMED_YET_WARNING);
	//	log("SM202 active.");
	target	= (int)(AA_FINAL_MOVE_DISTANCE * SM202_PositionRatio)*my_a; 
	log("SM202 target:%d, a=%.2f", target, my_a);
	speed	= (long)(SM202_AXIS_MOVE_SPEED)*my_a; // unit = pulses per second PPS
	accel	= (long)(SM202_AXIS_MOVE_ACCEL_TIME)*my_a;

	if((y=SM202_relative_movement(target, speed, accel, accel)) != 0x00){
		if(y==MACHINE_EMERGENCY_STOP_WARNING){ SM202_motor_stop(); return(MACHINE_EMERGENCY_STOP_WARNING); }
		if((y=SM202_relative_movement(target, speed, accel, accel)) != 0x00) return(y);
	}
	return(0x00);
}


//----------------------------------------------------------------------
int SM202_initialization(void)
{
	int ret;
	log("111");
	if((SM202_HOMED_FLAG==ON)||(SM202_LMT_P==ON)){
		if((ret=SM202_move_to_home_position())	!= 0) return(ret);
	}
	else{
		if((ret=SM202_encoder_reset())	!= 0) return(ret);
	}
	return(0x00);
}

void read_SM202_position(void)
{
	long position;

	WaitForSingleObject(SM202_mutex,INFINITE);
	ps400_get_cmdcounter(SM202_card, SM202_axis, &position);
	SM202_POSITION = (position);
	ReleaseMutex(SM202_mutex);
	return;
}

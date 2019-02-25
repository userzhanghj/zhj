#include "inf_motion_aa.h"

//=== Motion Card related Input function ========================================================
WORD PS400_motion_input_function(BYTE bBoardID, WORD axis)
{
	WORD	DI_Status;
	BYTE	In3Status;                            
	short	errcode;

	if((errcode = ps400_get_mdi_status(bBoardID, axis, &DI_Status)) != SUCCESS_NO_ERROR){
		return(0xFFFF);
	}
	if((errcode = ps400_get_in3(bBoardID, axis, &In3Status)) != SUCCESS_NO_ERROR){
		return(0xFFFF);
	}
	if(In3Status==0x01) DI_Status |= 0x200;
	else				DI_Status &= 0xFDFF;

	return(DI_Status);
}

short ps400_servo_motor_servo_on(BYTE bBoardID, WORD axis)
{
	short errcode;

	if((errcode = ps400_servo_on(bBoardID, axis, SERVO_ON, SERVO_MANUAL_OFF)) != SUCCESS_NO_ERROR){
		return(errcode);
	}
	if(bBoardID==0x00){
		mshm.CARD1_WR4_STATUS |= axis;
	}
	else if(bBoardID==0x01){
		mshm.CARD2_WR4_STATUS |= axis;
	}
	else if(bBoardID==0x02){
		mshm.CARD3_WR4_STATUS |= axis;
	}
	return(SUCCESS_NO_ERROR);
}

short ps400_servo_motor_servo_off(BYTE bBoardID, WORD axis)
{
	short errcode;

	if((errcode = ps400_servo_on(bBoardID, axis, SERVO_OFF, SERVO_MANUAL_OFF)) != SUCCESS_NO_ERROR){
		return(errcode);
	}
	if(bBoardID==0x00){
		mshm.CARD1_WR4_STATUS &= (axis^0xff);
	}
	else if(bBoardID==0x01){
		mshm.CARD2_WR4_STATUS &= (axis^0xff);
	}
	else if(bBoardID==0x02){
		mshm.CARD3_WR4_STATUS &= (axis^0xff);
	}
	HOME_FLAG = OFF;
	log("servo motor,home flag=%d",HOME_FLAG);
	return(SUCCESS_NO_ERROR);
}
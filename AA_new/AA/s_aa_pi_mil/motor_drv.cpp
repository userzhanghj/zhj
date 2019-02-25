#include "inf_motion_aa.h"

//*****************************************************************************
// Subroutine : Motion_Motor_Init(void)                                        
//*****************************************************************************
int Motion_Motor_Init(void)
{
int errcode;

	//==== reset all PISO-PS400 card ===========================================
	if((errcode=ps400_reset_all()) != SUCCESS_NO_ERROR) return(errcode);

	//--------------------------------------------------------------------------
	//==== set Card-1 configuration ============================================
	//==== set pulse mode configuration ========================================
	if((errcode=ps400_set_pls_cfg(SM101_card, SM101_axis, PULSE_MODE_PULSE_DIRECTION, PULSE_LOGIC_ACTIVE_LOW, PULSE_FORWARD_ACTIVE_LOW))!= SUCCESS_NO_ERROR) return(errcode);
	if((errcode=ps400_set_pls_cfg(SM102_card, SM102_axis, PULSE_MODE_PULSE_DIRECTION, PULSE_LOGIC_ACTIVE_LOW, PULSE_FORWARD_ACTIVE_LOW))!= SUCCESS_NO_ERROR) return(errcode);
//	if((errcode=ps400_set_pls_cfg(SM103_card, SM103_axis, PULSE_MODE_PULSE_DIRECTION, PULSE_LOGIC_ACTIVE_LOW, 0))!= SUCCESS_NO_ERROR) return(errcode);
	if((errcode=ps400_set_pls_cfg(SM103_card, SM103_axis, PULSE_MODE_PULSE_DIRECTION, PULSE_LOGIC_ACTIVE_LOW, PULSE_FORWARD_ACTIVE_LOW))!= SUCCESS_NO_ERROR) return(errcode);
	 
	//==== set encoder mode configurat1on ========================================
	if((errcode=ps400_set_enc_cfg(SM101_card, SM101_axis, ENCODER_MODE_CW_CCW, 0))!= SUCCESS_NO_ERROR) return(errcode);
	if((errcode=ps400_set_enc_cfg(SM102_card, SM102_axis, ENCODER_MODE_CW_CCW, 0))!= SUCCESS_NO_ERROR) return(errcode);
//	if((errcode=ps400_set_enc_cfg(SM103_card, SM103_axis, ENCODER_MODE_AB_DIVID_4, 0))!= SUCCESS_NO_ERROR) return(errcode);
	if((errcode=ps400_set_enc_cfg(SM103_card, SM103_axis, ENCODER_MODE_CW_CCW, 0))!= SUCCESS_NO_ERROR) return(errcode);

	//==== Set hardware limit signal enable/disable =====================================
	if((errcode=ps400_set_limit(SM101_card, SM101_axis, LIMIT_LOGIC_ACTIVE_HIGH, LIMIT_STOP_SUDDEN))!= SUCCESS_NO_ERROR) return(errcode);
	if((errcode=ps400_set_limit(SM102_card, SM102_axis, LIMIT_LOGIC_ACTIVE_HIGH, LIMIT_STOP_SUDDEN))!= SUCCESS_NO_ERROR) return(errcode);
//	if((errcode=ps400_set_limit(SM103_card, SM103_axis, LIMIT_LOGIC_ACTIVE_HIGH, LIMIT_STOP_SUDDEN))!= SUCCESS_NO_ERROR) return(errcode);
	if((errcode=ps400_set_limit(SM103_card, SM103_axis, LIMIT_LOGIC_ACTIVE_HIGH,  LIMIT_STOP_SUDDEN))!= SUCCESS_NO_ERROR) return(errcode);

	//==== Multiple = 8000000/R times ========
	if((errcode=ps400_set_range(SM101_card, SM101_axis, (int)SM101_RangeSet))!= SUCCESS_NO_ERROR) return(errcode);
	if((errcode=ps400_set_range(SM102_card, SM102_axis, (int)SM102_RangeSet))!= SUCCESS_NO_ERROR) return(errcode);
//	if((errcode=ps400_set_range(SM103_card, SM103_axis, (int)SM103_RangeSet))!= SUCCESS_NO_ERROR) return(errcode);
	if((errcode=ps400_set_range(SM103_card, SM103_axis, (int)SM104_RangeSet))!= SUCCESS_NO_ERROR) return(errcode);

	//==== enable In-Position-signal and set its active level ========
	if((errcode=ps400_set_inp(SM101_card, SM101_axis, INP_DISABLE_FEATURE,  INP_LOGIC_ACTIVE_LOW))	!= ERROR_SUCCESS) return(errcode);
	if((errcode=ps400_set_inp(SM102_card, SM102_axis, INP_DISABLE_FEATURE,  INP_LOGIC_ACTIVE_LOW))	!= ERROR_SUCCESS) return(errcode);
//	if((errcode=ps400_set_inp(SM103_card, SM103_axis, INP_ENABLE_FEATURE,   INP_LOGIC_ACTIVE_LOW))	!= ERROR_SUCCESS) return(errcode);
	if((errcode=ps400_set_inp(SM103_card, SM103_axis, INP_DISABLE_FEATURE,  INP_LOGIC_ACTIVE_LOW))	!= ERROR_SUCCESS) return(errcode);
// 
// 	//==== Set alarm signal enable/disable =====================================
	if((errcode=ps400_set_alarm(SM101_card, SM101_axis, ALARM_DISABLE_FEATURE, ALARM_LOGIC_ACTIVE_LOW))!= SUCCESS_NO_ERROR) return(errcode);
	if((errcode=ps400_set_alarm(SM102_card, SM102_axis, ALARM_DISABLE_FEATURE, ALARM_LOGIC_ACTIVE_LOW))!= SUCCESS_NO_ERROR) return(errcode);
//	if((errcode=ps400_set_alarm(SM103_card, SM103_axis, ALARM_DISABLE_FEATURE, ALARM_LOGIC_ACTIVE_LOW))!= SUCCESS_NO_ERROR) return(errcode);
	if((errcode=ps400_set_alarm(SM103_card, SM103_axis, ALARM_DISABLE_FEATURE, ALARM_LOGIC_ACTIVE_LOW))!= SUCCESS_NO_ERROR) return(errcode);

	//--------------------------------------------------------------------------
	//==== set Card-2 configuration ============================================
	//==== set pulse mode configuration ========================================
	if((errcode=ps400_set_pls_cfg(SM201_card, SM201_axis, PULSE_MODE_PULSE_DIRECTION, PULSE_LOGIC_ACTIVE_LOW, PULSE_FORWARD_ACTIVE_LOW))!= SUCCESS_NO_ERROR) return(errcode);
	if((errcode=ps400_set_pls_cfg(SM202_card, SM202_axis, PULSE_MODE_PULSE_DIRECTION, PULSE_LOGIC_ACTIVE_LOW, PULSE_FORWARD_ACTIVE_LOW))!= SUCCESS_NO_ERROR) return(errcode);
	if((errcode=ps400_set_pls_cfg(SM203_card, SM203_axis, PULSE_MODE_PULSE_DIRECTION, PULSE_LOGIC_ACTIVE_LOW, PULSE_FORWARD_ACTIVE_LOW))!= SUCCESS_NO_ERROR) return(errcode);
//	if((errcode=ps400_set_pls_cfg(SM204_card, SM204_axis, PULSE_MODE_PULSE_DIRECTION,	PULSE_LOGIC_ACTIVE_LOW, PULSE_FORWARD_ACTIVE_LOW))!= SUCCESS_NO_ERROR) return(errcode);

	//==== set encoder mode configurat1on ========================================
	if((errcode=ps400_set_enc_cfg(SM201_card, SM201_axis, ENCODER_MODE_AB, 0))!= SUCCESS_NO_ERROR) return(errcode);
	if((errcode=ps400_set_enc_cfg(SM202_card, SM202_axis, ENCODER_MODE_AB, 0))!= SUCCESS_NO_ERROR) return(errcode);
	if((errcode=ps400_set_enc_cfg(SM203_card, SM203_axis, ENCODER_MODE_AB, 0))!= SUCCESS_NO_ERROR) return(errcode);
//	if((errcode=ps400_set_enc_cfg(SM204_card, SM204_axis, ENCODER_MODE_AB_DIVID_4, 0))!= SUCCESS_NO_ERROR) return(errcode);

	//==== Set hardware limit signal enable/disable =====================================
	if((errcode=ps400_set_limit(SM201_card, SM201_axis, LIMIT_LOGIC_ACTIVE_HIGH, LIMIT_STOP_SUDDEN))!= SUCCESS_NO_ERROR) return(errcode);
	if((errcode=ps400_set_limit(SM202_card, SM202_axis, LIMIT_LOGIC_ACTIVE_HIGH, LIMIT_STOP_SUDDEN))!= SUCCESS_NO_ERROR) return(errcode);
	if((errcode=ps400_set_limit(SM203_card, SM203_axis, LIMIT_LOGIC_ACTIVE_HIGH, LIMIT_STOP_SUDDEN))!= SUCCESS_NO_ERROR) return(errcode);
//	if((errcode=ps400_set_limit(SM204_card, SM204_axis, LIMIT_LOGIC_ACTIVE_HIGH, LIMIT_STOP_SUDDEN))!= SUCCESS_NO_ERROR) return(errcode);

	//==== Multiple = 8000000/R times ========
	if((errcode=ps400_set_range(SM201_card, SM201_axis, (int)SM201_RangeSet))!= SUCCESS_NO_ERROR) return(errcode);
	if((errcode=ps400_set_range(SM202_card, SM202_axis, (int)SM202_RangeSet))!= SUCCESS_NO_ERROR) return(errcode);
	if((errcode=ps400_set_range(SM203_card, SM203_axis, (int)SM203_RangeSet))!= SUCCESS_NO_ERROR) return(errcode);
//	if((errcode=ps400_set_range(SM204_card, SM204_axis, (int)SM204_RangeSet))!= SUCCESS_NO_ERROR) return(errcode);

	//==== enable In-Position-signal and set its active level ========
	if((errcode=ps400_set_inp(SM201_card, SM201_axis, INP_DISABLE_FEATURE,  INP_LOGIC_ACTIVE_LOW))	!= ERROR_SUCCESS) return(errcode);
	if((errcode=ps400_set_inp(SM202_card, SM202_axis, INP_DISABLE_FEATURE,  INP_LOGIC_ACTIVE_LOW))	!= ERROR_SUCCESS) return(errcode);
	if((errcode=ps400_set_inp(SM203_card, SM203_axis, INP_DISABLE_FEATURE,  INP_LOGIC_ACTIVE_LOW))	!= ERROR_SUCCESS) return(errcode);
//	if((errcode=ps400_set_inp(SM204_card, SM204_axis, INP_DISABLE_FEATURE, INP_LOGIC_ACTIVE_LOW))	!= ERROR_SUCCESS) return(errcode);
// 
// 	//==== Set alarm signal enable/disable =====================================
	if((errcode=ps400_set_alarm(SM201_card, SM201_axis, ALARM_DISABLE_FEATURE, ALARM_LOGIC_ACTIVE_LOW))!= SUCCESS_NO_ERROR) return(errcode);
	if((errcode=ps400_set_alarm(SM202_card, SM202_axis, ALARM_DISABLE_FEATURE, ALARM_LOGIC_ACTIVE_LOW))!= SUCCESS_NO_ERROR) return(errcode);
	if((errcode=ps400_set_alarm(SM203_card, SM203_axis, ALARM_DISABLE_FEATURE, ALARM_LOGIC_ACTIVE_LOW))!= SUCCESS_NO_ERROR) return(errcode);
//	if((errcode=ps400_set_alarm(SM204_card, SM204_axis, ALARM_DISABLE_FEATURE, ALARM_LOGIC_ACTIVE_LOW))!= SUCCESS_NO_ERROR) return(errcode);

	return(0x00);
}

//==== define PISO-PS400 board number ============
#define PS400BoardID0			0
#define PS400BoardID1			1
#define PS400BoardID2			2

//==================================================================================  
#define ON			0x01
#define OFF			0x00
#define Normal		==0x00)?ON:OFF) 
#define Inverse		==0x00)?OFF:ON)

//======================================================================
// definition for motion control card-1
//======================================================================
#define SM101_card			PS400BoardID0
#define SM102_card			PS400BoardID0
#define SM103_card			PS400BoardID0
#define SM104_card			PS400BoardID0

#define SM101_axis			AXIS_X
#define SM102_axis			AXIS_Y
#define SM103_axis			AXIS_Z
#define SM104_axis			AXIS_U

#define Card1_X_Position	SM101_POSITION		
#define Card1_Y_Position	SM102_POSITION		
#define Card1_Z_Position	SM103_POSITION
#define Card1_U_Position	SM104_POSITION

#define SM101_mask			0x01
#define SM102_mask			0x02
#define SM103_mask			0x04
#define SM104_mask			0x08

//======================================================================
// definition for motion control card-2
//======================================================================
#define SM201_card			PS400BoardID1
#define SM202_card			PS400BoardID1
#define SM203_card			PS400BoardID1
#define SM204_card			PS400BoardID1

#define SM201_axis			AXIS_X
#define SM202_axis			AXIS_Y
#define SM203_axis			AXIS_Z
#define SM204_axis			AXIS_U

#define Card2_X_Position	SM201_POSITION		
#define Card2_Y_Position	SM202_POSITION		
#define Card2_Z_Position	SM203_POSITION
#define Card2_U_Position	SM204_POSITION

#define SM201_mask			0x01
#define SM202_mask			0x02
#define SM203_mask			0x04
#define SM204_mask			0x08

//========================================================================================
// Card-1, X-axis, SM101 -- 
#define SM101_LMT_N			(((PS400_motion_input_function(SM101_card,SM101_axis) & 0x0002)Inverse
#define SM101_LMT_P			(((PS400_motion_input_function(SM101_card,SM101_axis) & 0x0004)Inverse
#define SM101_SERVO_ALARM	(((PS400_motion_input_function(SM101_card,SM101_axis) & 0x0010)Inverse
#define SM101_HOME			(((PS400_motion_input_function(SM101_card,SM101_axis) & 0x0020)Inverse
#define SM101_INPOS			(((PS400_motion_input_function(SM101_card,SM101_axis) & 0x0080)Inverse
#define SM101_SERVO_READY	(((PS400_motion_input_function(SM101_card,SM101_axis) & 0x0200)Inverse
#define SM101_SERVO_CHECK	((((mshm.CARD1_WR4_STATUS) & SM101_mask)Normal	// X-servo-on check

#define SM101_SERVO_ON		ps400_servo_motor_servo_off(SM101_card, SM101_axis)
#define SM101_SERVO_OFF		ps400_servo_motor_servo_on(SM101_card, SM101_axis)

//========================================================================================
// Card-1, Y-axis, SM102 --  
#define SM102_LMT_N			(((PS400_motion_input_function(SM102_card,SM102_axis) & 0x0002)Inverse
#define SM102_LMT_P			(((PS400_motion_input_function(SM102_card,SM102_axis) & 0x0004)Inverse
#define SM102_SERVO_ALARM	(((PS400_motion_input_function(SM102_card,SM102_axis) & 0x0010)Inverse
#define SM102_HOME			(((PS400_motion_input_function(SM102_card,SM102_axis) & 0x0020)Inverse
#define SM102_INPOS			(((PS400_motion_input_function(SM102_card,SM102_axis) & 0x0080)Inverse
#define SM102_SERVO_READY	(((PS400_motion_input_function(SM102_card,SM102_axis) & 0x0200)Inverse
#define SM102_SERVO_CHECK	((((mshm.CARD1_WR4_STATUS) & SM102_mask)Normal	// Y-servo-on check

#define SM102_SERVO_ON		ps400_servo_motor_servo_off(SM102_card, SM102_axis)
#define SM102_SERVO_OFF		ps400_servo_motor_servo_on(SM102_card, SM102_axis)

//========================================================================================
// Card-1, Z-axis, SM103 --  
#define SM103_LMT_N			(((PS400_motion_input_function(SM103_card,SM103_axis) & 0x0002)Inverse
#define SM103_LMT_P			(((PS400_motion_input_function(SM103_card,SM103_axis) & 0x0004)Inverse
#define SM103_SERVO_ALARM	(((PS400_motion_input_function(SM103_card,SM103_axis) & 0x0010)Inverse
#define SM103_HOME			(((PS400_motion_input_function(SM103_card,SM103_axis) & 0x0020)Inverse
#define SM103_INPOS			(((PS400_motion_input_function(SM103_card,SM103_axis) & 0x0080)Inverse
#define SM103_SERVO_READY	(((PS400_motion_input_function(SM103_card,SM103_axis) & 0x0200)Inverse
#define SM103_SERVO_CHECK	((((mshm.CARD1_WR4_STATUS) & SM103_mask)Normal	// Z-servo-on check

#define SM103_SERVO_ON		ps400_servo_motor_servo_off(SM103_card, SM103_axis)
#define SM103_SERVO_OFF		ps400_servo_motor_servo_on(SM103_card, SM103_axis)

//========================================================================================
// Card-1, U-axis, SM104 --  
#define SM104_LMT_N			(((PS400_motion_input_function(SM103_card,SM103_axis) & 0x0002)Inverse
#define SM104_LMT_P			(((PS400_motion_input_function(SM103_card,SM103_axis) & 0x0004)Inverse
#define SM104_SERVO_ALARM	(((PS400_motion_input_function(SM103_card,SM103_axis) & 0x0010)Inverse
#define SM104_HOME			(((PS400_motion_input_function(SM103_card,SM103_axis) & 0x0020)Inverse
#define SM104_INPOS			(((PS400_motion_input_function(SM103_card,SM103_axis) & 0x0080)Inverse
#define SM104_SERVO_READY	(((PS400_motion_input_function(SM103_card,SM103_axis) & 0x0200)Inverse
#define SM104_SERVO_CHECK	((((mshm.CARD1_WR4_STATUS) & SM104_mask)Normal	// U-servo-on check

#define SM104_SERVO_ON	ps400_servo_motor_servo_off(SM103_card, SM103_axis)
#define SM104_SERVO_OFF	ps400_servo_motor_servo_on(SM103_card, SM103_axis)

//========================================================================================
// Card-2, X-axis, SM201 -- 
#define SM201_LMT_N			(((PS400_motion_input_function(SM201_card,SM201_axis) & 0x0002)Inverse
#define SM201_LMT_P			(((PS400_motion_input_function(SM201_card,SM201_axis) & 0x0004)Inverse
#define SM201_SERVO_ALARM	(((PS400_motion_input_function(SM201_card,SM201_axis) & 0x0010)Inverse
#define SM201_HOME			(((PS400_motion_input_function(SM201_card,SM201_axis) & 0x0020)Inverse
#define SM201_INPOS			(((PS400_motion_input_function(SM201_card,SM201_axis) & 0x0080)Inverse
#define SM201_SERVO_READY	(((PS400_motion_input_function(SM201_card,SM201_axis) & 0x0200)Inverse
#define SM201_SERVO_CHECK	((((mshm.CARD2_WR4_STATUS) & SM201_mask)Normal	// X-servo-on check

#define SM201_SERVO_ON		ps400_servo_motor_servo_off(SM201_card, SM201_axis)
#define SM201_SERVO_OFF		ps400_servo_motor_servo_on(SM201_card, SM201_axis)

//========================================================================================
// Card-2, Y-axis, SM202 --  
#define SM202_LMT_N			(((PS400_motion_input_function(SM202_card,SM202_axis) & 0x0002)Inverse
#define SM202_LMT_P			(((PS400_motion_input_function(SM202_card,SM202_axis) & 0x0004)Inverse
#define SM202_SERVO_ALARM	(((PS400_motion_input_function(SM202_card,SM202_axis) & 0x0010)Inverse
#define SM202_HOME			(((PS400_motion_input_function(SM202_card,SM202_axis) & 0x0020)Inverse
#define SM202_INPOS			(((PS400_motion_input_function(SM202_card,SM202_axis) & 0x0080)Inverse
#define SM202_SERVO_READY	(((PS400_motion_input_function(SM202_card,SM202_axis) & 0x0200)Inverse
#define SM202_SERVO_CHECK	((((mshm.CARD2_WR4_STATUS) & SM202_mask)Normal// Y-servo-on check

#define SM202_SERVO_ON		ps400_servo_motor_servo_off(SM202_card, SM202_axis)
#define SM202_SERVO_OFF		ps400_servo_motor_servo_on(SM202_card, SM202_axis)

//========================================================================================
// Card-2, Z-axis, SM203 --  
#define SM203_LMT_N			(((PS400_motion_input_function(SM203_card,SM203_axis) & 0x0002)Inverse
#define SM203_LMT_P			(((PS400_motion_input_function(SM203_card,SM203_axis) & 0x0004)Inverse
#define SM203_SERVO_ALARM	(((PS400_motion_input_function(SM203_card,SM203_axis) & 0x0010)Inverse
#define SM203_HOME			(((PS400_motion_input_function(SM203_card,SM203_axis) & 0x0020)Inverse
#define SM203_INPOS			(((PS400_motion_input_function(SM203_card,SM203_axis) & 0x0080)Inverse
#define SM203_SERVO_READY	(((PS400_motion_input_function(SM203_card,SM203_axis) & 0x0200)Inverse
#define SM203_SERVO_CHECK	((((mshm.CARD2_WR4_STATUS) & SM203_mask)Normal	// Z-servo-on check

#define SM203_SERVO_ON		ps400_servo_motor_servo_off(SM203_card, SM203_axis)
#define SM203_SERVO_OFF		ps400_servo_motor_servo_on(SM203_card, SM203_axis)

//========================================================================================
// Card-2, U-axis, SM204 --  
#define SM204_LMT_N			(((PS400_motion_input_function(SM204_card,SM204_axis) & 0x0002)Inverse
#define SM204_LMT_P			(((PS400_motion_input_function(SM204_card,SM204_axis) & 0x0004)Inverse
#define SM204_SERVO_ALARM	(((PS400_motion_input_function(SM204_card,SM204_axis) & 0x0010)Inverse
#define SM204_HOME			(((PS400_motion_input_function(SM204_card,SM204_axis) & 0x0020)Inverse
#define SM204_INPOS			(((PS400_motion_input_function(SM204_card,SM204_axis) & 0x0080)Inverse
#define SM204_SERVO_READY	(((PS400_motion_input_function(SM204_card,SM204_axis) & 0x0200)Inverse
#define SM204_SERVO_CHECK	((((mshm.CARD2_WR4_STATUS) & SM204_mask)Normal	// U-servo-on check
//#define SM204_STEPPER_CHECK	((((mshm.CARD2_WR4_STATUS) & SM204_mask)Normal	// U-servo-on check

#define SM204_STEPPER_ON	ps400_servo_motor_servo_off(SM204_card, SM204_axis)
#define SM204_SERVO_OFF		ps400_servo_motor_servo_on(SM204_card, SM204_axis)



//=======================================================================================
// motor-mutex definition
//=======================================================================================
// Card-1, X,Y,Z,U axes use Motors1_hMutex 
#define SM101_mutex			Motors1_hMutex
#define SM102_mutex			Motors1_hMutex
#define SM103_mutex			Motors1_hMutex
#define SM104_mutex			Motors1_hMutex

// Card-2, X,Y,Z,U axes use Motors2_hMutex 
#define SM201_mutex			Motors2_hMutex
#define SM202_mutex			Motors2_hMutex
#define SM203_mutex			Motors2_hMutex
#define SM204_mutex			Motors2_hMutex

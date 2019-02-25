#ifndef EXTERN
#define EXTERN extern 
#endif

#define MOTORS_PARA_NUM		300  //	300 = 30 x 10  parameters, assign 30 parameters for each motor
#define INT_RAM_NUM			300	 // General purpose integer type shared memory assignment
#define DOUBLE_RAM_NUM		200	 // General purpose floating type shared memory assignment 
#define TIMER_RAM_NUM		120	 // General purpose floating type shared memory assignment 

typedef struct _shared_memory
{
	long	SM101_ENCODER;	// Focus X -- Card1 X
	long	SM102_ENCODER;	// Focus Y -- Card1 Y
	long	SM103_ENCODER;	// Focus Z -- Card1 Z
	long	SM104_ENCODER;	// Focus R -- Card1 U

	long	SM201_ENCODER;	// Dispense X -- Card2 X
	long	SM202_ENCODER;	// Dispense Y -- Card2 Y
	long	SM203_ENCODER;	// Dispense Z -- Card2 Z
	long	SM204_ENCODER;	// Dispense R -- Card2 U

	int		SERVO_MOTOR_PARAMETERS[MOTORS_PARA_NUM];

	int		RAM_INT[INT_RAM_NUM];

	double  RAM_DOUBLE[INT_RAM_NUM];

	int		TIMER[TIMER_RAM_NUM];

	unsigned char   INPUT0_STATUS;		//I/O card #1 Port 0 (S00 ~ S07)
	unsigned char   INPUT1_STATUS;		//I/O card #1 Port 1 (S08 ~ S15)
	unsigned char   INPUT2_STATUS;		//I/O card #1 Port 2 (S16 ~ S23)
	unsigned char   INPUT3_STATUS;		//I/O card #1 Port 3 (S24 ~ S31)

	unsigned char   OUTPUT0_STATUS;		//I/O card #1 Port 0 (Y00 ~ Y07)
	unsigned char   OUTPUT1_STATUS;		//I/O card #1 Port 1 (Y08 ~ Y15)
	unsigned char   OUTPUT2_STATUS;		//I/O card #1 Port 2 (Y16 ~ Y23)
	unsigned char   OUTPUT3_STATUS;		//I/O card #1 Port 3 (Y24 ~ Y31)

	unsigned long	CARD1_WR4_STATUS;
	unsigned long	CARD2_WR4_STATUS;
	unsigned long	CARD3_WR4_STATUS;

	unsigned char	SM101_MOTOR_STATUS;
	unsigned char	SM102_MOTOR_STATUS;
	unsigned char	SM103_MOTOR_STATUS;
	unsigned char	SM104_MOTOR_STATUS;

	unsigned char	SM201_MOTOR_STATUS;
	unsigned char	SM202_MOTOR_STATUS;
	unsigned char	SM203_MOTOR_STATUS;
	unsigned char	SM204_MOTOR_STATUS;

	//-----------------------------------------
	unsigned long	Ps400_AddrBase[4];

	//-----------------------------------------
	unsigned long	P32C32U_AddrBase[4];

} MOTION_SHM;




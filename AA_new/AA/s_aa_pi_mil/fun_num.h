//*** AC servo motor number definition
#define Motor_SM101		1
#define Motor_SM102		2
#define Motor_SM103		3
#define Motor_SM104		4
#define Motor_SM201		5
#define Motor_SM202		6
#define Motor_SM203		7
#define Motor_SM204		8


//*** AC servo motor SM101 motion number definition ===================================
#define SM101_HOMING										1
#define SM101_MOVE_TO_HOME_POSITION							2
#define SM101_FOCUS_X_MOVE_TO_VISION_POSITION				3
#define SM101_FOCUS_X_OFFSET_MOVE_FROM_VISION_POSITION		4
#define SM101_FOCUS_X_MOVE_TO_LASER_POSITION				5
#define SM101_FOCUS_X_MOVE_TO_CONE_POSITION					6
#define SM101_FOCUS_X_INITIALIZATION						7
#define SM101_FOCUS_X_OFFSET_MOVE_FROM_CONE_POSITION		8
#define SM101_MOVE_TO_TARGET_DISTANCE						9

#ifdef DECLARE
int (*SM101_fun[])()={	NULL,												// fun no = 0;
						SM101_encoder_reset,								// 1;
						SM101_move_to_home_position,						// 2;
						SM101_move_to_load_position1,						// 3;
						SM101_move_to_load_position2,						// 4;
						SM101_move_to_load_position3,						// 5;
						SM101_move_to_load_position4,						// 6;
						SM101_move_to_load_position5,						// 7;
						SM101_move_to_load_position6,						// 8;

						NULL };										
#else 
extern int (*SM101_fun[])();
#endif

//*** AC servo motor SM102 motion number definition ===================================
#define SM102_HOMING										1
#define SM102_MOVE_TO_HOME_POSITION							2
#define SM102_FOCUS_Y_MOVE_TO_VISION_POSITION				3
#define SM102_FOCUS_Y_OFFSET_MOVE_FROM_VISION_POSITION		4
#define SM102_FOCUS_Y_MOVE_TO_LASER_POSITION				5
#define SM102_FOCUS_Y_MOVE_TO_CONE_POSITION					6
#define SM102_FOCUS_Y_INITIALIZATION						7
#define SM102_FOCUS_Y_OFFSET_MOVE_FROM_CONE_POSITION		8

#ifdef DECLARE
int (*SM102_fun[])()={	NULL,												// fun no = 0;v
						SM102_encoder_reset,								// 1;
						SM102_move_to_home_position,						// 2;
						SM102_move_to_load_position1,						// 3;
						SM102_move_to_load_position2,						// 4;
						SM102_move_to_load_position3,						// 5;
						SM102_move_to_load_position4,						// 6;
						SM102_move_to_load_position5,						// 7;
						SM102_move_to_load_position6,						// 8;

						NULL };										
#else 
extern int (*SM102_fun[])();
#endif

//*** AC servo motor SM103 motion number definition ===================================
#define SM103_HOMING											1
#define SM103_MOVE_TO_HOME_POSITION								2
#define SM103_FOCUS_Z_MOVE_TO_VISION_POSITION					3
#define SM103_FOCUS_Z_MOVE_TO_LASER_POSITION					4
#define SM103_FOCUS_Z_MOVE_TO_CONE_POSITION_WITH_LASER_OFFSET	5
#define SM103_FOCUS_Z_INITIALIZATION							6

#ifdef DECLARE
int (*SM103_fun[])()={	NULL,													// fun no = 0;
						SM103_encoder_reset,								// 1;
						SM103_move_to_home_position,						// 2;
						SM103_move_to_load_position1,						// 3;
						SM103_move_to_load_position2,						// 4;
						SM103_move_to_load_position3,						// 5;
						SM103_move_to_load_position4,						// 6;
						SM103_move_to_load_position5,						// 7;
						SM103_move_to_load_position6,						// 8;

						NULL };										
#else 
extern int (*SM103_fun[])();
#endif

//*** AC servo motor SM104 motion number definition ===================================
#define SM104_HOMING									1
#define SM104_MOVE_TO_HOME_POSITION						2
#define SM104_FOCUS_R_MOVE_TO_VISION_POSITION			3
#define SM104_FOCUS_R_OFFSET_MOVE_FROM_VISION_POSITION	4
#define SM104_FOCUS_R_INITIALIZATION					5
#define SM104_FOCUS_R_OFFSET_MOVE_FROM_CONE_POSITION	6
#define SM104_MOVE_TO_TARGET_DISTANCE					7

#ifdef DECLARE
int (*SM104_fun[])()={	NULL,												// fun no = 0;
						SM104_encoder_reset,								// 1;
						SM104_move_to_home_position,						// 2;
						SM104_move_to_load_position1,						// 3;
						SM104_move_to_load_position2,						// 4;
						SM104_move_to_load_position3,						// 5;
						SM104_move_to_load_position4,						// 6;
						SM104_move_to_load_position5,						// 7;
						SM104_move_to_load_position6,						// 8;

						NULL };										
#else 
extern int (*SM104_fun[])();
#endif

//*** AC servo motor SM201 motion number definition ===================================
#define SM201_HOMING											1
#define SM201_MOVE_TO_HOME_POSITION								2
#define SM201_DISPENSE_X_MOVE_TO_VISION_POSITION				3
#define SM201_DISPENSE_X_MOVE_TO_LASER_POSITION					4
#define SM201_DISPENSE_X_MOVE_TO_NEEDLE_POSITION				5
#define SM201_DISPENSE_X_MOVE_TO_CURE_CENTER_POSITION			6
#define SM201_DISPENSE_X_MOVE_TO_CURE_INDEX_POSITION_1			7
#define SM201_DISPENSE_X_MOVE_TO_CURE_INDEX_POSITION_2			8
#define SM201_DISPENSE_X_MOVE_TO_CURE_INDEX_POSITION_3			9
#define SM201_DISPENSE_X_MOVE_TO_CURE_INDEX_POSITION_4			10
#define SM201_DISPENSE_X_MOVE_TO_CURE_INDEX_POSITION_5			11
#define SM201_DISPENSE_X_MOVE_TO_TOUCH_POSITION					12
#define SM201_DISPENSE_X_MOVE_TO_GLASS_POSITION					13
#define SM201_DISPENSE_X_MOVE_TO_CLEAN_POSITION					14
#define SM201_DISPENSE_X_MOVE_TO_INDEX_POSITION_1				15
#define SM201_DISPENSE_X_MOVE_TO_INDEX_POSITION_2				16
#define SM201_DISPENSE_X_MOVE_TO_INDEX_POSITION_3				17
#define SM201_DISPENSE_X_MOVE_TO_INDEX_POSITION_4				18
#define SM201_DISPENSE_X_MOVE_TO_INDEX_POSITION_5				19
#define SM201_DISPENSE_X_MOVE_TO_VISION_ON_GLASS_POSITION		20
#define SM201_DISPENSE_X_MOVE_TO_LASER_ON_GLASS_POSITION		21
#define SM201_DISPENSE_X_OFFSET_MOVE_FROM_VISION_POSITION		22
#define SM201_DISPENSE_X_INITIALIZATION							23
#define SM201_DISPENSE_X_MOVE_VISION_CENTER						24
#define SM201_MOVE_TO_TARGET_DISTANCE							25

#ifdef DECLARE
int (*SM201_fun[])()={	NULL,													// fun no = 0;
						SM201_encoder_reset,									// 1;
						SM201_move_to_home_position,							// 2;
						SM201_move_to_load_position1,							// 3;


						SM201_initialization,									// 23;
						SM201_move_to_load_position2,							// 24;
						SM201_move_to_target_distance,
						NULL };										
#else 
extern int (*SM201_fun[])();
#endif

//*** AC servo motor SM202 motion number definition ===================================
#define SM202_HOMING											1
#define SM202_MOVE_TO_HOME_POSITION								2
#define SM202_DISPENSE_Y_MOVE_TO_VISION_POSITION				3
#define SM202_DISPENSE_Y_MOVE_TO_LASER_POSITION					4
#define SM202_DISPENSE_Y_MOVE_TO_NEEDLE_POSITION				5
#define SM202_DISPENSE_Y_MOVE_TO_CURE_CENTER_POSITION			6
#define SM202_DISPENSE_Y_MOVE_TO_CURE_INDEX_POSITION_1			7
#define SM202_DISPENSE_Y_MOVE_TO_CURE_INDEX_POSITION_2			8
#define SM202_DISPENSE_Y_MOVE_TO_CURE_INDEX_POSITION_3			9
#define SM202_DISPENSE_Y_MOVE_TO_CURE_INDEX_POSITION_4			10
#define SM202_DISPENSE_Y_MOVE_TO_CURE_INDEX_POSITION_5			11
#define SM202_DISPENSE_Y_MOVE_TO_TOUCH_POSITION					12
#define SM202_DISPENSE_Y_MOVE_TO_GLASS_POSITION					13
#define SM202_DISPENSE_Y_MOVE_TO_CLEAN_POSITION					14
#define SM202_DISPENSE_Y_MOVE_TO_INDEX_POSITION_1				15
#define SM202_DISPENSE_Y_MOVE_TO_INDEX_POSITION_2				16
#define SM202_DISPENSE_Y_MOVE_TO_INDEX_POSITION_3				17
#define SM202_DISPENSE_Y_MOVE_TO_INDEX_POSITION_4				18
#define SM202_DISPENSE_Y_MOVE_TO_INDEX_POSITION_5				19
#define SM202_DISPENSE_Y_MOVE_TO_VISION_ON_GLASS_POSITION		20
#define SM202_DISPENSE_Y_MOVE_TO_LASER_ON_GLASS_POSITION		21
#define SM202_DISPENSE_Y_OFFSET_MOVE_FROM_VISION_POSITION		22
#define SM202_DISPENSE_Y_INITIALIZATION							23
#define SM202_DISPENSE_Y_MOVE_VISION_CENTER						24
#define SM202_MOVE_TO_TARGET_DISTANCE							25

#ifdef DECLARE
int (*SM202_fun[])()={	NULL,													// fun no = 0;
						SM202_encoder_reset,									// 1;
						SM202_move_to_home_position,							// 2;
						SM202_move_to_load_position1,							// 3;


						SM202_initialization,									// 23;
						SM202_move_to_load_position2,							// 24;
						NULL };										
#else 
extern int (*SM202_fun[])();
#endif

//*** AC servo motor SM203 motion number definition ===================================
#define SM203_HOMING											1
#define SM203_MOVE_TO_HOME_POSITION								2
#define SM203_DISPENSE_Z_MOVE_TO_VISION_POSITION				3
#define SM203_DISPENSE_Z_MOVE_TO_LASER_POSITION					4
#define SM203_DISPENSE_Z_MOVE_TO_NEEDLE_POSITION				5
#define SM203_DISPENSE_Z_MOVE_TO_CURE_POSITION					6
#define SM203_DISPENSE_Z_MOVE_TO_TOUCH_POSITION					7
#define SM203_DISPENSE_Z_MOVE_TO_GLASS_POSITION					8
#define SM203_DISPENSE_Z_MOVE_TO_CLEAN_POSITION					9
#define SM203_DISPENSE_Z_MOVE_TO_INDEX_POSITION_1				10
#define SM203_DISPENSE_Z_MOVE_TO_INDEX_POSITION_2				11
#define SM203_DISPENSE_Z_MOVE_TO_INDEX_POSITION_3				12
#define SM203_DISPENSE_Z_MOVE_TO_INDEX_POSITION_4				13
#define SM203_DISPENSE_Z_MOVE_TO_INDEX_POSITION_5				14
#define SM203_DISPENSE_Z_DISPENSING_MOVE_UP						15
#define SM203_DISPENSE_Z_MOVE_TO_VISION_ON_GLASS_POSITION		16
#define SM203_DISPENSE_Z_MOVE_TO_LASER_ON_GLASS_POSITION		17
#define SM203_DISPENSE_Z_INITIALIZATION							18
#define SM203_MOVE_TO_TARGET_DISTANCE							25

#ifdef DECLARE
int (*SM203_fun[])()={	NULL,													// fun no = 0;
						SM203_encoder_reset,									// 1;
						SM203_move_to_home_position,							// 2;
						SM203_move_to_load_position1,							// 3;


						SM203_initialization,									// 23;
						SM203_move_to_load_position2,							// 24;
						NULL };										
#else 
extern int (*SM203_fun[])();
#endif

//*** AC servo motor SM204 motion number definition ===================================

#define SM204_HOMING											1
#define SM204_MOVE_TO_HOME_POSITION								2
#define SM204_DISPENSE_R_MOVE_TO_VISION_POSITION				3
#define SM204_DISPENSE_R_MOVE_TO_LASER_POSITION					4
#define SM204_DISPENSE_R_MOVE_TO_NEEDLE_POSITION				5
#define SM204_DISPENSE_R_MOVE_TO_CURE_POSITION					6
#define SM204_DISPENSE_R_MOVE_TO_TOUCH_POSITION					7
#define SM204_DISPENSE_R_MOVE_TO_GLASS_POSITION					8
#define SM204_DISPENSE_R_MOVE_TO_CLEAN_POSITION					9
#define SM204_DISPENSE_R_MOVE_TO_INDEX_POSITION_1				10
#define SM204_DISPENSE_R_MOVE_TO_INDEX_POSITION_2				11
#define SM204_DISPENSE_R_MOVE_TO_INDEX_POSITION_3				12
#define SM204_DISPENSE_R_MOVE_TO_INDEX_POSITION_4				13
#define SM204_DISPENSE_R_MOVE_TO_INDEX_POSITION_5				14
#define SM204_DISPENSE_R_DISPENSING_MOVE_UP						15
#define SM204_DISPENSE_R_MOVE_TO_VISION_ON_GLASS_POSITION		16
#define SM204_DISPENSE_R_MOVE_TO_LASER_ON_GLASS_POSITION		17
#define SM204_DISPENSE_R_INITIALIZATION							18
#define SM204_MOVE_TO_TARGET_DISTANCE							25

#ifdef DECLARE
int (*SM204_fun[])()={	NULL,													// fun no = 0;
	SM204_encoder_reset,									// 1;
	SM204_move_to_home_position,							// 2;
	SM204_move_to_load_position1,							// 3;


	SM204_initialization,									// 23;
	SM204_move_to_load_position2,							// 24;
	NULL };										
#else 
extern int (*SM204_fun[])();
#endif


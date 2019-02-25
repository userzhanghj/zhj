#include "interface.h"
#include "inf_motion_aa.h"

//===========================================================================================
// TriggerSM101 Thread
//===========================================================================================
DWORD TriggerSM101(LPVOID lpvThreadParm)
{
	SM101_TRIGGER = 0x00;
	log("sm101 ready.");
	while(1){
		while(SM101_TRIGGER==0x00) Sleep(20);
		log("sm101_trigger=%d", SM101_TRIGGER);
		SM101_MOTION_ERR = (SM101_fun[SM101_TRIGGER])(); 
		SM101_TRIGGER = 0x00;
		Sleep(2);
	}
	return(0);
}

//===========================================================================================
// TriggerSM102 Thread
//===========================================================================================
DWORD TriggerSM102(LPVOID lpvThreadParm)
{
	SM102_TRIGGER = 0x00;
	log("sm102 ready.");
	while(1){
		while(SM102_TRIGGER==0x00) Sleep(20);
		log("sm102_trigger=%d", SM102_TRIGGER);
		SM102_MOTION_ERR = (SM102_fun[SM102_TRIGGER])(); 
		SM102_TRIGGER = 0x00;
		Sleep(2);
	}
	return(0);
}

//===========================================================================================
// TriggerSM103 Thread
//===========================================================================================
DWORD TriggerSM103(LPVOID lpvThreadParm)
{
	SM103_TRIGGER = 0x00;
	while(1){
		while(SM103_TRIGGER==0x00) Sleep(20);
		SM103_MOTION_ERR = (SM103_fun[SM103_TRIGGER])(); 
		SM103_TRIGGER = 0x00;
		Sleep(2);
	}
	return(0);
}

//===========================================================================================
// TriggerSM104 Thread
//===========================================================================================
DWORD TriggerSM104(LPVOID lpvThreadParm)
{
	SM104_TRIGGER = 0x00;
	log("sm104 ready");
	while(1){
		while(SM104_TRIGGER==0x00) Sleep(20);
		log("sm104_trigger=%d", SM104_TRIGGER);
		SM104_MOTION_ERR = (SM104_fun[SM104_TRIGGER])(); 
		SM104_TRIGGER = 0x00;
		Sleep(2);
	}
	return(0);
}

//===========================================================================================
// TriggerSM201 Thread
//===========================================================================================
DWORD TriggerSM201(LPVOID lpvThreadParm)
{
	SM201_TRIGGER = 0x00;
	log("sm201 ready.");
	while(1){
		while(SM201_TRIGGER==0x00) Sleep(20);
		log("sm201_trigger=%d", SM201_TRIGGER);
		SM201_MOTION_ERR = (SM201_fun[SM201_TRIGGER])(); 
		SM201_TRIGGER = 0x00;
		Sleep(2);
	}
	return(0);
}

//===========================================================================================
// TriggerSM202 Thread
//===========================================================================================
DWORD TriggerSM202(LPVOID lpvThreadParm)
{
	SM202_TRIGGER = 0x00;
	while(1){
		while(SM202_TRIGGER==0x00) Sleep(20);
		SM202_MOTION_ERR = (SM202_fun[SM202_TRIGGER])(); 
		SM202_TRIGGER = 0x00;
		Sleep(2);
	}
	return(0);
}

//===========================================================================================
// TriggerSM203 Thread
//===========================================================================================
DWORD TriggerSM203(LPVOID lpvThreadParm)
{
	SM203_TRIGGER = 0x00;
	log("sm203 ready.");
	while(1){
		while(SM203_TRIGGER==0x00) Sleep(20);
		log("sm203_trigger=%d", SM203_TRIGGER);
		SM203_MOTION_ERR = (SM203_fun[SM203_TRIGGER])(); 
		SM203_TRIGGER = 0x00;
		Sleep(2);
	}
	return(0);
}

//===========================================================================================
// TriggerSM204 Thread
//===========================================================================================
DWORD TriggerSM204(LPVOID lpvThreadParm)
{
	SM204_TRIGGER = 0x00;
	while(1){
		while(SM204_TRIGGER==0x00) Sleep(20);
		SM204_MOTION_ERR = (SM204_fun[SM204_TRIGGER])(); 
		SM204_TRIGGER = 0x00;
		Sleep(2);
	}
	return(0);
}




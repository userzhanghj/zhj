#include "inf_motion_aa.h"

DWORD Readcntr(LPVOID lpvThreadParm)
{
	long position;

	while (1){
		//=== update card-1 Encoder value ===================================================
		WaitForSingleObject(Motors1_hMutex,INFINITE);
			ps400_get_cmdcounter(SM101_card, SM101_axis, &position);
			Card1_X_Position = position;
			
			ps400_get_cmdcounter(SM102_card, SM102_axis, &position);
			Card1_Y_Position = position;
			
//			ps400_get_cmdcounter(SM102_card, SM102_axis, &position);
// 			Card1_Z_Position = position;
			
			ps400_get_cmdcounter(SM103_card, SM103_axis, &position);
			Card1_Z_Position = (position);
		ReleaseMutex(Motors1_hMutex);
		Sleep(20);

		//=== update card-2 Encoder value ===================================================
		WaitForSingleObject(Motors2_hMutex,INFINITE);
			ps400_get_cmdcounter(SM201_card, SM201_axis, &position);
			Card2_X_Position = position;
			
			ps400_get_cmdcounter(SM202_card, SM202_axis, &position);
			Card2_Y_Position = position;
// 			
			ps400_get_cmdcounter(SM203_card, SM203_axis, &position);
 			Card2_Z_Position = position;
			
//			ps400_get_cmdcounter(SM204_card, SM204_axis, &position);
//			Card2_U_Position = position;
		ReleaseMutex(Motors2_hMutex);
		Sleep(20);




	}



	return FALSE;
}

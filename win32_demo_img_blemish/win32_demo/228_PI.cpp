#include <Windows.h>
#include <stdio.h>
#include <direct.h>
#include "s_win.h"
#include "s_img.h"
#include "resource.h"
#include "tuxiang.h"

#include <conio.h>
#include <time.h>
#include "Pi_gcs2_dll.h"
#include "PI_move.h"

#pragma comment(lib,"Pi_gcs2_dll.lib")
#pragma comment(lib, "s_win.lib")
#pragma comment(lib, "s_img.lib")
#define THREAD_NUM	16
using namespace ai_win;

extern HMENU hmenu;
//********************************************************************
int ID;
//srand ( ( int ) time ( NULL ) );
char axis[] = "X Y Z U V W";		// modify here to change moved axes
char axis2[] = "X Y Z";

BOOL bMovePossible;
double dMaxTravelPositive[6];
BOOL bIsMoving = TRUE;
double dPos[6] = { 0, 0, 0, 0, 0, 0 };
double dPos2[6] = { 0, 0, 0, 0, 0, 0 };
double dXd[6] = { 0, 0, 0, 0, 0, 0 };
double dStart[3] = { 0, 0, 0};
DWORD my_id[THREAD_NUM] = {0};
HANDLE my_handle[THREAD_NUM] = {NULL};
BOOL bIsMoving2[6] = { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE };
double sudu;
double Pos[6] = { 0, 0, 0, 0, 0, 0 };
double Pos2[6] = { 0, 0, 0, 0, 0, 0 };
//*********************************************************************
DlgPI dlg2_28;

bool MoveToPosition ( int ID, char* axis, double *dVal )
{
	
	if ( !PI_MOV ( ID, axis, dVal ) )
		return FALSE;
	return true;
}

DWORD CALLBACK get_point(LPVOID temp){
	HWND hdlg = (HWND) temp;
	double sd;

	while (1){
		PI_qPOS ( ID, "X Y Z U V W", Pos );
		PI_qPOS ( ID, "X Y Z U V W", Pos2 );
		set_dlg_item_float(hdlg,IDC_SX,Pos2[0],4);
		set_dlg_item_float(hdlg,IDC_SY,Pos2[1],4);
		set_dlg_item_float(hdlg,IDC_SZ,Pos2[2],4);
		set_dlg_item_float(hdlg,IDC_SU,Pos2[3],4);
		set_dlg_item_float(hdlg,IDC_SV,Pos2[4],4);
		set_dlg_item_float(hdlg,IDC_SW,Pos2[5],4);
		PI_qVLS(ID,&sd);
		set_dlg_item_float(hdlg,IDC_SSD,sd,FALSE);
		Sleep(100);
	}
	return FALSE;
}

DWORD CALLBACK set_gray(LPVOID temp){
	HWND hdlg = (HWND) temp;
	double s[3];
	while (1){
		if((Pos[0]!=0)||(Pos[1]!=0)||(Pos[2]!=0)||(Pos[3]!=0)||(Pos[4]!=0)||(Pos[5]!=0))
		{
			ITEM_GRAYED(hdlg,IDC_SETX);
			ITEM_GRAYED(hdlg,IDC_SETY);
			ITEM_GRAYED(hdlg,IDC_SETZ);
		}
		else
		{
			ITEM_ENABLED(hdlg,IDC_SETX);
			ITEM_ENABLED(hdlg,IDC_SETY);
			ITEM_ENABLED(hdlg,IDC_SETZ);
		}
		PI_qSPI(ID,axis2,s);
		set_dlg_item_float(hdlg,IDC_GETX,s[0],4);
		set_dlg_item_float(hdlg,IDC_GETY,s[1],4);
		set_dlg_item_float(hdlg,IDC_GETZ,s[2],4);
		Sleep(100);
	}
	return FALSE;
}

DWORD CALLBACK get_mess(LPVOID temp){
	HWND hdlg = (HWND) temp;

	while (1){
		if((Pos[0]==0)&&(Pos[1]==0)&&(Pos[2]==0)&&(Pos[3]==0)&&(Pos[4]==0)&&(Pos[5]==0))
		{
			dStart[0]=get_dlg_item_float(hdlg,IDC_SETX);
			dStart[1]=get_dlg_item_float(hdlg,IDC_SETY);
			dStart[2]=get_dlg_item_float(hdlg,IDC_SETZ);
			PI_SPI(ID,axis2,dStart);
		}
		/*dStart[0]=get_dlg_item_float(hdlg,IDC_SETX);
		dStart[1]=get_dlg_item_float(hdlg,IDC_SETY);
		dStart[2]=get_dlg_item_float(hdlg,IDC_SETZ);
		if(!PI_SPI(ID,axis2,dStart)){

		}*/
		Sleep(100);
	}
	return FALSE;
}

BOOL CALLBACK menuDlg_2_28(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_DLGMSG(hdlg, WM_INITDIALOG,	    dlg2_28.init);
		HANDLE_DLGMSG(hdlg, WM_COMMAND,		    dlg2_28.command);
		HANDLE_DLGMSG(hdlg, WM_SIZE,		    dlg2_28.size);
		HANDLE_DLGMSG(hdlg, WM_CLOSE,		    dlg2_28.close);
	}												 
	return FALSE;
}

BOOL DlgPI::init(HWND hdlg, HWND hfocus, LPARAM linit)
{
	RECT dlg_rect;
	//	ai_win::set_icon(hdlg, IDI_LOG, IDI_LOG);

	GetClientRect(hdlg, &dlg_rect);
	MoveWindow(hdlg, 50, 150, dlg_rect.right, dlg_rect.bottom, TRUE);
	ID = PI_InterfaceSetupDlg ( "" );
	if(0>ID){
		ITEM_GRAYED(hdlg,IDC_BJZ);
		ITEM_GRAYED(hdlg,IDC_MOVE);
		ITEM_GRAYED(hdlg,IDC_MOVE2);
		ITEM_GRAYED(hdlg,IDC_BXD);
		ITEM_GRAYED(hdlg,IDC_BUT);
		ITEM_GRAYED(hdlg,IDC_BZX);
	}
	else{
		ITEM_ENABLED(hdlg,IDC_BJZ);
		ITEM_ENABLED(hdlg,IDC_MOVE);
		ITEM_ENABLED(hdlg,IDC_MOVE2);
		ITEM_ENABLED(hdlg,IDC_BXD);
		ITEM_ENABLED(hdlg,IDC_BUT);
		ITEM_ENABLED(hdlg,IDC_BZX);
	}
	set_dlg_item_float(hdlg,IDC_EX,0.000,4);
	set_dlg_item_float(hdlg,IDC_EY,0.000,4);
	set_dlg_item_float(hdlg,IDC_EZ,0.000,4);
	set_dlg_item_float(hdlg,IDC_EU,0.000,4);
	set_dlg_item_float(hdlg,IDC_EV,0.000,4);
	set_dlg_item_float(hdlg,IDC_EW,0.000,4);

	if(ID>=0){
		my_handle[0]  = CreateThread(NULL, 0, get_point		,	(LPVOID)hdlg, 0, &my_id[0]);//创建线程
		my_handle[1]  = CreateThread(NULL, 0, set_gray		,	(LPVOID)hdlg, 0, &my_id[1]);
		my_handle[2]  = CreateThread(NULL, 0, get_mess		,	(LPVOID)hdlg, 0, &my_id[2]);
	}
	if(ID>=0){
		PI_qVLS(ID,&sudu);
		set_dlg_item_float(hdlg,IDC_SSD,sudu,4);
		set_dlg_item_float(hdlg,IDC_ESD,sudu,4);
		PI_SPI(ID,axis2,dStart);
	}
	dPos[0]=get_dlg_item_float(hdlg,IDC_EX);
	dPos[1]=get_dlg_item_float(hdlg,IDC_EY);
	dPos[2]=get_dlg_item_float(hdlg,IDC_EZ);
	dPos[3]=get_dlg_item_float(hdlg,IDC_EU);
	dPos[4]=get_dlg_item_float(hdlg,IDC_EV);
	dPos[5]=get_dlg_item_float(hdlg,IDC_EW);

	set_dlg_item_float(hdlg,IDC_GETX,dStart[0],4);
	set_dlg_item_float(hdlg,IDC_GETY,dStart[1],4);
	set_dlg_item_float(hdlg,IDC_GETZ,dStart[2],4);
	set_dlg_item_float(hdlg,IDC_SETX,dStart[0],4);
	set_dlg_item_float(hdlg,IDC_SETY,dStart[1],4);
	set_dlg_item_float(hdlg,IDC_SETZ,dStart[2],4);

	set_dlg_item_float(hdlg,IDC_XDX,dXd[0],4);
	set_dlg_item_float(hdlg,IDC_XDY,dXd[1],4);
	set_dlg_item_float(hdlg,IDC_XDZ,dXd[2],4);
	set_dlg_item_float(hdlg,IDC_XDU,dXd[3],4);
	set_dlg_item_float(hdlg,IDC_XDV,dXd[4],4);
	set_dlg_item_float(hdlg,IDC_XDW,dXd[5],4);
	return TRUE;
}

void DlgPI::command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify)
{
	switch (id){
	case IDC_BXD:
		dXd[0]=get_dlg_item_float(hdlg,IDC_XDX);
		dXd[1]=get_dlg_item_float(hdlg,IDC_XDY);
		dXd[2]=get_dlg_item_float(hdlg,IDC_XDZ);
		dXd[3]=get_dlg_item_float(hdlg,IDC_XDU);
		dXd[4]=get_dlg_item_float(hdlg,IDC_XDV);
		dXd[5]=get_dlg_item_float(hdlg,IDC_XDW);
		for(int i=0;i<6;i++){
			Pos[i]+=dXd[i];
		}
		PI_qTMX ( ID, "X Y Z U V W", dMaxTravelPositive );
		PI_qVMO ( ID, axis, Pos, &bMovePossible );
		if(FALSE == bMovePossible)
		{
			MessageBox(hdlg,"无法移动到目标位置!",NULL,NULL);
			break;
		}
		if ( !PI_IsMoving ( ID, "X Y Z U V W", bIsMoving2 ) )
			break;
		if((TRUE== bIsMoving2[0])||(TRUE == bIsMoving2[1])||(TRUE == bIsMoving2[2])||(TRUE == bIsMoving2[3])||(TRUE == bIsMoving2[4])||(TRUE == bIsMoving2[5]))
		{
			MessageBox(hdlg,"目标在移动中!",NULL,NULL);
			break;
		}
		sudu=get_dlg_item_float(hdlg,IDC_ESD);
		if(!PI_VLS(ID,sudu))
		{
			MessageBox(hdlg,"无法设定目标速度!",NULL,NULL);
			break;
		}
		if ( !MoveToPosition ( ID, axis, Pos ) )
			throw ( -4 );
		break;
	case IDC_BZX:
		dStart[0]=get_dlg_item_float(hdlg,IDC_SETX);
		dStart[1]=get_dlg_item_float(hdlg,IDC_SETY);
		dStart[2]=get_dlg_item_float(hdlg,IDC_SETZ);
		if(!PI_SPI(ID,axis2,dStart)){
			MessageBox(hdlg,"无法设置轴心!",NULL,NULL);
			break;;
		}
		break;
	case IDC_BUT:
		if(!PI_STP ( ID ))
		{
			MessageBox(hdlg,"error!",NULL,NULL);
			break;
		}
		set_dlg_item_float(hdlg,IDC_EX,Pos[0],4);
		set_dlg_item_float(hdlg,IDC_EY,Pos[1],4);
		set_dlg_item_float(hdlg,IDC_EZ,Pos[2],4);
		set_dlg_item_float(hdlg,IDC_EU,Pos[3],4);
		set_dlg_item_float(hdlg,IDC_EV,Pos[4],4);
		set_dlg_item_float(hdlg,IDC_EW,Pos[5],4);
		break;
	case IDC_BJZ:
		if ( !PI_FRF ( ID, axis ) )//*****************************************参考初始化
			throw ( -4 );
		break;
	case IDC_BRELINK:
		PI_CloseConnection ( ID );
		ID = PI_InterfaceSetupDlg ( "" );
		if(0>ID){
			ITEM_GRAYED(hdlg,IDC_BJZ);
			ITEM_GRAYED(hdlg,IDC_MOVE);
			ITEM_GRAYED(hdlg,IDC_MOVE2);
			ITEM_GRAYED(hdlg,IDC_BXD);
			ITEM_GRAYED(hdlg,IDC_BUT);
			ITEM_GRAYED(hdlg,IDC_BZX);
		}
		else{
			ITEM_ENABLED(hdlg,IDC_BJZ);
			ITEM_ENABLED(hdlg,IDC_MOVE);
			ITEM_ENABLED(hdlg,IDC_MOVE2);
			ITEM_ENABLED(hdlg,IDC_BXD);
			ITEM_ENABLED(hdlg,IDC_BUT);
			ITEM_ENABLED(hdlg,IDC_BZX);
		}
		if(ID>=0){
			my_handle[0]  = CreateThread(NULL, 0, get_point		,	(LPVOID)hdlg, 0, &my_id[0]);//创建线程
			my_handle[1]  = CreateThread(NULL, 0, set_gray		,	(LPVOID)hdlg, 0, &my_id[1]);
			my_handle[2]  = CreateThread(NULL, 0, get_mess		,	(LPVOID)hdlg, 0, &my_id[2]);
		}
		if(ID>=0){
			PI_qVLS(ID,&sudu);
			set_dlg_item_float(hdlg,IDC_SSD,sudu,4);
			set_dlg_item_float(hdlg,IDC_ESD,sudu,4);
			PI_SPI(ID,axis2,dStart);
		}
		break;
	case IDC_MOVE:
		dPos[0]=get_dlg_item_float(hdlg,IDC_EX);
		dPos[1]=get_dlg_item_float(hdlg,IDC_EY);
		dPos[2]=get_dlg_item_float(hdlg,IDC_EZ);
		dPos[3]=get_dlg_item_float(hdlg,IDC_EU);
		dPos[4]=get_dlg_item_float(hdlg,IDC_EV);
		dPos[5]=get_dlg_item_float(hdlg,IDC_EW);
		PI_qTMX ( ID, "X Y Z U V W", dMaxTravelPositive );
		PI_qVMO ( ID, axis, dPos, &bMovePossible );
		if(FALSE == bMovePossible)
		{
			MessageBox(hdlg,"无法移动到目标位置!",NULL,NULL);
			break;
		}
		if ( !PI_IsMoving ( ID, "X Y Z U V W", bIsMoving2 ) )
			break;
		if((TRUE== bIsMoving2[0])||(TRUE == bIsMoving2[1])||(TRUE == bIsMoving2[2])||(TRUE == bIsMoving2[3])||(TRUE == bIsMoving2[4])||(TRUE == bIsMoving2[5]))
		{
			MessageBox(hdlg,"目标在移动中!",NULL,NULL);
			break;
		}
		sudu=get_dlg_item_float(hdlg,IDC_ESD);
		if(!PI_VLS(ID,sudu))
		{
			MessageBox(hdlg,"无法设定目标速度!",NULL,NULL);
			break;
		}
		if ( !MoveToPosition ( ID, axis, dPos ) )
			throw ( -4 );
		break;
	case IDC_MOVE2:
		dPos[0]=0;
		dPos[1]=0;
		dPos[2]=0;
		dPos[3]=0;
		dPos[4]=0;
		dPos[5]=0;
		PI_qTMX ( ID, "X Y Z U V W", dMaxTravelPositive );
		if ( !PI_IsMoving ( ID, "X Y Z U V W", bIsMoving2 ) )
			break;
		if((TRUE== bIsMoving2[0])||(TRUE == bIsMoving2[1])||(TRUE == bIsMoving2[2])||(TRUE == bIsMoving2[3])||(TRUE == bIsMoving2[4])||(TRUE == bIsMoving2[5]))
		{
			MessageBox(hdlg,"目标在移动中!",NULL,NULL);
			break;
		}
		sudu=get_dlg_item_float(hdlg,IDC_ESD);
		if(!PI_VLS(ID,sudu))
		{
			MessageBox(hdlg,"无法设定目标速度!",NULL,NULL);
			break;
		}
		if ( !/*PI_GOH( ID, axis)*/ MoveToPosition ( ID, axis, dPos ))
			throw ( -4 );
		set_dlg_item_float(hdlg,IDC_EX,0.000,4);
		set_dlg_item_float(hdlg,IDC_EY,0.000,4);
		set_dlg_item_float(hdlg,IDC_EZ,0.000,4);
		set_dlg_item_float(hdlg,IDC_EU,0.000,4);
		set_dlg_item_float(hdlg,IDC_EV,0.000,4);
		set_dlg_item_float(hdlg,IDC_EW,0.000,4);
		set_dlg_item_float(hdlg,IDC_XDX,0.000,4);
		set_dlg_item_float(hdlg,IDC_XDY,0.000,4);
		set_dlg_item_float(hdlg,IDC_XDZ,0.000,4);
		set_dlg_item_float(hdlg,IDC_XDU,0.000,4);
		set_dlg_item_float(hdlg,IDC_XDV,0.000,4);
		set_dlg_item_float(hdlg,IDC_XDW,0.000,4);
		break;
	}
	switch (code_notify)
	{

	}
}

void DlgPI::size(HWND hdlg, unsigned int state, int width, int height)
{
	//MoveWindow(GetDlgItem(hdlg, IDC_LOG), 0, 0, width, height, TRUE);
}
	
void DlgPI::close(HWND hdlg)
{
	//ExitThread()
	//ExitThread(my_id[0]);
	//ExitThread(my_id[1]);
	//ExitThread(my_id[2]);
	dStart[0]=0;
	dStart[1]=0;
	dStart[2]=0;
	PI_STP ( ID );
	//PI_StopAll (ID);
	PI_VLS(ID,10);
	//PI_VLS(ID,10);
	//dinfo.show_log = FALSE;
	//CheckMenuItem(s_aa::hmenu, ID_FILE_LOG, MF_UNCHECKED);
	
	menu_228=0;
	CheckMenuItem(hmenu, ID_2_28, FALSE?MF_CHECKED:MF_UNCHECKED);
	EndDialog(hdlg, FALSE);

	PI_CloseConnection ( ID );
}


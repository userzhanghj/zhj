#include "inf_motion_pi.h"
#include "interface.h"
#include "inf_motion_aa.h"
int ID;
//srand ( ( int ) time ( NULL ) );
char axis[] = "X Y Z U V W";		// modify here to change moved axes
char axis2[] = "X Y Z";

BOOL bMovePossible;
double dMaxTravelPositive[6];
BOOL bIsMoving = TRUE;
double dPos[6] = { 0, 0, 0, 0, 0, 0 };
double dPos2[6] = { 0, 0, 0, 0, 0, 0 };
double dPos_p[5][6]={0};

double dXd[6] = { 0, 0, 0, 0, 0, 0 };
double dStart[3] = { 298.5, 0, 32};
DWORD my_id[THREAD_NUM] = {0};
HANDLE my_handle[THREAD_NUM] = {NULL};
BOOL bIsMoving2[6] = { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE };
double sudu;
double Pos[6] = { 0, 0, 0, 0, 0, 0 };
double Pos2[6] = { 0, 0, 0, 0, 0, 0 };
int point_num[5] = {0,1,2,3,4};
int is_change=0;
int move_p1_to_p5=0;
int point_id=0;
double pi_step=0;
int moving_down = 0;
int trans_button = 0;


bool MoveToPosition ( int ID, char* axis, double *dVal )
{

	if ( !PI_MOV ( ID, axis, dVal ) )
		return FALSE;
	return true;
}

void save_point(HWND hdlg,int i){
	dPos_p[point_num[i]][0]=Pos2[0];
	dPos_p[point_num[i]][1]=Pos2[1];
	dPos_p[point_num[i]][2]=Pos2[2];
	dPos_p[point_num[i]][3]=Pos2[3];
	dPos_p[point_num[i]][4]=Pos2[4];
	dPos_p[point_num[i]][5]=Pos2[5];
	ai_win::set_dlg_item_float(hdlg, IDC_EX2, dPos_p[0][0],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EY2, dPos_p[0][1],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EZ2, dPos_p[0][2],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EU2, dPos_p[0][3],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EV2, dPos_p[0][4],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EW2, dPos_p[0][5],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EX3, dPos_p[1][0],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EY3, dPos_p[1][1],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EZ3, dPos_p[1][2],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EU3, dPos_p[1][3],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EV3, dPos_p[1][4],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EW3, dPos_p[1][5],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EX4, dPos_p[2][0],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EY4, dPos_p[2][1],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EZ4, dPos_p[2][2],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EU4, dPos_p[2][3],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EV4, dPos_p[2][4],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EW4, dPos_p[2][5],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EX5, dPos_p[3][0],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EY5, dPos_p[3][1],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EZ5, dPos_p[3][2],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EU5, dPos_p[3][3],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EV5, dPos_p[3][4],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EW5, dPos_p[3][5],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EX6, dPos_p[4][0],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EY6, dPos_p[4][1],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EZ6, dPos_p[4][2],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EU6, dPos_p[4][3],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EV6, dPos_p[4][4],4);
	ai_win::set_dlg_item_float(hdlg, IDC_EW6, dPos_p[4][5],4);
}

DWORD CALLBACK get_point(LPVOID temp){
	HWND hdlg = (HWND) temp;
	double sd;

	while (1){
		PI_qPOS ( ID, "X Y Z U V W", Pos );
		PI_qPOS ( ID, "X Y Z U V W", Pos2 );
		ai_win::set_dlg_item_float(hdlg,IDC_SX,Pos2[0],4);
		ai_win::set_dlg_item_float(hdlg,IDC_SY,Pos2[1],4);
		ai_win::set_dlg_item_float(hdlg,IDC_SZ,Pos2[2],4);
		ai_win::set_dlg_item_float(hdlg,IDC_SU,Pos2[3],4);
		ai_win::set_dlg_item_float(hdlg,IDC_SV,Pos2[4],4);
		ai_win::set_dlg_item_float(hdlg,IDC_SW,Pos2[5],4);
		PI_qVLS(ID,&sd);
		ai_win::set_dlg_item_float(hdlg,IDC_SSD,sd,FALSE);
		Sleep(20);
	}
	return FALSE;
}

DWORD CALLBACK set_gray(LPVOID temp){
	HWND hdlg = (HWND) temp;
	double s[3];
	while (1){
		//if((Pos[0]!=0)||(Pos[1]!=0)||(Pos[2]!=0)||(Pos[3]!=0)||(Pos[4]!=0)||(Pos[5]!=0))
		//{
		//	ITEM_GRAYED(hdlg,IDC_SETX);
		//	ITEM_GRAYED(hdlg,IDC_SETY);
		//	ITEM_GRAYED(hdlg,IDC_SETZ);
		//}
		//else
		//{
		//	ITEM_ENABLED(hdlg,IDC_SETX);
		//	ITEM_ENABLED(hdlg,IDC_SETY);
		//	ITEM_ENABLED(hdlg,IDC_SETZ);
		//}
		PI_qSPI(ID,axis2,s);
		ai_win::set_dlg_item_float(hdlg,IDC_GETX,s[0],4);
		ai_win::set_dlg_item_float(hdlg,IDC_GETY,s[1],4);
		ai_win::set_dlg_item_float(hdlg,IDC_GETZ,s[2],4);
		Sleep(100);
	}
	return FALSE;
}

DWORD CALLBACK get_mess(LPVOID temp){
	HWND hdlg = (HWND) temp;

	while (1){
		//if((Pos[0]==0)&&(Pos[1]==0)&&(Pos[2]==0)&&(Pos[3]==0)&&(Pos[4]==0)&&(Pos[5]==0))
		//{
		//	dStart[0]=ai_win::get_dlg_item_float(hdlg,IDC_SETX);
		//	dStart[1]=ai_win::get_dlg_item_float(hdlg,IDC_SETY);
		//	dStart[2]=ai_win::get_dlg_item_float(hdlg,IDC_SETZ);
		//	PI_SPI(ID,axis2,dStart);
		//}
		dPos_p[0][0]=ai_win::get_dlg_item_float(hdlg, IDC_EX2);
		dPos_p[0][1]=ai_win::get_dlg_item_float(hdlg, IDC_EY2);
		dPos_p[0][2]=ai_win::get_dlg_item_float(hdlg, IDC_EZ2);
		dPos_p[0][3]=ai_win::get_dlg_item_float(hdlg, IDC_EU2);
		dPos_p[0][4]=ai_win::get_dlg_item_float(hdlg, IDC_EV2);
		dPos_p[0][5]=ai_win::get_dlg_item_float(hdlg, IDC_EW2);
		dPos_p[1][0]=ai_win::get_dlg_item_float(hdlg, IDC_EX3);
		dPos_p[1][1]=ai_win::get_dlg_item_float(hdlg, IDC_EY3);
		dPos_p[1][2]=ai_win::get_dlg_item_float(hdlg, IDC_EZ3);
		dPos_p[1][3]=ai_win::get_dlg_item_float(hdlg, IDC_EU3);
		dPos_p[1][4]=ai_win::get_dlg_item_float(hdlg, IDC_EV3);
		dPos_p[1][5]=ai_win::get_dlg_item_float(hdlg, IDC_EW3);
		dPos_p[2][0]=ai_win::get_dlg_item_float(hdlg, IDC_EX4);
		dPos_p[2][1]=ai_win::get_dlg_item_float(hdlg, IDC_EY4);
		dPos_p[2][2]=ai_win::get_dlg_item_float(hdlg, IDC_EZ4);
		dPos_p[2][3]=ai_win::get_dlg_item_float(hdlg, IDC_EU4);
		dPos_p[2][4]=ai_win::get_dlg_item_float(hdlg, IDC_EV4);
		dPos_p[2][5]=ai_win::get_dlg_item_float(hdlg, IDC_EW4);
		dPos_p[3][0]=ai_win::get_dlg_item_float(hdlg, IDC_EX5);
		dPos_p[3][1]=ai_win::get_dlg_item_float(hdlg, IDC_EY5);
		dPos_p[3][2]=ai_win::get_dlg_item_float(hdlg, IDC_EZ5);
		dPos_p[3][3]=ai_win::get_dlg_item_float(hdlg, IDC_EU5);
		dPos_p[3][4]=ai_win::get_dlg_item_float(hdlg, IDC_EV5);
		dPos_p[3][5]=ai_win::get_dlg_item_float(hdlg, IDC_EW5);
		dPos_p[4][0]=ai_win::get_dlg_item_float(hdlg, IDC_EX6);
		dPos_p[4][1]=ai_win::get_dlg_item_float(hdlg, IDC_EY6);
		dPos_p[4][2]=ai_win::get_dlg_item_float(hdlg, IDC_EZ6);
		dPos_p[4][3]=ai_win::get_dlg_item_float(hdlg, IDC_EU6);
		dPos_p[4][4]=ai_win::get_dlg_item_float(hdlg, IDC_EV6);
		dPos_p[4][5]=ai_win::get_dlg_item_float(hdlg, IDC_EW6);
		//********************************************************
		pi_step=ai_win::get_dlg_item_float(hdlg, IDC_PI_STEP);

		/*dStart[0]=get_dlg_item_float(hdlg,IDC_SETX);
		dStart[1]=get_dlg_item_float(hdlg,IDC_SETY);
		dStart[2]=get_dlg_item_float(hdlg,IDC_SETZ);
		if(!PI_SPI(ID,axis2,dStart)){

		}*/
		//if(ID<0)
		//	ID = PI_InterfaceSetupDlg ( "" );
		//
		Sleep(10);
	}
	return FALSE;
}

void chang_p(int a,int n){
	int i,j;
	j=point_num[n];
	for(i=0;i<5;i++)
	{	
		if(point_num[i]==a){
			point_num[i]=j;
			if(i!=n)
				is_change=1;
		}
	}
	point_num[n]=a;
}

DWORD CALLBACK set_point(LPVOID temp){
		HWND hdlg =(HWND) temp;
		while(1){
			if(is_change){
				COMBO_SETCURSEL(hdlg, IDC_COMBO1, point_num[0]);
				COMBO_SETCURSEL(hdlg, IDC_COMBO2, point_num[1]);
				COMBO_SETCURSEL(hdlg, IDC_COMBO3, point_num[2]);
				COMBO_SETCURSEL(hdlg, IDC_COMBO4, point_num[3]);
				COMBO_SETCURSEL(hdlg, IDC_COMBO5, point_num[4]);
				ai_win::write_integer_key(pi_path, "pioint", "point_num1",		 point_num[0]);
				ai_win::write_integer_key(pi_path, "pioint", "point_num2",		 point_num[1]);
				ai_win::write_integer_key(pi_path, "pioint", "point_num3",		 point_num[2]);
				ai_win::write_integer_key(pi_path, "pioint", "point_num4",		 point_num[3]);
				ai_win::write_integer_key(pi_path, "pioint", "point_num5",		 point_num[4]);
				is_change=0;
				//log("88:%d,%d,%d,%d,%d,",point_num[0],point_num[1],point_num[2],point_num[3],point_num[4]);
			}
			Sleep(100);
		}
}

DWORD CALLBACK PI_status(LPVOID temp){
		HWND hdlg =(HWND) temp;
		while(1){
			PI_IsMoving ( ID, "X Y Z U V W", bIsMoving2 );
			if(((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])))	{		
				moving_down = 0;	
				//log("is moving");
			}
			else{
				moving_down = 1;
				//log("move down.");
			}
			Sleep(200);
		}
}

bool move_safe(){
	Pos2[2]=0;
	bool n;
	n=PI_move_to_point(Pos2);
	return n;
}

bool move_home(){
//	if(move_safe())
//		return FALSE;
	if ( !PI_FRF ( ID, axis ) )//*****************************************参考初始化
		return FALSE;
	if ( !PI_IsMoving ( ID, "X Y Z U V W", bIsMoving2 ) ){
		log("is moving check err.");
		return FALSE;
	}
	while(((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])))
	{
		//log("目标在移动中,请稍后重试");
		Sleep(50);
	}
	int timeout = 100;
	while(timeout-->0 && !PI_SPI(ID,axis2,dStart)){
		Sleep(10);
	}
	if(timeout<=0)
	{
		log("move home error");
		return FALSE;
	}
	else
	{
		ai_win::set_dlg_item_float(HWND_PI_MOTION,IDC_GETX,dStart[0],4);
		ai_win::set_dlg_item_float(HWND_PI_MOTION,IDC_GETY,dStart[1],4);
		ai_win::set_dlg_item_float(HWND_PI_MOTION,IDC_GETZ,dStart[2],4);
		return TRUE;
	}
}
#include "interface.h"
#include "inf_motion_pi.h"
#include "inf_motion_aa.h"

char pi_path[MAX_PATH]	   = {0};
class DlgPI:public ai_win::Dlg
{
public:
	virtual BOOL init(HWND hdlg, HWND hfocus, LPARAM linit);
	virtual void command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify);
	virtual void close(HWND hdlg);
private:
	HBRUSH mybrush;
};

DlgPI motion_pi;

BOOL CALLBACK motion_pi_dlg_process(HWND hdlg, unsigned int message, 
	WPARAM wParam, LPARAM lParam)
{
	switch (message){
		HANDLE_DLGMSG(hdlg, WM_INITDIALOG,  motion_pi.init);
		HANDLE_DLGMSG(hdlg, WM_COMMAND,		motion_pi.command);
		HANDLE_DLGMSG(hdlg, WM_CLOSE,		motion_pi.close);
		//		HANDLE_DLGMSG(hdlg, WM_CTLCOLORDLG, motion_dlg.ctlcolor_dlg);
	}
	return FALSE;
}

BOOL DlgPI::init(HWND hdlg, HWND hfocus, LPARAM linit)
{
	ID=-1;
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
	ai_win::set_dlg_item_float(hdlg,IDC_EX,0.000,4);
	ai_win::set_dlg_item_float(hdlg,IDC_EY,0.000,4);
	ai_win::set_dlg_item_float(hdlg,IDC_EZ,0.000,4);
	ai_win::set_dlg_item_float(hdlg,IDC_EU,0.000,4);
	ai_win::set_dlg_item_float(hdlg,IDC_EV,0.000,4);
	ai_win::set_dlg_item_float(hdlg,IDC_EW,0.000,4);

	if(ID>=0){
		my_handle[0]  = CreateThread(NULL, 0, get_point		,	(LPVOID)hdlg, 0, &my_id[0]);//创建线程
		my_handle[1]  = CreateThread(NULL, 0, set_gray		,	(LPVOID)hdlg, 0, &my_id[1]);
		my_handle[2]  = CreateThread(NULL, 0, get_mess		,	(LPVOID)hdlg, 0, &my_id[2]);
		my_handle[4]  = CreateThread(NULL, 0, set_point		,	(LPVOID)hdlg, 0, &my_id[4]);
		my_handle[5]  = CreateThread(NULL, 0, PI_status		,	(LPVOID)hdlg, 0, &my_id[5]);
	
		PI_qVLS(ID,&sudu);
		ai_win::set_dlg_item_float(hdlg,IDC_SSD,sudu,4);
		ai_win::set_dlg_item_float(hdlg,IDC_ESD,sudu,4);
		PI_SPI(ID,axis2,dStart);
	}
	dPos[0]=ai_win::get_dlg_item_float(hdlg,IDC_EX);
	dPos[1]=ai_win::get_dlg_item_float(hdlg,IDC_EY);
	dPos[2]=ai_win::get_dlg_item_float(hdlg,IDC_EZ);
	dPos[3]=ai_win::get_dlg_item_float(hdlg,IDC_EU);
	dPos[4]=ai_win::get_dlg_item_float(hdlg,IDC_EV);
	dPos[5]=ai_win::get_dlg_item_float(hdlg,IDC_EW);

	//dStart[0] = ai_win::read_float_key(pi_path, "pipoint", "pi_x");
	//dStart[1] = ai_win::read_float_key(pi_path, "pipoint", "pi_y");
	//dStart[2] = ai_win::read_float_key(pi_path, "pipoint", "pi_z");
	
	ai_win::set_dlg_item_float(hdlg,IDC_SETX,dStart[0],4);
	ai_win::set_dlg_item_float(hdlg,IDC_SETY,dStart[1],4);
	ai_win::set_dlg_item_float(hdlg,IDC_SETZ,dStart[2],4);

	ai_win::set_dlg_item_float(hdlg,IDC_XDX,dXd[0],4);
	ai_win::set_dlg_item_float(hdlg,IDC_XDY,dXd[1],4);
	ai_win::set_dlg_item_float(hdlg,IDC_XDZ,dXd[2],4);
	ai_win::set_dlg_item_float(hdlg,IDC_XDU,dXd[3],4);
	ai_win::set_dlg_item_float(hdlg,IDC_XDV,dXd[4],4);
	ai_win::set_dlg_item_float(hdlg,IDC_XDW,dXd[5],4);
	//***************************************************************获得地址
	sprintf(pi_path, "%s\\recipe\\%s\\pi_point.ini", s_aa::dir, s_aa::recipe_name);

	//****************************************************************获得步长
	pi_step = ai_win::read_float_key(pi_path, "pipoint", "pi_step");

	ai_win::set_dlg_item_float(hdlg,IDC_PI_STEP,pi_step,4);
	//***************************************************************
	COMBO_ADDSTR(hdlg, IDC_COMBO1, "Point_1");
	COMBO_ADDSTR(hdlg, IDC_COMBO1, "Point_2");
	COMBO_ADDSTR(hdlg, IDC_COMBO1, "Point_3");
	COMBO_ADDSTR(hdlg, IDC_COMBO1, "Point_4");
	COMBO_ADDSTR(hdlg, IDC_COMBO1, "Point_5");
	COMBO_ADDSTR(hdlg, IDC_COMBO2, "Point_1");
	COMBO_ADDSTR(hdlg, IDC_COMBO2, "Point_2");
	COMBO_ADDSTR(hdlg, IDC_COMBO2, "Point_3");
	COMBO_ADDSTR(hdlg, IDC_COMBO2, "Point_4");
	COMBO_ADDSTR(hdlg, IDC_COMBO2, "Point_5");
	COMBO_ADDSTR(hdlg, IDC_COMBO3, "Point_1");
	COMBO_ADDSTR(hdlg, IDC_COMBO3, "Point_2");
	COMBO_ADDSTR(hdlg, IDC_COMBO3, "Point_3");
	COMBO_ADDSTR(hdlg, IDC_COMBO3, "Point_4");
	COMBO_ADDSTR(hdlg, IDC_COMBO3, "Point_5");
	COMBO_ADDSTR(hdlg, IDC_COMBO4, "Point_1");
	COMBO_ADDSTR(hdlg, IDC_COMBO4, "Point_2");
	COMBO_ADDSTR(hdlg, IDC_COMBO4, "Point_3");
	COMBO_ADDSTR(hdlg, IDC_COMBO4, "Point_4");
	COMBO_ADDSTR(hdlg, IDC_COMBO4, "Point_5");
	COMBO_ADDSTR(hdlg, IDC_COMBO5, "Point_1");
	COMBO_ADDSTR(hdlg, IDC_COMBO5, "Point_2");
	COMBO_ADDSTR(hdlg, IDC_COMBO5, "Point_3");
	COMBO_ADDSTR(hdlg, IDC_COMBO5, "Point_4");
	COMBO_ADDSTR(hdlg, IDC_COMBO5, "Point_5");
	
	//SELECT_IMG_SOURCE = ai_win::read_integer_key(s_aa::config_path, "img", "select_img_source");
	dPos_p[0][0] = ai_win::read_float_key(pi_path, "pipoint", "point_1_x");
	dPos_p[0][1] = ai_win::read_float_key(pi_path, "pipoint", "point_1_y");
	dPos_p[0][2] = ai_win::read_float_key(pi_path, "pipoint", "point_1_z");
	dPos_p[0][3] = ai_win::read_float_key(pi_path, "pipoint", "point_1_u");
	dPos_p[0][4] = ai_win::read_float_key(pi_path, "pipoint", "point_1_v");
	dPos_p[0][5] = ai_win::read_float_key(pi_path, "pipoint", "point_1_w");
	dPos_p[1][0] = ai_win::read_float_key(pi_path, "pipoint", "point_2_x");
	dPos_p[1][1] = ai_win::read_float_key(pi_path, "pipoint", "point_2_y");
	dPos_p[1][2] = ai_win::read_float_key(pi_path, "pipoint", "point_2_z");
	dPos_p[1][3] = ai_win::read_float_key(pi_path, "pipoint", "point_2_u");
	dPos_p[1][4] = ai_win::read_float_key(pi_path, "pipoint", "point_2_v");
	dPos_p[1][5] = ai_win::read_float_key(pi_path, "pipoint", "point_2_w");
	dPos_p[2][0] = ai_win::read_float_key(pi_path, "pipoint", "point_3_x");
	dPos_p[2][1] = ai_win::read_float_key(pi_path, "pipoint", "point_3_y");
	dPos_p[2][2] = ai_win::read_float_key(pi_path, "pipoint", "point_3_z");
	dPos_p[2][3] = ai_win::read_float_key(pi_path, "pipoint", "point_3_u");
	dPos_p[2][4] = ai_win::read_float_key(pi_path, "pipoint", "point_3_v");
	dPos_p[2][5] = ai_win::read_float_key(pi_path, "pipoint", "point_3_w");
	dPos_p[3][0] = ai_win::read_float_key(pi_path, "pipoint", "point_4_x");
	dPos_p[3][1] = ai_win::read_float_key(pi_path, "pipoint", "point_4_y");
	dPos_p[3][2] = ai_win::read_float_key(pi_path, "pipoint", "point_4_z");
	dPos_p[3][3] = ai_win::read_float_key(pi_path, "pipoint", "point_4_u");
	dPos_p[3][4] = ai_win::read_float_key(pi_path, "pipoint", "point_4_v");
	dPos_p[3][5] = ai_win::read_float_key(pi_path, "pipoint", "point_4_w");
	dPos_p[4][0] = ai_win::read_float_key(pi_path, "pipoint", "point_5_x");
	dPos_p[4][1] = ai_win::read_float_key(pi_path, "pipoint", "point_5_y");
	dPos_p[4][2] = ai_win::read_float_key(pi_path, "pipoint", "point_5_z");
	dPos_p[4][3] = ai_win::read_float_key(pi_path, "pipoint", "point_5_u");
	dPos_p[4][4] = ai_win::read_float_key(pi_path, "pipoint", "point_5_v");
	dPos_p[4][5] = ai_win::read_float_key(pi_path, "pipoint", "point_5_w");
	//COMBO_SETCURSEL(hdlg, IDC_SELECT_IMG_DEVICE, SELECT_IMG_SOURCE);
	point_num[0] = ai_win::read_integer_key(pi_path, "pipoint", "point_num1");
	point_num[1] = ai_win::read_integer_key(pi_path, "pipoint", "point_num2");
	point_num[2] = ai_win::read_integer_key(pi_path, "pipoint", "point_num3");
	point_num[3] = ai_win::read_integer_key(pi_path, "pipoint", "point_num4");
	point_num[4] = ai_win::read_integer_key(pi_path, "pipoint", "point_num5");
	COMBO_SETCURSEL(hdlg, IDC_COMBO1, point_num[0]);
	COMBO_SETCURSEL(hdlg, IDC_COMBO2, point_num[1]);
	COMBO_SETCURSEL(hdlg, IDC_COMBO3, point_num[2]);
	COMBO_SETCURSEL(hdlg, IDC_COMBO4, point_num[3]);
	COMBO_SETCURSEL(hdlg, IDC_COMBO5, point_num[4]);

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

	sudu=ai_win::get_dlg_item_float(HWND_PI_MOTION,IDC_ESD);

	return TRUE;
}

void DlgPI::command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify)
{
	int timeout;
	int err = 0;
	switch (id){
		//**************************************************************************各轴在其方向上的点动
	case IDC_PI_SETP_X1:
		err = PI_move_step_y(pi_step);
		break;
	case IDC_PI_SETP_X2:
		err = PI_move_step_y(-pi_step);
		break;
	case IDC_PI_SETP_Y1:
		err = PI_move_step_x(pi_step);
		break;
	case IDC_PI_SETP_Y2:
		err = PI_move_step_x(-pi_step);
		break;
	case IDC_PI_SETP_Z1:
		err = PI_move_step_z(pi_step);
		break;
		
	case IDC_PI_SETP_Z2:
		err = PI_move_step_z(-pi_step);
		break;

	case IDC_PI_SETP_U1:
		err = PI_move_step_yt(pi_step);
		break;
	case IDC_PI_SETP_U2:
		err = PI_move_step_yt(-pi_step);
		break;
	case IDC_PI_SETP_V1:
		err = PI_move_step_xt(pi_step);
		break;
	case IDC_PI_SETP_V2:
		err = PI_move_step_xt(-pi_step);
		break;
	case IDC_PI_SETP_W1:
		Pos[5]+=pi_step;
		PI_qTMX ( ID, "X Y Z U V W", dMaxTravelPositive );
		PI_qVMO ( ID, axis, Pos, &bMovePossible );
		if(FALSE == bMovePossible)
		{
			log("无法移动到目标位置!");
			break;
		}
		if ( !PI_IsMoving ( ID, "X Y Z U V W", bIsMoving2 ) )
			break;
		if(((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])))
		{
			log("目标在移动中,请稍后重试");
			break;
		}
		//sudu=ai_win::get_dlg_item_float(hdlg,IDC_ESD);
		//if(!PI_VLS(ID,sudu))
		//{
		//	log("设定速度错误!");
		//}
		if ( !MoveToPosition ( ID, axis, Pos ) )
			throw ( -4 );
		break;
	case IDC_PI_SETP_W2:
		Pos[5]-=pi_step;
		PI_qTMX ( ID, "X Y Z U V W", dMaxTravelPositive );
		PI_qVMO ( ID, axis, Pos, &bMovePossible );
		if(FALSE == bMovePossible)
		{
			log("无法移动到目标位置!");
			break;
		}
		if ( !PI_IsMoving ( ID, "X Y Z U V W", bIsMoving2 ) )
			break;
		if(((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])))
		{
			log("目标在移动中,请稍后重试");
			break;
		}
		//sudu=ai_win::get_dlg_item_float(hdlg,IDC_ESD);
		//if(!PI_VLS(ID,sudu))
		//{
		//	log("设定速度错误!");
		//}
		if ( !MoveToPosition ( ID, axis, Pos ) )
			throw ( -4 );
		break;

		//**************************************************************************移动到给定的P1-P5点位置
	case IDC_TO_P1:
		err = PI_move_to_point(dPos_p[point_num[0]]);
		break;
	case IDC_TO_P2:
		err = PI_move_to_point(dPos_p[point_num[1]]);
		break;
	case IDC_TO_P3:
		err = PI_move_to_point(dPos_p[point_num[2]]);
		break;
	case IDC_TO_P4:
		err = PI_move_to_point(dPos_p[point_num[3]]);
		break;
	case IDC_TO_P5:
		err = PI_move_to_point(dPos_p[point_num[4]]);
		break;
	//****************************************************************************保存当前位置坐标
	case IDC_SAVE1:
		save_point(hdlg,0);
		break;
	case IDC_SAVE2:
		save_point(hdlg,1);
		break;
	case IDC_SAVE3:
		save_point(hdlg,2);
		break;
	case IDC_SAVE4:
		save_point(hdlg,3);
		break;
	case IDC_SAVE5:
		save_point(hdlg,4);
		break;

	//****************************************************************************
	case IDC_PI_MOVE2:								//相对移动量
		dXd[0]=ai_win::get_dlg_item_float(hdlg,IDC_XDX);
		dXd[1]=ai_win::get_dlg_item_float(hdlg,IDC_XDY);
		dXd[2]=ai_win::get_dlg_item_float(hdlg,IDC_XDZ);
		dXd[3]=ai_win::get_dlg_item_float(hdlg,IDC_XDU);
		dXd[4]=ai_win::get_dlg_item_float(hdlg,IDC_XDV);
		dXd[5]=ai_win::get_dlg_item_float(hdlg,IDC_XDW);
		for(int i=0;i<6;i++){
			Pos[i]+=dXd[i];
		}
		PI_qTMX ( ID, "X Y Z U V W", dMaxTravelPositive );
		PI_qVMO ( ID, axis, Pos, &bMovePossible );
		if(FALSE == bMovePossible)
		{
			log("无法移动到目标位置!");
			break;
		}
		if ( !PI_IsMoving ( ID, "X Y Z U V W", bIsMoving2 ) )
			break;
		if(((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])))
		{
			log("目标在移动中,请稍后重试");
			break;
		}
		//sudu=ai_win::get_dlg_item_float(hdlg,IDC_ESD);
		//if(!PI_VLS(ID,sudu))
		//{
		//	log("设定速度错误!");
		//	//break;
		//}
		if ( !MoveToPosition ( ID, axis, Pos ) )
			throw ( -4 );
		break;
	case IDC_PI_SET_ZORE:							//设置轴心位置
		dStart[0]=ai_win::get_dlg_item_float(hdlg,IDC_SETX);
		dStart[1]=ai_win::get_dlg_item_float(hdlg,IDC_SETY);
		dStart[2]=ai_win::get_dlg_item_float(hdlg,IDC_SETZ);
		if(!PI_SPI(ID,axis2,dStart)){
			log("设置轴心错误!");
			//break;;
		}		
		sudu=ai_win::get_dlg_item_float(hdlg,IDC_ESD);
		if(!PI_VLS(ID,sudu))
		{
			log("设定速度错误!");
			//break;
		}
		break;
	case IDC_PI_STOP:								//停止运动按钮
		EMERGENCY_STOP_FLAG = true;
		if(!PI_STP ( ID ))
		{
			MessageBox(hdlg,"error!",NULL,NULL);
			break;
		}
		move_p1_to_p5=0;
		break;
	case IDC_PI_START:								//初始位置校准

		if ( !PI_FRF ( ID, axis ) )//*****************************************参考初始化
			throw ( -4 );
		timeout = 1000;
		while(timeout-->0 && !PI_SPI(ID,axis2,dStart)){
			Sleep(20);
		}
		if(timeout<=0)
		{
			log("move home time out");
		}
		else
		{
			ai_win::set_dlg_item_float(HWND_PI_MOTION,IDC_GETX,dStart[0],4);
			ai_win::set_dlg_item_float(HWND_PI_MOTION,IDC_GETY,dStart[1],4);
			ai_win::set_dlg_item_float(HWND_PI_MOTION,IDC_GETZ,dStart[2],4);
		}
		break;
	case IDC_PI_RELINK:								//重新连接PI
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
		//if(ID>=0){
		//	my_handle[0]  = CreateThread(NULL, 0, get_point		,	(LPVOID)hdlg, 0, &my_id[0]);//创建线程
		//	my_handle[1]  = CreateThread(NULL, 0, set_gray		,	(LPVOID)hdlg, 0, &my_id[1]);
		//	my_handle[2]  = CreateThread(NULL, 0, get_mess		,	(LPVOID)hdlg, 0, &my_id[2]);
		//}
		if(ID>=0){
			PI_qVLS(ID,&sudu);
			ai_win::set_dlg_item_float(hdlg,IDC_SSD,sudu,4);
			ai_win::set_dlg_item_float(hdlg,IDC_ESD,sudu,4);
			PI_SPI(ID,axis2,dStart); 
		}
		break;
	case IDC_PI_MOVE:								//运动到给定的位置
		dPos[0]=ai_win::get_dlg_item_float(hdlg,IDC_EX);
		dPos[1]=ai_win::get_dlg_item_float(hdlg,IDC_EY);
		dPos[2]=ai_win::get_dlg_item_float(hdlg,IDC_EZ);
		dPos[3]=ai_win::get_dlg_item_float(hdlg,IDC_EU);
		dPos[4]=ai_win::get_dlg_item_float(hdlg,IDC_EV);
		dPos[5]=ai_win::get_dlg_item_float(hdlg,IDC_EW);
		PI_qTMX ( ID, "X Y Z U V W", dMaxTravelPositive );
		PI_qVMO ( ID, axis, dPos, &bMovePossible );
		if(FALSE == bMovePossible)
		{
			log("无法移动到目标位置!");
			break;
		}
		if ( !PI_IsMoving ( ID, "X Y Z U V W", bIsMoving2 ) )
			break;
		if(((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])))
		{
			log("目标在移动中,请稍后重试");
			break;
		}
		//sudu=ai_win::get_dlg_item_float(hdlg,IDC_ESD);
		//if(!PI_VLS(ID,sudu))
		//{
		//	log("设定速度错误!");
		//	//break;
		//}
		if ( !MoveToPosition ( ID, axis, dPos ) )
			throw ( -4 );
		break;

	case IDC_PI_ZORE:							//回到原点（0）位置
		dPos[0]=0;
		dPos[1]=0;
		dPos[2]=0;
		dPos[3]=0;
		dPos[4]=0;
		dPos[5]=0;
		PI_qTMX ( ID, "X Y Z U V W", dMaxTravelPositive );
		if ( !PI_IsMoving ( ID, "X Y Z U V W", bIsMoving2 ) )
			log("error");
		if(((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])))
		{
			log("目标在移动中,请稍后重试");
		}
		//sudu=ai_win::get_dlg_item_float(hdlg,IDC_ESD);
		//if(!PI_VLS(ID,sudu))
		//{
		//	log("设定速度错误!");
		//	//break;
		//}
		if ( !MoveToPosition ( ID, axis, dPos ))
			throw ( -4 );
		if ( !PI_IsMoving ( ID, "X Y Z U V W", bIsMoving2 ) ){
			log("is moving check err.");
		}
		while(((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])))
		{
			//log("目标在移动中,请稍后重试");
			Sleep(50);
		}
		break;
	}
	switch (code_notify){
	case CBN_SELCHANGE:
		{
			int index = COMBO_GETCURSEL(hdlg, IDC_COMBO1);
			chang_p(index,0);
			if(is_change) break;
			index = COMBO_GETCURSEL(hdlg, IDC_COMBO2);
			chang_p(index,1);
			if(is_change) break;
			index = COMBO_GETCURSEL(hdlg, IDC_COMBO3);
			chang_p(index,2);
			if(is_change) break;
			index = COMBO_GETCURSEL(hdlg, IDC_COMBO4);
			chang_p(index,3);
			if(is_change) break;
			index = COMBO_GETCURSEL(hdlg, IDC_COMBO5);
			chang_p(index,4);
			//ai_win::write_integer_key(s_aa::config_path, "img", "select_img_source",		 SELECT_IMG_SOURCE);
		}
		break;
	}
}
	
void DlgPI::close(HWND hdlg)
{
	//***************************************************************获得地址
	ai_win::write_float_key(pi_path, "pipoint", "pi_step",pi_step );

	ai_win::write_float_key(pi_path, "pipoint", "pi_x",dStart[0] );
	ai_win::write_float_key(pi_path, "pipoint", "pi_y",dStart[1] );
	ai_win::write_float_key(pi_path, "pipoint", "pi_z",dStart[2] );
	//********
	ai_win::write_float_key(pi_path, "pipoint", "point_1_x",dPos_p[0][0] );
	ai_win::write_float_key(pi_path, "pipoint", "point_1_y",dPos_p[0][1] );
	ai_win::write_float_key(pi_path, "pipoint", "point_1_z",dPos_p[0][2] );
	ai_win::write_float_key(pi_path, "pipoint", "point_1_u",dPos_p[0][3] );
	ai_win::write_float_key(pi_path, "pipoint", "point_1_v",dPos_p[0][4] );
	ai_win::write_float_key(pi_path, "pipoint", "point_1_w",dPos_p[0][5] );
	ai_win::write_float_key(pi_path, "pipoint", "point_2_x",dPos_p[1][0] );
	ai_win::write_float_key(pi_path, "pipoint", "point_2_y",dPos_p[1][1] );
	ai_win::write_float_key(pi_path, "pipoint", "point_2_z",dPos_p[1][2] );
	ai_win::write_float_key(pi_path, "pipoint", "point_2_u",dPos_p[1][3] );
	ai_win::write_float_key(pi_path, "pipoint", "point_2_v",dPos_p[1][4] );
	ai_win::write_float_key(pi_path, "pipoint", "point_2_w",dPos_p[1][5] );
	ai_win::write_float_key(pi_path, "pipoint", "point_3_x",dPos_p[2][0] );
	ai_win::write_float_key(pi_path, "pipoint", "point_3_y",dPos_p[2][1] );
	ai_win::write_float_key(pi_path, "pipoint", "point_3_z",dPos_p[2][2] );
	ai_win::write_float_key(pi_path, "pipoint", "point_3_u",dPos_p[2][3] );
	ai_win::write_float_key(pi_path, "pipoint", "point_3_v",dPos_p[2][4] );
	ai_win::write_float_key(pi_path, "pipoint", "point_3_w",dPos_p[2][5] );
	ai_win::write_float_key(pi_path, "pipoint", "point_4_x",dPos_p[3][0] );
	ai_win::write_float_key(pi_path, "pipoint", "point_4_y",dPos_p[3][1] );
	ai_win::write_float_key(pi_path, "pipoint", "point_4_z",dPos_p[3][2] );
	ai_win::write_float_key(pi_path, "pipoint", "point_4_u",dPos_p[3][3] );
	ai_win::write_float_key(pi_path, "pipoint", "point_4_v",dPos_p[3][4] );
	ai_win::write_float_key(pi_path, "pipoint", "point_4_w",dPos_p[3][5] );
	ai_win::write_float_key(pi_path, "pipoint", "point_5_x",dPos_p[4][0] );
	ai_win::write_float_key(pi_path, "pipoint", "point_5_y",dPos_p[4][1] );
	ai_win::write_float_key(pi_path, "pipoint", "point_5_z",dPos_p[4][2] );
	ai_win::write_float_key(pi_path, "pipoint", "point_5_u",dPos_p[4][3] );
	ai_win::write_float_key(pi_path, "pipoint", "point_5_v",dPos_p[4][4] );
	ai_win::write_float_key(pi_path, "pipoint", "point_5_w",dPos_p[4][5] );

	PI_STP ( ID );
	EndDialog(hdlg, FALSE);

//	PI_CloseConnection ( ID );
}

int PI_move_step_z(double value){
	int i;
	double pos[6] = {0};
	int cnt=0;
	PI_qPOS ( ID, "X Y Z U V W", pos );
//	sudu=ai_win::get_dlg_item_float(HWND_PI_MOTION,IDC_ESD);
	pos[2]+=value;
	log("move step :%f,z pos :%f",value,pos[2]);
	PI_qTMX ( ID, "Z", dMaxTravelPositive );
	PI_qVMO ( ID, axis, pos, &bMovePossible );
	if(FALSE == bMovePossible)
	{
		log("无法移动到目标位置!");
		return 1;
	}
	if(! PI_IsMoving ( ID, axis, bIsMoving2 ) ){
		log("is moving check err");
		return 1;
	}
	i = 0;
	while((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])){
		//log("目标在移动中,请稍后重试");
		Sleep(20);
	}

	//if(!PI_VLS(ID,sudu))
	//{
	//	log("设定速度错误!");
	//}

	if ( !MoveToPosition ( ID, axis, pos ) )
		throw ( -4 );
	if(! PI_IsMoving ( ID, axis, bIsMoving2 ) ){
		log("is moving check err");
		return 1;
	}
	i = 0;
	while((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])){
		//log("目标在移动中,请稍后重试");
		Sleep(20);
	}
	log("i1=%d",i);
	while(abs(pos[2]-Pos[2])>0.002 && cnt < 5){
		MoveToPosition ( ID, axis, pos );
		cnt++;
		i = 0;
		while((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])){
			//log("目标在移动中,请稍后重试");
			Sleep(20);
		}
		log("i2=%d",i);
	}
	log("movedown");
	return 0;
}

int PI_move_step_xy(double value_x, double value_y){
	int i;
	double pos[6] = {0};
	int cnt=0;
	PI_qPOS ( ID, "X Y Z U V W", pos );
//	sudu=ai_win::get_dlg_item_float(HWND_PI_MOTION,IDC_ESD);
	pos[0]+=value_y;
	pos[1]+=value_x;
	log("move step :%f,z pos :%f move step :%f,x pos :%f",value_x,pos[1],value_y,pos[0]);
	PI_qTMX ( ID, "X,Y", dMaxTravelPositive );
	PI_qVMO ( ID, axis, pos, &bMovePossible );
	if(FALSE == bMovePossible)
	{
		log("无法移动到目标位置!");
		return 1;
	}
	if(! PI_IsMoving ( ID, axis, bIsMoving2 ) ){
		log("is moving check err");
		return 1;
	}
	i = 0;
	while((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])){
		//log("目标在移动中,请稍后重试");
		Sleep(20);
	}

	if ( !MoveToPosition ( ID, axis, pos ) )
		throw ( -4 );
	log("Pos :%f,z pos :%f Pos :%f,z pos :%f",Pos[1],pos[1],Pos[0],pos[0]);
	if(! PI_IsMoving ( ID, axis, bIsMoving2 ) ){
		log("is moving check err");
		return 1;
	}
	while((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])){
		//log("目标在移动中,请稍后重试");
		Sleep(20);
	}
	while((abs(pos[0]-Pos[0])>0.002 ||abs(pos[1]-Pos[1])>0.002 )&& cnt < 5){
		MoveToPosition ( ID, axis, pos );
		cnt++;

		if(! PI_IsMoving ( ID, axis, bIsMoving2 ) ){
			log("is moving check err");
			return 1;
		}
		i = 0;
		while((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])){
			//log("目标在移动中,请稍后重试");
			Sleep(20);
		}
	}
	return 0;
}


int PI_move_step_x(double value){
	int i;
	double pos[6] = {0};
	int cnt=0;
	PI_qPOS ( ID, "X Y Z U V W", pos );
//	sudu=ai_win::get_dlg_item_float(HWND_PI_MOTION,IDC_ESD);
	pos[0]+=value;
	log("move step :%f,x pos :%f",value,pos[0]);
	PI_qTMX ( ID, "X", dMaxTravelPositive );
	PI_qVMO ( ID, axis, pos, &bMovePossible );
	if(FALSE == bMovePossible)
	{
		log("无法移动到目标位置!");
		return 1;
	}
	if(! PI_IsMoving ( ID, axis, bIsMoving2 ) ){
		log("is moving check err");
		return 1;
	}
	while((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])){
		//log("目标在移动中,请稍后重试");
		Sleep(20);
	}

	if ( !MoveToPosition ( ID, axis, pos ) )
		throw ( -4 );
	if(! PI_IsMoving ( ID, axis, bIsMoving2 ) ){
		log("is moving check err");
		return 1;
	}
	i = 0;
	while((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])){
		//log("目标在移动中,请稍后重试");
		Sleep(20);
	}
	log("Pos :%f,z pos :%f",Pos[0],pos[0]);
	while(abs(pos[0]-Pos[0])>0.002 && cnt < 5){
		MoveToPosition ( ID, axis, pos );
		cnt++;
	
		if(! PI_IsMoving ( ID, axis, bIsMoving2 ) ){
			log("is moving check err");
			return 1;
		}
		while((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])){
			//log("目标在移动中,请稍后重试");
			Sleep(20);
		}
	}
	return 0;
}

int PI_move_step_y(double value){
	int i;
	double pos[6] = {0};
	int cnt=0;
	PI_qPOS ( ID, "X Y Z U V W", pos );
	//sudu=ai_win::get_dlg_item_float(HWND_PI_MOTION,IDC_ESD);
	pos[1]+=value;
	log("move step :%f,z pos :%f",value,pos[1]);
	PI_qTMX ( ID, "Y", dMaxTravelPositive );
	PI_qVMO ( ID, axis, pos, &bMovePossible );
	if(FALSE == bMovePossible)
	{
		log("无法移动到目标位置!");
		return 1;
	}
	if(! PI_IsMoving ( ID, axis, bIsMoving2 ) ){
		log("is moving check err");
		return 1;
	}
	i = 0;
	while((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])){
		//log("目标在移动中,请稍后重试");
		Sleep(20);
	}

	if ( !MoveToPosition ( ID, axis, pos ) )
		throw ( -4 );
	log("Pos :%f,z pos :%f",Pos[1],pos[1]);
	if(! PI_IsMoving ( ID, axis, bIsMoving2 ) ){
		log("is moving check err");
		return 1;
	}
	i = 0;
	while((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])){
		//log("目标在移动中,请稍后重试");
		Sleep(20);
	}
	while(abs(pos[1]-Pos[1])>0.002 && cnt < 5){
		MoveToPosition ( ID, axis, pos );
		cnt++;
	
		if(! PI_IsMoving ( ID, axis, bIsMoving2 ) ){
			log("is moving check err");
			return 1;
		}
		i = 0;
		while((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])){
			//log("目标在移动中,请稍后重试");
			Sleep(20);
		}
	}
	return 0;
}

int PI_move_step_xt(double value){
	int i;
	double pos[6] = {0};
	int cnt=0;
	PI_qPOS ( ID, "X Y Z U V W", pos );
	//sudu=ai_win::get_dlg_item_float(HWND_PI_MOTION,IDC_ESD);
	pos[3]+=value;
	log("move step :%f,u pos :%f",value,pos[3]);
	PI_qTMX ( ID, "U", dMaxTravelPositive );
	PI_qVMO ( ID, axis, pos, &bMovePossible );
	if(FALSE == bMovePossible)
	{
		log("无法移动到目标位置!");
		return 1;
	}
	if(! PI_IsMoving ( ID, axis, bIsMoving2 ) ){
		log("is moving check err");
		return 1;
	}
	i = 0;
	while((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])){
		//log("目标在移动中,请稍后重试");
		Sleep(20);
	}

	if ( !MoveToPosition ( ID, axis, pos ) )
		throw ( -4 );
	if(! PI_IsMoving ( ID, axis, bIsMoving2 ) ){
		log("is moving check err");
		return 1;
	}
	while((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])){
		//log("目标在移动中,请稍后重试");
		Sleep(20);
	}
	log("Pos :%f,z pos :%f",Pos[3],pos[3]);
	while(abs(pos[3]-Pos[3])>0.002 && cnt < 5){
		MoveToPosition ( ID, axis, pos );
		cnt++;
		if(! PI_IsMoving ( ID, axis, bIsMoving2 ) ){
			log("is moving check err");
			return 1;
		}
		while((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])){
			//log("目标在移动中,请稍后重试");
			Sleep(20);
		}
	}
	return 0;
}

int PI_move_step_yt(double value){
	int i;
	double pos[6] = {0};
	int cnt=0;
	PI_qPOS ( ID, "X Y Z U V W", pos );
	//sudu=ai_win::get_dlg_item_float(HWND_PI_MOTION,IDC_ESD);
	pos[4]+=value;
	log("move step :%f,v pos :%f",value,pos[4]);
	PI_qTMX ( ID, "V", dMaxTravelPositive );
	PI_qVMO ( ID, axis, pos, &bMovePossible );
	if(FALSE == bMovePossible)
	{
		log("无法移动到目标位置!");
		return 1;
	}
	if(! PI_IsMoving ( ID, axis, bIsMoving2 ) ){
		log("is moving check err");
		return 1;
	}
	i = 0;
	while((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])){
		//log("目标在移动中,请稍后重试");
		Sleep(20);
	}
	//if(!PI_VLS(ID,sudu))
	//{
	//	log("设定速度错误!");

	//}
	if ( !MoveToPosition ( ID, axis, pos ) )
		throw ( -4 );
	if(! PI_IsMoving ( ID, axis, bIsMoving2 ) ){
		log("is moving check err");
		return 1;
	}
	i = 0;
	while((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])){
		
		//log("目标在移动中,请稍后重试");
		Sleep(20);
	}
	log("Pos :%f,z pos :%f",Pos[4],pos[4]);
	while(abs(pos[4]-Pos[4])>0.002 && cnt < 5){
		MoveToPosition ( ID, axis, pos );
		cnt++;
		if(! PI_IsMoving ( ID, axis, bIsMoving2 ) ){
			log("is moving check err");
			return 1;
		}
		while((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])){
			
			//log("目标在移动中,请稍后重试");
			Sleep(20);
		}
	}
	return 0;
}

int PI_move_step_yxt(double value,double value2){
	int i;
	double pos[6] = {0};
	int cnt=0;
	PI_qPOS ( ID, "X Y Z U V W", pos );
	//sudu=ai_win::get_dlg_item_float(HWND_PI_MOTION,IDC_ESD);
	pos[4]+=value;
	pos[3]+=value2;
	log("move step :%f,v pos :%f move step :%f,u pos :%f-----move uv",value,pos[4],value2,pos[3]);
	PI_qTMX ( ID, "VX Y Z U V W", dMaxTravelPositive );
	PI_qVMO ( ID, axis, pos, &bMovePossible );
	if(FALSE == bMovePossible)
	{
		log("无法移动到目标位置!");
		return 1;
	}
	if(! PI_IsMoving ( ID, axis, bIsMoving2 ) ){
		log("is moving check err");
		return 1;
	}
	while((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])){
		//log("目标在移动中,请稍后重试");
		Sleep(20);
	}
	if ( !MoveToPosition ( ID, axis, pos ) )
		throw ( -4 );
	if(! PI_IsMoving ( ID, axis, bIsMoving2 ) ){
		log("is moving check err");
		return 1;
	}
	while((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])){
		
		//log("目标在移动中,请稍后重试");
		Sleep(20);
	}
	log("Pos :%f,z pos :%f Pos :%f,z pos :%f",Pos[4],pos[4],Pos[3],pos[3]);
	while(abs(pos[4]-Pos[4])>0.002 && cnt < 5){
		MoveToPosition ( ID, axis, pos );
		cnt++;
		if(! PI_IsMoving ( ID, axis, bIsMoving2 ) ){
			log("is moving check err");
			return 1;
		}
		i = 0;
		while((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5])){
			//log("目标在移动中,请稍后重试");
			Sleep(20);
		}
	}
	return 0;
}

int PI_move_to_point(double posArray[6]){
	int i=0;
//	sudu=ai_win::get_dlg_item_float(HWND_PI_MOTION,IDC_ESD);
	PI_qTMX ( ID, "X Y Z U V W", dMaxTravelPositive );
	PI_qVMO ( ID, axis, posArray/*[point_num[0]]*/ , &bMovePossible );
	if(FALSE == bMovePossible)
	{
		log("无法移动到目标位置");
		return 1;
	}
	if ( !PI_IsMoving ( ID, "X Y Z U V W", bIsMoving2 ) ){
		log("is moving check err.");
		return 1;
	}
	while((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5]))
	{
		//log("目标在移动中,请稍后重试");
		Sleep(50);
	}
	i = 0;

	if ( !MoveToPosition ( ID, axis, posArray ) )
		throw ( -4 );
		
	if ( !PI_IsMoving ( ID, "X Y Z U V W", bIsMoving2 ) ){
		log("is moving check err.");
		return 1;
	}
	while((bIsMoving2[0])||(bIsMoving2[1])||(bIsMoving2[2])||(bIsMoving2[3])||(bIsMoving2[4])||(bIsMoving2[5]))
	{
		//log("目标在移动中,请稍后重试");
		Sleep(50);
	}
	log("i:%d",i);
	return 0;
}





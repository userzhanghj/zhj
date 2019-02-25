#include <Windows.h>
#include <stdio.h>
#include <time.h>
#include "Pi_gcs2_dll.h"
#pragma comment(lib,"Pi_gcs2_dll.lib")

//********************************************************************
#define THREAD_NUM	16
extern int ID;
//srand ( ( int ) time ( NULL ) );
extern char axis[];		// modify here to change moved axes
extern char axis2[];

extern BOOL bMovePossible;
extern double dMaxTravelPositive[6];
extern BOOL bIsMoving;
extern double dPos[6];
extern double dPos2[6];
extern double dPos_p[5][6];

extern double dXd[6];
extern double dStart[3];
extern DWORD my_id[THREAD_NUM];
extern HANDLE my_handle[THREAD_NUM];
extern BOOL bIsMoving2[6];
extern double sudu;
extern double Pos[6];
extern double Pos2[6];

//*******************************Point1 to Point5相关
extern int move_p1_to_p5;
extern int point_num[5];
extern int is_change;
extern int point_id;

//*******************************单轴相对运动相关
extern double pi_step;


//*********************************************************************

bool MoveToPosition ( int ID, char* axis, double *dVal );

void chang_p(int a,int n);

void save_point(HWND hdlg,int i);

DWORD CALLBACK get_point(LPVOID temp);

DWORD CALLBACK set_gray(LPVOID temp);

DWORD CALLBACK get_mess(LPVOID temp);

DWORD CALLBACK move_point(LPVOID temp);

DWORD CALLBACK set_point(LPVOID temp);

DWORD CALLBACK PI_status(LPVOID temp);

bool move_safe();

bool move_home();

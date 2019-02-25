#include <Windows.h>
#include <stdio.h>
#include <direct.h>
#include "s_win.h"
#include "s_img.h"
#include "resource.h"

//int d1,d2,d3,s;
char dir[MAX_PATH];						//数据地址相关
char config_path[MAX_PATH];

int sum(int a,int b,int c)				
{
	return b+a+c;
}

int jiorou(int a)						
{
	int n=a%2;
	if(n==1)
		return 1;
	else
		return 0;
}
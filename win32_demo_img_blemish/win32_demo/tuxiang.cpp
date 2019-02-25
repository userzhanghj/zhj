#include <Windows.h>
#include <stdio.h>
#include <direct.h>
#include "s_win.h"
#include "s_img.h"
#include "resource.h"

char dir2[MAX_PATH];					//Í¼Æ¬µØÖ·Ïà¹Ø
char bmp_path[MAX_PATH];

#define MAX_IMAGE_W		5500
#define MAX_IMAGE_H		4500

BYTE bmp_buf[MAX_IMAGE_W*MAX_IMAGE_H*3] = {0};			//Ô­Í¼
BYTE bmp_buf1[MAX_IMAGE_W*MAX_IMAGE_H*3] = {0};			//ÑØYÖá¾µÏñ
BYTE bmp_buf2[MAX_IMAGE_W*MAX_IMAGE_H*3] = {0};			//ÑØXÖá¾µÏñ
BYTE bmp_buf3[MAX_IMAGE_W*MAX_IMAGE_H*3] = {0};			//ÑØX¡¢YÖá¾µÏñ
BYTE bmp_buf_y[MAX_IMAGE_W*MAX_IMAGE_H] = {0};			//»Ò¶ÈÍ¼Ïñ
BYTE bmp_buf_ys[MAX_IMAGE_W*MAX_IMAGE_H] = {0};			//Ñ¹ËõÍ¼Ïñ
BYTE bmp_buf_ys1[MAX_IMAGE_W*MAX_IMAGE_H] = {0};		//XÑ¹ËõÍ¼Ïñ
BYTE bmp_buf_jc[MAX_IMAGE_W*MAX_IMAGE_H] = {0};			//è¦´Ã¼ì²âÍ¼Ïñ
BYTE bmp_buf_gl[MAX_IMAGE_W*MAX_IMAGE_H*3] ={0};
BYTE bmp_buf_jt[MAX_IMAGE_W*MAX_IMAGE_H*3] ={0};
BYTE bmp_buf_jtyt[MAX_IMAGE_W*MAX_IMAGE_H*3] ={0};

int img_w=0;
int img_h = 0;
int img_ch = 0;

int img_wh ;
int img_hh ;

int img_whys = 0;
int img_hhys = 0;

ai_img::Output img_out;
ai_img::Convert imgT;

int imgx=0;
int imgy=0;
int imgw=0;
int imgh=0;


int x_buf=6;
int k_buf=50;											//ÓÃÓÚ´æ´¢ÏñËØÏà¹Ø



BYTE bmp_kt[MAX_IMAGE_W*MAX_IMAGE_H*3] = {0};					//¿ÙÍ¼
int sizex=1;
int sizey=1;


HMENU hmenu;
HANDLE unique_mutex;

//??????????????????????????

HINSTANCE appInstance;
HWND hwnd[100] = {NULL};



int menu_21 =0;
int menu_221=0;
int menu_223=0;
int menu_224=0;
int menu_225=0;
int menu_226=0;
int menu_227=0;
int menu_228=0;

int open_bmp=0;
int hdh_bmp	=0;
int ys_bmp	=0;
int jc_bmp	=0;
int gl_bmp	=0;


int x_bmp	=0;
int y_bmp	=0;
int xy_bmp	=0;


int close_pattern_x;
int close_pattern_y;

int bmp_save=0;

HDC memdc;
RECT rt;
RECT rect;

int median(int x1, int x2, int x3){
	int i;
	i=max(min(x1,x2),min(x2,x3));
	return i;
}


void jianche(BYTE *buf,int w,int h,int xsc,int xsk){
	int xw=xsk/2;
	int zuo,you,zh;
	int ml,mr;
	int m;
	for(int i=0;i<h;i++){
		for(int j=0;j<w;j++){
			zh=i*w+j;
			zuo=max((i*w),(zh-xw));
			you=min((i*w+w-1),(zh+xw));
			if(i!=0&&i!=(h-1)){
				ml=median(buf[zuo-w],buf[zuo],buf[zuo+w]);
				mr=median(buf[you-w],buf[you],buf[you+w]);
			}
			else{
				ml=buf[zuo];
				mr=buf[you];
			}
			m=abs((ml+mr)/2-buf[zh]);
			if(m>xsc)
				bmp_buf_jc[zh]=0;
			else
				bmp_buf_jc[zh]=255;
			//if(m<xsc)
			//	bmp_buf_jc[zh]=0;
			//else
			//	bmp_buf_jc[zh]=255;
		}
	}
}

void jietuyt(BYTE *buf,int x,int y,int w,int h,int iw,int ih){
	int n,m;
	for(n=0;n<ih;n++){
		for(m=0;m<iw;m++){
			bmp_buf_jtyt[3*(n*iw+m)]=buf[3*(n*iw+m)];
			bmp_buf_jtyt[3*(n*iw+m)+1]=buf[3*(n*iw+m)+1];
			bmp_buf_jtyt[3*(n*iw+m)+2]=buf[3*(n*iw+m)+2];
		}
	}
	//**********************************************************************Ê®×ÖÏß
	n=ih/2;
	for(m=0;m<iw;m++){
		bmp_buf_jtyt[3*(n*iw+m)]=147;
		bmp_buf_jtyt[3*(n*iw+m)+1]=20;
		bmp_buf_jtyt[3*(n*iw+m)+2]=255;
	}
	n--;
	for(m=0;m<iw;m++){
		bmp_buf_jtyt[3*(n*iw+m)]=147;
		bmp_buf_jtyt[3*(n*iw+m)+1]=20;
		bmp_buf_jtyt[3*(n*iw+m)+2]=255;
	}
	m=iw/2;
	for(n=0;n<ih;n++){
		bmp_buf_jtyt[3*(n*iw+m)]=147;
		bmp_buf_jtyt[3*(n*iw+m)+1]=20;
		bmp_buf_jtyt[3*(n*iw+m)+2]=255;
		
	}
	m--;
	for(n=0;n<ih;n++){
		bmp_buf_jtyt[3*(n*iw+m)]=147;
		bmp_buf_jtyt[3*(n*iw+m)+1]=20;
		bmp_buf_jtyt[3*(n*iw+m)+2]=255;
	}
	//*******************************************************************¿ò
	n=y;
	for(m=x;m<x+w;m++){
		bmp_buf_jtyt[3*(n*iw+m)]=255;
		bmp_buf_jtyt[3*(n*iw+m)+1]=0;
		bmp_buf_jtyt[3*(n*iw+m)+2]=0;
	}
	n--;
	for(m=x;m<x+w;m++){
		bmp_buf_jtyt[3*(n*iw+m)]=255;
		bmp_buf_jtyt[3*(n*iw+m)+1]=0;
		bmp_buf_jtyt[3*(n*iw+m)+2]=0;
	}
	n=y+h;
	for(m=x;m<x+w;m++){
		bmp_buf_jtyt[3*(n*iw+m)]=255;
		bmp_buf_jtyt[3*(n*iw+m)+1]=0;
		bmp_buf_jtyt[3*(n*iw+m)+2]=0;
	}
	n--;
	for(m=x;m<x+w;m++){
		bmp_buf_jtyt[3*(n*iw+m)]=255;
		bmp_buf_jtyt[3*(n*iw+m)+1]=0;
		bmp_buf_jtyt[3*(n*iw+m)+2]=0;
	}
	m=x;
	for(n=y;n<y+h;n++){
		bmp_buf_jtyt[3*(n*iw+m)]=255;
		bmp_buf_jtyt[3*(n*iw+m)+1]=0;
		bmp_buf_jtyt[3*(n*iw+m)+2]=0;

	}
	m--;
	for(n=y;n<y+h;n++){
		bmp_buf_jtyt[3*(n*iw+m)]=255;
		bmp_buf_jtyt[3*(n*iw+m)+1]=0;
		bmp_buf_jtyt[3*(n*iw+m)+2]=0;
	}
	m=x+w;
	for(n=y;n<y+h;n++){
		bmp_buf_jtyt[3*(n*iw+m)]=255;
		bmp_buf_jtyt[3*(n*iw+m)+1]=0;
		bmp_buf_jtyt[3*(n*iw+m)+2]=0;

	}
	m--;
	for(n=y;n<y+h;n++){
		bmp_buf_jtyt[3*(n*iw+m)]=255;
		bmp_buf_jtyt[3*(n*iw+m)+1]=0;
		bmp_buf_jtyt[3*(n*iw+m)+2]=0;
	}
	//*************************************************************************¿òÊ®×ÖÏß
	n=y+h/2;
	for(m=x;m<x+w;m++){
		bmp_buf_jtyt[3*(n*iw+m)]=0;
		bmp_buf_jtyt[3*(n*iw+m)+1]=140;
		bmp_buf_jtyt[3*(n*iw+m)+2]=255;
	}
	n--;
	for(m=x;m<x+w;m++){
		bmp_buf_jtyt[3*(n*iw+m)]=0;
		bmp_buf_jtyt[3*(n*iw+m)+1]=140;
		bmp_buf_jtyt[3*(n*iw+m)+2]=255;
	}
	m=x+w/2;
	for(n=y;n<y+h;n++){
		bmp_buf_jtyt[3*(n*iw+m)]=0;
		bmp_buf_jtyt[3*(n*iw+m)+1]=140;
		bmp_buf_jtyt[3*(n*iw+m)+2]=255;

	}
	m--;
	for(n=y;n<y+h;n++){
		bmp_buf_jtyt[3*(n*iw+m)]=0;
		bmp_buf_jtyt[3*(n*iw+m)+1]=140;
		bmp_buf_jtyt[3*(n*iw+m)+2]=255;
	}
}

void jietu(BYTE *buf,int x,int y,int w,int h){
	int n,m;
	for(n=0;n<h;n++){
		for(m=0;m<w;m++){
			bmp_buf_jt[3*(n*w+m)]=buf[3*(img_w*(y+n)+x+m)];
			bmp_buf_jt[3*(n*w+m)+1]=buf[3*(img_w*(y+n)+x+m)+1];
			bmp_buf_jt[3*(n*w+m)+2]=buf[3*(img_w*(y+n)+x+m)+2];
		}
	}
	/*for(n=0;n<(640*480*3);n++)
		bmp_buf_jt[n]=buf[n];*/
}

void draw_line(HDC memdc,int x,int y,RECT rt){
//	HFONT hfont	 = CreateFont(50, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "ºÚÌå");  //13M camera
	HFONT hfont	 = CreateFont(24, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "ºÚÌå");  //2M camera
	HFONT old_font = (HFONT)SelectObject(memdc, hfont);
	HPEN pen		= CreatePen(PS_SOLID, 2, COLOR_DEEP_PINK);
	HPEN old_pen = (HPEN)SelectObject(memdc, pen);
	SetTextColor(memdc, COLOR_BLUE);
	int nw,nh;
	nw=rt.right-rt.left;
	nh=rt.bottom-rt.top;

	ai_img::Output::draw_crossline(memdc,x+nw/2,y+nh/2,nw-2,nh-2);
	/*ai_img::Output::draw_crossline(memdc, mysbw.search_rt[i].left+mysbw.search_w[select_num]/2, 
		mysbw.search_rt[i].top+mysbw.search_h[select_num]/2, mysbw.search_w[select_num], 
		mysbw.search_h[select_num]);*/
	char buffer[100] = {0};//
	sprintf(buffer,"#1");

	TextOut(memdc,x,y,buffer,strlen(buffer));

	DeleteObject(hfont);
	DeleteObject(pen);
}

void draw_reg_mark_chart(HDC memdc,int x,int y,int w,int h)
{

	HPEN pen		= CreatePen(PS_SOLID, 2, COLOR_BLUE);
	HPEN old_pen = (HPEN)SelectObject(memdc, pen);

	//if(rt.left!=0)
	SelectObject(memdc,GetStockObject(NULL_BRUSH));//ÉèÖÃ±³¾°Í¸Ã÷
	//Rectangle(memdc,rt.left, rt.top, rt.right, rt.bottom);
	Rectangle(memdc,x,y,x+w,y+h);
	DeleteObject(pen);
	pen		= CreatePen(PS_SOLID, 2, COLOR_DARK_ORANGE);
	old_pen = (HPEN)SelectObject(memdc, pen);
	ai_img::Output::draw_crossline(memdc,x+w/2,y+h/2,w,h);
	DeleteObject(pen);
}

int yasuo(BYTE *buf,int w,int h,int sx,int sy,int sw,int sh){
	int su=0;
	for(int i=0;i<h;i++){
		for(int j=0;j<sw;j++){
			su=0;
			for(int n=0;n<sx;n++)
				su=su+buf[i*w+j*sx+n];
			bmp_buf_ys1[i*sw+j]=su/sx;
		}
	}
	for(int i=0;i<sw;i++){
		for(int j=0;j<sh;j++){
			su=0;
			for(int n=0;n<sy;n++)
				su=su+bmp_buf_ys1[(j*sy+n)*sw+i];
			bmp_buf_ys[j*sw+i]=su/sy;
		}
	}


	return 0;
}

int koutu(BYTE *buf,int w,int h,int r,int b,int g){
	for (int i=0;i<h;i++)
	{
		for (int j=0;j<w;j++)
		{
			bmp_kt[3*(i*w+w-j-1)]=buf[3*(i*w+w-j-1)];
			bmp_kt[3*(i*w+w-j-1)+1]=buf[3*(i*w+w-j-1)+1];
			bmp_kt[3*(i*w+w-j-1)+2]=buf[3*(i*w+w-j-1)+2];

			if (buf[3*(i*w+w-j-1)]>r&&buf[3*(i*w+w-j-1)+1]>b&&buf[3*(i*w+w-j-1)+2]>g)
			{
				bmp_kt[3*(i*w+w-j-1)] = 255;
				bmp_kt[3*(i*w+w-j-1)+1] = 255;
				bmp_kt[3*(i*w+w-j-1)+2] = 255;
			}
			/*else{
				bmp_kt[3*(i*w+w-j-1)] = 0;
				bmp_kt[3*(i*w+w-j-1)+1] = 0;
				bmp_kt[3*(i*w+w-j-1)+2] = 0;
			}*/
		}
	}
	return 0;
}
int koutu2(BYTE *buf,int w,int h){
	int r,g,b;
	for (int i=0;i<h;i++)
	{
		for (int j=0;j<w;j++)
		{
			bmp_kt[3*(i*w+j)]=buf[3*(i*w+j)];
			bmp_kt[3*(i*w+j)+1]=buf[3*(i*w+j)+1];
			bmp_kt[3*(i*w+j)+2]=buf[3*(i*w+j)+2];
			b=buf[3*(i*w+j)];//********************************************************/////////////ÏñËØ´æ´¢Ë³Ðò
			g=buf[3*(i*w+j)+1];
			r=buf[3*(i*w+j)+2];
			if (r>(g+b))					//ºìÉ«
			{
				bmp_kt[3*(i*w+j)] = 255;
				bmp_kt[3*(i*w+j)+1] = 255;
				bmp_kt[3*(i*w+j)+2] = 255;
			}
			if(r>b&&g>b&&r>170&&g>170){			//»ÆÉ«
				bmp_kt[3*(i*w+j)] = 255;
				bmp_kt[3*(i*w+j)+1] = 255;
				bmp_kt[3*(i*w+j)+2] = 255;
			}
			//if(g>(r+b)){					//ÂÌÉ«
			if(g>150&&b<130&&r<110){					//ÂÌÉ«
				bmp_kt[3*(i*w+j)] = 255;
				bmp_kt[3*(i*w+j)+1] = 255;
				bmp_kt[3*(i*w+j)+2] = 255;
			}
			/*else{
				bmp_kt[3*(i*w+w-j-1)] = 0;
				bmp_kt[3*(i*w+w-j-1)+1] = 0;
				bmp_kt[3*(i*w+w-j-1)+2] = 0;
			}*/
		}
	}
	return 0;
}

int mirror(BYTE *buf,int w,int h)		//mode=0,XÏò mode=1,YÏò mode=2,XYÏò
{
	//if(mode==0){
	for (int i=0;i<h;i++)
	{
		for (int j=0;j<w;j++)
		{
			bmp_buf2[3*(i*w+w-j-1)] = buf[3*(i*w+j)];
			bmp_buf2[3*(i*w+w-j-1)+1] = buf[3*(i*w+j)+1];
			bmp_buf2[3*(i*w+w-j-1)+2] = buf[3*(i*w+j)+2];
		}
	}
	//}
	//else if (mode==1)
	//{
	for (int i=0;i<h;i++)
	{
		for (int j=0;j<w;j++)
		{
			bmp_buf1[3*((h-i)*w+j)] = buf[3*(i*w+j)];
			bmp_buf1[3*((h-i)*w+j)+1] = buf[3*(i*w+j)+1];
			bmp_buf1[3*((h-i)*w+j)+2] = buf[3*(i*w+j)+2];
		}
	}
	//}
	//else if(mode==2)
	//{
	for (int i=0;i<h;i++)
	{
		for (int j=0;j<w;j++)
		{
			bmp_buf1[3*((h-i)*w+j)] = buf[3*(i*w+j)];
			bmp_buf1[3*((h-i)*w+j)+1] = buf[3*(i*w+j)+1];
			bmp_buf1[3*((h-i)*w+j)+2] = buf[3*(i*w+j)+2];
		}
	}
	for (int i=0;i<h;i++)
	{
		for (int j=0;j<w;j++)
		{
			bmp_buf3[3*(i*w+w-j-1)] = bmp_buf1[3*(i*w+j)];
			bmp_buf3[3*(i*w+w-j-1)+1] = bmp_buf1[3*(i*w+j)+1];
			bmp_buf3[3*(i*w+w-j-1)+2] = bmp_buf1[3*(i*w+j)+2];
		}
	}
	//}

	return 0;
}

int guolv(const BYTE *source, BYTE *dest, int cx, int cy, int pattern_x, int pattern_y, int close_val)
{
	int sum = 0;
	int sub_sum = 0;
	int avg = 0, sub_avg = 0;
	int w= pattern_x;
	int h= pattern_y;

	BYTE *dest_t = new BYTE[cx*cy];

	memset(dest_t, 255, cx*cy);
	int min_val = 255;

	int pixel_count = 0;

	for (int j=h; j<cy-h; j+=h){
		for (int i=w; i<cx-w; i+=w){

			pixel_count = 0;
			for (int n=j; n<j+h*2; n++){
				for (int m=i; m<i+w*2; m++){
					if (source[n*cx+m] <= 1) pixel_count++;
				}
			}

			if (pixel_count>close_val){
				for (int n=j; n<j+h*2; n++){
					for (int m=i; m<i+w*2; m++){
						dest_t[n*cx+m] =source[n*cx+m];
					}
				}
			}
		}
	}

	sum = 0;
	for (int i=0; i<cx*cy; i++){
		if (dest_t[i] == 255){
			dest[i*3] = 255;
			dest[i*3+1] = 255;
			dest[i*3+2] = 255;
		}
		else{
			dest[i*3] = 0;
			dest[i*3+1] = 0;
			dest[i*3+2] = 255;
		}
		sum += dest[i*3];
	}
	avg = sum/(cx*cy);
	delete [] dest_t;

	return min_val;
}


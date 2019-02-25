//********************************************************
//图片相关的定义

#ifndef _tuxiang_H_
#define _tuxiang_H_

#ifndef _tuxiang_
#define _tuxiang_ extern "C" __declspec(dllimport)
#endif
extern char dir2[MAX_PATH];					//图片地址相关
extern char bmp_path[MAX_PATH];

#define MAX_IMAGE_W		5500
#define MAX_IMAGE_H		4500

extern BYTE bmp_buf[MAX_IMAGE_W*MAX_IMAGE_H*3] ;		//原图
extern BYTE bmp_buf1[MAX_IMAGE_W*MAX_IMAGE_H*3] ;		//沿Y轴镜像
extern BYTE bmp_buf2[MAX_IMAGE_W*MAX_IMAGE_H*3] ;		//沿X轴镜像
extern BYTE bmp_buf3[MAX_IMAGE_W*MAX_IMAGE_H*3] ;		//沿X、Y轴镜像
extern BYTE bmp_buf_y[MAX_IMAGE_W*MAX_IMAGE_H] ;		//灰度图像
extern BYTE bmp_buf_ys[MAX_IMAGE_W*MAX_IMAGE_H] ;		//压缩图像
extern BYTE bmp_buf_ys1[MAX_IMAGE_W*MAX_IMAGE_H] ;		//X压缩图像
extern BYTE bmp_buf_jc[MAX_IMAGE_W*MAX_IMAGE_H] ;		//瑕疵检测图像
extern BYTE bmp_buf_gl[MAX_IMAGE_W*MAX_IMAGE_H*3] ;		//过滤图像
extern BYTE bmp_buf_jt[MAX_IMAGE_W*MAX_IMAGE_H*3] ;		//截图图像
extern BYTE bmp_buf_jtyt[MAX_IMAGE_W*MAX_IMAGE_H*3] ;	//截图原图图像



extern int img_w ;
extern int img_h ;
extern int img_ch ;

extern int img_wh ;
extern int img_hh ;

extern int img_whys ;
extern int img_hhys ;

extern ai_img::Output img_out;
extern ai_img::Convert imgT;


extern int x_buf,k_buf;											//用于存储像素相关



extern BYTE bmp_kt[MAX_IMAGE_W*MAX_IMAGE_H*3] ;					//抠图
extern int sizex;
extern int sizey;


//***************************************************************

extern BOOL CALLBACK menuDlg_2_1 (HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK menuDlg_2_21(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK menuDlg_2_22(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK menuDlg_2_23(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK menuDlg_2_24(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK menuDlg_2_25(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK menuDlg_2_26(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK menuDlg_2_27(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
extern BOOL CALLBACK menuDlg_2_28(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
//创建菜单相关

extern HMENU hmenu;
extern HANDLE unique_mutex;

//??????????????????????????

extern HINSTANCE appInstance;
extern HWND hwnd[100];
#define HWND_APP				hwnd[0]
#define HWND_MAIN				hwnd[1]
#define HWND_2_1				hwnd[2]
#define HWND_2_21				hwnd[3]
#define HWND_2_22				hwnd[4]
#define HWND_2_23				hwnd[5]
#define HWND_2_24				hwnd[6]
#define HWND_2_25				hwnd[7]
#define HWND_2_26				hwnd[8]
#define HWND_2_27				hwnd[9]
#define HWND_2_28				hwnd[10]
typedef struct _tagDlgInfo
{
	int mdlg_x;
	int mdlg_y;
	int mdlg_extend;

	int show_2_1;
	int show_2_21;
	int show_2_22;
	int show_2_23;
	int show_2_24;
	int show_2_25;
	int show_2_26;
	int show_2_27;
	int show_2_28;
}DLGINFO;
static DLGINFO DlgStatus;

extern int menu_21 ;		//确保窗口唯一
extern int menu_221;
extern int menu_223;
extern int menu_224;
extern int menu_225;
extern int menu_226;
extern int menu_227;
extern int menu_228;
extern int open_bmp;		//功能保护
extern int x_bmp;
extern int y_bmp;
extern int xy_bmp;
extern int hdh_bmp;
extern int ys_bmp;
extern int jc_bmp;
extern int gl_bmp;

extern int imgx,imgy,imgw,imgh;

extern int close_pattern_x;
extern int close_pattern_y ;

extern int bmp_save;		//保存当前预览

extern HDC memdc;
extern RECT rt;
extern RECT rect;
# define SLANTED_BW_NUM 5;
void draw_reg_mark_chart(HDC memdc,int x,int y,int w,int h);//画线画框
void draw_line(HDC memdc,int x,int y,RECT rt);

void jietu(BYTE *buf,int x,int y,int w,int h);					//截取框中图片
void jietuyt(BYTE *buf,int x,int y,int w,int h,int iw,int ih);	//截取位置原图

//***********************************************************
//过滤函数
int guolv(const BYTE *source, BYTE *dest, int cx, int cy, int pattern_x, int pattern_y, int close_val);
//镜像函数
int mirror(BYTE *buf,int w,int h);
//抠图函数
int koutu(BYTE *buf,int w,int h,int r,int b,int g);
int koutu2(BYTE *buf,int w,int h);
//收缩函数
int yasuo(BYTE *buf,int w,int h,int sx,int sy,int sw,int sh);
//瑕疵检测函数
int median(int x1, int x2, int x3);
void jianche(BYTE *buf,int w,int h,int xsc,int xsk);

#endif

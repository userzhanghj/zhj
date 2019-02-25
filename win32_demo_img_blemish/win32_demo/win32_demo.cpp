#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#include <direct.h>
#include "s_win.h"
#include "resource.h"
#include "s_img.h"
#include "tuxiang.h"
#include "creatwnd.h"
#include "qiuhe.h"

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#pragma comment(lib,"s_win.lib")
#pragma comment(lib, "s_img.lib")

HWND happ = NULL;


class MainDlg:public ai_win::Dlg
{
public:
	virtual BOOL init(HWND hdlg, HWND hfocus, LPARAM linit);
	virtual void command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify);
	virtual void hscroll(HWND hdlg, HWND hctrl, UINT code, int pos);
	virtual void close(HWND hdlg);
private:
	char file_path[MAX_PATH];
};

//*********************************************************************************************

MainDlg main;
BOOL CALLBACK main_dlg_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_DLGMSG(hdlg, WM_INITDIALOG, main.init	);
		HANDLE_DLGMSG(hdlg, WM_COMMAND,	   main.command	);
		HANDLE_DLGMSG(hdlg, WM_HSCROLL,	   main.hscroll );
		HANDLE_DLGMSG(hdlg, WM_CLOSE,	   main.close	);
		break;
	}
	return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	/*if (DialogBox(hInstance, (LPCTSTR)IDD_MAIN, NULL, main_dlg_process) == -1){//bug
		MessageBox(NULL, TEXT("Program init fail."), TEXT("Warning"), 0);
		return 1;
	}

	return 0;*/
	//*************************************************************************************
	static char app_name[] = "Image_Scenery";
	MSG msg;
	WNDCLASSEX wnd_class;

	wnd_class.cbSize		= sizeof(WNDCLASSEX);
	wnd_class.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS ;
	wnd_class.lpfnWndProc   = WndProc;
	wnd_class.cbClsExtra    = 0;
	wnd_class.cbWndExtra    = 0;
	wnd_class.hInstance     = hInstance;
	wnd_class.hIcon         = LoadIcon(hInstance, (LPCTSTR)IDI_TEST);
	wnd_class.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wnd_class.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wnd_class.lpszClassName = app_name;
	wnd_class.lpszMenuName  = NULL;
	wnd_class.hIconSm       = LoadIcon(wnd_class.hInstance, (LPCTSTR)IDI_TEST);

	HWND_APP = CreateWindowEx(WS_EX_ACCEPTFILES, 
		app_name, "Image_Scenery", 
		WS_THICKFRAME ,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL, 
		NULL, 
		hInstance, 
		NULL);

	HWND_MAIN = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, (DLGPROC)main_dlg_process);
	// 	if (DialogBox(hInstance, (LPCTSTR)IDD_MAIN, NULL, main_dlg_process) == -1){//bug
	// 		MessageBox(NULL, TEXT("Program init fail."), TEXT("Warning"), 0);
	// 		return 1;
	// 	}
	appInstance = hInstance;
	ShowWindow (HWND_MAIN, nShowCmd);
	UpdateWindow (HWND_MAIN);

	while (GetMessage (&msg, NULL, 0, 0))
	{
		if(!IsDialogMessage(HWND_APP, &msg)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;

}
//¥”Œƒº˛÷–∂¡»°≤Œ ˝//≥ı ºªØ
BOOL MainDlg::init(HWND hdlg, HWND hfocus, LPARAM linit)				
{
	happ = hdlg;
	RECT rt;
	GetClientRect(hdlg, &rt);
	MoveWindow(hdlg, 100, 100, rt.right, rt.bottom, FALSE);
	ai_win::set_icon(hdlg, IDI_TEST, IDI_TEST);
	//**********************************************************************
	
	//¥¥Ω®≤Àµ•œ‡πÿ
	MENUINFO mi;
	hmenu = ai_win::create_menu(hdlg, IDR_MENU);
	mi.fMask   = MIM_BACKGROUND;
	mi.hbrBack = CreateSolidBrush(RGB(255, 10, 10));
	SetMenuInfo(hmenu, &mi);
	DrawMenuBar(hdlg);
	//*******************************************************************
	//◊Ûœ¬Ω«œ‘ æœ‡πÿ
	int sub_width[6] = {90, 150, 270, 390, 470, -1};
	HWND versionStatus = ai_win::create_statusbar(hdlg, sub_width, sizeof(sub_width)/sizeof(int), 0);
	char version[20] = {0};
	ai_win::get_version(NULL, TRUE, version);

	char status_string1[100] = {0};
	sprintf(status_string1, " %s", version);

	SendMessage(versionStatus, SB_SETTEXT, 0, (LPARAM)" @2018");
	SendMessage(versionStatus, SB_SETTEXT, 1, (LPARAM)status_string1);
	SendMessage(versionStatus, SB_SETTEXT, 2, (LPARAM)"Image Files");
	SendMessage(versionStatus, SB_SETTEXT, 3, (LPARAM)"shiyan");
	SendMessage(versionStatus, SB_SETTEXT, 4, (LPARAM)"");
	SendMessage(versionStatus, SB_SETTEXT, 5, (LPARAM)"");

	//*******************************************************************
	
	ai_win::get_current_directory(dir);		//ªÒ»°Œƒº˛Œª÷√
	sprintf(config_path,"%s\\config.ini",dir);

	if(!imgT.init_lib()) return FALSE;

	//****************************************************
	//ai_win::read_integer_key();				
	//ai_win::write_integer_key(config_path,"calculate","d1",d1);
	close_pattern_x = ai_win::read_integer_key(config_path, "blemish", "close_pattern_x");
	close_pattern_y = ai_win::read_integer_key(config_path, "blemish", "close_pattern_y");
	//******************************************************************
	SET_SLIDER_RANGE(hdlg,IDC_SLX,0,600);
	SET_SLIDER_RANGE(hdlg,IDC_SLY,0,600);
	SET_SLIDER_RANGE(hdlg,IDC_SLW,0,200);
	SET_SLIDER_RANGE(hdlg,IDC_SLH,0,200);

	//*******************
	//RECT dlg_rect;
	//GetWindowRect(hdlg, &dlg_rect);

	//	MoveWindow(hdlg, dlg_rect.left, dlg_rect.top, dlg_rect.right-dlg_rect.left, 0, TRUE);
	return TRUE;
}

bool is_open_win= true;
//ª·“ª÷±—≠ª∑¥À∫Ø ˝//∞¥≈•µ„ª˜œ‡πÿ≤Ÿ◊˜
void MainDlg::command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify)		
{
	char path[MAX_PATH] = {0};
	//int gc;
	int val;
	switch (id){
	case ID_OPEN_WIN:
	RECT dlg_rect;
	GetWindowRect(hdlg, &dlg_rect);
		if (is_open_win == TRUE)
		{
			is_open_win = FALSE;
			MoveWindow(hdlg, dlg_rect.left, dlg_rect.top, dlg_rect.right-dlg_rect.left, 60, TRUE);
		}
		else
		{
			is_open_win = TRUE;
			MoveWindow(hdlg, dlg_rect.left, dlg_rect.top, dlg_rect.right-dlg_rect.left, 400, TRUE);
		}

		break;

	case ID_2_1:
		if(!menu_21){
			menu_21=1;
			DlgStatus.show_2_1 = TRUE;
			CheckMenuItem(hmenu, id, DlgStatus.show_2_1?MF_CHECKED:MF_UNCHECKED);
			HWND_2_1 = CreateDialog((HINSTANCE)GetWindowLong(hdlg, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_2_1),
				hdlg, (DLGPROC)menuDlg_2_1);
			ShowWindow(HWND_2_1,DlgStatus.show_2_1?SW_NORMAL:SW_HIDE);
			
		}
		break;

	case ID_2_21:
		if(!menu_221){
			menu_221=1;
			DlgStatus.show_2_21 = TRUE;
			CheckMenuItem(hmenu, id, DlgStatus.show_2_21?MF_CHECKED:MF_UNCHECKED);
			HWND_2_21 = CreateDialog((HINSTANCE)GetWindowLong(hdlg, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_2_21),
				hdlg, (DLGPROC)menuDlg_2_21);
			ShowWindow(HWND_2_21,DlgStatus.show_2_21?SW_NORMAL:SW_HIDE);
		}
		break;
	case IDC_OPEN:																	//Õº∆¨æµœÒ
		if(open_bmp){
			mirror(bmp_buf, img_w, img_h);											//X÷·æµœÒ
			img_out.clear_bmp_stream();											
			//X÷·æµœÒ
			img_out.display(GetDlgItem(hdlg,IDC_SHOWN),bmp_buf2, img_w, img_h, img_ch);
			x_bmp=1;
			bmp_save=1;
		}
		if(menu_221&&open_bmp){
			img_out.clear_bmp_stream();											
			//X÷·æµœÒ
			img_out.display(GetDlgItem(HWND_2_21,IDC_SHOW2),bmp_buf2, img_w, img_h, img_ch);
		}
		break;
	case IDC_OPEN3:																	//Õº∆¨æµœÒ
		if(open_bmp){
			mirror(bmp_buf, img_w, img_h);											
			img_out.clear_bmp_stream();			
			img_out.display(GetDlgItem(hdlg,IDC_SHOWN),bmp_buf1, img_w, img_h, img_ch);
			y_bmp=1;
			bmp_save=2;
		}
		if(menu_221&&open_bmp){
			mirror(bmp_buf, img_w, img_h);											//Y÷·æµœÒ
			img_out.clear_bmp_stream();											
			//Y÷·æµœÒ
			img_out.display(GetDlgItem(HWND_2_21,IDC_SHOW2),bmp_buf1, img_w, img_h, img_ch);
		}
		break;
	case IDC_OPEN2:																	//Õº∆¨æµœÒ
		if(open_bmp){
			mirror(bmp_buf, img_w, img_h);											
			img_out.clear_bmp_stream();					
			img_out.display(GetDlgItem(hdlg,IDC_SHOWN),bmp_buf3, img_w, img_h, img_ch);
			xy_bmp=1;
			bmp_save=3;
		}
		if(menu_221&&open_bmp){
			mirror(bmp_buf, img_w, img_h);											//X°¢Y÷·æµœÒ
			img_out.clear_bmp_stream();											
			//X°¢Y÷·æµœÒ
			img_out.display(GetDlgItem(HWND_2_21,IDC_SHOW2),bmp_buf3, img_w, img_h, img_ch);
		}
		break;
	case ID_2_23:
		if(!menu_223){
			menu_223=1;
			DlgStatus.show_2_23 = TRUE;
			CheckMenuItem(hmenu, id, DlgStatus.show_2_23?MF_CHECKED:MF_UNCHECKED);
			HWND_2_23= CreateDialog((HINSTANCE)GetWindowLong(hdlg, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_2_23),
				hdlg, (DLGPROC)menuDlg_2_23);
			ShowWindow(HWND_2_23,DlgStatus.show_2_23?SW_NORMAL:SW_HIDE);
		}
		break;
	case IDC_HDH:																//Õº∆¨ª“∂»ªØ
		if(open_bmp){
			imgT.bmp24_to_y8(bmp_buf,img_w,img_h,NULL,bmp_buf_y);
			//ª“∂»ªØÕºœÒ
			//œ‘ æª“∂»ÕºœÒ ±£¨img_ch=1
			img_out.clear_bmp_stream();
			img_out.display(GetDlgItem(hdlg,IDC_SHOWN),bmp_buf_y, img_w, img_h, 1);	//œ‘ æª“∂»ÕºœÒ
			hdh_bmp=1;
			bmp_save=4;
		}
		if(menu_223&&open_bmp){
			img_out.clear_bmp_stream();
			img_out.display(GetDlgItem(HWND_2_23,IDC_SHOW6),bmp_buf_y, img_w, img_h, 1);	//œ‘ æª“∂»ÕºœÒ
		}
		break;

	case ID_2_24:
		if(!menu_224){
			menu_224=1;
			DlgStatus.show_2_24 = TRUE;
			CheckMenuItem(hmenu, id, DlgStatus.show_2_24?MF_CHECKED:MF_UNCHECKED);
			HWND_2_24= CreateDialog((HINSTANCE)GetWindowLong(hdlg, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_2_24),
				hdlg, (DLGPROC)menuDlg_2_24);
			ShowWindow(HWND_2_24,DlgStatus.show_2_24?SW_NORMAL:SW_HIDE);
		}
		break;
	case IDC_YSTX:															//ÕºœÒ—πÀı
		if(menu_224&&hdh_bmp){
			if(GET_CHECKER(HWND_2_24,IDC_CX))
				sizex=GetDlgItemInt(HWND_2_24,IDC_XSX,0,FALSE);
			if(GET_CHECKER(HWND_2_24,IDC_CY))
				sizey=GetDlgItemInt(HWND_2_24,IDC_XSY,0,FALSE);
			img_whys=img_w/sizex;
			img_hhys=img_h/sizey;
			yasuo(bmp_buf_y,img_w,img_h,sizex,sizey,img_whys,img_hhys);
			img_out.clear_bmp_stream();
			img_out.display(GetDlgItem(HWND_2_24,IDC_SHOW7),bmp_buf_ys,img_w/sizex,img_h/sizey, 1);	//œ‘ æÕºœÒ
			ys_bmp=1;
			bmp_save=5;
		}
		if(hdh_bmp){
			img_whys=img_w/sizex;
			img_hhys=img_h/sizey;
			yasuo(bmp_buf_y,img_w,img_h,sizex,sizey,img_whys,img_hhys);
			img_out.clear_bmp_stream();
			img_out.display(GetDlgItem(hdlg,IDC_SHOWN),bmp_buf_ys,img_w/sizex,img_h/sizey, 1);	//œ‘ æÕºœÒ
			ys_bmp=1;
		}
		break;
	case ID_2_25:
		if(!menu_225){
			menu_225=1;
			DlgStatus.show_2_25 = TRUE;
			CheckMenuItem(hmenu, id, DlgStatus.show_2_25?MF_CHECKED:MF_UNCHECKED);
			HWND_2_25= CreateDialog((HINSTANCE)GetWindowLong(hdlg, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_2_25),
				hdlg, (DLGPROC)menuDlg_2_25);
			ShowWindow(HWND_2_25,DlgStatus.show_2_25?SW_NORMAL:SW_HIDE);
		}
		break;
	case IDC_XCJC:
		if(menu_225&&ys_bmp){
			x_buf=GET_SLIDER_POS(HWND_2_25,IDC_XSC);
			k_buf=GET_SLIDER_POS(HWND_2_25,IDC_XSK);
			jianche(bmp_buf_ys,img_whys,img_hhys,x_buf,k_buf);
			img_out.clear_bmp_stream();
			img_out.display(GetDlgItem(HWND_2_25,IDC_SHOW8),bmp_buf_jc, img_whys, img_hhys, 1);
		}
		if(ys_bmp){
			jianche(bmp_buf_ys,img_whys,img_hhys,x_buf,k_buf);
			img_out.clear_bmp_stream();
			img_out.display(GetDlgItem(hdlg,IDC_SHOWN),bmp_buf_jc, img_whys, img_hhys, 1);
			jc_bmp=1;
			bmp_save=6;
		}
		break;
	case ID_2_26:
		if(!menu_226){
			menu_226=1;
			DlgStatus.show_2_26 = TRUE;
			CheckMenuItem(hmenu, id, DlgStatus.show_2_26?MF_CHECKED:MF_UNCHECKED);
			HWND_2_26= CreateDialog((HINSTANCE)GetWindowLong(hdlg, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_2_26),
				hdlg, (DLGPROC)menuDlg_2_26);
			ShowWindow(HWND_2_26,DlgStatus.show_2_26?SW_NORMAL:SW_HIDE);
		}
		break;
	case IDC_GL:
		if(menu_226&&jc_bmp){
			val=GET_SLIDER_POS(HWND_2_26,IDC_HK);
			guolv(bmp_buf_jc,bmp_buf_gl,img_whys, img_hhys,close_pattern_x,close_pattern_y,val);
			img_out.clear_bmp_stream();
			img_out.display(GetDlgItem(HWND_2_26,IDC_SHOW),bmp_buf_gl, img_whys, img_hhys, 3);
		}
		if(jc_bmp)
		{
			guolv(bmp_buf_jc,bmp_buf_gl,img_whys, img_hhys,close_pattern_x,close_pattern_y,20);
			img_out.clear_bmp_stream();
			img_out.display(GetDlgItem(hdlg,IDC_SHOWN),bmp_buf_gl, img_whys, img_hhys, 3);
			gl_bmp=1;
			bmp_save=7;
		}
		break;
	case ID_2_27:
		if(!menu_227){
			DlgStatus.show_2_27 = TRUE;
			CheckMenuItem(hmenu, id, DlgStatus.show_2_27?MF_CHECKED:MF_UNCHECKED);
			HWND_2_27= CreateDialog((HINSTANCE)GetWindowLong(hdlg, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_2_27),
				hdlg, (DLGPROC)menuDlg_2_27);
			ShowWindow(HWND_2_27,DlgStatus.show_2_27?SW_NORMAL:SW_HIDE);
		}
		break;
	case ID_2_28:
		if(!menu_228){
			menu_228=1;
			DlgStatus.show_2_28 = TRUE;
			CheckMenuItem(hmenu, id, DlgStatus.show_2_28?MF_CHECKED:MF_UNCHECKED);
			HWND_2_28= CreateDialog((HINSTANCE)GetWindowLong(hdlg, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_2_28),
				hdlg, (DLGPROC)menuDlg_2_28);
			ShowWindow(HWND_2_28,DlgStatus.show_2_28?SW_NORMAL:SW_HIDE);
		}
		break;
	case ID_1_1:
		ai_win::open_file(hdlg, path, FILE_TYPE_BMP_ONLY);							//ªÒ»°Õº∆¨µÿ÷∑path
		ai_img::read_bmp(path, bmp_buf, img_w, img_h, img_ch);
		img_out.clear_bmp_stream();
		//----------------------------------------------------------------------------------
		
		img_out.display(GetDlgItem(hdlg,IDC_SHOW1),bmp_buf, img_w, img_h, img_ch);	// ‰≥ˆÕº∆¨
		img_out.display(GetDlgItem(hdlg,IDC_SHOWN),bmp_buf, img_w, img_h, img_ch);
		open_bmp=1;
		bmp_save=0;
		break;

	case ID_1_2:
		//RBG∏Ò Ω
		if(x_bmp){
			sprintf(bmp_path,"%s\\pic\\jingxiangX.bmp",dir);
			ai_img::save_bmp24_image(bmp_path,bmp_buf2,img_w,img_h);//±£¥ÊXæµœÒÕº∆¨
		}
		if(y_bmp){
			sprintf(bmp_path,"%s\\pic\\jingxiangY.bmp",dir);
			ai_img::save_bmp24_image(bmp_path,bmp_buf1,img_w,img_h);//±£¥ÊYæµœÒÕº∆¨
		}
		if(xy_bmp){
			sprintf(bmp_path,"%s\\pic\\jingxiangXY.bmp",dir);
			ai_img::save_bmp24_image(bmp_path,bmp_buf3,img_w,img_h);//±£¥ÊX.YæµœÒÕº∆¨
		}

		//ª“∂»ÕºœÒ∏Ò Ω
		if(hdh_bmp){
			sprintf(bmp_path,"%s\\pic\\huidu.bmp",dir);
			ai_img::save_bmp8_image(bmp_path,bmp_buf_y,img_w,img_h);//±£¥Êª“∂»Õº∆¨
		}
		if(ys_bmp){
			sprintf(bmp_path,"%s\\pic\\yasuo.bmp",dir);
			ai_img::save_bmp8_image(bmp_path,bmp_buf_ys,img_whys,img_hhys);//±£¥Ê—πÀıÕº∆¨
		}
		if(jc_bmp){
			sprintf(bmp_path,"%s\\pic\\jianche.bmp",dir);
			ai_img::save_bmp8_image(bmp_path,bmp_buf_jc,img_whys,img_hhys);//±£¥ÊºÏ≤‚Õº∆¨
		}
		if(open_bmp){
			sprintf(bmp_path,"%s\\pic\\yuantu.bmp",dir);
			ai_img::save_bmp24_image(bmp_path,bmp_buf,img_w,img_h);//±£¥Ê‘≠Õº∆¨
		}
		if(gl_bmp){
			sprintf(bmp_path,"%s\\pic\\guolv.bmp",dir);
			ai_img::save_bmp24_image(bmp_path,bmp_buf_gl,img_whys,img_hhys);//±£¥Ê‘≠Õº∆¨
		}
		break;
	case IDC_SAVE:
		//RBG∏Ò Ω
		if(x_bmp&&bmp_save==1){
			sprintf(bmp_path,"%s\\pic\\jingxiangX.bmp",dir);
			ai_img::save_bmp24_image(bmp_path,bmp_buf2,img_w,img_h);//±£¥ÊXæµœÒÕº∆¨
		}
		if(y_bmp&&bmp_save==2){
			sprintf(bmp_path,"%s\\pic\\jingxiangY.bmp",dir);
			ai_img::save_bmp24_image(bmp_path,bmp_buf1,img_w,img_h);//±£¥ÊYæµœÒÕº∆¨
		}
		if(xy_bmp&&bmp_save==3){
			sprintf(bmp_path,"%s\\pic\\jingxiangXY.bmp",dir);
			ai_img::save_bmp24_image(bmp_path,bmp_buf3,img_w,img_h);//±£¥ÊX.YæµœÒÕº∆¨
		}

		//ª“∂»ÕºœÒ∏Ò Ω
		if(hdh_bmp&&bmp_save==4){
			sprintf(bmp_path,"%s\\pic\\huidu.bmp",dir);
			ai_img::save_bmp8_image(bmp_path,bmp_buf_y,img_w,img_h);//±£¥Êª“∂»Õº∆¨
		}
		if(ys_bmp&&bmp_save==5){
			sprintf(bmp_path,"%s\\pic\\yasuo.bmp",dir);
			ai_img::save_bmp8_image(bmp_path,bmp_buf_ys,img_whys,img_hhys);//±£¥Ê—πÀıÕº∆¨
		}
		if(jc_bmp&&bmp_save==6){
			sprintf(bmp_path,"%s\\pic\\jianche.bmp",dir);
			ai_img::save_bmp8_image(bmp_path,bmp_buf_jc,img_whys,img_hhys);//±£¥ÊºÏ≤‚Õº∆¨
		}
		if(open_bmp&&bmp_save==0){
			sprintf(bmp_path,"%s\\pic\\yuantu.bmp",dir);
			ai_img::save_bmp24_image(bmp_path,bmp_buf,img_w,img_h);//±£¥Ê‘≠Õº∆¨
		}
		if(gl_bmp&&bmp_save==7){
			sprintf(bmp_path,"%s\\pic\\guolv.bmp",dir);
			ai_img::save_bmp24_image(bmp_path,bmp_buf_gl,img_whys,img_hhys);//±£¥Ê‘≠Õº∆¨
		}
		break;
	}

	switch (code_notify)
	{
	}
}

void MainDlg::close(HWND hdlg){
	PostQuitMessage(0);
}

void MainDlg::hscroll(HWND hdlg, HWND hctrl, UINT code, int pos)			//ª¨øÈœ‡πÿ≤Ÿ◊˜
{
	if(open_bmp){
		GetClientRect(GetDlgItem(hdlg, IDC_SHOW1),&rect); 
		int x,y,w,h;
		int nx,ny,nw,nh;
		int rtw,rth;
		rtw=rect.right-rect.left;
		rth=rect.bottom-rect.top;
		//****************0,0,366,234
		imgx=GET_SLIDER_POS(hdlg,IDC_SLX);
		x=0;
		imgy=GET_SLIDER_POS(hdlg,IDC_SLY);
		y=0;
		imgw=GET_SLIDER_POS(hdlg,IDC_SLW);

		imgh=GET_SLIDER_POS(hdlg,IDC_SLH); 
		imgw=100+imgw;
		imgh=100+imgh;


		imgw=4*(imgw/4);
		imgx=min(img_w-imgw,imgx);
		imgy=min(img_h-imgh,imgy);
		nw=(rtw*imgw)/img_w;
		nh=(rth*imgh)/img_h;
		nx=(rtw*imgx)/img_w;
		ny=rth-((rth*imgy)/img_h)-nh;;
		//nx=max(0,min(nx,(rtw-nw)));
		//ny=max(min(ny,(rth-nh)),0);

		SetDlgItemInt(hdlg,IDC_DIT1,nx,FALSE);
		SetDlgItemInt(hdlg,IDC_DIT2,ny,FALSE);
		SetDlgItemInt(hdlg,IDC_DIT3,imgw,FALSE);
		SetDlgItemInt(hdlg,IDC_DIT4,imgh,FALSE);

		//InvalidateRect(hdlg,&rect,0);
		img_out.clear_bmp_stream();
		img_out.display(GetDlgItem(hdlg,IDC_SHOW1),bmp_buf, img_w, img_h, img_ch);	// ‰≥ˆÕº∆¨

		draw_line(GetDC(hdlg),x,y,rect);
		draw_reg_mark_chart(GetDC(hdlg),x+nx,y+ny,nw,nh);

		/*imgx=(nx*img_w)/rtw;
		imgy=rth-(ny*img_h)/rth;
		imgw=(nw*img_w)/rtw;
		imgh=(nh*img_h)/rth;*/
		jietu(bmp_buf,imgx,imgy,imgw,imgh);
		jietuyt(bmp_buf,imgx,imgy,imgw,imgh,img_w,img_h);
		img_out.clear_bmp_stream();
		img_out.display(GetDlgItem(hdlg,IDC_SHOWN),bmp_buf_jt, imgw, imgh);	// ‰≥ˆÕº∆¨
		sprintf(bmp_path,"%s\\pic\\jietu.bmp",dir);
		ai_img::save_bmp24_image(bmp_path,bmp_buf_jt,imgw,imgh);
		sprintf(bmp_path,"%s\\pic\\jietuyt.bmp",dir);
		ai_img::save_bmp24_image(bmp_path,bmp_buf_jtyt,img_w,img_h);
	}
}





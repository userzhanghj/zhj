#include <stdio.h>
#include <windowsx.h>
#include <windows.h>
#include <COMMCTRL.H>
#include "tcpip.h"
#include "resource.h"
#include "ai_link.h"
#include "s_img.h"
#include "s_win.h"


#pragma comment(lib, "s_win.lib")
#pragma comment(lib, "ai_link.lib")

using namespace ai_win;

#define INIT_DLG_WIDTH		430
#define EXTEND_DLG_WIDTH	760 


enum{client_side, server_side}station;

HWND hmain;
ai_link::TCPIP tcp_ip;

Dlgip dlgip;

SOCKET client_handshake;
BOOL server_connected = TRUE;
char input_message[10][PACK_SIZE] = {0};

HWND hlog = NULL;
HWND hlog_dlg = NULL;
extern HMENU hmenu;
int is_dlg_extend = FALSE;

TCPIP_V tcpip_v;

INIEdit config[] = {
	{IDC_SIDE_SELECT,		DLGTYPE_COMBO_INT,		&tcpip_v.select_side,		 "select_side"		 },
	{IDC_TCPIP_IP_ADDRESS,	DLGTYPE_INPUT_STRING,	&tcpip_v.ip_addr,			 "client_ip"		 },
	{IDC_TCPIP_PORT,		DLGTYPE_INPUT_INT,		&tcpip_v.port,				 "port"				 },
	{IDC_TCPIP_CMD_STR,		DLGTYPE_INPUT_STRING,	&tcpip_v.cmd_str,			 "cmd_str"			 },
	{IDC_TCPIP_CMD_COUNT,	DLGTYPE_INPUT_INT,		&tcpip_v.cmd_count,			 "cmd_count"		 },
	{IDC_TCPIP_CMD_DELAY,	DLGTYPE_INPUT_INT,		&tcpip_v.cmd_delay,			 "cmd_delay"		 },

	{NULL, NULL, NULL, NULL},


};


void TCPIPDlg::draw_button_icon(HWND hbutton, HICON hicon, int icon_width, int icon_height, BOOL button_disable)
{
	RECT button_rect;

	HDC hdc = GetDC(hbutton);

	GetClientRect(hbutton, &button_rect);
	int x0 = (button_rect.right - icon_width)/2;
	int y0 = (button_rect.bottom - icon_height)/2;
	DrawState(hdc, NULL, NULL, (LPARAM)hicon, 0, x0, y0, icon_width, icon_height, (button_disable?DSS_DISABLED:DSS_NORMAL)|DST_ICON);
	
	ReleaseDC(hbutton, hdc);
}

void TCPIPDlg::draw_own_button_ex(HWND hdlg, int idc, int idi, int is_disabled)
{
	HDC hdc = GetDC(GetDlgItem(hdlg, idc));

	RECT button_rect;
	GetClientRect(GetDlgItem(hdlg, idc), &button_rect);
	const int offset = 1;
	button_rect.left	+=offset;
	button_rect.right	-=offset;
	button_rect.top		+=offset;
	button_rect.bottom	-=offset;
	HBRUSH background_brush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	FillRect(hdc, &button_rect, background_brush);
	
	HICON hicon = LoadIcon((HINSTANCE)GetWindowLong(hdlg, GWL_HINSTANCE), MAKEINTRESOURCE(idi));
	ICONINFO info;
	GetIconInfo(hicon, &info);
	int icon_width = info.xHotspot*2;
	int icon_height = info.yHotspot*2;
	draw_button_icon(GetDlgItem(hdlg, idc), hicon, icon_width, icon_height, is_disabled);
	DeleteObject(hicon);
	DeleteObject(background_brush);
	ReleaseDC(GetDlgItem(hdlg, idc), hdc);
}

void send_log(HWND hdlg,const char *format, ...)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	HWND hlog = GetDlgItem(hdlg, IDC_LOG_LIST);
	char temp[1024] = {0};
	char log_str[1024] = {0};
	va_list args;
	va_start(args,format);

	vsprintf(temp,format,args);
	sprintf(log_str, "[%02d:%02d:%02d]>%s", st.wHour, st.wMinute, st.wSecond, temp);//jishi
	int index =SendMessage(hlog, LB_ADDSTRING, 0, (LPARAM) log_str);
	SendMessage(hlog, LB_SETCURSEL, index, 0);
	ai_win::save_log(log_str, "sys", "sys_log", 512);
}
void send_log_saved(HWND hdlg,const char *format, ...)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	HWND hlog = GetDlgItem(hdlg, IDC_LOG_LIST);
	char temp[1024] = {0};
	char log_str[1024] = {0};
	va_list args;
	va_start(args,format);

	vsprintf(temp,format,args);
	sprintf(log_str, "[%02d:%02d:%02d]>%s", st.wHour, st.wMinute, st.wSecond, temp);//jishi
	int index =SendMessage(hlog, LB_ADDSTRING, 0, (LPARAM) log_str);
	SendMessage(hlog, LB_SETCURSEL, index, 0);
	ai_win::save_log(log_str, "sys", "sys_log2", 512);
}


DWORD WINAPI pack_receive_thread(LPVOID temp)
{
	SOCKET receive_socket = (SOCKET)temp;
	int status = 0;
	char pack[PACK_SIZE] = {0};
	char status_message[PACK_SIZE] = {0};
	send_log_saved(hmain ,"======================================");
	while(1)
	{
		memset(status_message, 0, sizeof(status_message));
		memset(pack, 0, sizeof(pack));
		status = tcp_ip.receive_pack(receive_socket, pack, sizeof(pack)-1);
		send_log(hmain, "status=%d", status);
		if (status > 0 )
		{
			pack[status] = '\0';
			send_log_saved(hmain, "R[%2d]:%s", strlen(pack), pack);
		}
		else
		{
			switch (status)
			{
			case RECEIVE_COMMUNICATION_DISCONNECT:
				strcpy(status_message, "communication disconnect.");
				if (tcpip_v.select_side == client_side) ITEM_ACTIVE(hmain, IDC_TCPIP_CONNECT);
				break;
				
			case RECEIVE_COMMUNICATION_RESET:
				strcpy(status_message, "communication reset.");
				if (tcpip_v.select_side == client_side) ITEM_ACTIVE(hmain, IDC_TCPIP_CONNECT);
				break;
				
			case RECEIVE_COMMUNICATION_ABORTED:
				strcpy(status_message, "communication aborted.");
				break;
				
			case RECEIVE_COMMUNICATION_INTERRUPT:
				strcpy(status_message, "communication interrupt.");
				break;
				
			case 0:
				strcpy(status_message, "communication unknown error.");
				break;
				
			}
			break;
		}

		Sleep(1);
	}
	send_log_saved(hmain, "======================================");
	send_log_saved(hmain, status_message);
	tcp_ip.shutdown_socket(receive_socket);
	tcp_ip.close_socket(receive_socket);
	send_log_saved(hmain, "receive socket close.");
	return FALSE;
}

DWORD WINAPI loop_thread(LPVOID temp)
{
	HWND hdlg = (HWND)temp;
	send_log_saved(hmain, "LOOP THREAD START====");

	int cnt   = GetDlgItemInt(hdlg, IDC_TCPIP_CMD_COUNT, NULL, FALSE);
	int delay = GetDlgItemInt(hdlg, IDC_TCPIP_CMD_DELAY, NULL, FALSE);
	char str[100] = {0};
	GetDlgItemText(hdlg, IDC_TCPIP_CMD_STR, str, sizeof(str));
	int data_size = 0;
	while (cnt-->0)
	{
		data_size = tcp_ip.send_pack((tcpip_v.select_side == server_side)?client_handshake:tcp_ip.get_client_socket(), str, 2);
		send_log_saved(hmain, "Cm d:%s", str);
		Sleep(delay);
	}

	send_log_saved(hmain, "LOOP THREAD END====");
	ITEM_ENABLED(hdlg, IDC_TCPIP_CMD);
	ITEM_ENABLED(hdlg, IDC_TCPIP_CMD_COUNT);
	ITEM_ENABLED(hdlg, IDC_TCPIP_CMD_DELAY);
	ITEM_ENABLED(hdlg, IDC_TCPIP_CMD_STR);

	return FALSE;
}

DWORD WINAPI client_detect_thread(LPVOID temp)
{
	tcp_ip.server_listen(8);
	HANDLE hthread;
	DWORD thread_id;
	while(server_connected)
	{
		char ip_address[20] = {0};
		client_handshake = tcp_ip.server_accept_client(ip_address);
		if (client_handshake == ERROR_ACCEPT_CLIENT_FAIL)
		{
			send_log_saved(hmain, "client detect canceled.");
			break;
		}
		int port = GetDlgItemInt(hmain, IDC_TCPIP_PORT, NULL, FALSE);
		send_log_saved(hmain, "Accepted client:%s:%d", ip_address, port);
		send_log_saved(hmain, "client:%d", client_handshake);
		thread_id = 0;
		hthread = CreateThread(NULL, 0, pack_receive_thread, (LPVOID)client_handshake, 0, &thread_id);
		if (hthread == NULL)
		{
			send_log_saved(hmain, "create pack receive thread failed(%d).", GetLastError());
			break;
		}
		else
		{
			send_log_saved(hmain, "create pack receive thread ok.");
		}
		CloseHandle(hthread);

		ITEM_ENABLED(hmain, IDC_TCPIP_INPUT);
		ITEM_ENABLED(hmain, IDC_TCPIP_SEND);

	}

	tcp_ip.close_server_socket();
	return FALSE;
}

TCPIPDlg::TCPIPDlg()
{
	is_connected = FALSE;
}

char path[MAX_PATH] = {0};

BOOL TCPIPDlg::init(HWND hdlg, HWND hfocus, LPARAM linit)
{	
	char dir[MAX_PATH] = {0};
	get_current_directory(dir);

	sprintf(path, "%s\\config.ini", dir);
	tcpip_v.log_box_extend_height = ai_win::read_integer_key(path, "setting", "log_box_extend_height");

	is_init_finished = FALSE;
	hmain = hdlg;
	set_icon(hdlg, IDI_LOGO, IDI_LOGO);

	RECT dlg_rect;
	GetWindowRect(hdlg, &dlg_rect);
	MoveWindow(hdlg, 200, 100, INIT_DLG_WIDTH, dlg_rect.bottom-dlg_rect.top+tcpip_v.log_box_extend_height, FALSE);
	hlog = GetDlgItem(hdlg, IDC_LOG_LIST);
	RECT rt;
	GetWindowRect(hlog, &rt);

	POINT pt;				//@
	pt.x = rt.left;
	pt.y = rt.top;
	ScreenToClient(GetParent(hlog), &pt);

	

	MoveWindow(hlog, pt.x, pt.y, rt.right-rt.left, rt.bottom-rt.top+tcpip_v.log_box_extend_height, FALSE);

// 	hlog_dlg = CreateDialog((HINSTANCE)GetWindowLong(hdlg, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_LOG), hdlg, (DLGPROC)log_dlg_process);
// 	Sleep(100);

	COMBO_ADDSTR(hdlg, IDC_SIDE_SELECT, "Client");
	COMBO_ADDSTR(hdlg, IDC_SIDE_SELECT, "Server");



//	load_keys_to_dlg_items(path, "setting", hdlg, config, sizeof(config)/sizeof(ai_win::INIFile));
	load_keys(path, "setting", hdlg, config, sizeof(config)/sizeof(config[0]));
	tcpip_v.extend_dlg_enabled = read_integer_key(path, "setting", "extend_dlg_enabled");

	/*if (tcpip_v.extend_dlg_enabled == 0)
	{
		ITEM_GRAYED(hdlg, IDC_EXTEND_BUTTON);
	}*/

	if (tcpip_v.select_side == 0)
	{
		ITEM_ENABLED(hdlg, IDC_TCPIP_IP_ADDRESS);
	}
	else
	{
		ITEM_GRAYED(hdlg, IDC_TCPIP_IP_ADDRESS);
	}

	ITEM_GRAYED(hdlg, IDC_TCPIP_INPUT);
	ITEM_GRAYED(hdlg, IDC_TCPIP_SEND);
	
	COMBO_SETCURSEL(hdlg, IDC_TCPIP_INPUT, 0);

	int sub_width[2] = {100, -1};
	HWND status_bar = create_statusbar(hdlg, sub_width, sizeof(sub_width)/sizeof(int), 0);
	
	char version[20] = {0};

	get_version(NULL,TRUE, version);

	char status_string1[100] = {0};
	sprintf(status_string1, " Version: %s", version);
	
	SendMessage(status_bar, SB_SETTEXT, 0, (LPARAM)status_string1);
	SendMessage(status_bar, SB_SETTEXT, 1, (LPARAM)" www.a-view.com.cn");

	is_init_finished = TRUE;
	return TRUE;
}

void TCPIPDlg::command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify)
{
	switch (code_notify)
	{
	case LBN_DBLCLK:
		if (hctrl == hlog)
		{
			LISTBOX_RESET(hdlg, IDC_LOG_LIST);
		}
		break;

	case CBN_SELCHANGE:
	case EN_CHANGE:
		if (is_init_finished)
		{
			save_keys(path, "setting", hdlg, config, sizeof(config)/sizeof(config[0]));
			
			if (tcpip_v.select_side == 0)
			{
				ITEM_ENABLED(hdlg, IDC_TCPIP_IP_ADDRESS);
			}
			else
			{
				ITEM_GRAYED(hdlg, IDC_TCPIP_IP_ADDRESS);
			}

		}

		break;
	}

	switch (id)
	{
	case IDC_TCPIP_CONNECT:
		if (is_connected == FALSE)
		{
			int status;
			char status_message[PACK_SIZE] = {0};
			int port = GetDlgItemInt(hdlg, IDC_TCPIP_PORT, NULL, FALSE);
			
			char ip_address[20] = {0};
			GetDlgItemText(hdlg, IDC_TCPIP_IP_ADDRESS, ip_address, sizeof(ip_address));

			draw_own_button_ex(hdlg, IDC_TCPIP_CONNECT, IDI_TCPIP_CONNECT, FALSE);

			if (tcpip_v.select_side == server_side)
			{
				status = tcp_ip.build_server(port);

				if (status == ERROR_CREATE_SOCKET)			
				{
					strcpy(status_message, "create socket failed!");
					draw_own_button_ex(hdlg, IDC_TCPIP_CONNECT, IDI_TCPIP_DISCONNECT, FALSE);
				}
				else if (status == ERROR_SERVER_EXISTED)	
				{
					strcpy(status_message, "server already existed!");
					draw_own_button_ex(hdlg, IDC_TCPIP_CONNECT, IDI_TCPIP_DISCONNECT, FALSE);
				}
				else if (status == ERROR_BIND_FAIL)			
				{
					strcpy(status_message, "bind server failed!");
					draw_own_button_ex(hdlg, IDC_TCPIP_CONNECT, IDI_TCPIP_DISCONNECT, FALSE);
				}
				else
				{
					draw_own_button_ex(hdlg, IDC_TCPIP_CONNECT, IDI_TCPIP_CONNECT, FALSE);
					is_connected = TRUE;
					server_connected = TRUE;

					DWORD thread_id;
					HANDLE hthread = CreateThread(NULL, 0, client_detect_thread, 0, 0, &thread_id);
					if (hthread == NULL)	strcpy(status_message, "create client detect thread failed.");
					else					{
						strcpy(status_message, "build server success!");
						send_log_saved(hmain, "Accepted client:%s:%d", ip_address, port);
					}

					CloseHandle(hthread);

					ITEM_GRAYED(hdlg, IDC_SIDE_SELECT);
					ITEM_GRAYED(hdlg, IDC_TCPIP_IP_ADDRESS);
					ITEM_GRAYED(hdlg, IDC_TCPIP_PORT);
				}
				send_log_saved(hmain, "%s", status_message);
			}
			else
			{
				status = tcp_ip.client_connecting(ip_address, port);

				if (status == ERROR_CREATE_SOCKET)					
				{
					strcpy(status_message, "create socket failed!");
					draw_own_button_ex(hdlg, IDC_TCPIP_CONNECT, IDI_TCPIP_DISCONNECT, FALSE);
				}
				else if (status == ERROR_IP_ADDRESS_UNRECOGNIZED)	
				{
					strcpy(status_message, "ip address unrecognized!");
					draw_own_button_ex(hdlg, IDC_TCPIP_CONNECT, IDI_TCPIP_DISCONNECT, FALSE);
				}
				else if (status == ERROR_CONNECT_FAIL)				
				{
					strcpy(status_message, "client connecting failed!");
					draw_own_button_ex(hdlg, IDC_TCPIP_CONNECT, IDI_TCPIP_DISCONNECT, FALSE);
				}
				else 
				{
					draw_own_button_ex(hdlg, IDC_TCPIP_CONNECT, IDI_TCPIP_CONNECT, FALSE);
					is_connected = TRUE;

					DWORD thread_id;
					HANDLE hthread = CreateThread(NULL, 0, pack_receive_thread, (LPVOID)tcp_ip.get_client_socket(), 0, &thread_id);
					if (hthread == NULL)	strcpy(status_message, "create pack receive thread failed.");
					else					strcpy(status_message, "client connecting success!");
					
					CloseHandle(hthread);

					ITEM_GRAYED(hdlg, IDC_SIDE_SELECT);
					ITEM_GRAYED(hdlg, IDC_TCPIP_IP_ADDRESS);
					ITEM_GRAYED(hdlg, IDC_TCPIP_PORT);

					ITEM_ENABLED(hdlg, IDC_TCPIP_INPUT);
					ITEM_ENABLED(hdlg, IDC_TCPIP_SEND);
				}
				send_log_saved(hmain, "%s", status_message);
			}


		}
		else
		{
			draw_own_button_ex(hdlg, IDC_TCPIP_CONNECT, IDI_TCPIP_DISCONNECT, FALSE);
			is_connected = FALSE;
			if (tcpip_v.select_side == server_side)
			{
				server_connected = FALSE;
				tcp_ip.shutdown_server();
				tcp_ip.close_server_socket();

				tcp_ip.shutdown_socket(client_handshake);
				tcp_ip.close_socket(client_handshake);
			}
			else
			{
				tcp_ip.shutdown_client();
				tcp_ip.close_client_socket();
			}

			ITEM_ENABLED(hdlg, IDC_SIDE_SELECT);
			ITEM_ENABLED(hdlg, IDC_TCPIP_IP_ADDRESS);
			ITEM_ENABLED(hdlg, IDC_TCPIP_PORT);

			ITEM_GRAYED(hdlg, IDC_TCPIP_INPUT);
			ITEM_GRAYED(hdlg, IDC_TCPIP_SEND);
		}
		break;

	case IDC_TCPIP_SEND:
		{
			char pack[PACK_SIZE] = {0};
		//	unsigned char pack[PACK_SIZE] = {0};
			GetDlgItemText(hdlg, IDC_TCPIP_INPUT, pack, sizeof(pack));
			
			int data_size = tcp_ip.send_pack((tcpip_v.select_side == server_side)?client_handshake:tcp_ip.get_client_socket(), pack, strlen(pack));
			if (data_size == ERROR_SEND_PACK_FAIL)
			{
				send_log_saved(hmain, "send pack fail.");
			}
			else
			{
				send_log_saved(hmain, "S[%2d]:%s", data_size, pack);
			}
			keep_combo_record(hdlg, IDC_TCPIP_INPUT, input_message, 10);
		}
		break;
	case IDC_TCPIP_SEND2:
		{
			char pack[PACK_SIZE] = {0};
			GetDlgItemText(hdlg, IDC_TCPIP_INPUT2, pack, sizeof(pack));
			pack[strlen(pack)]=NULL;
			send_log_saved(hmain, "send pack%s", pack);
			int data_size = tcp_ip.send_pack((tcpip_v.select_side == server_side)?client_handshake:tcp_ip.get_client_socket(), pack, strlen(pack));
			if (data_size == ERROR_SEND_PACK_FAIL)
			{
				send_log_saved(hmain, "send pack fail.");
			}
			else
			{
				send_log_saved(hmain, "S[%2d]:%s", data_size, pack);
			}
			keep_combo_record(hdlg, IDC_TCPIP_INPUT, input_message, 10);
		}
		break;
	case IDC_TCPIP_CMD:
		{
			DWORD thread_id = 0;
			HANDLE hthread = CreateThread(NULL, 0, loop_thread, hdlg, 0,  &thread_id);
			CloseHandle(hthread);
			ITEM_GRAYED(hdlg, IDC_TCPIP_CMD);
			ITEM_GRAYED(hdlg, IDC_TCPIP_CMD_COUNT);
			ITEM_GRAYED(hdlg, IDC_TCPIP_CMD_DELAY);
			ITEM_GRAYED(hdlg, IDC_TCPIP_CMD_STR);
		}
		
		
		break;

	case IDC_SHOW_LOG:
		if (GET_CHECKER(hdlg, id))
		{
			RECT main_rect;
			GetClientRect(hmain, &main_rect);
			ShowWindow(hlog_dlg, SW_NORMAL);
			move_window(hmain, hlog_dlg, main_rect.right-main_rect.left+10, 0);
		}
		else
		{
			ShowWindow(hlog_dlg, SW_HIDE);
		}
		break;

	case IDC_EXTEND_BUTTON:
		{
			RECT dlg_rect;
			GetWindowRect(hdlg, &dlg_rect);
			if (is_dlg_extend == TRUE)
			{
				is_dlg_extend = FALSE;
				SetDlgItemText(hdlg, id, ">>");
				MoveWindow(hdlg, dlg_rect.left, dlg_rect.top, INIT_DLG_WIDTH, dlg_rect.bottom-dlg_rect.top, TRUE);
			}
			else
			{
				is_dlg_extend = TRUE;
				SetDlgItemText(hdlg, id, "<<");
				MoveWindow(hdlg, dlg_rect.left, dlg_rect.top, EXTEND_DLG_WIDTH, dlg_rect.bottom-dlg_rect.top, TRUE);
			}
		}
		break;

	}
}

void TCPIPDlg::draw_item(HWND hdlg, const DRAWITEMSTRUCT *pdis)
{
	int icon_id = 0;
	switch (pdis->CtlID)
	{
	case IDC_TCPIP_CONNECT:
		icon_id = is_connected?IDI_TCPIP_CONNECT:IDI_TCPIP_DISCONNECT;
		break;
	}
	
	if (pdis->CtlType == ODT_BUTTON)
	{
		int style = BUTTON_FRAME_FLAT | BUTTON_IMG_ICON | BUTTON_INNER_IMGONLY;
		draw_own_button(hdlg, pdis, icon_id, style);
	}
	
}


void Dlgip::close(HWND hdlg)
{
	//dinfo.show_log = FALSE;
	//CheckMenuItem(s_aa::hmenu, ID_FILE_LOG, MF_UNCHECKED);
	CheckMenuItem(hmenu, ID_2_27, FALSE?MF_CHECKED:MF_UNCHECKED);
	EndDialog(hdlg, FALSE);
}
TCPIPDlg tcpip_dlg;

BOOL CALLBACK menuDlg_2_27(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam)
{
	hmain=hdlg;
	switch (message)
	{
		HANDLE_DLGMSG(hdlg, WM_INITDIALOG,	tcpip_dlg.init);
		HANDLE_DLGMSG(hdlg, WM_COMMAND,		tcpip_dlg.command);
		HANDLE_DLGMSG(hdlg, WM_DRAWITEM,	tcpip_dlg.draw_item);
		HANDLE_DLGMSG(hdlg, WM_CLOSE,		dlgip.close);
	}

	return FALSE;

}


BOOL LogDlg::init(HWND hdlg, HWND hfocus, LPARAM linit)
{
	hlog = GetDlgItem(hdlg, IDC_LOG);
	RECT dlg_rect;
	GetClientRect(hdlg, &dlg_rect);
	MoveWindow(hdlg, 300, 100, dlg_rect.right, dlg_rect.bottom, TRUE);
	MoveWindow(GetDlgItem(hdlg, IDC_LOG), 0, 0, dlg_rect.right, dlg_rect.bottom, TRUE);
	SetWindowPos(hdlg, HWND_TOPMOST, 0, 0, dlg_rect.right - dlg_rect.left, dlg_rect.bottom - dlg_rect.top, SWP_HIDEWINDOW/*SWP_SHOWWINDOW*/);

	return TRUE;
}
void LogDlg::command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify)
{
	switch (code_notify)
	{
	case LBN_DBLCLK:
		if (hctrl == GetDlgItem(hdlg, IDC_LOG))
		{
			LISTBOX_RESET(hdlg, IDC_LOG);
		}
		break;
	}
}
void LogDlg::size(HWND hwnd, unsigned int state, int width, int height)
{
	MoveWindow(GetDlgItem(hwnd, IDC_LOG), 0, 0, width, height, TRUE);
} 
LogDlg log_dlg;
BOOL CALLBACK log_dlg_process(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_DLGMSG(hDlg, WM_INITDIALOG,	log_dlg.init);
		HANDLE_DLGMSG(hDlg, WM_COMMAND,		log_dlg.command);
		HANDLE_DLGMSG(hDlg, WM_SIZE,		log_dlg.size);
		HANDLE_DLGMSG(hDlg, WM_CLOSE,		dlgip.close);
	}
	return FALSE;
}
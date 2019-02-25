#ifndef _TCPIP_H_
#define _TCPIP_H_


typedef struct _tagTCPIP_V
{
	int select_side;
	char ip_addr[20];
	int port;
	char cmd_str[20];
	int cmd_count;
	int cmd_delay;
	int extend_dlg_enabled;

	int log_box_extend_height;
}TCPIP_V;

#define PACK_SIZE		256
class Dlgip
{
public:
	virtual void close(HWND hdlg);

private:
	HBRUSH mybrush;
};

class TCPIPDlg
{
public:
	TCPIPDlg();
	virtual BOOL init(HWND hdlg, HWND hfocus, LPARAM linit);
	virtual void command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify);
	virtual void draw_item(HWND hdlg, const DRAWITEMSTRUCT *pdis);
	//virtual void close(HWND hdlg);
private:
	int is_connected;
	int is_init_finished;
	void draw_button_icon(HWND hbutton, HICON hicon, int icon_width, int icon_height, BOOL button_disable);
	void draw_own_button_ex(HWND hdlg, int idc, int idi, int is_disabled);
};

BOOL CALLBACK log_dlg_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);
class LogDlg
{
public:
	virtual BOOL init(HWND hdlg, HWND hfocus, LPARAM linit);
	virtual void command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify);
	virtual void size(HWND hwnd, unsigned int state, int width, int height);
	//virtual void close(HWND hdlg);
};


#endif
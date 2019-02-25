#ifndef _PI_MOVE_H_
#define _PI_MOVE_H_

class DlgPI
{
public:
	virtual BOOL init(HWND hdlg, HWND hfocus, LPARAM linit);
	virtual void command(HWND hdlg, int id, HWND hctrl, unsigned int code_notify);
	virtual void size(HWND hwnd, unsigned int state, int width, int height);
	virtual void close(HWND hdlg);
private:
	HBRUSH mybrush;
};

#endif

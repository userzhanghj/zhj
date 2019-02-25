
#define _CRT_SECURE_NO_WARNINGS

#include "interface.h"
extern BOOL CALLBACK raw_config_dlg_process(HWND hdlg, unsigned int message, WPARAM wParam, LPARAM lParam);

extern char shared_raw_img_path[MAX_PATH];
//////////////////////////////////////////////////////////////////////////
//free image proc.
LONG g_nHandler = 0;
fi_handle g_load_address;
inline unsigned _stdcall _ReadProc(void *buffer, unsigned size, unsigned count, fi_handle handle) {
	BYTE *tmp = (BYTE *)buffer;

	for (unsigned c = 0; c < count; c++) {
		memcpy(tmp, g_load_address, size);
		g_load_address = (BYTE *)g_load_address + size;
		tmp += size;
	}

	return count;
}

inline unsigned _stdcall _WriteProc(void *buffer, unsigned size, unsigned count, fi_handle handle) {
		// there's not much use for saving the bitmap into memory now, is there?
		return size;
}

inline int _stdcall _SeekProc(fi_handle handle, long offset, int origin) {
	assert(origin != SEEK_END);

	if (origin == SEEK_SET) {
		g_load_address = (BYTE *)handle + offset;
	} else {
		g_load_address = (BYTE *)g_load_address + offset;
	}

	return 0;
}

inline long _stdcall _TellProc(fi_handle handle) {
	//	assert((int)handle > (int)g_load_address);
	return ((int)g_load_address - (int)handle);
}
//free image proc.
//////////////////////////////////////////////////////////////////////////

void measure_single_pixel(int x, int y)
{
	if (GET_CHECKER(HWND_MEASURE, IDC_SELECT_RGB24)){
		int b_val = PIXEL_B(x, y, IMG_WIDTH, IMG_HEIGHT);
		int g_val = PIXEL_G(x, y, IMG_WIDTH, IMG_HEIGHT);
		int r_val = PIXEL_R(x, y, IMG_WIDTH, IMG_HEIGHT);
		float y_val = 0.299f*(float)s_aa::bmp_buf[r_val] + 0.587f*(float)s_aa::bmp_buf[g_val] + 0.114f*(float)s_aa::bmp_buf[b_val] + 0.5f;	


		ai_win::set_dlg_item_int(HWND_MEASURE, IDC_PIXEL_B, s_aa::bmp_buf[b_val]);
		ai_win::set_dlg_item_int(HWND_MEASURE, IDC_PIXEL_G, s_aa::bmp_buf[g_val]);
		ai_win::set_dlg_item_int(HWND_MEASURE, IDC_PIXEL_R, s_aa::bmp_buf[r_val]);
		ai_win::set_dlg_item_float(HWND_MEASURE, IDC_PIXEL_GRAY, y_val, 2);
	}
	else if (GET_CHECKER(HWND_MEASURE, IDC_SELECT_RAW8)){
		int result = TRUE;
		BYTE *myraw8 = new BYTE[IMG_WIDTH*IMG_HEIGHT];
		if (IMG_RAW_FORMAT == RAW_8BIT){
			memcpy(myraw8, s_aa::img_buf, IMG_WIDTH*IMG_HEIGHT);
		}
		else if (IMG_RAW_FORMAT == RAW_10BIT){
			ai_img::raw10_to_raw8(s_aa::img_buf, myraw8, IMG_WIDTH, IMG_HEIGHT);
		}
		else if (IMG_RAW_FORMAT == RAW_12BIT){
			ai_img::raw12_to_raw8(s_aa::img_buf, myraw8, IMG_WIDTH, IMG_HEIGHT);
		}
		else{
			log("raw format is not matched.");
			result = FALSE;
		}
		if (result == TRUE){
			int r, gr, b, gb;
			if (IMG_PIXEL_ORDER == RAW_OUTPUT_BGGR){
				b  = myraw8[y*IMG_WIDTH+x];
				gb = myraw8[y*IMG_WIDTH+x+1];
				gr = myraw8[(y+1)*IMG_WIDTH+x];
				r  = myraw8[(y+1)*IMG_WIDTH+x+1];
			}
			else if (IMG_PIXEL_ORDER == RAW_OUTPUT_RGGB){
				r  = myraw8[y*IMG_WIDTH+x];
				gr = myraw8[y*IMG_WIDTH+x+1];
				gb = myraw8[(y+1)*IMG_WIDTH+x];
				b  = myraw8[(y+1)*IMG_WIDTH+x+1];
			}
			else if (IMG_PIXEL_ORDER == RAW_OUTPUT_GBRG){
				gb = myraw8[y*IMG_WIDTH+x];
				b  = myraw8[y*IMG_WIDTH+x+1];
				r  = myraw8[(y+1)*IMG_WIDTH+x];
				gr = myraw8[(y+1)*IMG_WIDTH+x+1];
			}
			else if (IMG_PIXEL_ORDER == RAW_OUTPUT_GRBG){
				gr = myraw8[y*IMG_WIDTH+x];
				r  = myraw8[y*IMG_WIDTH+x+1];
				b  = myraw8[(y+1)*IMG_WIDTH+x];
				gb = myraw8[(y+1)*IMG_WIDTH+x+1];
			}
		}
		delete [] myraw8;
	}

}

void measure_rect_pixels(RECT rt)
{
	int a,b,c,d;
	a=max(MEASURE_END_X,MEASURE_START_X);
	b=min(MEASURE_END_X,MEASURE_START_X);
	c=max(MEASURE_END_Y,MEASURE_START_Y);
	d=min(MEASURE_END_Y,MEASURE_START_Y);
	int w = (int)((a-b)*IMG_RATIO_X);
	int h = (int)((c-d)*IMG_RATIO_Y);
	//ai_win::set_dlg_item_int(HWND_MEASURE, IDC_PIXEL_X2, w);
	//ai_win::set_dlg_item_int(HWND_MEASURE, IDC_PIXEL_Y2, h);
	float avg[4] = {0.0f}; 
	float min[4] = {0.0f};
	float max[4] = {0.0f};	
	if (GET_CHECKER(HWND_MEASURE, IDC_SELECT_RGB24)){

		
		ai_img::get_bmp_roi_info(s_aa::bmp_buf, IMG_WIDTH, IMG_HEIGHT, &rt, avg, min, max);

		ai_win::set_dlg_item_float(HWND_MEASURE, IDC_PIXEL_B2,	  avg[0], 2);
		ai_win::set_dlg_item_float(HWND_MEASURE, IDC_PIXEL_G2,	  avg[1], 2);
		ai_win::set_dlg_item_float(HWND_MEASURE, IDC_PIXEL_R2,	  avg[2], 2);
		ai_win::set_dlg_item_float(HWND_MEASURE, IDC_PIXEL_GRAY2, avg[3], 2);
		ai_win::set_dlg_item_float(HWND_MEASURE, IDC_PIXEL_B3,	  min[0], 2);
		ai_win::set_dlg_item_float(HWND_MEASURE, IDC_PIXEL_G3,	  min[1], 2);
		ai_win::set_dlg_item_float(HWND_MEASURE, IDC_PIXEL_R3,	  min[2], 2);
		ai_win::set_dlg_item_float(HWND_MEASURE, IDC_PIXEL_GRAY3, min[3], 2);
		ai_win::set_dlg_item_float(HWND_MEASURE, IDC_PIXEL_B4,	  max[0], 2);
		ai_win::set_dlg_item_float(HWND_MEASURE, IDC_PIXEL_G4,	  max[1], 2);
		ai_win::set_dlg_item_float(HWND_MEASURE, IDC_PIXEL_R4,	  max[2], 2);
		ai_win::set_dlg_item_float(HWND_MEASURE, IDC_PIXEL_GRAY4, max[3], 2);
	}
	//else if (GET_CHECKER(HWND_MEASURE, IDC_SELECT_RAW8)){
	//	int result = TRUE;
	//	BYTE *myraw8 = new BYTE[IMG_WIDTH*IMG_HEIGHT];
	//	if (IMG_RAW_FORMAT == RAW_8BIT){
	//		memcpy(myraw8, s_aa::img_buf, IMG_WIDTH*IMG_HEIGHT);
	//	}
	//	else if (IMG_RAW_FORMAT == RAW_10BIT){
	//		ai_img::raw10_to_raw8(s_aa::img_buf, myraw8, IMG_WIDTH, IMG_HEIGHT);
	//	}
	//	else if (IMG_RAW_FORMAT == RAW_12BIT){
	//		ai_img::raw12_to_raw8(s_aa::img_buf, myraw8, IMG_WIDTH, IMG_HEIGHT);
	//	}
	//	else{
	//		log("raw format is not matched.");
	//		result = FALSE;
	//	}
	//	double sum[4] = {0};
	//	int v[4] = {0};
	//	for (int i=0; i<4; i++){
	//		min[i] = 255.0f;
	//		max[i] = 0.0f;
	//	}
	//	if (result == TRUE){
	//		float avg_r, avg_gr, avg_b, avg_gb;
	//		int min_r, min_gr, min_b, min_gb;
	//		int max_r, max_gr, max_b, max_gb;
	//		int total_size = (rt.right-rt.left)*(rt.bottom-rt.top)/4;
	//		for (int j=rt.top; j<rt.bottom; j+=2){		
	//			for (int i=rt.left; i<rt.right; i+=2){
	//				v[0] = myraw8[j*IMG_WIDTH+i];
	//				v[1] = myraw8[j*IMG_WIDTH+i+1];
	//				v[2] = myraw8[(j+1)*IMG_WIDTH+i];
	//				v[3] = myraw8[(j+1)*IMG_WIDTH+i+1];
	//				for (int m=0; m<4; m++){
	//					sum[m] += v[m];
	//					if (v[m]>max[m]) max[m] = (float)v[m];
	//					if (v[m]<min[m]) min[m] = (float)v[m];
	//				}
	//			}
	//		}
	//		avg[0] = (float)sum[0]/total_size;
	//		avg[1] = (float)sum[1]/total_size;
	//		avg[2] = (float)sum[2]/total_size;
	//		avg[3] = (float)sum[3]/total_size;

	//		if (IMG_PIXEL_ORDER == RAW_OUTPUT_BGGR){
	//			avg_b  = avg[0]; min_b =  (int)min[0]; max_b = (int)max[0];
	//			avg_gb = avg[1]; min_gb = (int)min[1]; max_gb = (int)max[1];
	//			avg_gr = avg[2]; min_gr = (int)min[2]; max_gr = (int)max[2];
	//			avg_r  = avg[3]; min_r =  (int)min[3]; max_r = (int)max[3];
	//		}
	//		else if (IMG_PIXEL_ORDER == RAW_OUTPUT_RGGB){
	//			avg_r  = avg[0]; min_r =  (int)min[0]; max_r = (int)max[0];
	//			avg_gr = avg[1]; min_gr = (int)min[1]; max_gr = (int)max[1];
	//			avg_gb = avg[2]; min_gb = (int)min[2]; max_gb = (int)max[2];
	//			avg_b  = avg[3]; min_b =  (int)min[3]; max_b = (int)max[3];
	//		}
	//		else if (IMG_PIXEL_ORDER == RAW_OUTPUT_GBRG){
	//			avg_gb = avg[0]; min_gb = (int)min[0]; max_gb = (int)max[0];
	//			avg_b  = avg[1]; min_b  = (int)min[1]; max_b  = (int)max[1];
	//			avg_r  = avg[2]; min_r  = (int)min[2]; max_r  = (int)max[2];
	//			avg_gr = avg[3]; min_gr = (int)min[3]; max_gr = (int)max[3];
	//		}
	//		else if (IMG_PIXEL_ORDER == RAW_OUTPUT_GRBG){
	//			avg_gr = avg[0]; min_gr = (int)min[0]; max_gr = (int)max[0];
	//			avg_r  = avg[1]; min_r  = (int)min[1]; max_r  = (int)max[1];
	//			avg_b  = avg[2]; min_b  = (int)min[2]; max_b  = (int)max[2];
	//			avg_gb = avg[3]; min_gb = (int)min[3]; max_gb = (int)max[3];
	//		}


	//	
	//	}

	//	delete [] myraw8;
	//}
}

void MyImage::draw_on_window(HDC memdc, LPVOID temp)
{
	RECT rt;
	int a,b,c,d,w,h;
	a=max(MEASURE_END_X,MEASURE_START_X);
	b=min(MEASURE_END_X,MEASURE_START_X);
	c=max(MEASURE_END_Y,MEASURE_START_Y);
	d=min(MEASURE_END_Y,MEASURE_START_Y);
	rt.left   = max((int)(b*IMG_RATIO_X),0);
	rt.top    = max((int)(d*IMG_RATIO_Y),0);
	rt.right  = min((int)(a*IMG_RATIO_X),IMG_WIDTH);
	rt.bottom = min((int)(c*IMG_RATIO_Y),IMG_HEIGHT);
	w=rt.right-rt.left;
	h=rt.bottom-rt.top;
	ai_win::set_dlg_item_int(HWND_MEASURE, IDC_PIXEL_X2, max(0,w));
	ai_win::set_dlg_item_int(HWND_MEASURE, IDC_PIXEL_Y2, max(0,h));

	measure_rect_pixels(rt);

	SetBkMode(memdc, TRANSPARENT);
	SelectObject(memdc, GetStockObject(NULL_BRUSH));

	pen		 = CreatePen(PS_DASH, 1, COLOR_DARK_ORANGE);
	old_pen  = (HPEN)SelectObject(memdc, pen);
	hfont	 = CreateFont(20, 0,0,0,20,0,0,0,ANSI_CHARSET,3,2,1, VARIABLE_PITCH | FF_ROMAN, "黑体");
	old_font = (HFONT)SelectObject(memdc, hfont);

	if (s_aa::gline.show_center_line){
		DeleteObject(pen);
		pen = CreatePen(s_aa::gline.center_line_type, s_aa::gline.center_line_width, s_aa::gline.center_cr);
		old_pen = (HPEN)SelectObject(memdc, pen);
		draw_crossline(memdc, IMG_WIDTH/2, IMG_HEIGHT/2, IMG_WIDTH, IMG_HEIGHT);
		
	}

	if (s_aa::gline.show_field_line){

		DeleteObject(pen);
		pen = CreatePen(s_aa::gline.field_line_type, s_aa::gline.field_line_width, s_aa::gline.field_cr);
		old_pen = (HPEN)SelectObject(memdc, pen);
		if (s_aa::gline.field_location >1) s_aa::gline.field_location = 1;
		if (s_aa::gline.field_location<=0) s_aa::gline.field_location = 0.1f;

		int length = (int)(sqrt((float)(IMG_WIDTH*IMG_WIDTH+IMG_HEIGHT*IMG_HEIGHT))*s_aa::gline.field_location)/2;
		int left   = IMG_WIDTH/2-length;
		int right  = IMG_WIDTH/2+length;
		int top	   = IMG_HEIGHT/2-length;
		int bottom = IMG_HEIGHT/2+length;
		Ellipse(memdc, left, top, right, bottom);
		
	}

	DeleteObject(pen);
	pen		= CreatePen(PS_SOLID, 3, COLOR_BLUE);
	old_pen = (HPEN)SelectObject(memdc, pen);
	//////////////////////////////////////////////////////////////////////////
	if (ENABLE_IMG_ZOOM_IN && DRAW_ZOOM_IN_AREA_DONE == FALSE && DRAW_MOVING_MOUSE == TRUE){
		Rectangle(memdc, (int)(MOUSE_START_X*IMG_RATIO_X), (int)(MOUSE_START_Y*IMG_RATIO_Y), (int)(MOUSE_END_X*IMG_RATIO_X), (int)(MOUSE_END_Y*IMG_RATIO_Y));
		char str[MAX_PATH] = {0};
		sprintf(str, "zoom: x=%d, y=%d, w=%d, h=%d", (int)(MOUSE_START_X*IMG_RATIO_X), (int)(MOUSE_START_Y*IMG_RATIO_Y), 
				(int)((MOUSE_END_X-MOUSE_START_X)*IMG_RATIO_X), (int)((MOUSE_END_Y-MOUSE_START_Y)*IMG_RATIO_Y));
		SET_STATUSBAR_TEXT(HWND_STATUS, SBINDEX_TEXT, str);
	}

	if (ENABLE_MEASURE && DRAW_ZOOM_IN_AREA_DONE != TRUE){
		DeleteObject(pen);
		pen		= CreatePen(PS_SOLID, 3, COLOR_RED);
		old_pen = (HPEN)SelectObject(memdc, pen);

		if (IMG_RAW_FORMAT == RAW_8BIT || IMG_RAW_FORMAT == RAW_10BIT || IMG_RAW_FORMAT == RAW_12BIT){
			ITEM_ENABLED(HWND_MEASURE, IDC_SELECT_RAW8);
		}
		else {
			ITEM_GRAYED(HWND_MEASURE, IDC_SELECT_RAW8);
			SET_CHECKER(HWND_MEASURE, IDC_SELECT_RGB24, TRUE);
		}
	
		if (MEASURE_START_X == MEASURE_END_X && MEASURE_START_Y == MEASURE_END_Y){
			int my_x = (int)(MEASURE_START_X*IMG_RATIO_X);
			int my_y = (int)(MEASURE_START_Y*IMG_RATIO_Y);
		//	log("measureX=%d,measureY=%d",MEASURE_START_X,MEASURE_START_Y);
			ai_win::set_dlg_item_int(HWND_MEASURE, IDC_PIXEL_X, my_x);
			ai_win::set_dlg_item_int(HWND_MEASURE, IDC_PIXEL_Y, my_y);
			draw_crossline(memdc, my_x, my_y, IMG_WIDTH/100, IMG_HEIGHT/100);
			measure_single_pixel(my_x, my_y);
			//*********************************************************************************************
		}
		else /*if (MEASURE_END_X>MEASURE_START_X && MEASURE_END_Y>MEASURE_START_Y)*/{
			
			ai_win::set_dlg_item_int(HWND_MEASURE, IDC_PIXEL_X, rt.left);
			ai_win::set_dlg_item_int(HWND_MEASURE, IDC_PIXEL_Y, rt.top);
			if(is_move)
				Rectangle(memdc, rt.left, rt.top, rt.right, rt.bottom);
			
			char str[MAX_PATH] = {0};
			
			sprintf(str, "Measure: x=%d, y=%d, w=%d, h=%d", rt.left, rt.top, rt.right-rt.left, rt.bottom-rt.top);
			SET_STATUSBAR_TEXT(HWND_STATUS, SBINDEX_TEXT, str);
			//*********************************************************************************************************************
// 			if (MEASURE_DRAW_RECT_DONE){
// 							measure_rect_pixels(rt);
// 						//}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//test drawing

	if (s_aa::test_option[TI_REG_MARK_CHART].draw_result_on == TRUE){
		draw_reg_mark_chart(memdc);
	}

	//////////////////////////////////////////////////////////////////////////
	//tools drawing
	for (int i=0; i<TOOL_CNT; i++){
		if (RESULT_LOAD_TOOL(i) == TRUE){
			s_aa::imtool[i].draw_on_global_img(memdc);
		}
	}

	DeleteObject(hfont);
	SelectObject(memdc, (HFONT)old_font);

	DeleteObject(pen);
	SelectObject(memdc, (HPEN)old_pen);
}

void make_demosaic(BYTE *bmp, BYTE *raw8, int img_w, int img_h)
{
	IMG_DEPTH = 3;
	if (IMG_SELECT_DEMOSAIC == 0){
		for (int i=0; i<img_h; i++){
			memcpy(bmp+i*img_w, raw8+(img_h-1-i)*img_w, img_w);
		}
		IMG_DEPTH = 1;
	}
	else if (IMG_SELECT_DEMOSAIC == 1){
		ai_img::raw8_to_bmp24_2x2(IMG_PIXEL_ORDER, raw8, bmp, img_w, img_h);
	}
	else if (IMG_SELECT_DEMOSAIC == 2){
		ai_img::raw8_to_bmp24_3x3(IMG_PIXEL_ORDER, raw8, bmp, img_w, img_h);
	}
	else return;
}

int parse_raw(BYTE *raw, BYTE *bmp, int w, int h, float read_pixel_width)
{
	int result = TRUE;
	switch (IMG_RAW_FORMAT)
	{
	case RAW_YUV:
		result = FALSE;
		break;

	case RAW_8BIT:
		if (read_pixel_width == 1.0f){
			make_demosaic(bmp, raw, IMG_WIDTH, IMG_HEIGHT);
		}
		else result = FALSE;
		break;

	case RAW_10BIT:
		if (read_pixel_width == 2.0f){
			BYTE *raw8 = new BYTE[IMG_WIDTH*IMG_HEIGHT];
			ai_img::raw10_to_raw8(raw, raw8, IMG_WIDTH, IMG_HEIGHT);
			make_demosaic(bmp, raw8, IMG_WIDTH, IMG_HEIGHT);
			delete [] raw8;
		}
		else result = FALSE;
		break;

	case RAW_12BIT:
		if (read_pixel_width == 2.0f){
			BYTE *raw8 = new BYTE[IMG_WIDTH*IMG_HEIGHT];
			ai_img::raw12_to_raw8(raw, raw8, IMG_WIDTH, IMG_HEIGHT);
			make_demosaic(bmp, raw8, IMG_WIDTH, IMG_HEIGHT);

			delete [] raw8;
		}
		else result = FALSE;
		break;

	case RAW_MIPI10BIT:
		if (read_pixel_width == 2.0f/*1.25f*/){//(w*5/4)*h/(w*h)
			BYTE *raw8 = new BYTE[IMG_WIDTH*IMG_HEIGHT];
			ai_img::mipi10_to_raw8(raw, raw8, IMG_WIDTH, IMG_HEIGHT);
			make_demosaic(bmp, raw8, IMG_WIDTH, IMG_HEIGHT);
			delete [] raw8;
		}
		else result = FALSE;

		break;

	default:
		result = FALSE;
		break;
	}

	return result;
}

int parse_img(const char *img_path, int img_format)
{
	int result = TRUE;
	FREE_IMAGE_FORMAT fif;
	char img_str[20] = {0};
	switch(img_format)
	{
	case IMG_JPG:	fif = FIF_JPEG; sprintf(img_str, "Jpeg");	  break;
	case IMG_PNG:	fif = FIF_PNG;  sprintf(img_str, "Png");	  break;
	case IMG_TIFF: fif = FIF_TIFF; sprintf(img_str, "Tif/Tiff"); break;
	case IMG_GIF:  fif = FIF_GIF;  sprintf(img_str, "Gif");	  break;
	default: result = FALSE; break;
	}
	if (result == TRUE){
		FreeImage_Initialise();
		FIBITMAP *fimg = FreeImage_Load(fif, img_path);
		BYTE * img = FreeImage_GetBits(fimg);
		IMG_WIDTH  = FreeImage_GetWidth(fimg);
		IMG_HEIGHT = FreeImage_GetHeight(fimg);
		IMG_DEPTH  = FreeImage_GetBPP(fimg)/8;

		if (fimg != NULL && IMG_WIDTH>0 && IMG_HEIGHT>0){
			log("Read %s:%s", img_str, img_path);
			log("%s W:%d, H:%d, %dbit", img_str, IMG_WIDTH, IMG_HEIGHT, IMG_DEPTH*8);
			memcpy(s_aa::bmp_buf_t, img, IMG_WIDTH*IMG_HEIGHT*IMG_DEPTH);
		}
		else{
			log("Read %s Fail.", img_str);
		}
		FreeImage_Unload(fimg);
		FreeImage_DeInitialise();
	}

	return result;
}

int parse_img_stream(BYTE *stream, int img_format)
{
	int result = TRUE;
	FreeImage_Initialise();
	FreeImageIO io;
	io.read_proc  = _ReadProc;
	io.write_proc = _WriteProc;
	io.tell_proc  = _TellProc;
	io.seek_proc  = _SeekProc;
	g_load_address = stream;
	FIBITMAP *dib = FreeImage_LoadFromHandle((FREE_IMAGE_FORMAT)img_format, &io, (fi_handle)stream);
	BYTE *img = FreeImage_GetBits(dib);
	IMG_WIDTH = FreeImage_GetWidth(dib);
	IMG_HEIGHT = FreeImage_GetHeight(dib);
	IMG_DEPTH = FreeImage_GetBPP(dib)/8;
	if (dib != NULL && IMG_WIDTH >0 && IMG_HEIGHT >0){
		memcpy(s_aa::bmp_buf_t, img, IMG_WIDTH*IMG_HEIGHT*IMG_DEPTH*8);
	}
	else result = FALSE;
	FreeImage_Unload(dib);
	FreeImage_DeInitialise();
	return result;
}

int save_stream_to_image_file(BYTE *stream, int img_format, char *path)
{
	BITMAPFILEHEADER bmp_header; // Header for Bitmap file
	BITMAPINFO		 bmp_info;

	int shift_bits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	DWORD image_size = IMG_WIDTH*IMG_HEIGHT*3;

	bmp_header.bfType      = ((WORD)('M'<<8)|'B');
	bmp_header.bfSize      = shift_bits + image_size;
	bmp_header.bfReserved1 = 0;
	bmp_header.bfReserved2 = 0;
	bmp_header.bfOffBits   = shift_bits;

	bmp_info.bmiHeader.biSize            = sizeof(BITMAPINFOHEADER);
	bmp_info.bmiHeader.biWidth           = IMG_WIDTH;
	bmp_info.bmiHeader.biHeight          = IMG_HEIGHT;
	bmp_info.bmiHeader.biPlanes          = 1;
	bmp_info.bmiHeader.biBitCount        = 24;
	bmp_info.bmiHeader.biCompression     = BI_RGB;
	bmp_info.bmiHeader.biSizeImage       = 0; 
	bmp_info.bmiHeader.biXPelsPerMeter   = 0;
	bmp_info.bmiHeader.biYPelsPerMeter   = 0;
	bmp_info.bmiHeader.biClrUsed         = 0;
	bmp_info.bmiHeader.biClrImportant    = 0;

	
	BYTE *bmp = new BYTE[IMG_WIDTH*IMG_HEIGHT*4+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+4];
	memcpy(bmp, &bmp_header, sizeof(BITMAPFILEHEADER));
	memcpy(bmp+sizeof(BITMAPFILEHEADER), &bmp_info, sizeof(BITMAPINFOHEADER));
	memcpy(bmp+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER), stream, IMG_WIDTH*IMG_HEIGHT*3+4);

	int result = TRUE;
	FreeImage_Initialise();
	FreeImageIO io;
	io.read_proc  = _ReadProc;
	io.write_proc = _WriteProc;
	io.tell_proc  = _TellProc;
	io.seek_proc  = _SeekProc;
	g_load_address = /*(fi_handle)*/bmp;
	FIBITMAP *dib = FreeImage_LoadFromHandle(FIF_BMP, &io, (fi_handle)bmp);
	FreeImage_Save((FREE_IMAGE_FORMAT)img_format, dib, path);
	FreeImage_Unload(dib);
	FreeImage_DeInitialise();
	delete [] bmp;
	return result;
}

//////////////////////////////////////////////////////////////////////////
int get_bmp_file(char *img_path)
{
	int result = ai_img::read_bmp(img_path, s_aa::bmp_buf_t, IMG_WIDTH, IMG_HEIGHT, IMG_DEPTH);
	if (result == TRUE){
		log("Read BMP:%s", img_path);
		log("BMP W:%d, H:%d, %dbit", IMG_WIDTH, IMG_HEIGHT, IMG_DEPTH*8);
	}
	else{
		log("Read BMP Fail:%s.", img_path);
	}

	return result;
}

int read_img_file(char *img_path)
{
	int result = FALSE;
	int n = strlen(img_path);
	ai_win::uppercase(img_path);

	char img_type[20] = {0};

	if (img_path[n-3] == 'B' && img_path[n-2] == 'M' && img_path[n-1] == 'P'){
		IMG_FORMAT = IMG_BMP;
		strcpy(img_type, "BMP");
		result = get_bmp_file(img_path);
	}
	else if (img_path[n-3]=='J' && img_path[n-2]=='P' && img_path[n-1]=='G'){
		IMG_FORMAT = IMG_JPG;
		strcpy(img_type, "JPEG");
		result = parse_img(img_path, IMG_FORMAT);
	}
	else if (img_path[n-3] == 'P' && img_path[n-2] == 'N' && img_path[n-1] == 'G'){
		IMG_FORMAT = IMG_PNG;
		strcpy(img_type, "PNG");
		result = parse_img(img_path, IMG_FORMAT);
	}
	else if ((img_path[n-3] == 'I' && img_path[n-2] == 'F' && img_path[n-1] == 'F') ||
			 (img_path[n-3] == 'T' && img_path[n-2] == 'I' && img_path[n-1] == 'F')){
		IMG_FORMAT = IMG_TIFF;
		strcpy(img_type, "TIFF");
		result = parse_img(img_path, IMG_FORMAT);
	}
	else if (img_path[n-3] == 'G' && img_path[n-2] == 'I' && img_path[n-1] == 'F'){
		IMG_FORMAT = IMG_GIF;
		strcpy(img_type, "GIF");
		result = parse_img(img_path, IMG_FORMAT);
	}

	char mystr[100] = {0};
	if (result == TRUE){
		int n = strlen(img_path);
		int index = 0;
		for (int i=0; i<n; i++){
			if (img_path[n-i] == '\\'){
				index = i;
				break;
			}
		}
		char file_name[MAX_PATH] = {0};
		sprintf(file_name, "%s", img_path+n-index);
		char img_size[100] = {0};
		sprintf(img_size, "%dx%d", IMG_WIDTH, IMG_HEIGHT);
	//	update_scenery_title(file_name, img_size, s_aa::img_format_str[IMG_FORMAT], "0fps");

		DISPLAY_IMG_START_X = 0;
		DISPLAY_IMG_START_Y = 0;
		DISPLAY_IMG_WIDTH  = IMG_WIDTH;
		DISPLAY_IMG_HEIGHT = IMG_HEIGHT;
		sprintf(mystr, "%dx%d, %s", IMG_WIDTH, IMG_HEIGHT, img_type);
		SET_STATUSBAR_TEXT(HWND_STATUS, SBINDEX_IMAGE_INFO, mystr);
	}
	else{
		SET_STATUSBAR_TEXT(HWND_STATUS, SBINDEX_IMAGE_INFO, "N/A");
	}

	return result;
}

int get_image_ex(BYTE *bmp_buf, int dummy_cnt, int avg_cnt)
{
	if (dummy_cnt<0)  dummy_cnt = 0;
	if (dummy_cnt>10) dummy_cnt = 10;
	if (avg_cnt<=0)	  avg_cnt = 1;
	if (avg_cnt>10)	  avg_cnt = 10;
//	log("w=%d, h=%d", IMG_WIDTH, IMG_HEIGHT);
	int *buffer = new int[IMG_WIDTH*IMG_HEIGHT*2];
	if (buffer == NULL){
		return FALSE;
	}

	BYTE *data_array[10] = {NULL};
	
	for (int i=0; i<avg_cnt; i++){
		data_array[i] = new BYTE[IMG_WIDTH*IMG_HEIGHT*2];
	}
	
	for (int i=0; i<dummy_cnt; i++){
		s_aa::pimhw->get_image(s_aa::img_buf);
	}

	int count = 0;
	int result = TRUE; 
	int try_cnt = 1000;
	while (try_cnt--){
		if (s_aa::pimhw->get_image(s_aa::img_buf)){ //raw10;
			ai_img::mipi10_to_raw8(s_aa::img_buf, s_aa::raw8_buf, IMG_WIDTH, IMG_HEIGHT);
				make_demosaic(s_aa::bmp_buf, s_aa::raw8_buf, IMG_WIDTH, IMG_HEIGHT);
				result = TRUE;
				break;
// 			if (average_img(s_aa::raw8_buf, buffer, data_array, IMG_WIDTH*IMG_HEIGHT, &count, avg_cnt) == TRUE){
// 				make_demosaic(s_aa::bmp_buf, s_aa::raw8_buf, IMG_WIDTH, IMG_HEIGHT);
// 				result = TRUE;
// 				break;
// 			}
		}
		Sleep(10);
	}

	if (try_cnt<=0)	result = FALSE;

	for (int i=0; i<avg_cnt; i++){
		delete [] data_array[i];
	}

	delete [] buffer;
	return result;
}

int extern_cam_output(void)
{
	int result = FALSE;
	int pixe_width;
	IMG_WIDTH			= EXT_CAM_IMG_WIDTH;
	IMG_HEIGHT			= EXT_CAM_IMG_HEIGHT;
	IMG_DEPTH			= EXT_CAM_IMG_DEPTH;
	IMG_FORMAT			= EXT_CAM_IMG_FORMAT;

	IMG_RAW_FORMAT		= EXT_CAM_IMG_RAW_FORMAT;
	IMG_RAW_HEADER		= EXT_CAM_IMG_RAW_HEADER;
	IMG_SELECT_DEMOSAIC = EXT_CAM_IMG_SELECT_DEMOSAIC;
	IMG_PIXEL_ORDER		= EXT_CAM_IMG_PIXEL_ORDER;

	DISPLAY_IMG_START_X = 0;
	DISPLAY_IMG_START_Y = 0;
	DISPLAY_IMG_WIDTH   = IMG_WIDTH;
	DISPLAY_IMG_HEIGHT = IMG_HEIGHT;

	if (EXT_CAM_IMG_FORMAT == IMG_RAW){
		memcpy(s_aa::img_buf, shm->img_buf, IMG_WIDTH*IMG_HEIGHT*2);
		pixe_width = ((IMG_RAW_FORMAT == RAW_8BIT)?1:2);
		result = parse_raw(s_aa::img_buf, s_aa::bmp_buf, IMG_WIDTH, IMG_HEIGHT, (float)pixe_width);
	}
	else if (EXT_CAM_IMG_FORMAT == IMG_MIPIRAW){
		memcpy(s_aa::img_buf, shm->img_buf, IMG_WIDTH*IMG_HEIGHT*2);
		if (IMG_RAW_FORMAT == RAW_8BIT){
			make_demosaic(s_aa::bmp_buf, s_aa::img_buf, IMG_WIDTH, IMG_HEIGHT);
		}
		else if (IMG_RAW_FORMAT == RAW_10BIT){
			BYTE *raw8 = new BYTE[IMG_WIDTH*IMG_HEIGHT];
			ai_img::mipi10_to_raw8(s_aa::img_buf, raw8, IMG_WIDTH, IMG_HEIGHT);
			make_demosaic(s_aa::bmp_buf, raw8, IMG_WIDTH, IMG_HEIGHT);
			delete [] raw8;
		}
		else if (IMG_RAW_FORMAT == RAW_12BIT){
			BYTE *raw12 = new BYTE[IMG_WIDTH*IMG_HEIGHT*2];
			BYTE *raw8 = new BYTE[IMG_WIDTH*IMG_HEIGHT];
			ai_img::mipi12_to_raw12(s_aa::img_buf, raw12, IMG_WIDTH, IMG_HEIGHT);
			ai_img::raw12_to_raw8(raw12, raw8, IMG_WIDTH, IMG_HEIGHT);
			make_demosaic(s_aa::bmp_buf, raw8, IMG_WIDTH, IMG_HEIGHT);
			delete [] raw8;
			delete [] raw12;
		}
		else result = FALSE;
	}
	else if (EXT_CAM_IMG_FORMAT == IMG_JPG  || EXT_CAM_IMG_FORMAT == IMG_GIF ||
		EXT_CAM_IMG_FORMAT == IMG_TIFF || EXT_CAM_IMG_FORMAT == IMG_PNG){
			result = parse_img_stream(shm->img_buf, EXT_CAM_IMG_FORMAT);
	}
	else if (EXT_CAM_IMG_FORMAT == IMG_BMP){
		memcpy(s_aa::bmp_buf, shm->img_buf, IMG_WIDTH*IMG_HEIGHT*IMG_DEPTH);
		result = TRUE;
	}

	return result;
}

int get_img_stream(void)
{
	if (IMG_CAPTURE_READY == FALSE) return FALSE;

	int result = FALSE;
	int timeout = 100;

	if (SELECT_IMG_SOURCE == IMG_SOURCE_EXTERN_DEVICE){
		EXTERN_INPUT_IMG_SOURCE_OCCUPIED = TRUE; 

		HANDSHAKE_REQUEST_IMG_SIGNAL = TRUE;
		HANDSHAKE_RECEIVE_SIGNAL = EXTERN_IMG_DEVICE_GET_IMG;
		while (HANDSHAKE_REQUEST_IMG_SIGNAL == TRUE && timeout-->=0) Sleep(10);
		if (timeout>=0 && HANDSHAKE_REQUEST_IMG_SIGNAL == FALSE){
			result = extern_cam_output();
		}
		else result = FALSE;
		EXTERN_INPUT_IMG_SOURCE_OCCUPIED = FALSE;

	}
	else if (SELECT_IMG_SOURCE == IMG_SOURCE_FILE){
		result = TRUE; // default s_aa::img_buf has img in
	}
	else if (SELECT_IMG_SOURCE == IMG_SOURCE_INTERNAL_DEVICE){
		if (IMG_FORMAT == IMG_BMP){
			result = s_aa::pimhw->get_image(s_aa::bmp_buf);
		}
		else if (IMG_FORMAT == IMG_RAW && IMG_RAW_FORMAT == RAW_MIPI10BIT){
			result = get_image_ex(s_aa::bmp_buf, IMG_DUMMY_CNT, IMG_AVERAGE_CNT);
//			result = s_aa::pimhw->get_image(s_aa::img_buf);
// 			if (result == TRUE){
// 				ai_img::mipi10_to_raw8(s_aa::img_buf, s_aa::raw8_buf, IMG_WIDTH, IMG_HEIGHT);
// 				make_demosaic(s_aa::bmp_buf, s_aa::raw8_buf, IMG_WIDTH, IMG_HEIGHT);
// 			}
		}
		else result = FALSE;
	}
	return result;
}

int check_img_file_status(HWND hdlg, char *path)
{
	char save_file_message[MAX_PATH] = {0};
	if (_access(path, 0) == FALSE){
		sprintf(save_file_message, "%s is already exists. Do you want to replace it?", path);
		if (MessageBox(hdlg, save_file_message, "Save as", MB_YESNO|MB_ICONWARNING) == IDNO){
			return FALSE;
		}

		if (_access(path, 6) == -1){
			MessageBox(hdlg, "Could not save file.", "File save error", MB_OK|MB_ICONSTOP);
			return FALSE;
		}
	}

	return TRUE;
}

int save_bmp24_img_file(HWND hdlg, char *path)
{
	if (strcmp((path+strlen(path)-4), ".bmp") != 0){
		strcat(path, ".bmp");
	}
	if (check_img_file_status(hdlg, path) == FALSE)  return FALSE;

	int result = FALSE;
	if (IMG_DEPTH == 3){
		result = ai_img::save_bmp24_image(path, s_aa::bmp_buf, IMG_WIDTH, IMG_HEIGHT);
	}
	else{
		MessageBox(hdlg, "Image source format can't convert to bmp24", "Error", 0);
	}

	return result;
}

int save_bmp8_img_file(HWND hdlg, char *path)
{
	char save_file_message[MAX_PATH] = {0};
	if (strcmp((path+strlen(path)-4), ".bmp") != 0){
		strcat(path, ".bmp");
	}
	if (check_img_file_status(hdlg, path) == FALSE){
		return FALSE;
	}

	int result = FALSE;
	if (IMG_DEPTH == 3){
		BYTE *bmp8 = new BYTE[IMG_WIDTH*IMG_HEIGHT];
		for (int j=0; j<IMG_HEIGHT; j++){
			for (int i=0; i<IMG_WIDTH; i++){
				bmp8[j*IMG_WIDTH+i] = s_aa::bmp_buf[j*IMG_WIDTH*3 + i*3+1];
			}
		}
		result = ai_img::save_bmp8_image(path, bmp8, IMG_WIDTH, IMG_HEIGHT);
		delete [] bmp8;
	}
	else{
		result = ai_img::save_bmp8_image(path, s_aa::bmp_buf, IMG_WIDTH, IMG_HEIGHT);
	}

	return result;
}

int save_jpeg_img_file(HWND hdlg, char *path)
{
	if (strcmp((path+strlen(path)-4), ".jpg") != 0 || strcmp((path+strlen(path)-5), ".jpeg") != 0){
		strcat(path, ".jpg");
	}

	if (check_img_file_status(hdlg, path) == FALSE){
		return FALSE;
	}

	return save_stream_to_image_file(s_aa::bmp_buf, FIF_JPEG, path);
}

int save_png_img_file(HWND hdlg, char *path)
{
	if (strcmp((path+strlen(path)-4), ".png") != 0){
		strcat(path, ".png");
	}

	if (check_img_file_status(hdlg, path) == FALSE){
		return FALSE;
	}

	return save_stream_to_image_file(s_aa::bmp_buf, FIF_PNG, path);
}

int save_tiff_img_file(HWND hdlg, char *path)
{
	if (strcmp((path+strlen(path)-4), ".tif") != 0 || strcmp((path+strlen(path)-5), ".tiff") != 0){
		strcat(path, ".tif");
	}
	if (check_img_file_status(hdlg, path) == FALSE){
		return FALSE;
	}

	return save_stream_to_image_file(s_aa::bmp_buf, FIF_TIFF, path);
}

int save_img_file(HWND hdlg)
{
	int result = TRUE;
	char path[MAX_PATH] = {0};
	int n = ai_win::save_file(hdlg, path, FILE_TYPE_IMAGES);
	switch (n)
	{
	case 1: //bmp24
		result = save_bmp24_img_file(hdlg, path);
		break;

	case 2: //bmp8
		result = save_bmp8_img_file(hdlg, path);
		break;

	case 6: //jpeg
		result = save_jpeg_img_file(hdlg, path);
		break;

	case 7: //png
		result = save_png_img_file(hdlg, path);
		break;

	case 8:
		result = save_tiff_img_file(hdlg, path);
		break;

	default:
		result = FALSE;
		break;
	}
	log("save image:%s", path);

	return result;
}

int save_scenry_bmp24(HWND hdlg, char *path, int start_x, int start_y, int w, int h)
{
	if (w%8 != 0) w = w+ (8-w%8);
	if (strcmp((path+strlen(path)-4), ".bmp") != 0){
		strcat(path, ".bmp");
	}
	if (check_img_file_status(hdlg, path) == FALSE)  return FALSE;

	int result = FALSE;
	if (IMG_DEPTH != 3){
		log("image depth is not 24bit.");
		return FALSE;
	}

	BYTE *mybmp = new BYTE[w*h*3];
	for (int j=start_y; j<start_y+h; j++){
		for (int i=start_x; i<start_x+w; i++){
			mybmp[(h-1-j+start_y)*w*3+(i-start_x)*3]   = s_aa::bmp_buf[(IMG_HEIGHT-1-j)*IMG_WIDTH*3+i*3];
			mybmp[(h-1-j+start_y)*w*3+(i-start_x)*3+1] = s_aa::bmp_buf[(IMG_HEIGHT-1-j)*IMG_WIDTH*3+i*3+1];
			mybmp[(h-1-j+start_y)*w*3+(i-start_x)*3+2] = s_aa::bmp_buf[(IMG_HEIGHT-1-j)*IMG_WIDTH*3+i*3+2];
		}
	}
	result = ai_img::save_bmp24_image(path, mybmp, w, h);
	delete [] mybmp;

	return result;
}

int save_scenery_image(HWND hdlg)
{
	int result = FALSE;
	char path[MAX_PATH] = {0};
	int n = ai_win::save_file(hdlg, path, FILE_TYPE_BMP_RAW);
	switch (n)
	{
	case 1: //bmp24
		result = save_scenry_bmp24(hdlg, path, DISPLAY_IMG_START_X, DISPLAY_IMG_START_Y, DISPLAY_IMG_WIDTH, DISPLAY_IMG_HEIGHT);
		break;
	}
	log("scenery image saved.[%d]", result);
	return result;
}

int save_info_image(HWND hdlg)
{
	int result = FALSE;
	char path[MAX_PATH] = {0};
	int n = ai_win::save_file(hdlg, path, FILE_TYPE_BMP_ONLY);
	if (n == 1){
		result = s_aa::myimg.save_dib_to_bmp(path, IMG_WIDTH, IMG_HEIGHT);
	}

	return result;
}

int average_img(BYTE *data, int *buffer, BYTE **data_array, int pixel_count, int *counting, int avg_cnt)
{
	int max_count = pixel_count;
	int result = TRUE;
	int cnt = *counting;

	if (cnt == (avg_cnt-1)){
		if (avg_cnt > 1){
			for (int i=0; i<max_count; i+=4){ //increase i steps, i+=8,12, it will save time
				for (int j=0; j<cnt; j++){
					buffer[i]   += data_array[j][i];
					buffer[i+1] += data_array[j][i+1];
					buffer[i+2] += data_array[j][i+2];
					buffer[i+3] += data_array[j][i+3];
				}

				data[i]   = (BYTE)((buffer[i]+data[i])/avg_cnt);
				data[i+1] = (BYTE)((buffer[i+1]+data[i+1])/avg_cnt);
				data[i+2] = (BYTE)((buffer[i+1]+data[i+2])/avg_cnt);
				data[i+3] = (BYTE)((buffer[i+1]+data[i+3])/avg_cnt);
			}
		}

		result = TRUE;
	}
	else{
		memcpy(data_array[cnt], data, pixel_count);
		(*counting) += 1;
		result = FALSE;
	}

	return result;
}

BOOL open_single_img_file(HWND hdlg)
{
	int result = TRUE;
	char img_path[MAX_PATH] = {0};
	if (ai_win::open_file(hdlg, img_path)){
		result = read_img_file(img_path);
		//读取文件大小
		HANDLE hfile = CreateFile(img_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hfile!= INVALID_HANDLE_VALUE){
			int file_size = GetFileSize(hfile, NULL);
			ai_win::set_dlg_item_float(HWND_IMG_INFO, IDC_IMAGE_TOTAL_SIZE3, (float)file_size/(1024*1024), 2);
		}
		CloseHandle(hfile);	
	}

	return result;
}

int ImgFile::init(int id)
{
	log("img file init.");
	return TRUE;
}

int ImgFile::run()
{
	log("img file run.");
	int img_size = 0;
//	int result = open_single_img_file(HWND_MAIN, s_aa::img_buf, s_aa::bmp_buf, IMG_FORMAT, IMG_WIDTH, IMG_HEIGHT, IMG_DEPTH, img_size);
	//	log("")
	int result = open_single_img_file(HWND_MAIN);
	return result;
}

int ImgFile::cap(BYTE *buf, int buf_size)
{
	//log("img file capture.");
	return TRUE;
}

int ImgFile::stop()
{
	return TRUE;
}

int ImgFile::destroy()
{
	return TRUE;
}

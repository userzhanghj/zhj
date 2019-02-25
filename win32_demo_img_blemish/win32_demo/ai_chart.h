#ifndef _AI_CHART_H_
#define _AI_CHART_H_

#ifndef AI_EXPORT_CHART
#define AI_EXPORT_CHART extern "C" __declspec(dllimport) 
#endif

#define WHITE	0XFF
#define BLACK	0X00

#define EDGE_CENTER_WIDTH					32 
#define FFT_NUM								1024	//2^FFT_COFF
#define LSF_EFFECTIVE_DATA_OFFSET			(((FFT_NUM)-(EDGE_CENTER_WIDTH)*4)/2)
#define FFT_DISPLAY_NUM						(FFT_NUM/4)

typedef struct _tagBlock
{
	int is_white;
	int qualified;
	int width;
	int height;
	int center_x;
	int center_y;
	double sfr_val[4];
	double esf[2000];
	double lsf[2000];
	double freq[1000];
	double amp[1000];
}Block;


namespace multi_sfr
{
	AI_EXPORT_CHART int  fine_tuning_block_center(const BYTE *buf, int w, int h, 
												  int offset_x, int offset_y, 
												  int start_x, int start_y, 
												  int block_size_up_limit, 
												  Block &block);
	AI_EXPORT_CHART int  get_neigbor_block_center(const BYTE *buf, int w, int h, 
												  int offset_x, int offset_y, 
												  int start_x, int start_y, 
												  int block_size_low_limit, 
												  int block_size_up_limit, 
												  int direct, Block &block);
};

namespace cross_line
{
	AI_EXPORT_CHART int    get_center(const BYTE *bmp, int bmp_w, int bmp_h, 
								      RECT sb_rt, int cl_size, int cl_thickness, int &c_x, int &c_y);
	AI_EXPORT_CHART double get_rotated_angle(int *cl_center_x, int *cl_center_y);
	AI_EXPORT_CHART float  get_2p_distance(float x1, float y1, float x2, float y2);
	AI_EXPORT_CHART void   get_rotated_new_xy(double x, double y, double angle, double &new_x, double &new_y);
};

namespace slanted_bw
{
	AI_EXPORT_CHART int get_target_center(BYTE *bin, int w, int h, 
										  int pattern_size, 
										  int corner_size, 
										  int &center_x, int &center_y, 
										  int *edge_x, int *edge_y);
	AI_EXPORT_CHART int get_pattern_center(BYTE *p, int w, int h, 
										   int pattern_size, 
										   int &center_x, int &center_y, 
										   int *edge_x, int *edge_y);
	AI_EXPORT_CHART int get_pattern_center2(BYTE *p, int w, int h, 
											int pattern_size, 
											int &center_x, int &center_y, 
											int *edge_x, int *edge_y);
	AI_EXPORT_CHART int find_roi_h_edge(const BYTE *roi, int start_x, int start_y, 
										int w, int h, 
										double *edge_x, 
										double&slope, 
										double &offset, 
										int &left_black);
	AI_EXPORT_CHART int find_roi_v_edge(const BYTE *roi, 
										int start_x, int start_y, 
										int w, int h, 
										double *edge_y, 
										double&slope, 
										double &offset, 
										int &left_black);

	AI_EXPORT_CHART void get_cross_xy(double slope0, 
									  double offset0, 
									  double slope1, 
									  double offset1, 
									  double &cross_x, 
									  double &cross_y);
}

namespace color
{
	AI_EXPORT_CHART float get_color_delta(BYTE *roi_bmp, int roi_w, int roi_h, int ref_l, int ref_a, int ref_b, int use_lab);//use_lab=1
	AI_EXPORT_CHART double get_snr(BYTE *roi_bmp_y, int roi_w, int roi_h);
	AI_EXPORT_CHART float get_wb_delta(BYTE *roi_bmp, int roi_w, int roi_h);
	AI_EXPORT_CHART void get_avg_lab(BYTE *roi_bmp, int roi_w, int roi_h, double *avg_l, double *avg_a, double *avg_b);
	AI_EXPORT_CHART void get_avg_rgb(BYTE *roi_bmp, int roi_w, int roi_h, float *r,float* g,float *b);

}
namespace fov
{
	AI_EXPORT_CHART int get_fov(BYTE *roi_bmp, int roi_w, int roi_h);
}
#endif
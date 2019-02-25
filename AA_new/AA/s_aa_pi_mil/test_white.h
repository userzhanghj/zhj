#ifndef _TEST_WHITE_H_
#define _TEST_WHITE_H_

typedef struct _tagWhite
{
	double test_time;
	int test_result;

	int area_start_x;
	int area_start_y;

	int area_h_width;
	int area_v_height;

	int is_particle_test_on;
	int particle_test_result;
	float particle_up_limit;
	float particle_low_limit;
	float particle_up_result;
	float particle_low_result;
	int is_particle_draw_on;


	int is_blemish_test_on;
	int blemish_test_result;
	int blemish_radius;
	int blemish_threshold;
	int blemish_close;

	int blemish_shrink_divider;
	int blemish_pattern_width;
	int blemish_pattern_height;

	int blemish_scale_x;
	int blemish_scale_y;
	int is_blemish_draw_on;


	int is_ls_test_on;
	int ls_test_result;
	int is_lens_shading_draw_on;
	int ls_start_x;
	int ls_start_y;
	int ls_test_spec;
	int ls_box_size_ratio;
	double ls_val[5];
	int ls_result[5];
	POINT ls_box[5];
	int ls_box_w;
	int ls_box_h;
	int blemish_useX;
	int blemish_useY;

	int    is_oc_test_on;
	int    oc_test_result;
	int	   is_oc_draw_on;
	float  oc_limit_percent;
	double oc_result_percent;
	int    oc_limit_pixel;
	POINT  oc_result_pos;
	int    oc_x_low_limit;
	int	   oc_x_up_limit;
	int    oc_y_low_limit;
	int    oc_y_up_limit;
	int    oc_x_result;
	int    oc_y_result;


	int is_save_raw_image;
	int is_save_bmp_image;
	int is_save_result_image;
	int is_save_white_report;

	char report_path[MAX_PATH];
	int is_report_path_on;
}White_V;





#endif
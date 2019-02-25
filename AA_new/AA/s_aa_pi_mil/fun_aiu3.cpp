#define _CRT_SECURE_NO_WARNINGS
#include "inf_cap_bd.h"
#include "img_cap_device.h"

#pragma comment(lib, "ai_u3_inf.lib")

int AIU3_BD::init(int id)
{
	log("recv aiu3 init.");
	char dir[MAX_PATH] = {0};
	ai_win::get_current_directory(dir);

	char sensor_name[100] = {0};
	char path[MAX_PATH] = {0};
	sprintf(path, "%s\\recipe\\config.ini", dir);
	ai_win::read_string_key(path, "system", "default_recipe_setting", sensor_name, sizeof(sensor_name));
	sprintf(sensor_path, "%s\\recipe\\%s\\%s.ini", dir, sensor_name, sensor_name);

	int dev_cnt = open_device(id);
	log("dev_cnt = %d",dev_cnt);
	if (dev_cnt>0){
		return TRUE;
	}

	return FALSE;
}

int AIU3_BD::run()
{
	int result = init_sensor(sensor_path);
	if (result >0){
		result = set_image_on(dev_id);
	}
	return result;
}

int AIU3_BD::cap(BYTE *buf, int buf_size)
{
	int pixel_cnt = get_image_pixel_cnt();
//	log("pix_cnt:%d",pixel_cnt);
	int rt = get_image_frame(pixel_cnt, buf);

	return rt;
}

int AIU3_BD::stop()
{
	set_sensor_power_off();
	set_sensor_mclk_off();
	set_sensor_io_off();

	set_image_off(para.bd_id);
	return TRUE;
}

int AIU3_BD::destroy()
{
	if (is_device_open(para.bd_id)== TRUE){
		destroy_device(para.bd_id);
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
BOOL set_sensor_power_on()
{
	int result = FALSE;
	for (int i=0; i<4; i++){
		if (para.power_ch_on[i]){
			result = set_program_power(i, para.power_ch_vol[i]);
			if (result != TRUE){
				log("power ch %d on fail.", i);
				break;
			}
		}
	}

	if (result){
		result = set_program_power(7, para.io_level);
		if (result == FALSE){
			log("io level set fail.");
		}
	}

	if (result)	log("set all power on OK.");

	return result;
}

BOOL set_sensor_power_off(void)
{
	int result = FALSE;
	for (int i=0; i<4; i++){
		if (para.power_ch_on[i]){
			result = set_program_power(i, 0.0f);
			if (result == FALSE){
				log("power ch %d off fail.", i);
				break;
			}
		}
	}
	return result;
}

BOOL set_sensor_mclk_on()
{
	log("mclk=%f", para.mclock);
	float freq = para.mclock;

	int result = set_mclk(freq);
	if (result <=0){
		log("set mclk val=%.1f fail.", para.mclock);
	}

	return result;
}

BOOL set_sensor_mclk_off()
{
	int result = set_mclk(0);
	log("set mclk off %s", ((result<=0)?"fail":"OK"));
	return result;
}

BOOL set_sensor_io_on()
{
	int result =TRUE;

	if (para.pwdn_level == TRUE){
		set_pwdn(TRUE);	 Sleep(3); 
		set_pwdn(FALSE); Sleep(3);
		set_pwdn(TRUE);
	}
	else{
		set_pwdn(TRUE);  Sleep(3);
		set_pwdn(FALSE); Sleep(3);
	}

	if (para.reset_level == TRUE){
		set_reset(TRUE);  Sleep(3);
		set_reset(FALSE); Sleep(3);
		set_reset(TRUE);  Sleep(1);
	}
	else{
		set_reset(FALSE); Sleep(3);
		set_reset(TRUE);  Sleep(3);
		set_reset(FALSE); 
	}

	if (result == TRUE){
		for (int i=0; i<4; i++){
			result = set_gpio(i, para.gpio[i]);
			if (result == FALSE){
				log("set gpio%d=%d fail.", i, para.gpio[i]);
				break;
			}
		}
	}

	if (result == TRUE){
		log("set IO level ON OK.");
	}

	return result;
}

BOOL set_sensor_io_off()
{
	int result = FALSE;
	result = set_reset(FALSE);
	if (result == FALSE){
		log("set reset off fail.");
	}

	result = set_pwdn(FALSE);
	if (result == FALSE){
		log("set pwdn off fail.");
	}

	if (result == TRUE){
		for (int i=0; i<4; i++){
			result = set_gpio(i, FALSE);
			if (result == FALSE){
				log("set gpio%d off fail.", i);
				break;
			}
		}
	}
	if (result == TRUE){
		log("set IO level off OK.");
	}

	return result;
}

BOOL run_register(const char *ini_path, char *section)
{
	char line_buf[500] = {0};
	int  start_write = FALSE;
	char tmp_cmd[50] = {0};
	int result = FALSE;
	log("register file: %s", ini_path);
	FILE *fp = fopen(ini_path, "r");
	if (fp == NULL){
		return FALSE;
	}

	memset(line_buf, 0, 500);
	while (fgets(line_buf, 500, fp)){
		memset(tmp_cmd, 0, strlen(tmp_cmd));
		sscanf(line_buf, "%s", tmp_cmd);
		if (0 == strcmp(section, tmp_cmd)){ //mode
			log("%s", tmp_cmd);
			if (start_write == FALSE) start_write = TRUE;
		}
		else if (0 == strcmp("[END]", tmp_cmd) && start_write == TRUE){
			start_write = FALSE;
			break;
		}

		if (start_write){
			result = parse_cmd(line_buf);
			if (result == FALSE){
				break;
			}
		}
	}
	log("run register ok.");
	fclose(fp);
	return TRUE;
}

int parse_cmd(char *line_buf)
{
	BYTE slave;
	LONG addr, data;
	WORD reg_addr, reg_data;
	char tmp_cmd[50] = {0};
	char tmp_data[50] = {0};
	int tmp_idata = 0;
	int n = 0;
	float f = 0.0f;
	for (int i=0; i<5; i++){
		line_buf[i] = toupper(line_buf[i]);
	}

	if (line_buf[0] == '0' && line_buf[1] == 'X'){
		sscanf(line_buf, "%hx%hx", &reg_addr, &reg_data);
		log("M=%d, 0x%04x, 0x%02x", para.i2c_mode, reg_addr, reg_data);
		if (!write_i2c(para.slave_id, para.i2c_mode, reg_addr, reg_data)){
			if (!write_i2c(para.slave_id, para.i2c_mode, reg_addr, reg_data)){
				log("I2C Fail:%x,%x", reg_addr, reg_data);

				return FALSE;
			}
		}
		//		
		Sleep(0);

	}
	if (line_buf[0] == 'F' && line_buf[1] == 'P'){
		sscanf(line_buf, "%s%hx%x%x", tmp_cmd, &slave, &addr, &data);
		log("%s 0x%02x 0x%02x 0x%02x", tmp_cmd, slave, addr, data);
		if (line_buf[4] == '0'){
			if (write_i2c_fpga_08_08(slave, addr, data) != TRUE){
				if (write_i2c_fpga_08_08(slave, addr, data) != TRUE){
					log("Write FPGA I2C Fail.");
				}
			}
		}
		else if (line_buf[4] == '1'){
			if (write_i2c_fpga_16_08(slave, addr, data) != TRUE){
				if (write_i2c_fpga_16_08(slave, addr, data) != TRUE){
					log("Write FPGA I2C Fail.");
				}
			}
		}
		else if (line_buf[4] == '2'){
			if (write_i2c_fpga_08_16(slave, addr, data) != TRUE){
				if (write_i2c_fpga_08_16(slave, addr, data) != TRUE){
					log("Write FPGA I2C Fail.");
				}
			}
		}
		else if (line_buf[4] == '3'){
			if (write_i2c_fpga_16_16(slave, addr, data) != TRUE){
				if (write_i2c_fpga_16_16(slave, addr, data) != TRUE){
					log("Write FPGA I2C Fail.");
				}
			}
		}
	}
	else if (line_buf[0] == 'S' && line_buf[1] == 'L' && line_buf[2] == 'E'){
		sscanf(line_buf, "%s%s", tmp_cmd, tmp_data);
		tmp_idata = atoi(tmp_data);
		Sleep(tmp_idata);
		log("SLEEP %d", tmp_idata);
	}
	else if (line_buf[0] == 'S' && line_buf[1] == 'L' && line_buf[2] == 'A'){
		sscanf(line_buf, "%s%hx", tmp_cmd, &para.slave_id);
		log("slave id=0x%02x", para.slave_id);
	}
	else if (line_buf[0] == 'E' && line_buf[1] == 'N' && line_buf[2] == 'B'){
		sscanf(line_buf, "%s%s", tmp_cmd, tmp_data);
		n = atoi(tmp_data);
		set_pwdn(n);
	}
	else if (line_buf[0] == 'R' && line_buf[1] == 'E' && line_buf[2] == 'S'){
		sscanf(line_buf, "%s%s", tmp_cmd, tmp_data);
		n = atoi(tmp_data);
		set_reset(n);
	}
	else if (line_buf[0] == 'I' && line_buf[1] == '2' && line_buf[2] == 'C' && line_buf[3] == 'M'){
		sscanf(line_buf, "%s%s", tmp_cmd, tmp_data);
		para.i2c_mode = atoi(tmp_data);
	}
	else if (line_buf[0] == 'I' && line_buf[1] == '2' && line_buf[2] == 'C' && line_buf[3] == 'T'){
		sscanf(line_buf, "%s%s", tmp_cmd, tmp_data);
		para.i2c_type = atoi(tmp_data);
	}
	else if (line_buf[0] == 'M' && line_buf[1] == 'C' && line_buf[2] == 'L' && line_buf[3] == 'O'){
		sscanf(line_buf, "%s%s", tmp_cmd, tmp_data);
		f = (float)atoi(tmp_data);
		if (f>64) f = 24.0f;
		if (f<0)  f = 0.0f;
		set_mclk((DWORD)f);
	}

	memset(line_buf, 0, strlen(line_buf));

	return TRUE;
}

int import_reg_cmd(HWND hdlg, const char *ini_path, const char *section)
{
	char reg_buffer[REG_LIST_SIZE] = {0};
	GetPrivateProfileSection(section, reg_buffer, sizeof(reg_buffer), ini_path);

	char delimiter[] = "\n";

	char *token= reg_buffer;
	if (token == NULL) return FALSE;

	int n = 0;
	int line_limit = 500;
	while ((n=strlen(token))>0 && line_limit-- >0){	
		for (int i=0; i<n; i++){
			if (*(token+i) == 0x09){
				*(token+i) = 0x20;
			}
		}
		LISTBOX_ADDSTR(hdlg, IDC_REG_LIST, token);
		token = token+n+1;
	}

	return TRUE;	
}

BOOL decode_input_image_data()
{
	if (para.interface_type == 0){
		select_input(1);
		if (para.output_format == 0 || para.output_format == 1)      set_data_mode(0);
		else if (para.output_format == 2 || para.output_format == 3) set_data_mode(1);
	}
	else{
		select_input(0);
		if (para.interface_type == 1)	   set_data_mode(0);
		else if (para.interface_type == 2) set_data_mode(1);
		else if (para.interface_type == 3) set_data_mode(2);
		else if (para.interface_type == 4) set_data_mode(3);
	}


	int x, y;
	int result = get_image_resolution(&x, &y);
	if (result == FALSE){
		log("get image resolution fail.");
		return FALSE;
	}

	log("get image resolution:x=%d, y=%d", x, y);

	return TRUE;
}

int init_sensor(char *path)
{
	int result = TRUE;
	if (para.select_power_on){
		result = set_sensor_power_on();
		if (result == FALSE){
			log("set sensor power on fail.");
			set_sensor_power_off();
			return FALSE;
		}
	}

	if (para.select_mclk_on){
		result = set_sensor_mclk_on();
		if (result == FALSE){
			log("set sensor mclk on fail.");
			set_sensor_mclk_off();
			return FALSE;
		}
	}

	Sleep(10);

	if (para.select_io_on){
		result = set_sensor_io_on();
		if (result == FALSE){
			log("set sensor io on fail.");
			set_sensor_io_off();
			return FALSE;
		}
	}
	set_vsync_polar(para.vsync_polarity);
	set_pclock_polar(para.pclk_polarity);
	log("v=%d, p=%d", para.vsync_polarity, para.pclk_polarity);

	if (para.select_register_on){
		result = run_register(path, "[Register]");

		if (result == FALSE){
			log("run register fail.");
			return FALSE;
		}
	}

	if (para.select_image_on){
		result = decode_input_image_data();
		if (result == FALSE){
			log("decode input image data fail.");
			return FALSE;
		}
	}
	

	return TRUE;
}

int get_image_pixel_cnt(void)
{
	int pixel_cnt = 0;
	int	 mipidivider = 4, countperpixel=1, pixelalign=5;

	if (para.output_format == 0){
		pixelalign = 5;
		mipidivider = 5;   //8bit : 5/5
		countperpixel = 2; //twice byte size for YUV. 
	}
	else if (para.output_format == 1){
		pixelalign = 5;
		mipidivider = 5;   //8bit : 5/5
		countperpixel = 1;
	}
	else if (para.output_format == 2){
		pixelalign = 5;
		mipidivider = 4;   //10bit MIPI : 5/4
		countperpixel = 1;
	}
	else{
		;
	}

	pixel_cnt = countperpixel*(para.image_width*pixelalign/mipidivider)*para.image_height;

	return pixel_cnt;
}



#ifndef _common_struct
#define  _common_struct





#pragma pack(1)
typedef struct{
	unsigned char  barcode_address[5];
  unsigned char  barcode_reserved[7];
	unsigned short barcode_id;
	unsigned char  reserved[4];
}Device;


#pragma pack(1)
typedef struct{
	unsigned short saved_device_cnt;
	unsigned int   update_code;
}Device_control;

#pragma pack(1)
typedef struct{
 unsigned char wifi_name_password_change;
 unsigned char user_able2_program_vcm_en;
 unsigned char reserved[2];	
}CONFIG;

#pragma pack(1)
typedef struct{
	char str[64];
}string;



#pragma pack(1)
typedef struct{
	unsigned char prog_stat;
	unsigned char prog_mem;
	unsigned char resrved[3];
}VCM_PROG;




#define E2_START_PAGE_CONFIG                      50
#define E2_START_PAGE_CONFIG_SIZE                 4

#define E2_START_PAGE_WIFI_PASSWORD               59
#define E2_START_PAGE_WIFI_NAME                   55
#define E2_START_PAGE_ADMIN_LOGINE_PASSWORD       51


#define E2_START_PAGE_DEVICE                      64
#define E2_START_PAGE_DEVICE_CONTROL              63
#define DEVICE_MAX_CNT                            22

extern Device      device[DEVICE_MAX_CNT];
extern Device_control  dc_control;


extern void c_struct_init(void);
extern void e2_read_device(Device *dev,int address);
extern void e2_write_device(Device *dev,int address);
extern void e2_read_device_contol(Device_control *dc);
extern void e2_write_device_contol(Device_control *dc);

extern void e2_read_config(CONFIG *dc);
extern void e2_write_config(CONFIG *dc);

extern void e2_read_wifi_password(string *str);
extern void e2_write_wifi_password(string *str);

extern void e2_read_wifi_name(string *str);
extern void e2_write_wifi_name(string *str);

extern void e2_read_admin_password(string *str);
extern void e2_write_admin_password(string *str);

#endif












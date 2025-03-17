#ifndef _cmd_f_
#define _cmd_f_
#include "cmsis_os.h"


#define CMD_POCKET_MAX_DATA_SIZE 64

typedef struct{
	unsigned char source;
	unsigned char table_locate;
	unsigned short cmd;
}CMD_format;



typedef struct{
	CMD_format cmd;
	int        arg;
	int        size;
	char       data[CMD_POCKET_MAX_DATA_SIZE];
  char       device_id;
}CMD_pocket;

////////////////////////////////////////////////////////////////////////////
#define CMD_SAVED_CNT  20

#define CMD_DEST_WIFI_RESPOND_Q          0  
#define CMD_DEST_ADVANCE_OPTION_Q        1
#define CMD_DEST_NRF_OPTION_DE_Q         2
#define CMD_DEST_NRF_OPTION_DC_Q         3
#define CMD_DEST_WM_Q                    4


#define CMD_CODE_ERROR          0 
#define CMD_CODE_DEGET          0X62 
#define CMD_CODE_DESET          0XD8
#define CMD_CODE_DESTT          0XB0


#define CMD_CODE_DCGID          0X1
#define CMD_CODE_DCSTP          0X2
#define CMD_CODE_DCDEL          0X3
#define CMD_CODE_DCCNT          0X4
#define CMD_CODE_DCUPD          0X5


#define CMD_CODE_COCCT          0X45
#define CMD_CODE_COCLP          0X46
#define CMD_CODE_COCWN          0X47
#define CMD_CODE_COCWP          0X48
#define CMD_CODE_COLGI          0X49


#define CMD_CODE_VCADD          0X70
#define CMD_CODE_VCDEL          0X71
#define CMD_CODE_VCRUN          0X72
#define CMD_CODE_VCSTT          0X75
#define CMD_CODE_VCSTO          0X73
#define CMD_CODE_VCPRI          0X74



extern const char cmd_saved_strings[CMD_SAVED_CNT][6];
extern const  CMD_format cmd_saved_inf[CMD_SAVED_CNT];

///////////////////////////////////////////////////////////////////////////

extern osMailQId wifi_respond_q;

extern osMailQId advance_option_q;

extern osMailQId nrf_option_de_dc_q;

extern osMailQId wm_q;



////////////////////////////////////////////////////////////////////////////
extern void cmd_init(void);
extern void cmd_finder(char *data,int len,int device_id);
#endif


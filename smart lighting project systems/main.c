#include "cmd_f.h"
#include "wifi_respond.h"
#include "cmsis_os.h"
#include <stdio.h>
#include "gpdma_lpc17xx.h"
#include "ESP_CON.H"
#include "e2.h"
#include "common_struct.h"
#include "nrf_de.h"
#include "co.h"

extern int stdout_init (void);


#define AT_CMD_RESEND  20
#define AT_CMD_ERROR   200

#define AT_RST_CMD_RESEND  10
#define AT_RST_CMD_ERROR   201

#define AT_NOR_CMD_RESEND  4
#define AT_NOR_CMD_ERROR   202

char cmd_data[128];
string __align(4) str0;
string __align(4) str1;
CONFIG __align(4) conf;






int esp_write_patch(char *data,int len,int resend){
	while(resend--){
  esp_write(cmd_data,len);
  if(esp_wait_pass()) return 0;//pass				
	}
	return 1;//error
}



int esp_init_boot(int mode){
int len,gpv0;

switch(mode){
	case 4:
	dc_control.saved_device_cnt=0;	
	dc_control.update_code=0;	
	e2_write_device_contol(&dc_control);
	
	conf.wifi_name_password_change=0;
	e2_write_config(&conf);	
	
	for(gpv0=0;gpv0<DEVICE_MAX_CNT;gpv0++){
	if(device[gpv0].barcode_id){
  device[gpv0].barcode_id=0;
  e2_write_device(&device[gpv0],gpv0);		
	}		
	}
	len=sprintf(str0.str,"kendall_jenner");
	str0.str[len+1]=0;
	e2_write_wifi_name(&str0);
	len=sprintf(str1.str,"asdfghjkl");
	e2_write_wifi_password(&str1);	
	
	len=sprintf(str1.str,"asdfghjkl  ");
	e2_write_admin_password(&str1); //system restarted
  break;
	case 3:
  osDelay(1000);
  len=sprintf(cmd_data,"AT\r\n");
  if(esp_write_patch(cmd_data,len,AT_CMD_RESEND)) return AT_CMD_ERROR;		
	break;
	case 2:
	len=sprintf(cmd_data,"AT+RST\r\n");
  if(esp_write_patch(cmd_data,len,AT_RST_CMD_RESEND)) return AT_RST_CMD_ERROR;
	osDelay(1000);
	case 1:
	len=sprintf(cmd_data,"AT+CWMODE_DEF=2\r\n");
  if(esp_write_patch(cmd_data,len,AT_NOR_CMD_RESEND)) return AT_NOR_CMD_ERROR;
  
	e2_read_wifi_name(&str0);
  e2_read_wifi_password(&str1);
	puts(str0.str);
	len=sprintf(cmd_data,"AT+CWSAP_DEF=\"%s\",\"%s\",1,4\r\n",str0.str,str1.str);
  if(esp_write_patch(cmd_data,len,AT_NOR_CMD_RESEND)) return AT_NOR_CMD_ERROR;  
	
	case 0:
	len=sprintf(cmd_data,"AT+CIPMUX=1\r\n");
  if(esp_write_patch(cmd_data,len,AT_NOR_CMD_RESEND)) return AT_NOR_CMD_ERROR;
	len=sprintf(cmd_data,"AT+CIPSERVER=1,1212\r\n");
  if(esp_write_patch(cmd_data,len,AT_NOR_CMD_RESEND)) return AT_NOR_CMD_ERROR;	
	break;
}



return 0;
}










char de[64];
int main(){
	GPDMA_Initialize();
	stdout_init();

	c_struct_init();
	esp_init();
	
//	esp_init_boot(4); //for first e2 prom reseting
	esp_init_boot(3);
//	esp_init_boot(2); //reset for first configs
//	esp_init_boot(1);
	esp_init_boot(0);  //for afer first test this code
	
	
	wifi_respond_init();
	co_init();
	de_dc_init();

	
	sprintf(de,"System initalled\n");
	puts(de);
	
//	cmd_finder(" //DEGXT/3/2=21                    ",26,0);
//	cmd_finder((char *)"//DSGET/3/2=21                 ",12,1);
//	cmd_finder((char *)" //DXXGET/3/2=21                ",26,0);
//	cmd_finder((char *)" //DEGXXT/3/2=21                ",26,1);	
	

	while(1){
		osDelay(osWaitForever);
	}
}







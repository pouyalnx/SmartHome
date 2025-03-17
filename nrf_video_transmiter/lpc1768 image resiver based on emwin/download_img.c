#include "gui.h"
#include "nrf.h"
#include "cmsis_os.h"
#include "menu.h"
#include <string.h>
#include "format.h"
#include "0.h"
#include "search_sender.h"
#include "lpc17xx.h"


#define TIME_OUT   30
#define TIME_OUT_2 80
int nrf_send(unsigned short *pocket){
	int gpv0;
		
	nrf_set_rf_mode(0);
	nrf_write_data((unsigned char *)pocket,32);
	
	nrf_irq_clr_stat();
	nrf_enable_rf(1);	
	
	for(gpv0=0;gpv0<TIME_OUT;gpv0++){
		osDelay(1);
		if(nrf_irq_stat()) break;
	}
	
	if(nrf_read_stat()&0x20){
	nrf_enable_rf(0);
	return 1;
	}
	
	nrf_irq_clr_stat();
	nrf_write_command(0XE3,0,0);
	for(gpv0=0;gpv0<TIME_OUT_2;gpv0++){
		osDelay(1);
			if(nrf_irq_stat()){
	      
				 if(nrf_read_stat()&0x20){
	       nrf_enable_rf(0);
	       return 1;
	       }
				 
				nrf_irq_clr_stat();
			}
		}
	
	nrf_enable_rf(0);
	return 0;		
		
	}
	
	
int nrf_send_type2(unsigned short *pocket){
	int gpv0;
		
	nrf_set_rf_mode(0);
	nrf_write_data((unsigned char *)pocket,32);
	
	nrf_irq_clr_stat();
	nrf_enable_rf(1);	
	
	for(gpv0=0;gpv0<TIME_OUT;gpv0++){
		osDelay(1);
		if(nrf_irq_stat()) break;
	}
	
	if(nrf_read_stat()&0x20){
	nrf_enable_rf(0);
		LPC_GPIO1->FIOSET=1;
	return 1;
	}
  /*	
	nrf_irq_clr_stat();
	nrf_write_command(0XE3,0,0);
	for(gpv0=0;gpv0<TIME_OUT_2;gpv0++){
		osDelay(1);
			if(nrf_irq_stat()){
	      
				 if(nrf_read_stat()&0x20){
	       nrf_enable_rf(0);
	       return 1;
	       }
				 
				nrf_irq_clr_stat();
			}
		}
	*/
	nrf_enable_rf(0);
	return 0;			
	}	
	
void print_error(int error_code){
  GUI_SetColor(GUI_WHITE);
  GUI_FillRect(10,10,110,110);
	GUI_SetColor(GUI_DARKBLUE);
  GUI_DrawRect(10,10,110,110);	
  GUI_DrawRect(11,11,109,109);	
  GUI_DrawRect(12,12,108,108);		
  GUI_SetBkColor(GUI_WHITE);	
	GUI_SetColor(GUI_BLUE);
	GUI_SetFont(&GUI_Font20B_1);
	GUI_DispStringHCenterAt("ERROR!",60,30);
	GUI_SetFont(GUI_FONT_13_1);
	GUI_DispStringAt("error code:",16,60);  	
  GUI_DispDec(error_code,2);
  GUI_DispStringAt("any key for back...",16,90);	
	lcd_update();
	while(input_read()==0){
		osDelay(50);
	}
  while(input_read()){
		osDelay(50);
	}
}	
	

int dl_image(image_format_t *format){
	int gpv0,gpv1,time_out;
	unsigned short pocket[16];
	nrf_set_rf_mode(1);
	nrf_enable_rf(1);
	nrf_irq_clr_stat();
	nrf_write_command(0xe1,NULL,0);
	nrf_write_command(0xe2,NULL,0);	
	gpv1=format->decoder_init()-1;
	gpv0=0;
	time_out=0;
	while(gpv0<gpv1){
		if(nrf_irq_stat()){
			LPC_GPIO1->FIOSET=2;
			nrf_read_data((unsigned char *)pocket,32);
	nrf_write_command(0xe2,NULL,0);	
			gpv0=format->decoder_data(pocket);
			nrf_irq_clr_stat();
		}
		time_out++; //act as timeout
		if(time_out>20000000) return 0;
	}
	lcd_update();
  return 1;
}



#define TIME_OUT_RESIVER 10
int dl_image_detect(image_format_t *format){
	int gpv0,gpv1;
	unsigned short pocket[16];
	nrf_set_rf_mode(1);
	nrf_irq_clr_stat();	
	nrf_enable_rf(1);
	nrf_write_command(0xe1,NULL,0);
	nrf_write_command(0xe2,NULL,0);
	for(gpv0=0;gpv0<TIME_OUT_RESIVER;gpv0++){
		if(nrf_irq_stat()){
		//	LPC_GPIO1->FIOSET=2;
			nrf_irq_clr_stat();
			nrf_read_data((unsigned char *)pocket,32);
			if(format->decoder_data(pocket)){
				nrf_enable_rf(0);
				return 1;
			}
		}
		osDelay(1);
	}
	nrf_enable_rf(0);
	return 0;
}




#define REPEAT_REQ 80

void download(image_format_t *format){
	int gpv0,gpv1;
	unsigned short pocket[16];
	nrf_set_address(trasmitter_inf.address,0);
	nrf_set_address(trasmitter_inf.address,100);
	nrf_set_channel(trasmitter_inf.channel);
	nrf_write_command(0xe1,NULL,0);
	nrf_write_command(0xe2,NULL,0);	
	pocket[0]='T'|'I'<<8;
  pocket[1]=format->id;
  pocket[2]=0;		
  pocket[3]=0;
	
	LPC_GPIO1->FIODIR|=3;
	LPC_GPIO1->FIOCLR=3;
	
	while(1){	
	for(gpv0=0;gpv0<REPEAT_REQ;gpv0++){
		if(nrf_send_type2(pocket)) break;
  	else if(dl_image_detect(format)) break;
	}		
  if(gpv0==REPEAT_REQ){
    print_error(1);
		return;
	}
	gpv1=dl_image(format);
  if(gpv1==0){
  print_error(2);
	return;
	}
  if(input_read()) break;	
	}
	while(input_read());
	osDelay(20);
}







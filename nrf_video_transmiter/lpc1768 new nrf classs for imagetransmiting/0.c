#include "lpc17xx.h"
#include "nrf.h"
#include "cmsis_os.h"
#include "gpdma_lpc17xx.h"
#include <stdio.h>
#include <string.h>
#include "1.h"
#include "2.h"
#include "3.h"

#define IMAGE_FORMAT_CNT 1

unsigned short image[15370];

typedef struct{
	int (*incoder_init)(void);
	int (*incoder_data)(unsigned short *in,unsigned short id);
	unsigned char id;
}image_format_t;

image_format_t  formats[IMAGE_FORMAT_CNT];

unsigned char tx_address[5]={0xab,0x2c,0xfa,0xf2,0xaa};
unsigned char tx_channel=10;

unsigned char search_address[5]={0xf,0xa,0xf,0xa,25};
unsigned char sof_pocket[32];
unsigned char transmitter_name[16]="video link1     ";


extern void stdout_init(void);
extern void stdout_set(int page,int column);
extern void stdout_invert(int in);
extern void stdout_cmdmode(int in);









int if_rgb565_init(void){
	//120*128/15
	return 1024;
}

int if_rgb565_data(unsigned short *pocket,unsigned short id){
	int gpv0;
	if(id<1024){
		gpv0=id*15;
		memcpy(&pocket[1],&image[gpv0],30);
		pocket[0]=id;
		return 1;
	}
	return 0;
}


#define CAPTURING_REQ      5
#define DOWNLOAD_REQ_WAIT  5





#define RESEND_TIME  50
#define TIME_OUT     40

int transfer_image(image_format_t *data_coder){
	unsigned short data[16];
	int gpv0,gpv1,gpv2;
	
	osDelay(2);
	nrf_set_rf_mode(0);
	
	gpv1=data_coder->incoder_init();

	
	for(gpv0=0;gpv0<gpv1;gpv0++){
			data_coder->incoder_data(data,gpv0);
	    nrf_write_data((unsigned char *)data,32);
		  nrf_irq_clr_stat();
		  nrf_enable_rf(1);
		
		  for(gpv2=0;gpv2<TIME_OUT;gpv2++){
				if(nrf_irq_stat()) break;
				osDelay(1);
			}
		   
		  if((nrf_read_stat()&0x20)==0){
				nrf_write_command(0xe3,NULL,0);
				nrf_irq_clr_stat();
				
				for(gpv2=0;gpv2<TIME_OUT;gpv2++){
					if(nrf_irq_stat()){
					if(nrf_read_stat()&0x20){
					break;	
					}
				  nrf_irq_clr_stat();
					}
					osDelay(1);
				}
				
			}
			
		  nrf_enable_rf(0);		
		
	}
}


#define PAGE_X 128
#define PAGE_Y 120

void image_replaser(const unsigned char *img,unsigned short *out){
	int gpv0,gpv1,gpv2,gpv3,gpv4,gpv5,x;
	unsigned short *img_type;
	img_type=(unsigned short*)img;
  gpv2=img_type[0]*img_type[1]*2;	
	x=img_type[0];
	for(gpv0=4,gpv3=0,gpv4=0,gpv5=0;gpv0<gpv2;gpv0+=2,gpv3++,gpv5++){
		out[gpv3]=img[gpv0]<<8|img[gpv0+1];
		if(gpv5==x){
			gpv4++;
			gpv5=0;
			gpv3=PAGE_X*gpv4;
			if(gpv4>=PAGE_Y) break;
		}
		
	}
}








int main_app(void){
	int gpv0,gpv1,gpv2;
	unsigned char resive_pocket[32];
	gpv0=sprintf((char*)sof_pocket,"TR%s",transmitter_name);
	sof_pocket[18]=tx_address[0];
	sof_pocket[19]=tx_address[1];
	sof_pocket[20]=tx_address[2];
	sof_pocket[21]=tx_address[3];
	sof_pocket[22]=tx_address[4];	
	sof_pocket[23]=tx_channel;
	sof_pocket[24]=0;
	
	
	
	formats[0].id=0;
	formats[0].incoder_data=if_rgb565_data;
	formats[0].incoder_init=if_rgb565_init;
	
	

	nrf_init();		
//	stdout_init();
//	stdout_invert(1);
//	stdout_cmdmode(1);
  image_replaser(i1,image);
	gpv1=0;
	while(1){
//air handshaking
	nrf_set_channel(2);	
	nrf_set_rf_mode(0);
	nrf_write_command(0xe1,NULL,0);
	nrf_write_command(0xe2,NULL,0);				
	nrf_set_address(search_address,100);
	nrf_set_address(search_address,0);
	nrf_write_data(sof_pocket,32);
	nrf_irq_clr_stat();
	osDelay(1);	
	nrf_enable_rf(1);	
	for(gpv0=0;gpv0<CAPTURING_REQ && nrf_irq_stat()==0;gpv0++){
		osDelay(1);
	}
	nrf_enable_rf(0);
	
//air wait for download	
	nrf_set_channel(tx_channel);
	nrf_set_rf_mode(1);
	nrf_write_command(0xe1,NULL,0);
	nrf_write_command(0xe2,NULL,0);			
	nrf_set_address(tx_address,100);
	nrf_set_address(tx_address,0);
  nrf_irq_clr_stat();
	osDelay(1);	
	nrf_enable_rf(1);	
	for(gpv0=0;gpv0<DOWNLOAD_REQ_WAIT;gpv0++){
   	osDelay(1);
		if(nrf_irq_stat()){
			nrf_read_data(resive_pocket,32);
			nrf_irq_clr_stat();	
				if(resive_pocket[0]=='T' && resive_pocket[1]=='I'){
    	nrf_enable_rf(0);
				if(resive_pocket[2]<IMAGE_FORMAT_CNT){ //id of first pocket		
				transfer_image(&formats[resive_pocket[2]]);
			
		    switch(gpv1){
					case 0:
					image_replaser(i1,image);	
					gpv1++;
					break;
					case 1:
					image_replaser(i2,image);							
					gpv1++;
					break;
					case 2:
					image_replaser(i3,image);							
					gpv1=0;
					break;
					default:
						gpv1=0;
				}					
				}			
			break;	
			}
		}
		
	}
	
	nrf_enable_rf(0);	
	}
	return 1;
}




int main(){
	GPDMA_Initialize();
	return main_app();
	while(1);
}









#include "rl_usb.h"
#include "cmsis_os.h"
#include "spi_user.h"


unsigned int sound_label;
unsigned int mute_req;





unsigned char data[32];
unsigned short *convert_val;
int main(){
	int gpv0,gpv1;
	convert_val=(unsigned short *)data;
	NRF_init();
	USBD_Initialize(0);
	USBD_Connect(0);
	  while(1){
		if(mute_req==false){
		if(USBD_ADC_ReceivedSamplesAvailable(0)>15){
		if(USBD_ADC_ReadSamples(0,data,16)==16){		
		for(gpv0=0;gpv0<16;gpv0++){
			gpv1=convert_val[gpv0];
			gpv1+=0x8000;
			gpv1&=0xffff;
			gpv1*=sound_label;
			gpv1>>=8;		
		  convert_val[gpv0]=gpv1;
		}
		nrf_write_data(data,32);
		while(NRF_irq_flag==0);
		for(gpv0=0;gpv0<80;gpv0++);
		}			
		}
		}
	  }
}

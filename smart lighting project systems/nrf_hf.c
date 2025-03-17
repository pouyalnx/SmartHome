#include "lpc177x_8x.h"
#include <stdio.h>
#include "nrf.h"
#include "common_struct.h"
#include "cmd_f.h"
#include "error_table.h"


#define NRF_HF_RESEND    10
#define NRF_HF_TIMEOUT   100


int nrf_destt_hf(Device *dev){
	unsigned short msg[4];
	int gpv0;
	msg[0]=dev->barcode_id;
	msg[1]=CMD_CODE_DESTT<<8;
	nrf_set_addr(dev->barcode_address);
	
	for(gpv0=0;gpv0<NRF_HF_RESEND;gpv0++){
		if(nrf_send_data((char *)msg,4)){
			return 0;
		}
	}
	return ERROR_NRF__RF_ERROR;
}





int nrf_deset_hf(Device *dev,char *data,int len){
	unsigned short msg[16];
	int gpv0,a,b,msg_len;
	
	msg_len=2;
	for(gpv0=0;gpv0<len;gpv0++){
		if(data[gpv0]=='<'){
		  if(sscanf(&data[gpv0+1],"%d,%d",&a,&b)==2){
			  msg[msg_len++]=a;
				msg[msg_len++]=b;
			}
			else{
				return ERROR_NRF__DATA_PACKIG;
			}
		}	
	}
	
	msg[0]=dev->barcode_id;
	msg[1]=CMD_CODE_DESET<<8|(msg_len-2);
	nrf_set_addr(dev->barcode_address);
	
	
	for(gpv0=0;gpv0<NRF_HF_RESEND;gpv0++){
		if(nrf_send_data(data,msg_len)){
			return 0;
		}
	}
	return ERROR_NRF__RF_ERROR;
}



int nrf_deget_hf(char *in,int in_len,char *out,int *out_len,Device *dev){
unsigned short msg_in[16],msg_out[16];
int gpv0,gpv1,gpv2,msg_out_len;

	
for(gpv0=0,msg_out_len=2;gpv0<in_len;gpv0++){
if(in[gpv0]=='<'){
	if(sscanf(&in[gpv0+1],"%d",&gpv1)==1){
		msg_out[msg_out_len++]=gpv1;
	}
	else{
		return ERROR_NRF__DATA_PACKIG;
	}
}
}	

msg_out[0]=dev->barcode_id;
msg_out[1]=CMD_CODE_DEGET<<8|(msg_out_len-2);

nrf_set_addr(dev->barcode_address);

for(gpv0=0;gpv0<NRF_HF_RESEND;gpv0++){
if(nrf_send_data((char *)msg_out,msg_out_len)){	
  nrf_read_data_enable_os();
  for(gpv1=0;gpv1<NRF_HF_TIMEOUT;gpv1++){
    if(nrf_read_data_os((char *)msg_in)){
			
			if(msg_in[0]==msg_out[0] && (msg_in[1]&0xff00)==(msg_out[1]&0xff00)){
       //msg_out_len used as varible			  
				msg_out_len=msg_in[1]&0xff;
				*out_len=0;
				for(gpv2=2;msg_out_len>0;msg_out_len-=2,gpv2+=2){
				*out_len+=sprintf(&out[*out_len],"<%d,%d>,",msg_in[gpv2],msg_in[gpv2+1]);
				}
				return 0;
			}
			else{
         nrf_read_data_enable_os();				
			}
			
    }	
   osDelay(1);
	}
 nrf_read_data_disable_os();
}
}

return ERROR_NRF__RF_ERROR;
}


void nrf_hf_init(void){
	nrf_init();
}





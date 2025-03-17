#include "error_table.h"
#include "nrf_hf.h"
#include "cmd_f.h"
#include "common_struct.h"
#include "cmsis_os.h"
#include "nrf_de.h"
#include <stdio.h>
#include <string.h>



//as both de and dc system use as both group instruction is same we combine this groups


int hex2dec(char *str,int len,unsigned int *number){
	int gpv0,deg;
	*number=0;
	for(gpv0=0;gpv0<len;gpv0++){
		if(str[gpv0]>='0' && str[gpv0]<='9'){
		deg=str[gpv0]-'0';	
		}
		else if(str[gpv0]>='a' && str[gpv0]<='f'){
		deg=str[gpv0]-'a'+10;	
		}
		else if(str[gpv0]>='A' && str[gpv0]<='F'){
		deg=str[gpv0]-'A'+10;	
		}
		else return ERROR_HEX2BIN__HEXFORMAT;
		
		*number|=deg<<4*(len-gpv0-1);
	}
	return 0;//success
}


int device2barcode(char *data,int *len,Device *dev){
  *len=sprintf(data,"%x%x%x%x%x%x",dev->barcode_address[0],dev->barcode_address[1],dev->barcode_address[2],dev->barcode_address[3],dev->barcode_address[4],dev->barcode_id);
	data[(*len)++]=dev->barcode_reserved[0];
	data[(*len)++]=dev->barcode_reserved[1];
	data[(*len)++]=dev->barcode_reserved[2];
	data[(*len)++]=dev->barcode_reserved[3];
	data[(*len)++]=dev->barcode_reserved[4];
	data[(*len)++]=dev->barcode_reserved[5];
	data[(*len)++]=dev->barcode_reserved[6];	
	data[(*len)]=0; //null
	return 0; //success	
}


int barcode2device(char *data,int len,Device *dev){
  int gpv0;
	unsigned int temple;
	if(len<20) return ERROR_BARCODE__SIZE; 
	
	gpv0=hex2dec(&data[0],2,&temple);
	if(gpv0)  return gpv0;	
  dev->barcode_address[0]=temple;
	
	gpv0=hex2dec(&data[2],2,&temple);
	if(gpv0)  return gpv0;	
  dev->barcode_address[1]=temple;
	
	gpv0=hex2dec(&data[4],2,&temple);
	if(gpv0)  return gpv0;
  dev->barcode_address[2]=temple;	

	gpv0=hex2dec(&data[6],2,&temple);
	if(gpv0)  return gpv0;
  dev->barcode_address[3]=temple;	

	gpv0=hex2dec(&data[8],2,&temple);
	if(gpv0)  return gpv0;
  dev->barcode_address[4]=temple;	

	gpv0=hex2dec(&data[10],4,&temple);
	if(gpv0)  return gpv0;
	dev->barcode_id=temple;

	dev->barcode_reserved[0]=data[14];
	dev->barcode_reserved[1]=data[15];
	dev->barcode_reserved[2]=data[16];
	dev->barcode_reserved[3]=data[17];
	dev->barcode_reserved[4]=data[18];
	dev->barcode_reserved[5]=data[19];
	dev->barcode_reserved[6]=data[20];
	
return 0; //success
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void dc_dccnt(CMD_pocket *pk){
pk->size=sprintf(pk->data,"%d,%d",dc_control.update_code,dc_control.saved_device_cnt);	
}



void dc_dcdel(CMD_pocket *pk){
	if(pk->arg>=DEVICE_MAX_CNT){
		pk->size=sprintf(pk->data,"E%d",ERROR_DC_ARG_TO_BIG);
	  return ;
	}
	if(device[pk->arg].barcode_id==0 || dc_control.saved_device_cnt==0){
		pk->size=sprintf(pk->data,"E%d",ERROR_DC_DEVICE_NOT_EXSIST);		
	  return ;
	}
	
	
	//PLEASE ADD ADMIN STAT
	
	device[pk->arg].barcode_id=0;
	dc_control.saved_device_cnt--;
	dc_control.update_code++;
	if(dc_control.update_code>0xfffffffc) dc_control.update_code=0;	
	
	e2_write_device(&device[pk->arg],pk->arg);
	e2_write_device_contol(&dc_control);
	pk->size=sprintf(pk->data,"%d",dc_control.update_code);	  
}


void dc_dcgid(CMD_pocket *pk){
	int gpv0,gpv1;
	char barcode[32];
	if(pk->arg>=DEVICE_MAX_CNT){
		pk->size=sprintf(pk->data,"E%d",ERROR_DC_ARG_TO_BIG);
	  return ;
	}
	
	for(gpv1=0,gpv0=0;gpv1<DEVICE_MAX_CNT;gpv1++){
		if(device[gpv1].barcode_id){ 
		gpv0++;
		if(gpv0==pk->arg) break;
		}
	}
	
	if(gpv1==DEVICE_MAX_CNT){
		pk->size=sprintf(pk->data,"E%d",ERROR_DC_DEVICE_NOT_EXSIST);		
	  return ;
	}	
	
	gpv0=device2barcode(barcode,&pk->size,&device[gpv1]);
	if(gpv0!=0) pk->size=sprintf(pk->data,"E%d",gpv0); 	

	
	barcode[pk->size]=0;
  pk->size=sprintf(pk->data,"%d,%s",gpv1,barcode);
}


void dc_dcstp(CMD_pocket *pk){
	int gpv0,gpv1,gpv2;
	Device dev;
	char barcode[32];
	//WE MUST HAVE AN COMPAER CMD
	
	
	if(dc_control.saved_device_cnt>=DEVICE_MAX_CNT){
		pk->size=sprintf(pk->data,"E%d",ERROR_DC_NOT_ENOUGTH_MEMORY);
	  return ;		
	}
	
	for(gpv0=0;gpv0<DEVICE_MAX_CNT;gpv0++){
	  if(device[gpv0].barcode_id==0) break;	
	}
  
	if(gpv0==DEVICE_MAX_CNT){
		pk->size=sprintf(pk->data,"E%d",ERROR_DC_MEMORY_FREE_FAULT); //with this error we are sure about that problem is about e2prom memory
	  return ;	
	}		
	
	gpv1=barcode2device(pk->data,pk->size,&dev);
	if(gpv1!=0){
		pk->size=sprintf(pk->data,"E%d",gpv1);
	  return ;			
	}
	
	for(gpv2=0,gpv1=0;gpv1<dc_control.saved_device_cnt && gpv2<DEVICE_MAX_CNT ;gpv2++){
	  if(device[gpv2].barcode_id){
			 gpv1++;
       if(device[gpv2].barcode_address[0]==dev.barcode_address[0])
       if(device[gpv2].barcode_address[1]==dev.barcode_address[1])
       if(device[gpv2].barcode_address[2]==dev.barcode_address[2])
       if(device[gpv2].barcode_address[3]==dev.barcode_address[3])
       if(device[gpv2].barcode_address[4]==dev.barcode_address[4]){
		      pk->size=sprintf(pk->data,"E%d",EEROR_DC_ALREADY_INSTALLED);
	        return ;		
			 }				 
		}			
	}
	
	gpv1=nrf_destt_hf(&dev);	
	if(gpv1!=0){
		pk->size=sprintf(pk->data,"E%d",gpv1);
	  return ;			
	}
	
	dc_control.saved_device_cnt++;
	dc_control.update_code++;
	if(dc_control.update_code>0xfffffffc) dc_control.update_code=0;
	
	device[gpv0]=dev;
	
	e2_write_device(&device[gpv0],gpv0);
	
	e2_write_device_contol(&dc_control);
  pk->arg=gpv0;	
  
	pk->data[pk->size]=0;
  strcpy(barcode,pk->data);	
	
	pk->size=sprintf(pk->data,"%d,%s",dc_control.update_code,barcode);
}




void dc_dcupd(CMD_pocket *pk){
pk->size=sprintf(pk->data,"%d",dc_control.update_code);		
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void de_destt(CMD_pocket *pk){
	int gpv1;
////////////////////common check function//////////////////////
	if(pk->arg>=DEVICE_MAX_CNT){
		pk->size=sprintf(pk->data,"E%d",ERROR_DC_ARG_TO_BIG);
	  return ;
	}
	if(device[pk->arg].barcode_id==0){
		pk->size=sprintf(pk->data,"E%d",ERROR_DC_DEVICE_NOT_EXSIST);		
	  return ;
	}
///////////////////////////////////////////////////////////////		

	gpv1=nrf_destt_hf(&device[pk->arg]);	
	if(gpv1!=0){
		pk->size=sprintf(pk->data,"E%d",gpv1);
	  return ;			
	}
	
	 pk->size=sprintf(pk->data," ");
	 pk->size=0;
}


void de_deset(CMD_pocket *pk){
	int gpv1;
////////////////////common check function//////////////////////
	if(pk->arg>=DEVICE_MAX_CNT){
		pk->size=sprintf(pk->data,"E%d",ERROR_DC_ARG_TO_BIG);
	  return ;
	}
	if(device[pk->arg].barcode_id==0){
		pk->size=sprintf(pk->data,"E%d",ERROR_DC_DEVICE_NOT_EXSIST);		
	  return ;
	}
///////////////////////////////////////////////////////////////	
	gpv1=nrf_deset_hf(&device[pk->arg],pk->data,pk->size);	
	if(gpv1!=0){
		pk->size=sprintf(pk->data,"E%d",gpv1);
	  return ;			
	}
	
	 pk->size=sprintf(pk->data," ");	
	 pk->size=0;	
}



void de_deget(CMD_pocket *pk){
	int gpv1,len;
	char buffer[64];
	////////////////////common check function//////////////////////
	if(pk->arg>=DEVICE_MAX_CNT){
		pk->size=sprintf(pk->data,"E%d",ERROR_DC_ARG_TO_BIG);
	  return ;
	}
	if(device[pk->arg].barcode_id==0){
		pk->size=sprintf(pk->data,"E%d",ERROR_DC_DEVICE_NOT_EXSIST);		
	  return ;
	}
///////////////////////////////////////////////////////////////		
	gpv1=nrf_deget_hf(pk->data,pk->size,buffer,&len,&device[pk->arg]);	
	if(gpv1!=0){
		pk->size=sprintf(pk->data,"E%d",gpv1);
	  return ;			
	}
	
 pk->data[pk->size]=0;
 pk->size=len;
 buffer[len]=0; 
 strcpy(pk->data,buffer); 
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void de_dc_thread(const void *arg){
osEvent event;
CMD_pocket pocket,*pk;
	
	while(1){
	event=osMailGet(nrf_option_de_dc_q,osWaitForever);
	if(event.status==osEventMail){
  pk=event.value.p;		
  pocket=*pk;
	osMailFree(nrf_option_de_dc_q,pk);
		
	
		
		switch(pocket.cmd.source){	
		case CMD_DEST_NRF_OPTION_DC_Q:
		switch(pocket.cmd.cmd){
				case CMD_CODE_DCCNT:
					dc_dccnt(&pocket);
				break;
				case CMD_CODE_DCDEL:
					dc_dcdel(&pocket);
				break;
				case CMD_CODE_DCGID:	
				 dc_dcgid(&pocket);					
				break;
				case CMD_CODE_DCSTP:
					dc_dcstp(&pocket);
				break;
				case CMD_CODE_DCUPD:
					dc_dcupd(&pocket);
				break;
			}
			
		break;
		case CMD_DEST_NRF_OPTION_DE_Q:
		switch(pocket.cmd.cmd){
				case CMD_CODE_DEGET:
        de_deget(&pocket);					
				break;
				case CMD_CODE_DESET:
				de_deset(&pocket);	
				break;
				case CMD_CODE_DESTT:
					de_destt(&pocket);
				break;			
			}			
		break;
	}
	
	pk=osMailAlloc(wifi_respond_q,osWaitForever);
	*pk=pocket;
	osMailPut(wifi_respond_q,pk);
	}
}
}

osThreadDef(de_dc_thread,osPriorityNormal,1,0);





void de_dc_init(void){
nrf_hf_init();
osThreadCreate(osThread(de_dc_thread),NULL);
}



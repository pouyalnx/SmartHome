#include "driver_usart.h"
#include <stdio.h>
#include "esp_res.h"
#include  "cmsis_os.h"

#define MAX_RES_SIZE          2048
#define ESP_BUFFER_CHECK_TIME 1
#define ESP_RES_TIME_OUT      2000   //MS
#define ESP_BUFFER_OVERFLOW   2000


extern ARM_DRIVER_USART Driver_USART2;
ARM_DRIVER_USART *esp;

char res_buffer[MAX_RES_SIZE];
volatile int esp_stat;


void esp_clr_buffer(int *last_cnt,ARM_DRIVER_USART *ptr,char *buffer,int MAX_SIZE){
	if(*last_cnt==ptr->GetRxCount()){
			ptr->Control(ARM_USART_ABORT_RECEIVE,0);
			ptr->Receive(buffer,MAX_SIZE);
			*last_cnt=0;		
	}
}



void ESP_res(const void *arg){
int timeout,gpv0,last_cnt,currnt_cnt;
timeout=0;
last_cnt=0;	
while(1){
currnt_cnt=esp->GetRxCount();	
if(esp_stat==ESP_STAT_BUSY){
for(gpv0=1;gpv0<currnt_cnt;gpv0++){


if(res_buffer[gpv0]=='>'){	
last_cnt=gpv0;
esp_stat=ESP_STAT_OK_READY;	
break;
}

if(res_buffer[gpv0]=='K' && res_buffer[gpv0-1]=='O'){	
last_cnt=gpv0;
esp_stat=ESP_STAT_OK_READY;	
esp_clr_buffer(&last_cnt,esp,res_buffer,MAX_RES_SIZE);	
break;
}

if(res_buffer[gpv0]=='R' && res_buffer[gpv0-1]=='O' && gpv0>3 && res_buffer[gpv0-3]=='R' && res_buffer[gpv0-4]=='E'){	
last_cnt=gpv0;
esp_stat=ESP_STAT_OK_READY;	
esp_clr_buffer(&last_cnt,esp,res_buffer,MAX_RES_SIZE);
break;
}


if(res_buffer[gpv0]=='L' && res_buffer[gpv0-1]=='I' && gpv0>2 && res_buffer[gpv0-2]=='A' && res_buffer[gpv0-3]=='F'){	
last_cnt=gpv0;
esp_stat=ESP_STAT_OK_READY;
esp_clr_buffer(&last_cnt,esp,res_buffer,MAX_RES_SIZE);	
break;
}

}

timeout++;
if(timeout>ESP_RES_TIME_OUT){
last_cnt=gpv0;
esp_stat=ESP_STAT_OK_READY;	
}

}
else{
timeout=0;

for(gpv0=last_cnt;gpv0<currnt_cnt;gpv0++){

if(res_buffer[gpv0]=='T' &&	gpv0>7 && res_buffer[gpv0-1]=='C' && res_buffer[gpv0-2]=='E' && res_buffer[gpv0-5]=='O' && res_buffer[gpv0-6]=='C' && res_buffer[gpv0-7]==','){
	//
}

if(res_buffer[gpv0]=='D' &&	gpv0>6 && res_buffer[gpv0-1]=='E' && res_buffer[gpv0-2]=='S' && res_buffer[gpv0-4]=='L' && res_buffer[gpv0-5]=='C' && res_buffer[gpv0-6]==','){
	//
}

}	
	
}

if(currnt_cnt>ESP_BUFFER_OVERFLOW){
			esp->Control(ARM_USART_ABORT_RECEIVE,0);
			esp->Receive(res_buffer,MAX_RES_SIZE);
			last_cnt=0;	
}

osDelay(ESP_BUFFER_CHECK_TIME);
}
}



osThreadDef(ESP_res,osPriorityNormal,1,0); //for better performance incres peformance

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void esp_write(char *data,int len){
	esp->Control(ARM_USART_ABORT_RECEIVE,0);
	esp->Send(data,len);
	while(esp->GetStatus().tx_busy) osDelay(1);
	esp->Receive(res_buffer,MAX_RES_SIZE);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ESP_res_init(void){
esp=&Driver_USART2;
//USART INIT	
	
esp_stat=ESP_STAT_OK_READY;	
osThreadCreate(osThread(ESP_res),NULL);	

}









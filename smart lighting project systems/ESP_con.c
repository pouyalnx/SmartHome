#include "driver_usart.h"
#include <stdio.h>
#include "esp_con.h"
#include "cmsis_os.h"
#include "cmd_f.h"

#define MAX_RES_SIZE               2048
#define ESP_BUFFER_CHECK_TIME      1
#define ESP_RES_TIME_OUT           2000   //MS
#define ESP_RES_DATA_TIME_OUT      1000   //MS
#define ESP_BUFFER_OVERFLOW        2000

#define DEBUG  0

extern ARM_DRIVER_USART Driver_USART2;
ARM_DRIVER_USART *esp;

char res_buffer[MAX_RES_SIZE];
volatile int esp_stat;


users user[MAX_USER_POSSIBLE];

void esp_clr_buffer(int *last_cnt,ARM_DRIVER_USART *ptr,char *buffer,int MAX_SIZE){
	if(*last_cnt==ptr->GetRxCount()){
			ptr->Control(ARM_USART_ABORT_RECEIVE,0);
			ptr->Receive(buffer,MAX_SIZE);
			*last_cnt=0;		
	}
}



void ESP_res(const void *arg){
int timeout,gpv0,last_cnt,currnt_cnt,gpv1;
int ipd_len,ipd_id,res_len,data_point;
	
timeout=0;
last_cnt=0;	

#if DEBUG==1	
puts("resive thread is online");
#endif	
	
while(1){
currnt_cnt=esp->GetRxCount();
/*
#if DEBUG==1	
if(currnt_cnt)	
puts(res_buffer);
#endif	
*/
	
if(esp_stat==ESP_STAT_BUSY){
for(gpv0=1;gpv0<currnt_cnt;gpv0++){


if(res_buffer[gpv0]=='>'){	
last_cnt=gpv0+1;
esp_stat=ESP_STAT_OK_READY;	
res_buffer[gpv0]='x';
	break;
}

if(res_buffer[gpv0]=='K' && res_buffer[gpv0-1]=='O'){	
last_cnt=gpv0+1;
esp_stat=ESP_STAT_OK_READY;	
res_buffer[gpv0]='x';
esp_clr_buffer(&last_cnt,esp,res_buffer,MAX_RES_SIZE);	

		#if DEBUG==1
puts("ok");	
#endif
	
break;
}

if(res_buffer[gpv0]=='R' && res_buffer[gpv0-1]=='O' && gpv0>3 && res_buffer[gpv0-3]=='R' && res_buffer[gpv0-4]=='E'){	
last_cnt=gpv0+1;
esp_stat=ESP_STAT_OK_READY;
res_buffer[gpv0]='x';	
esp_clr_buffer(&last_cnt,esp,res_buffer,MAX_RES_SIZE);

	#if DEBUG==1	
puts("error");		
#endif
	
break;
}


if(res_buffer[gpv0]=='L' && res_buffer[gpv0-1]=='I' && gpv0>2 && res_buffer[gpv0-2]=='A' && res_buffer[gpv0-3]=='F'){	
last_cnt=gpv0+1;
esp_stat=ESP_STAT_OK_READY;
res_buffer[gpv0]='x';	
esp_clr_buffer(&last_cnt,esp,res_buffer,MAX_RES_SIZE);	

	
#if DEBUG==1	
puts("fail");	
#endif
	
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
	gpv1=res_buffer[gpv0-8]-'0';
  if(gpv0>=0 && gpv0<MAX_USER_POSSIBLE){
		user[gpv0].stat=USER_STAT_ONLINE;
	}

	#if DEBUG==1
	puts("connect");
	#endif
  res_buffer[gpv0]='x';		
	last_cnt=gpv0+1;	
}

if(res_buffer[gpv0]=='D' &&	gpv0>6 && res_buffer[gpv0-1]=='E' && res_buffer[gpv0-2]=='S' && res_buffer[gpv0-4]=='L' && res_buffer[gpv0-5]=='C' && res_buffer[gpv0-6]==','){
	gpv1=res_buffer[gpv0-8]-'0';
  if(gpv0>=0 && gpv0<MAX_USER_POSSIBLE){
		user[gpv0].stat=USER_STAT_OFFLINE;
	}

	#if DEBUG==1
	puts("disconnect");
	#endif
  res_buffer[gpv0]='x';	
	last_cnt=gpv0+1;
}


if(res_buffer[gpv0]==':' && gpv0>8){
  res_buffer[gpv0]='x';	
	data_point=gpv0+1;
	gpv1=gpv0-4;


	for(;gpv1>2;gpv1--){
		if(res_buffer[gpv1]=='D' && res_buffer[gpv1-1]=='P' && res_buffer[gpv1-2]=='I' && res_buffer[gpv1-3]=='+'){

			
			if(sscanf(&res_buffer[gpv1+2],"%d,%d",&ipd_id,&ipd_len)!=2){
			ipd_len=0;
			ipd_id=0;
			}
			break;
		}
	}

	timeout=0;
	if(ipd_len!=0){
		

  
	//if needed add busy flag here	
	while(timeout++<ESP_RES_DATA_TIME_OUT){
	res_len=esp->GetRxCount()-data_point;
	if(res_len>=ipd_len){
  //data is ready in size and point
	
  #if DEBUG==1		
	res_buffer[data_point+ipd_len]=0;
	puts(&res_buffer[data_point]);	
	#endif
		
	cmd_finder(&res_buffer[data_point],ipd_len,ipd_id);	
	gpv0=data_point+ipd_len;
	break;
	}
  osDelay(1);	
	}
}	
	
	last_cnt=gpv0+1;
  esp_clr_buffer(&last_cnt,esp,res_buffer,MAX_RES_SIZE);	

}
}
}


if(currnt_cnt>ESP_BUFFER_OVERFLOW){
			esp->Control(ARM_USART_ABORT_RECEIVE,0);
			esp->Receive(res_buffer,MAX_RES_SIZE);
			last_cnt=0;	
}

do{
osDelay(ESP_BUFFER_CHECK_TIME);
}while(esp_stat==ESP_STAT_BUSY_TX);

}
}



osThreadDef(ESP_res,osPriorityNormal,1,0); //for better performance incres peformance

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void esp_write(char *data,int len){

   #if DEBUG==1	
   puts(data);	
   #endif		  
	
	esp->Control(ARM_USART_ABORT_RECEIVE,0);
	esp->Send(data,len);
	//we can use another flag for avoid any process in esp_res when try to send
	esp_stat=ESP_STAT_BUSY_TX;
	while(esp->GetStatus().tx_busy){
	osDelay(1);
	}
	esp_stat=ESP_STAT_BUSY;
	esp->Receive(res_buffer,MAX_RES_SIZE);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////

int esp_wait_pass(void){
	while(esp_stat!=ESP_STAT_OK_READY && esp_stat!=ESP_STAT_ERROR) osDelay(1);
	if(esp_stat==ESP_STAT_OK_READY) return 1;
	return 0;
}

/*

void esp_setup(void){
char esp_setup_buffer[128];
int len;
	
	
len=sprintf(esp_setup_buffer,"AT\r\n");
	
do{	
esp_write(esp_setup_buffer,len);
esp_wait_pass();
}while(esp_stat==ESP_STAT_ERROR);




len=sprintf(esp_setup_buffer,"AT+CWMODE_CUR=2\r\n");
esp_write(esp_setup_buffer,len);
esp_wait_pass();

len=sprintf(esp_setup_buffer,"AT+CWSAP_CUR=\"KENDALL_JENNER\",\"asdfghjkl\",1,4\r\n");
esp_write(esp_setup_buffer,len);
esp_wait_pass();

len=sprintf(esp_setup_buffer,"AT+CIPMUX=1\r\n");
esp_write(esp_setup_buffer,len);
esp_wait_pass();

len=sprintf(esp_setup_buffer,"AT+CIPSERVER=1,1212\r\n");
esp_write(esp_setup_buffer,len);
esp_wait_pass();

}
*/

void esp_init(void){

esp=&Driver_USART2;
esp->Initialize(NULL);	
esp->PowerControl(ARM_POWER_FULL);
esp->Control(ARM_USART_MODE_ASYNCHRONOUS,115200);
esp->Control(ARM_USART_CONTROL_RX,1);	
esp->Control(ARM_USART_CONTROL_TX,1);			
cmd_init();	

//USART INIT	
user[0].stat=USER_STAT_OFFLINE;
user[1].stat=USER_STAT_OFFLINE;
user[2].stat=USER_STAT_OFFLINE;
user[3].stat=USER_STAT_OFFLINE;	
esp_stat=ESP_STAT_OK_READY;	

	
osThreadCreate(osThread(ESP_res),NULL);	

//esp_setup();
}









#include <stdio.h>
#include "cmd_f.h"
#include "cmsis_os.h"
#include "esp_con.h"

#define RESEND_CNT 5

//#define DEBUG 

#ifdef DEBUG
char debug[128];
#endif

char wifi_msg[120],wifi_msg_data[120];	
void wifi_respond_f(const void *arg){
CMD_pocket *pkt;
int pkt_size;	
osEvent event;
int size,gpv0;
while(1){
event=osMailGet(wifi_respond_q,osWaitForever);
if(event.status==osEventMail){
pkt=event.value.p;		
pkt->data[pkt->size]=0;	
	

	
pkt_size=sprintf(wifi_msg_data,"//%s/%d/%d=%s\r\n",cmd_saved_strings[pkt->cmd.table_locate],pkt->arg,pkt->size,pkt->data);	
size=sprintf(wifi_msg,"AT+CIPSEND=%d,%d\r\n",pkt->device_id,pkt_size);

for(gpv0=0;gpv0<RESEND_CNT;gpv0++){
esp_write(wifi_msg,size);
if(esp_wait_pass()){
esp_write(wifi_msg_data,pkt_size);
if(esp_wait_pass()){
break;
}	
}	
}
	


#ifdef DEBUG
sprintf(debug,"%s %d %d %s",cmd_saved_strings[pkt->cmd.table_locate],pkt->arg,pkt->size,pkt->data);
puts(debug);	
#endif

osMailFree(wifi_respond_q,pkt);	
}
}
}	




osThreadDef(wifi_respond_f,osPriorityNormal,1,0);




void wifi_respond_init(void){
osThreadCreate(osThread(wifi_respond_f),NULL);	
}














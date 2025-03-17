#include <stdio.h>
#include <string.h>
#include "common_struct.h"
#include "esp_con.h"
#include "co.h"
#include "error_table.h"
#include "cmsis_os.h"
#include "cmd_f.h"




void co_cocct(CMD_pocket *ptr){
	int gpv0,user_cnt,admin_cnt;
	
	for(gpv0=0,user_cnt=0,admin_cnt=0;gpv0<MAX_USER_POSSIBLE;gpv0++){
		if(user[gpv0].stat==USER_STAT_ONLINE) user_cnt++;
		else if(user[gpv0].stat==ADMIN_STAT)  admin_cnt++;
	}
	
	ptr->size=sprintf(ptr->data,"U%d,A%d,M%d",user_cnt,admin_cnt,ptr->device_id);
}



////////////////////////////  //COCLP/SIZE_NEW_PASS/ALL_SIZEE=NEW_PASSWPRD,OLD_PASSWORD

#define MIN_PASSWORD_SIZE  8
#define MAX_PASSWORD_SIZE  30

void co_coclp(CMD_pocket *ptr){
  string __align(4) str;
	int gpv0,size;
	char *old_pass,*new_pass;
	
	if(ptr->arg>ptr->size){
		ptr->size=sprintf(ptr->data,"E%d",ERROE_CO_ARG_ERROR);
		return ;		
	}
	
	if(user[ptr->device_id].stat!=ADMIN_STAT){
		ptr->size=sprintf(ptr->data,"E%d",ERROR_CO_ACCESS);
		return ;
	}
	
	e2_read_admin_password(&str);

	old_pass=&ptr->data[ptr->arg+1];
	new_pass=&ptr->data[0];
	ptr->data[ptr->arg]=0;
  ptr->data[ptr->size]=0;	
	
	if(ptr->arg<MIN_PASSWORD_SIZE || ptr->arg>MAX_PASSWORD_SIZE){
		ptr->size=sprintf(ptr->data,"E%d",ERROE_CO_LENGTH);
		return ;		
	}
	
	if(strcmp(old_pass,str.str)!=0){
		ptr->size=sprintf(ptr->data,"E%d",ERROR_CO_PASS_FALSE);
		return ;			
	}
	
	size=ptr->arg;
	for(gpv0=0;gpv0<size;gpv0++){
		if((new_pass[gpv0]>='a' && new_pass[gpv0]<='z')||(new_pass[gpv0]>='A' && new_pass[gpv0]<='Z')||(new_pass[gpv0]>='0' && new_pass[gpv0]<='9')||new_pass[gpv0]=='-'){
		str.str[gpv0]=new_pass[gpv0];
		}
		else{
		ptr->size=sprintf(ptr->data,"E%d",ERROR_CO_BANNED_CHAR);
		return ;			
		}
	}
	
		str.str[gpv0]=' ';
		str.str[gpv0+1]=' ';
		str.str[gpv0+2]='\0';
	 
	  e2_write_admin_password(&str);
	
	  sprintf(ptr->data," ");
	  ptr->size=0;
}

///////////////////////////////////////// //COCWN/0/SIZE=WIFI_NAME
#define MAX_WIFI_NAME_SIZE 30

void co_cocwn(CMD_pocket *ptr){
  string __align(4) str;
	CONFIG __align(4) conf;	
	
	if(ptr->size>MAX_WIFI_NAME_SIZE || ptr->size==0){
		ptr->size=sprintf(ptr->data,"E%d",ERROE_CO_LENGTH);
		return ;		
	}

	if(user[ptr->device_id].stat!=ADMIN_STAT){
		ptr->size=sprintf(ptr->data,"E%d",ERROR_CO_ACCESS);
		return ;
	}
	
	e2_read_wifi_name(&str);
	ptr->data[ptr->size]=0;
	strcpy(str.str,ptr->data);
	e2_write_wifi_name(&str);
	
	e2_read_config(&conf);
	conf.wifi_name_password_change=0xff;
	e2_write_config(&conf);
	
	sprintf(ptr->data," ");
	ptr->size=0;	
}
////////////////////////////  //COCWP/SIZE_NEW_PASS/ALL_SIZEE=NEW_PASSWPRD,OLD_PASSWORD

#define MIN_WIFI_PASSWORD_SIZE  8
#define MAX_WIFI_PASSWORD_SIZE  30

void co_cocwp(CMD_pocket *ptr){
  string __align(4) str;
	CONFIG __align(4) conf;
	int gpv0,size;
	char *old_pass,*new_pass;
	
	if(ptr->arg>ptr->size){
		ptr->size=sprintf(ptr->data,"E%d",ERROE_CO_ARG_ERROR);
		return ;		
	}
	
	if(user[ptr->device_id].stat!=ADMIN_STAT){
		ptr->size=sprintf(ptr->data,"E%d",ERROR_CO_ACCESS);
		return ;
	}
	
	e2_read_wifi_password(&str);

	old_pass=&ptr->data[ptr->arg+1];
	new_pass=&ptr->data[0];
	ptr->data[ptr->arg]=0;
  ptr->data[ptr->size]=0;	
	
	if(ptr->arg<MIN_WIFI_PASSWORD_SIZE || ptr->arg>MAX_WIFI_PASSWORD_SIZE){
		ptr->size=sprintf(ptr->data,"E%d",ERROE_CO_LENGTH);
		return ;		
	}
	
	if(strcmp(old_pass,str.str)!=0){
		ptr->size=sprintf(ptr->data,"E%d",ERROR_CO_PASS_FALSE);
		return ;			
	}
	
	size=ptr->arg;
	for(gpv0=0;gpv0<size;gpv0++){
		if((new_pass[gpv0]>='a' && new_pass[gpv0]<='z')||(new_pass[gpv0]>='A' && new_pass[gpv0]<='Z')||(new_pass[gpv0]>='0' && new_pass[gpv0]<='9')||new_pass[gpv0]=='-'){
		str.str[gpv0]=new_pass[gpv0];
		}
		else{
		ptr->size=sprintf(ptr->data,"E%d",ERROR_CO_BANNED_CHAR);
		return ;			
		}
	}
	
		str.str[gpv0]='\0';
		str.str[gpv0+1]='\0';
	 
	  e2_write_wifi_password(&str);

	  e2_read_config(&conf);
	  conf.wifi_name_password_change=0xff;  //after reset request restup
	  e2_write_config(&conf);
	
	
	  sprintf(ptr->data," ");
	  ptr->size=0;
}


/////////////////////////////////////////////////////////////// //COLGI/0/SIZE=PASSWORD

void co_colgi(CMD_pocket *ptr){
string __align(4) str;
	
if(user[ptr->device_id].stat!=ADMIN_STAT){
	ptr->size=sprintf(ptr->data,"E%d",ERROR_CO_LOGINED);
	return ;
}

e2_read_admin_password(&str);
ptr->data[ptr->size]=0;

if(strcmp(ptr->data,str.str)!=0){
	ptr->size=sprintf(ptr->data,"E%d",ERROR_CO_PASS_FALSE);
	return ;	
}

user[ptr->device_id].stat=ADMIN_STAT;

sprintf(ptr->data," ");
ptr->size=0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void co_thread(const void *arg){
osEvent event;
CMD_pocket pocket,*ptr;	
	while(1){
	event=osMailGet(advance_option_q,osWaitForever);
	if(event.status==osEventMail){
		ptr=event.value.p;
		pocket=*ptr;
		osMailFree(advance_option_q,ptr);
		switch(pocket.cmd.cmd){
			case CMD_CODE_COCCT:
				co_cocct(&pocket);
			break;
			case CMD_CODE_COCLP:
				co_coclp(&pocket);
			break;
			case CMD_CODE_COCWN:
				co_cocwn(&pocket);
			break;
			case CMD_CODE_COCWP:
				co_cocwp(&pocket);
			break;
			case CMD_CODE_COLGI:
				co_colgi(&pocket);
			break;			
		}
		ptr=osMailAlloc(wifi_respond_q,osWaitForever);
		*ptr=pocket;
		osMailPut(wifi_respond_q,ptr);
	}
	}
}
osThreadDef(co_thread,osPriorityNormal,1,0);








void co_init(void){
	osThreadCreate(osThread(co_thread),NULL);
}















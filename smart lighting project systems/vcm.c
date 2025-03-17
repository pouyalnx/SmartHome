#include "vcm.h"
#include "cmsis_os.h"
#include <stdio.h>
#include "cmd_f.h"
#include "common_struct.h"
#include "esp_con.h"
#include "error_table.h"

static VCM_PROG __align(4) appk_stat;


void vcadd(CMD_pocket *ptr){
	CONFIG __align(4) cfg;
	e2_read_config(&cfg);
	
	if(cfg.user_able2_program_vcm_en==0 && user[ptr->device_id].stat!=ADMIN_STAT){
		ptr->size=sprintf(ptr->data,"E%d",ERROR_VC_ACCESS_USER);
		return ;
	}		
	if(ptr->arg>VCM_PROGRAM_CNT){
		ptr->size=sprintf(ptr->data,"E%d",ERROR_VC_INPUT_ERROR);
		return ;		
	}
	
	if(appk_stat.prog_mem==VCM_STAT_APP_MEM_FILLED){
		ptr->size=sprintf(ptr->data,"E%d",ERROR_VC_MEMORY);
		return ;				
	}
	
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
void vcm_thread(const void *arg){
osEvent event;
CMD_pocket *ptr,pocket;
while(1){
	event=osMailGet(wm_q,osWaitForever);
	if(event.status==osEventMail){
	ptr=event.value.p;
	pocket=*ptr;
	osMailFree(wm_q,ptr);
		switch(pocket.cmd.cmd){
			case CMD_CODE_VCADD:
			
			break;
			case CMD_CODE_VCDEL:
				
			break;
			case CMD_CODE_VCPRI:
			
			break;
			case CMD_CODE_VCRUN:
			
			break;
			case CMD_CODE_VCSTO:
			
			break;
			case CMD_CODE_VCSTT:
				
			break;
			default:
			pocket.size=sprintf(pocket.data,"undefined request...");	
	  }
		ptr=osMailAlloc(wifi_respond_q,osWaitForever);
		*ptr=pocket;
		osMailPut(wifi_respond_q,ptr);
	}
}	
}
osThreadDef(vcm_thread,osPriorityNormal,1,0);





void vcm_init(void){
	osThreadCreate(osThread(vcm_thread),NULL);
}





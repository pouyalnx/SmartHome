#include <stdio.h>
#include "cmd_f.h"
#include "cmsis_os.h"
#include "error_table.h"
#include <string.h>

#define DEBUG

osMailQId wifi_respond_q;
osMailQDef(wifi_respond_q,16,CMD_pocket);

osMailQId advance_option_q;
osMailQDef(advance_option_q,16,CMD_pocket);

osMailQId nrf_option_de_dc_q;
osMailQDef(nrf_option_de_dc_q,16,CMD_pocket);

osMailQId wm_q;
osMailQDef(wm_q,16,CMD_pocket);




///////////////////////////////////////////////////////////////////////////



const  char cmd_saved_strings[CMD_SAVED_CNT][6]={
	"  ERR\0",

	"COCCT\0",
	"COCLP\0",
	"COCWN\0",
	"COCWP\0",
	"COLGI\0",
	
	"DCCNT\0",	
	"DCDEL\0",	
	"DCGID\0",
	"DCSTP\0",
	"DCUPD\0",
	"DEGET\0",
	"DESET\0",
	"DESTT\0",
	
	"VCADD\0",
	"VCDEL\0",
	"VCPRI\0",	
	"VCRUN\0",	
	"VCSTO\0",
  "VCSTT\0",
	
};


const  CMD_format cmd_saved_inf[CMD_SAVED_CNT]={
	{CMD_DEST_WIFI_RESPOND_Q,0,CMD_CODE_ERROR},

	{CMD_DEST_ADVANCE_OPTION_Q,1,CMD_CODE_COCCT},
	{CMD_DEST_ADVANCE_OPTION_Q,2,CMD_CODE_COCLP},
	{CMD_DEST_ADVANCE_OPTION_Q,3,CMD_CODE_COCWN},
	{CMD_DEST_ADVANCE_OPTION_Q,4,CMD_CODE_COCWP},
	{CMD_DEST_ADVANCE_OPTION_Q,5,CMD_CODE_COLGI},	
	
	{CMD_DEST_NRF_OPTION_DC_Q,6,CMD_CODE_DCCNT},	
	{CMD_DEST_NRF_OPTION_DC_Q,7,CMD_CODE_DCDEL},	
	{CMD_DEST_NRF_OPTION_DC_Q,8,CMD_CODE_DCGID},	
	{CMD_DEST_NRF_OPTION_DC_Q,9,CMD_CODE_DCSTP},	
	{CMD_DEST_NRF_OPTION_DC_Q,10,CMD_CODE_DCUPD},	
	
	{CMD_DEST_NRF_OPTION_DE_Q,11,CMD_CODE_DEGET},
	{CMD_DEST_NRF_OPTION_DE_Q,12,CMD_CODE_DESET},
	{CMD_DEST_NRF_OPTION_DE_Q,13,CMD_CODE_DESTT},	

	{CMD_DEST_WM_Q,14,CMD_CODE_VCADD},
	{CMD_DEST_WM_Q,15,CMD_CODE_VCDEL},	
	{CMD_DEST_WM_Q,16,CMD_CODE_VCPRI},
	{CMD_DEST_WM_Q,17,CMD_CODE_VCRUN},
	{CMD_DEST_WM_Q,18,CMD_CODE_VCSTO},
	{CMD_DEST_WM_Q,19,CMD_CODE_VCSTT},		
};





//////////////////////////////////////////////////////////////////////////




CMD_format cmd_search(char *temple){
	int start,end,i,gpv0,gpv1;
	end=CMD_SAVED_CNT;
	start=0;
	gpv1=0;
	while(end>start && gpv1<CMD_SAVED_CNT){
	i=(start+end)/2;		
	gpv0=strcmp(cmd_saved_strings[i],temple);	
	if(gpv0==0){
	return cmd_saved_inf[i];
	}
	else if(gpv0>0)    end=i;
	else               start=i;	
	gpv1++;
	}
	return cmd_saved_inf[0];
}



int cmd_pocket_sender(CMD_pocket pkt){
	CMD_pocket *pocket;	
	

	
	
	switch(pkt.cmd.source){
	case CMD_DEST_WIFI_RESPOND_Q:
	pocket=osMailAlloc(wifi_respond_q,osWaitForever);
  *(pocket)=pkt;	
	osMailPut(wifi_respond_q,pocket);
	break;			
	case CMD_DEST_ADVANCE_OPTION_Q:
  pocket=osMailAlloc(advance_option_q,osWaitForever);
  *(pocket)=pkt;	
	osMailPut(advance_option_q,pocket);
	break;
	case CMD_DEST_NRF_OPTION_DE_Q:
	case CMD_DEST_NRF_OPTION_DC_Q: 
  pocket=osMailAlloc(nrf_option_de_dc_q,osWaitForever);
  *(pocket)=pkt;	
	osMailPut(nrf_option_de_dc_q,pocket);
	break;
	case CMD_DEST_WM_Q:
	pocket=osMailAlloc(wm_q,osWaitForever);
  *(pocket)=pkt;	
	osMailPut(wm_q,pocket);
	break;		
}
return 1;
}


void cmd_finder(char *data,int len,int device_id){
int gpv0,gpv1;	
CMD_pocket pocket;	
char cmd_path[6];



for(gpv0=0;gpv0<len;gpv0++){

if(data[gpv0]=='/' && data[gpv0+1]=='/'){

gpv0+=2;
cmd_path[0]=data[gpv0++];	
cmd_path[1]=data[gpv0++];	
cmd_path[2]=data[gpv0++];	
cmd_path[3]=data[gpv0++];	
cmd_path[4]=data[gpv0++];		
cmd_path[5]=0;
pocket.device_id=device_id;	
pocket.cmd=cmd_search(cmd_path);
	
if(pocket.cmd.table_locate>0){

gpv1=sscanf(&data[gpv0],"/%d/%d",&pocket.arg,&pocket.size);	


if(gpv1==2){

	
if(pocket.size>CMD_POCKET_MAX_DATA_SIZE) pocket.size=CMD_POCKET_MAX_DATA_SIZE;	
	
gpv1=0;	
while(gpv0<len && data[gpv0]!='='){
gpv0++;	
if(data[gpv0]=='/' && data[gpv0+1]=='/'){
gpv1=1;  //false flag
break;	
}
}
gpv0++;
for(gpv1=0;gpv1<pocket.size && gpv0<len;gpv1++,gpv0++){
	pocket.data[gpv1]=data[gpv0];
}
pocket.data[gpv1]=0;
}
else{
pocket.arg=0;
pocket.size=sprintf(pocket.data,"%d",ERROR_CMD__ARG_LENGTH);
}
}
else{	
pocket.arg=0;
pocket.size=sprintf(pocket.data,"%d",ERROR_CMD__CMD_NOT_FIND);
}


cmd_pocket_sender(pocket);
gpv0--; //for checking better
}


}
}





////////////////////////////////////////////////////////////////////////////

void cmd_init(void){
	wifi_respond_q=osMailCreate(osMailQ(wifi_respond_q),NULL);
	advance_option_q=osMailCreate(osMailQ(advance_option_q),NULL);
	nrf_option_de_dc_q=osMailCreate(osMailQ(nrf_option_de_dc_q),NULL);
	wm_q=osMailCreate(osMailQ(wm_q),NULL);	
}







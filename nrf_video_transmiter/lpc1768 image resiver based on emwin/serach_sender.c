#include "search_sender.h"
#include "nrf.h"
#include "cmsis_os.h"
#include <string.h>


#define SEARCH_TRANSMITTER_MAX_CNT           16


search_transmitter_t search_transmitter_data[SEARCH_TRANSMITTER_MAX_CNT];
int                  search_transmitter_data_cnt;  

#define SEARCH_TRANSMITTER_RESULT_MAX_CNT    5

search_transmitter_t search_transmitter_data_result[SEARCH_TRANSMITTER_RESULT_MAX_CNT];
int                  search_transmitter_data_result_cnt;  



#define MAX_CHECK_DELAY 2
#define MAX_SERCH_REPEAT 800

union select_tr_pocket{
		unsigned char resived_pocket[32];
	 
	  struct{
	      char sof[2];
			  char name[16];
	      unsigned char address[5];
			  unsigned char channel;
		}in_pocket;
};


unsigned char search_address[5]={0xf,0xa,0xf,0xa,25};



search_transmitter_t  trasmitter_inf;


int serach_tr_select_func(int in,MENU_OBJECT *menu){
trasmitter_inf=search_transmitter_data_result[menu->selected_item-SELECT_TR_MENU_ITEM];
trasmitter_inf.update_used_for_display=1;
	
return 2;
}








int search_tr(int in,MENU_OBJECT *menu){
	int gpv0,gpv1,gpv2,gpv3,gpv4;
  union select_tr_pocket data;


  search_transmitter_data_cnt=0;


	gpv4=0;
	nrf_write_command(0xe1,NULL,0);
	nrf_write_command(0xe2,NULL,0);		
	nrf_set_address(search_address,0);
	nrf_set_address(search_address,100);
		
  nrf_set_channel(2);
	
	nrf_set_rf_mode(1);
	nrf_irq_clr_stat();

	nrf_enable_rf(1);
	for(gpv0=0;gpv0<MAX_SERCH_REPEAT;gpv0++){
		osDelay(MAX_CHECK_DELAY);
	  if(nrf_irq_stat()){
	
		  gpv4++;
	    nrf_irq_clr_stat();	
   
			nrf_read_data(data.resived_pocket,32);
	
			if(data.in_pocket.sof[0]=='T' && data.in_pocket.sof[1]=='R'){
			for(gpv1=0;gpv1<search_transmitter_data_cnt;gpv1++){
				
				if(search_transmitter_data[gpv1].address[0]==data.in_pocket.address[0]){
				if(search_transmitter_data[gpv1].address[1]==data.in_pocket.address[1]){
				if(search_transmitter_data[gpv1].address[2]==data.in_pocket.address[2]){
				if(search_transmitter_data[gpv1].address[3]==data.in_pocket.address[3]){
				if(search_transmitter_data[gpv1].address[4]==data.in_pocket.address[4]){
					search_transmitter_data[gpv1].signal_power++;
					break;
				}					
				}					
				}					
				}					
				}
     
			}
			
			if((gpv1==search_transmitter_data_cnt)&&(search_transmitter_data_cnt<SEARCH_TRANSMITTER_MAX_CNT)){
				search_transmitter_data[search_transmitter_data_cnt].address[0]=data.in_pocket.address[0];
				search_transmitter_data[search_transmitter_data_cnt].address[1]=data.in_pocket.address[1];
				search_transmitter_data[search_transmitter_data_cnt].address[2]=data.in_pocket.address[2];
				search_transmitter_data[search_transmitter_data_cnt].address[3]=data.in_pocket.address[3];
				search_transmitter_data[search_transmitter_data_cnt].address[4]=data.in_pocket.address[4];
				search_transmitter_data[search_transmitter_data_cnt].signal_power=1;
				search_transmitter_data[search_transmitter_data_cnt].channel=data.in_pocket.channel;
				data.in_pocket.name[15]=0;
				strcpy(search_transmitter_data[search_transmitter_data_cnt].name,data.in_pocket.name);
				search_transmitter_data_cnt++;
			}
		}
		
		}
	}
	nrf_enable_rf(0);
	
	if(search_transmitter_data_cnt==0){
		menu->menu_items=SELECT_TR_MENU_ITEM;
		sprintf(menu->menu_item_text[0],"1)Search Sg=%d " ,gpv4);
		nrf_write_stat(0xff);
    trasmitter_inf.signal_power=0;
		trasmitter_inf.update_used_for_display=1;
		return 2;
	}
	
	
	if(search_transmitter_data_cnt>=SEARCH_TRANSMITTER_RESULT_MAX_CNT) search_transmitter_data_result_cnt=SEARCH_TRANSMITTER_RESULT_MAX_CNT;
	else                                                               search_transmitter_data_result_cnt=search_transmitter_data_cnt;
	
	for(gpv1=0;gpv1<search_transmitter_data_result_cnt;gpv1++){
	
	gpv2=-1;
  gpv3=0;		
	for(gpv0=0;gpv0<search_transmitter_data_cnt;gpv0++){
			if(search_transmitter_data[gpv0].signal_power>gpv2){
			gpv3=gpv0;				
			}
	}
	
	search_transmitter_data_result[gpv1]=search_transmitter_data[gpv3];
	search_transmitter_data[gpv3].signal_power=-1;
  }
	
	menu->menu_items=SELECT_TR_MENU_ITEM+search_transmitter_data_result_cnt;
	
	
	for(gpv0=0;gpv0<search_transmitter_data_result_cnt;gpv0++){
	menu->menu_item_text[SELECT_TR_MENU_ITEM+gpv0]=search_transmitter_data_result[gpv0].name;
	menu->menu_item_func[SELECT_TR_MENU_ITEM+gpv0]=serach_tr_select_func;
	}
	
		sprintf(menu->menu_item_text[0],"1)Search r=%d s=%d",search_transmitter_data_result_cnt,gpv4);
	
	return 2;
}












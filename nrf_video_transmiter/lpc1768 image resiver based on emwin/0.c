#include "cmsis_os.h"
#include "lpc17xx.h"
#include "gui.h"
#include "gpdma_lpc17xx.h"
#include "lcd.h"
#include "menu.h"
#include "0.h"
#include "search_sender.h"
#include "nrf.h"
#include "format.h"

void input_init(void){
	LPC_GPIO2->FIODIR&=~0X1E;
	LPC_GPIO2->FIODIR|=1|1<<6;
	LPC_GPIO2->FIOSET=1;
	LPC_GPIO2->FIOCLR=1<<6;
}


//up=1 down=2 right=3 left=4 center=5

int input_read(void){
	int gpv0;
	gpv0=~LPC_GPIO2->FIOPIN;
  if(gpv0 & 2) return 4;
	if(gpv0 & 4) return 5;
	if(gpv0 & 8) return 1;
	if(gpv0 & 16) return 3;
	if(gpv0 & 32) return 2;
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define signal_max_cnt        8
signal_t signal,signal_buffer[signal_max_cnt];



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MENU_OBJECT  main_menu,signal_menu,download_menu,format_menu;
#define MENU_STRING_SIZE    20

#define main_menu_item_cnt  3 


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void download(image_format_t *format);

int download_image(int key_state,MENU_OBJECT *menu){
download(format);
return 2;
}


















////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int format_image_back_req;
int format_image_back(int key_state,MENU_OBJECT *menu){
format_image_back_req=0;
return 0;
}


int format_image(int key_state,MENU_OBJECT *menu){

menu_create(&format_menu);
format_image_back_req=1;
while(format_image_back_req){
	menu_update(&format_menu);
}
return 2;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char select_tr_menu_char[3][MENU_STRING_SIZE]={
	"Select Tr", //as title
	"1)Serach",
	"2)Back"
};


int select_tr_back_req;
int select_transmitter_back(int key_state,MENU_OBJECT *menu){
select_tr_back_req=0;
return 0;
}

int select_transmitter(int key_state,MENU_OBJECT *menu){
int gpv0;

signal_menu.selected_item=0;
signal_menu.menu_items=SELECT_TR_MENU_ITEM;	
sprintf(signal_menu.menu_item_text[0],"1)Search");	
	
menu_create(&signal_menu);
select_tr_back_req=1;	
	
while(select_tr_back_req){
	menu_update(&signal_menu);
}

return 2;
}


















////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char main_menu_title[MENU_STRING_SIZE]={
	"CR \0",
};

char main_menu_item_text[main_menu_item_cnt][MENU_STRING_SIZE]={
	"1)Download Image",
	"2)Image Format",
	"3)Select Transmitter"
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(){
	int gpv0,gpv1;
	GPDMA_Initialize();
	lcd_init();
	GUI_Init();
  input_init();
	lcd_update();
  nrf_init();
  nrf_init();
	
	for(gpv0=0;gpv0<signal_max_cnt;gpv0++) signal_buffer[gpv0].char_stat=0;
	
	signal.address[0]=0;
	signal.address[1]=0;
	signal.address[2]=0;
	signal.address[3]=0;
	signal.address[4]=0;	
	
	signal.name[0]=0;
	signal.update_req=0;
	
	
	main_menu.input_funct=input_read;
	main_menu.menu_items=main_menu_item_cnt;
	main_menu.selected_item=0;
	main_menu.title_text=main_menu_title;	
	
	
	main_menu.menu_item_func[0]=download_image;
	main_menu.menu_item_func[1]=format_image;
	main_menu.menu_item_func[2]=select_transmitter;
	
	main_menu.menu_item_text[0]=main_menu_item_text[0];
	main_menu.menu_item_text[1]=main_menu_item_text[1];
	main_menu.menu_item_text[2]=main_menu_item_text[2];


  signal_menu.input_funct=input_read;
	signal_menu.menu_items=SELECT_TR_MENU_ITEM;
	
	signal_menu.selected_item=0;
	signal_menu.title_text=select_tr_menu_char[0];
	signal_menu.menu_item_text[0]=select_tr_menu_char[1];
	signal_menu.menu_item_text[1]=select_tr_menu_char[2];
  signal_menu.menu_item_func[0]=search_tr;	
	signal_menu.menu_item_func[1]=select_transmitter_back;
 
  image_format_init(&format_menu);
  
	menu_create(&main_menu);
	
//	GUI_DispStringAt("setting=",0,0);
//	nrf_write_stat(0x70);
//	GUI_DispHex(nrf_read_stat(),5);


//	GUI_DispStringAt("\nconfig=",0,20);
//	nrf_read_command(0,(unsigned char *)&gpv0,1);
//	GUI_DispHex(gpv0,3);

//	GUI_DispStringAt("\ndl=",0,40);
//	nrf_read_command(1,(unsigned char *)&gpv0,1);
//	GUI_DispHex(gpv0,3);

//	lcd_update();
	
	while(1){
	menu_update(&main_menu);
	if(trasmitter_inf.update_used_for_display){
		sprintf(main_menu.title_text,"CR %x:%x:%x:%x:%x",trasmitter_inf.address[0],trasmitter_inf.address[1],trasmitter_inf.address[2],trasmitter_inf.address[3],trasmitter_inf.address[4]);
		//sprintf(main_menu.title_text,"CR %s",trasmitter_inf.name);
		menu_create(&main_menu);
		trasmitter_inf.update_used_for_display=0;
	}
	}
}



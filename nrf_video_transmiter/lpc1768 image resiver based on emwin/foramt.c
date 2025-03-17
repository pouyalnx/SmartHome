#include <string.h>
#include "format.h"
#include "lcd.h"
#include "menu.h"
#include "0.h"

image_format_t *format;
image_format_t  if_formats[IMAGE_FORMAT_CNT];


char if_menu_text[2][20]={
	"Select format",
	"x) Back"
};


int if_j;

int if_rgb_565_inti(void){
	//120*128/15
	if_j=-1;
	return 1024;
}


int if_rgb_565_data(unsigned short *in){
	int gpv1;
	
//	if(if_j!=in[0]){
	gpv1=in[0]*15;
	memcpy(&lcd_ram[gpv1],&in[1],30);
	if_j=in[0];
	return if_j;
//  }
	
//	return 0;
}


int image_format_menu_select(int id,MENU_OBJECT *menu){
  format=&if_formats[id];
	return 0;
}

extern int format_image_back(int key_state,MENU_OBJECT *menu);


void image_format_init(MENU_OBJECT *menu){
	if_formats[0].decoder_data=if_rgb_565_data;
	if_formats[0].decoder_init=if_rgb_565_inti;
	if_formats[0].id=0;
	strcpy(if_formats[0].name,"1) RGB565");

  format=&if_formats[0];
	
	menu->input_funct=input_read;
	menu->menu_items=IMAGE_FORMAT_CNT+1;
	menu->menu_item_func[0]=image_format_menu_select;
	menu->menu_item_text[0]=if_formats[0].name;
	menu->menu_item_func[1]=format_image_back;
	menu->menu_item_text[1]=if_menu_text[1];
	menu->selected_item=0;
	menu->title_text=if_menu_text[0];

}












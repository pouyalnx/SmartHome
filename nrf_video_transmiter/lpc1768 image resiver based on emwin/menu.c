#include "menu.h"



int menu_create(MENU_OBJECT *menu){
int gpv0;
  	
	if(menu->menu_items>MENU_MAX_ITEM) menu->menu_items=MENU_MAX_ITEM;
	
	for(gpv0=0;gpv0<menu->menu_items;gpv0++){
	if(menu->menu_item_func[gpv0]==NULL) return 0;
	if(menu->menu_item_text[gpv0]==NULL) return 0;		
	}
	if(menu->input_funct==NULL) return 0;
	
GUI_SetFont(&MENU_ITEM_FONT);	
	
	
GUI_SetBkColor(MENU_BACK_COLOUR);	
GUI_SetColor(MENU_TEXT_COLOUR);
GUI_Clear();	

GUI_SetBkColor(MENU_TITLE_BK_COLUR);	
GUI_SetColor(MENU_TEXT_COLOUR);	

GUI_DispStringAtCEOL(menu->title_text,MENU_TITLE_XS,MENU_TITLE_YS);	
	
GUI_SetBkColor(MENU_BACK_COLOUR);	
GUI_SetColor(MENU_TEXT_COLOUR);	
	
for(gpv0=0;gpv0<menu->menu_items;gpv0++){
	if(gpv0==menu->selected_item){
		GUI_SetBkColor(MENU_SELECT_COLOUR);
		GUI_DispStringAtCEOL(menu->menu_item_text[gpv0],MENU_ITEM_XS,MENU_ITEM_YS+gpv0*MENU_ITEM_ITEM_DISTANCE);		
    GUI_SetBkColor(MENU_BACK_COLOUR);	
	}
	else{
		GUI_DispStringAtCEOL(menu->menu_item_text[gpv0],MENU_ITEM_XS,MENU_ITEM_YS+gpv0*MENU_ITEM_ITEM_DISTANCE);
	}
}

	LCD_UPDATE;
  return 1;
}


int menu_title_updata(MENU_OBJECT *menu,int refresh_lcd){
GUI_SetBkColor(MENU_TITLE_BK_COLUR);	
GUI_DispStringAtCEOL(menu->title_text,MENU_TITLE_XS,MENU_TITLE_YS);		
if(refresh_lcd) LCD_UPDATE;
}


int menu_update(MENU_OBJECT *menu){
int gpv0,update_req,last_selected_item;	
update_req=0;
last_selected_item=0;
gpv0=menu->input_funct();	
if(gpv0){	
MENU_DELAY_FUNCTION(MENU_INPUT_THRESHOLD);
if(gpv0==menu->input_funct()){
	
	switch(gpv0){
		case 1:
		last_selected_item=menu->selected_item;
		menu->selected_item--;
		if(menu->selected_item<0) menu->selected_item=menu->menu_items-1;
		update_req=1;
		break;
		case 2:
		last_selected_item=menu->selected_item;
		menu->selected_item++;
		if(menu->selected_item>=menu->menu_items) menu->selected_item=0;
		update_req=1;			
		break;
		case 5:
		case 3:
    case 4:			
		update_req=5;	
		break;	
	}

	switch(update_req){
		case 1:
		GUI_SetBkColor(MENU_SELECT_COLOUR);
		GUI_DispStringAtCEOL(menu->menu_item_text[menu->selected_item],MENU_ITEM_XS,MENU_ITEM_YS+menu->selected_item*MENU_ITEM_ITEM_DISTANCE);		
    GUI_SetBkColor(MENU_BACK_COLOUR);			
		GUI_DispStringAtCEOL(menu->menu_item_text[last_selected_item],MENU_ITEM_XS,MENU_ITEM_YS+last_selected_item*MENU_ITEM_ITEM_DISTANCE);
		LCD_UPDATE;
		break;
		case 5:
		GUI_SetBkColor(MENU_HOLDED_COLOUR);
		GUI_DispStringAtCEOL(menu->menu_item_text[menu->selected_item],MENU_ITEM_XS,MENU_ITEM_YS+menu->selected_item*MENU_ITEM_ITEM_DISTANCE);		
		LCD_UPDATE;		
		break;
	}

	while(menu->input_funct()) MENU_DELAY_FUNCTION(MENU_DELAY_UPDATE_IN);  
  if(gpv0==5 || gpv0==3 || gpv0==4)  last_selected_item=menu->menu_item_func[menu->selected_item](gpv0,menu);	
	


	switch(update_req){
		case 5:
	  switch(last_selected_item){		
		  case 0:
		  case 1:	
       GUI_SetBkColor(MENU_SELECT_COLOUR);
		   GUI_DispStringAtCEOL(menu->menu_item_text[menu->selected_item],MENU_ITEM_XS,MENU_ITEM_YS+menu->selected_item*MENU_ITEM_ITEM_DISTANCE);				
		   LCD_UPDATE;
			break;
			case 2:
			menu_create(menu);	
			break;
		}
		break;
	}

	
}	
}
	return 1;
}




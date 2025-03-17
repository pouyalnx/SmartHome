#ifndef _menu_v00
#define  _menu_v00

#include "gui.h"
#include "cmsis_os.h"
#include "lcd.h"
#include <stdio.h>










#define MENU_MAX_ITEM            8
#define MENU_ITEM_FONT           GUI_Font16B_1
#define MENU_ITEM_ITEM_DISTANCE  18
#define MENU_ITEM_XS             0
#define MENU_ITEM_YS             20

#define MENU_TITLE_XS            0
#define MENU_TITLE_YS            0
#define MENU_TITLE_BK_COLUR      GUI_GREEN
#define MENU_TITLE_TEXT_COLUR    GUI_WHITE


#define MENU_BACK_COLOUR    GUI_BLUE
#define MENU_TEXT_COLOUR    GUI_WHITE
#define MENU_HOLDED_COLOUR  GUI_CYAN
#define MENU_SELECT_COLOUR  GUI_YELLOW

#define MENU_DELAY_FUNCTION(x) 	osDelay(x) 
#define LCD_UPDATE              lcd_update()

#define MENU_INPUT_THRESHOLD   10
#define MENU_DELAY_UPDATE_IN   5



typedef struct{
	int menu_items;
	int selected_item;
	
	char *title_text;
	char *menu_item_text[MENU_MAX_ITEM];
	int (*menu_item_func[MENU_MAX_ITEM])(int in,void *menu); //in=input key code  return value 0=no action by menu 1=just item text must be updated 2=menu must created again 
	int (*input_funct)(void);
  
	
}MENU_OBJECT;








extern int menu_create(MENU_OBJECT *menu);
extern int menu_update(MENU_OBJECT *menu);
extern int menu_title_updata(MENU_OBJECT *menu,int refresh_lcd);




#endif


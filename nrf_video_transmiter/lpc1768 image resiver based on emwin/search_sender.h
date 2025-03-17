#ifndef search_sender_123
#define search_sender_123
#include "menu.h"

#define SELECT_TR_MENU_ITEM 2

extern int search_tr(int in,MENU_OBJECT *menu);
	
typedef struct{
	char name[16];
	unsigned char address[5];
	unsigned char channel;
	unsigned char update_used_for_display;
	short signal_power;
}search_transmitter_t;



#define SEARCH_TRANSMITTER_RESULT_MAX_CNT    5

extern search_transmitter_t search_transmitter_data_result[SEARCH_TRANSMITTER_RESULT_MAX_CNT];
extern int                  search_transmitter_data_result_cnt;  


extern search_transmitter_t  trasmitter_inf;





#endif


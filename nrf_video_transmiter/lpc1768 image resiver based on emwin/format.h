#ifndef _test_base_format
#define _test_base_format
#include "menu.h"


#define IMAGE_FORMAT_CNT 1

typedef struct{
	int (*decoder_init)(void);
	int (*decoder_data)(unsigned short *in);
	char name[17];
	unsigned char id;
}image_format_t;

extern image_format_t *format;
extern void image_format_init(MENU_OBJECT *menu);






#endif

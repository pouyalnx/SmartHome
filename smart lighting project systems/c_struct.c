#include "common_struct.h"
#include "e2.h"
#include <stdio.h>
#include "lpc177x_8x.h"






Device      device[DEVICE_MAX_CNT];
Device_control  dc_control;



////////////////////////////////////////////////////////////////////////

void e2_read_string(string *str,int page){
unsigned int *ptr;
ptr=(unsigned int *)str;
//string pack size is 64
e2_read(ptr,16,page);
e2_read(ptr+4,16,page+1);
e2_read(ptr+8,16,page+2);
e2_read(ptr+12,16,page+3);	
}
void e2_write_string(string *str,int page){
unsigned int *ptr;
ptr=(unsigned int *)str;
//string pack size is 64
e2_write(ptr,16,page);
e2_write(ptr+4,16,page+1);
e2_write(ptr+8,16,page+2);
e2_write(ptr+12,16,page+3);		
}

///////////////////////////////////////////////////////////////////////
//notic : as all transfers doned by int varibles struct align must be int
//warning all str must have align 4
void e2_read_wifi_password(string *str){
	e2_read_string(str,E2_START_PAGE_WIFI_PASSWORD);
}

void e2_write_wifi_password(string *str){
	e2_write_string(str,E2_START_PAGE_WIFI_PASSWORD);	
}

void e2_read_wifi_name(string *str){
	e2_read_string(str,E2_START_PAGE_WIFI_NAME);	
}
void e2_write_wifi_name(string *str){
	e2_write_string(str,E2_START_PAGE_WIFI_NAME);
}

void e2_read_admin_password(string *str){
	e2_read_string(str,E2_START_PAGE_ADMIN_LOGINE_PASSWORD);
}
void e2_write_admin_password(string *str){
	e2_write_string(str,E2_START_PAGE_ADMIN_LOGINE_PASSWORD);	
}


void e2_read_device(Device *dev,int address){
   e2_read((unsigned int *)dev,16,address+E2_START_PAGE_DEVICE);
}

void e2_write_device(Device *dev,int address){
   e2_write((unsigned int *)dev,16,address+E2_START_PAGE_DEVICE);
}

void e2_read_device_contol(Device_control *dc){
	e2_read((unsigned int *)dc,16,E2_START_PAGE_DEVICE_CONTROL);
}
void e2_write_device_contol(Device_control *dc){
	e2_write((unsigned int *)dc,16,E2_START_PAGE_DEVICE_CONTROL);	
}

void e2_read_config(CONFIG *dc){
	e2_read((unsigned int *)dc,E2_START_PAGE_CONFIG_SIZE,E2_START_PAGE_CONFIG);	
}
void e2_write_config(CONFIG *dc){
	e2_write((unsigned int *)dc,E2_START_PAGE_CONFIG_SIZE,E2_START_PAGE_CONFIG);		
}

//////////////////////////////////////////////////////////////////////

void c_struct_init(void){
int gpv0;
e2_init(SystemCoreClock);
for(gpv0=0;gpv0<DEVICE_MAX_CNT;gpv0++){
e2_read_device(&device[gpv0],gpv0);
}
e2_read_device_contol(&dc_control);

}



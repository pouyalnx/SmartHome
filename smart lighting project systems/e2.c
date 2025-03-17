#include <stdio.h>
#include "lpc177x_8x.h"
#include "e2.h"


#define E2_MEM_SIZE  4096


void e2_init(unsigned int core_clk){
unsigned int delay;
LPC_EEPROM->PWRDWN=0;
LPC_EEPROM->CLKDIV=(core_clk/375000)-1;
core_clk/=1000000;
delay=(core_clk*15)/1000+1;	
delay|=((core_clk*55)/1000+1)<<8;	
delay|=((core_clk*35)/1000+1)<<16;	
LPC_EEPROM->WSTATE=delay;
LPC_EEPROM->INT_SET_ENABLE=1<26;
}


int e2_write(unsigned int *data,int len,int page){
int gpv0;
page*=16;
if(page>=E2_MEM_SIZE) return 0;
len+=3;
len/=4;	
	
LPC_EEPROM->ADDR=page;
LPC_EEPROM->CMD=5;	

for(gpv0=0;gpv0<len;gpv0++){
LPC_EEPROM->INT_CLR_STATUS=1<<26;
LPC_EEPROM->WDATA=data[gpv0];
while((LPC_EEPROM->INT_STATUS & 1<<26)==0);	
}

page&=0xffc0;

LPC_EEPROM->INT_CLR_STATUS=1<<28;
LPC_EEPROM->ADDR=page;
LPC_EEPROM->CMD=6;	
while((LPC_EEPROM->INT_STATUS & 1<<28)==0);	

return 1;	
}


int e2_read(unsigned int *data,int len,int page){
int gpv0;
page*=16;
if(page>=E2_MEM_SIZE) return 0;
len+=3;
len/=4;	
	
LPC_EEPROM->INT_CLR_STATUS=1<<26;
LPC_EEPROM->ADDR=page;
LPC_EEPROM->CMD=10;	

	
for(gpv0=0;gpv0<len;gpv0++){
while((LPC_EEPROM->INT_STATUS & 1<<26)==0);	
data[gpv0]=LPC_EEPROM->RDATA;
LPC_EEPROM->INT_CLR_STATUS=1<<26;
}

return 1;	
}








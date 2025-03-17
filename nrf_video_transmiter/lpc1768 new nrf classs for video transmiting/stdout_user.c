/*-----------------------------------------------------------------------------
 * Name:    stdout_user.c
 * Purpose: STDOUT User Template
 * Rev.:    1.0.0
 *-----------------------------------------------------------------------------*/
 
/* Copyright (c) 2013 - 2015 ARM LIMITED
 
   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

/* ready function

extern void stdout_init(void);
extern void stdout_set(int page,int column);
extern void stdout_invert(int in);
extern void stdout_cmdmode(int in);
*/


#include "driver_i2c.h"
#define LCD_I2C_ADDR 0X3c



extern ARM_DRIVER_I2C Driver_I2C1;


typedef struct{
	unsigned char page_start;
	unsigned char column_start;
  unsigned char xor_val;
  unsigned char cmd_mode;
}SSD1306_inf;



//////////////varibles//////////////////////
ARM_DRIVER_I2C *ssd1306_lcd;
unsigned char buffer_stdout[128+16];

SSD1306_inf stdout_inf;


void SSD1306_send_data(void *data,int size,int continue_req){
  ssd1306_lcd->MasterTransmit(LCD_I2C_ADDR,data,size,continue_req);	
  while(ssd1306_lcd->GetStatus().busy);  //wait for last transfer
  
}




void stdout_power(int power){
	unsigned char data[2];
	data[0]=0X80;
	data[1]=0xae;
	if(power) data[1]=0xaf;
  SSD1306_send_data(data,2,false);	
}


void stdout_contrast(int value){
	unsigned char data[3];
	data[0]=0;
	data[1]=0x81;
	data[2]=value;
  SSD1306_send_data(data,3,false);	
}


void stdout_invert(int in){
	if(in) stdout_inf.xor_val=0xff;
	else   stdout_inf.xor_val=0;     
}

void stdout_cmdmode(int in){
	if(in) stdout_inf.cmd_mode=0xf;
	else   stdout_inf.cmd_mode=0;     
}


/**
  Put a character to the stdout
 
  \param[in]   ch  Character to output
  \return          The character written, or -1 on write error.
*/
void stdout_cls(void){
	int gpv0;
	buffer_stdout[0]=0x80;
	buffer_stdout[1]=0xb0|0;
	buffer_stdout[2]=0x80;
	buffer_stdout[3]=0;
	buffer_stdout[4]=0x80;
	buffer_stdout[5]=0x10;
	buffer_stdout[6]=0x40;
	for(gpv0=7;gpv0<135;gpv0++){
		buffer_stdout[gpv0]=0;
	}
	for(gpv0=0;gpv0<8;gpv0++){
	buffer_stdout[1]=0xb0|gpv0;		
	SSD1306_send_data(buffer_stdout,135,false);
	}
}


void stdout_init(void){
int gpv0;
ssd1306_lcd=&Driver_I2C1;	
ssd1306_lcd->Initialize(NULL);
ssd1306_lcd->PowerControl(ARM_POWER_FULL);	
ssd1306_lcd->Control(ARM_I2C_BUS_SPEED,ARM_I2C_BUS_SPEED_FAST);	

stdout_power(1);
	
buffer_stdout[0]=0x0;
buffer_stdout[1]=0xd5;	
buffer_stdout[2]=0xf0;
ssd1306_lcd->MasterTransmit(LCD_I2C_ADDR,buffer_stdout,3,true);	
while(ssd1306_lcd->GetStatus().busy);


buffer_stdout[0]=0x0;
buffer_stdout[1]=0xa8;
buffer_stdout[2]=0x3f;
buffer_stdout[3]=0;
ssd1306_lcd->MasterTransmit(LCD_I2C_ADDR,buffer_stdout,4,true);
while(ssd1306_lcd->GetStatus().busy);

buffer_stdout[0]=0x80;
buffer_stdout[1]=0x40;
buffer_stdout[2]=0x0;
buffer_stdout[3]=0x8d;
buffer_stdout[4]=0x14;	
ssd1306_lcd->MasterTransmit(LCD_I2C_ADDR,buffer_stdout,5,true);
while(ssd1306_lcd->GetStatus().busy);


buffer_stdout[0]=0;
buffer_stdout[1]=0x20;
buffer_stdout[2]=2;
SSD1306_send_data(buffer_stdout,3,true);


buffer_stdout[0]=0x80;
buffer_stdout[1]=0xa6;


buffer_stdout[2]=0x80;
buffer_stdout[3]=0xa4;


buffer_stdout[4]=0x80;
buffer_stdout[5]=0xa1;


buffer_stdout[6]=0x80;
buffer_stdout[7]=0xc8;
ssd1306_lcd->MasterTransmit(LCD_I2C_ADDR,buffer_stdout,8,true);
while(ssd1306_lcd->GetStatus().busy);

buffer_stdout[0]=0x0;
buffer_stdout[1]=0xda;
buffer_stdout[2]=0x12;
ssd1306_lcd->MasterTransmit(LCD_I2C_ADDR,buffer_stdout,3,true);
while(ssd1306_lcd->GetStatus().busy);

stdout_contrast(0X20);

buffer_stdout[0]=0x0;
buffer_stdout[1]=0xd9;
buffer_stdout[2]=0xF1;
ssd1306_lcd->MasterTransmit(LCD_I2C_ADDR,buffer_stdout,3,true);
while(ssd1306_lcd->GetStatus().busy);

buffer_stdout[0]=0x0;
buffer_stdout[1]=0xdb;
buffer_stdout[2]=0x40;
ssd1306_lcd->MasterTransmit(LCD_I2C_ADDR,buffer_stdout,3,false);
while(ssd1306_lcd->GetStatus().busy);

for(gpv0=0;gpv0<100000;gpv0++);

buffer_stdout[0]=0x80;
buffer_stdout[1]=0xaf;
ssd1306_lcd->MasterTransmit(LCD_I2C_ADDR,buffer_stdout,2,true);
while(ssd1306_lcd->GetStatus().busy);

buffer_stdout[0]=0;
buffer_stdout[1]=0x20;
buffer_stdout[2]=2;
SSD1306_send_data(buffer_stdout,3,false);

stdout_inf.column_start=0;
stdout_inf.page_start=0;
stdout_inf.xor_val=0;

stdout_cls();
}



void stdout_set(int page,int column){
	stdout_inf.page_start=page&7;
	stdout_inf.column_start=column&0x7f;
}


extern const unsigned char consolasf[][6];







void check_line_end(int ch){
if(ch=='\n'){
	stdout_inf.column_start=128;
}
else{
	stdout_inf.column_start+=6;
}

if(stdout_inf.column_start>122){
stdout_inf.page_start++;
stdout_inf.column_start=0;
	
	
if(stdout_inf.cmd_mode==0x1f){
stdout_inf.cmd_mode=0xff; //reqst
}	

if(stdout_inf.page_start>7){
	stdout_inf.page_start=0;
  
	if(stdout_inf.cmd_mode==0xf){
  stdout_inf.cmd_mode=0xff; //reqst
  }		
}

}

}







int stdout_putchar (int ch) {
  
	if(ch=='\n' || ch=='\t'){
	check_line_end('\n');
	return ch;
	}

	if(ch<' ' && ch>'~') ch=0X5F;
	else                 ch-=0x20;
	
	if(stdout_inf.cmd_mode==0xff){
  stdout_inf.cmd_mode=0x1f;
	buffer_stdout[0]=0x0;
	buffer_stdout[1]=0xd3;
	buffer_stdout[2]=stdout_inf.page_start*8+8;	
	SSD1306_send_data(buffer_stdout,3,true);
	}
	
	
	buffer_stdout[0]=0x80;
	buffer_stdout[1]=0xb0|stdout_inf.page_start;
	buffer_stdout[2]=0x80;
	buffer_stdout[3]=stdout_inf.column_start&0xf;
	buffer_stdout[4]=0x80;
	buffer_stdout[5]=0x10|((stdout_inf.column_start>>4)&0xf);
	buffer_stdout[6]=0x40;
	if(ch)
	buffer_stdout[7]=consolasf[ch][0]^stdout_inf.xor_val;	
	buffer_stdout[8]=consolasf[ch][1]^stdout_inf.xor_val;	
	buffer_stdout[9]=consolasf[ch][2]^stdout_inf.xor_val;	
	buffer_stdout[10]=consolasf[ch][3]^stdout_inf.xor_val;	
	buffer_stdout[11]=consolasf[ch][4]^stdout_inf.xor_val;	
	buffer_stdout[12]=consolasf[ch][5]^stdout_inf.xor_val;	
	buffer_stdout[13]=stdout_inf.xor_val;			
	buffer_stdout[14]=stdout_inf.xor_val;		
	
	SSD1306_send_data(buffer_stdout,14,false);
	
	check_line_end(0);
	
  return (ch);
}
 

//const font data;
const unsigned char consolasf[][6]=
{
/* Code: 0x20, ASCII Character: ' ' */
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

/* Code: 0x21, ASCII Character: '!' */
{0x00, 0x00, 0x5F, 0x00, 0x00, 0x00},

/* Code: 0x22, ASCII Character: '"' */
{0x00, 0x00, 0x03, 0x00, 0x03, 0x00},

/* Code: 0x23, ASCII Character: '#' */
{0x14, 0x74, 0x1F, 0x74, 0x1F, 0x14},

/* Code: 0x24, ASCII Character: '$' */
{0x00, 0x46, 0xE9, 0x5F, 0x49, 0x30},

/* Code: 0x25, ASCII Character: '%' */
{0x47, 0x35, 0x0F, 0x74, 0x52, 0x71},

/* Code: 0x26, ASCII Character: '&' */
{0x38, 0x4F, 0x49, 0x57, 0x78, 0x40},

/* Code: 0x27, ASCII Character: ''' */
{0x00, 0x00, 0x03, 0x00, 0x00, 0x00},

/* Code: 0x28, ASCII Character: '(' */
{0x00, 0x00, 0x3C, 0xC3, 0x00, 0x00},

/* Code: 0x29, ASCII Character: ')' */
{0x00, 0x00, 0xC3, 0x3C, 0x00, 0x00},

/* Code: 0x2A, ASCII Character: '*' */
{0x00, 0x05, 0x02, 0x0F, 0x02, 0x05},

/* Code: 0x2B, ASCII Character: '+' */
{0x00, 0x08, 0x08, 0x3E, 0x08, 0x08},

/* Code: 0x2C, ASCII Character: ',' */
{0x00, 0x00, 0xE0, 0xE0, 0x00, 0x00},

/* Code: 0x2D, ASCII Character: '-' */
{0x00, 0x10, 0x10, 0x10, 0x00, 0x00},

/* Code: 0x2E, ASCII Character: '.' */
{0x00, 0x00, 0x60, 0x60, 0x00, 0x00},

/* Code: 0x2F, ASCII Character: '/' */
{0x00, 0x80, 0x60, 0x1C, 0x03, 0x00},

/* Code: 0x30, ASCII Character: '0' */
{0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00},

/* Code: 0x31, ASCII Character: '1' */
{0x00, 0x42, 0x41, 0x7F, 0x40, 0x40},

/* Code: 0x32, ASCII Character: '2' */
{0x00, 0x42, 0x61, 0x51, 0x49, 0x46},

/* Code: 0x33, ASCII Character: '3' */
{0x00, 0x41, 0x49, 0x49, 0x36, 0x00},

/* Code: 0x34, ASCII Character: '4' */
{0x18, 0x14, 0x12, 0x11, 0x7F, 0x10},

/* Code: 0x35, ASCII Character: '5' */
{0x00, 0x4F, 0x49, 0x49, 0x31, 0x00},

/* Code: 0x36, ASCII Character: '6' */
{0x00, 0x3E, 0x4B, 0x49, 0x49, 0x30},

/* Code: 0x37, ASCII Character: '7' */
{0x00, 0x01, 0x41, 0x31, 0x0D, 0x03},

/* Code: 0x38, ASCII Character: '8' */
{0x00, 0x36, 0x4D, 0x49, 0x59, 0x36},

/* Code: 0x39, ASCII Character: '9' */
{0x00, 0x46, 0x49, 0x49, 0x29, 0x1E},

/* Code: 0x3A, ASCII Character: ':' */
{0x00, 0x00, 0x66, 0x66, 0x00, 0x00},

/* Code: 0x3B, ASCII Character: ';' */
{0x00, 0x00, 0xE6, 0xE6, 0x00, 0x00},

/* Code: 0x3C, ASCII Character: '<' */
{0x00, 0x00, 0x18, 0x24, 0x42, 0x00},

/* Code: 0x3D, ASCII Character: '=' */
{0x00, 0x28, 0x28, 0x28, 0x28, 0x28},

/* Code: 0x3E, ASCII Character: '>' */
{0x00, 0x42, 0x24, 0x18, 0x00, 0x00},

/* Code: 0x3F, ASCII Character: '?' */
{0x00, 0x00, 0x58, 0x09, 0x07, 0x00},

/* Code: 0x40, ASCII Character: '@' */
{0x7C, 0x83, 0x38, 0x3C, 0x21, 0x1F},

/* Code: 0x41, ASCII Character: 'A' */
{0x00, 0x70, 0x1E, 0x11, 0x1E, 0x70},

/* Code: 0x42, ASCII Character: 'B' */
{0x00, 0x7F, 0x49, 0x49, 0x49, 0x36},

/* Code: 0x43, ASCII Character: 'C' */
{0x00, 0x3E, 0x63, 0x41, 0x41, 0x00},

/* Code: 0x44, ASCII Character: 'D' */
{0x00, 0x7F, 0x41, 0x41, 0x63, 0x3E},

/* Code: 0x45, ASCII Character: 'E' */
{0x00, 0x7F, 0x49, 0x49, 0x49, 0x00},

/* Code: 0x46, ASCII Character: 'F' */
{0x00, 0x7F, 0x09, 0x09, 0x09, 0x00},

/* Code: 0x47, ASCII Character: 'G' */
{0x3C, 0x63, 0x41, 0x49, 0x79, 0x00},

/* Code: 0x48, ASCII Character: 'H' */
{0x00, 0x7F, 0x08, 0x08, 0x7F, 0x00},

/* Code: 0x49, ASCII Character: 'I' */
{0x00, 0x41, 0x41, 0x7F, 0x41, 0x41},

/* Code: 0x4A, ASCII Character: 'J' */
{0x00, 0x41, 0x41, 0x41, 0x3F, 0x00},

/* Code: 0x4B, ASCII Character: 'K' */
{0x00, 0x7F, 0x08, 0x36, 0x41, 0x00},

/* Code: 0x4C, ASCII Character: 'L' */
{0x00, 0x7F, 0x40, 0x40, 0x40, 0x00},

/* Code: 0x4D, ASCII Character: 'M' */
{0x7C, 0x03, 0x0C, 0x0C, 0x03, 0x7C},

/* Code: 0x4E, ASCII Character: 'N' */
{0x00, 0x7F, 0x0E, 0x30, 0x7F, 0x00},

/* Code: 0x4F, ASCII Character: 'O' */
{0x3E, 0x63, 0x41, 0x41, 0x63, 0x3E},

/* Code: 0x50, ASCII Character: 'P' */
{0x00, 0x7F, 0x09, 0x09, 0x09, 0x06},

/* Code: 0x51, ASCII Character: 'Q' */
{0x3E, 0x43, 0xC1, 0x61, 0x3E, 0x00},

/* Code: 0x52, ASCII Character: 'R' */
{0x00, 0x7F, 0x09, 0x19, 0x26, 0x40},

/* Code: 0x53, ASCII Character: 'S' */
{0x00, 0x46, 0x49, 0x49, 0x49, 0x31},

/* Code: 0x54, ASCII Character: 'T' */
{0x00, 0x01, 0x01, 0x7F, 0x01, 0x01},

/* Code: 0x55, ASCII Character: 'U' */
{0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F},

/* Code: 0x56, ASCII Character: 'V' */
{0x00, 0x07, 0x38, 0x40, 0x38, 0x07},

/* Code: 0x57, ASCII Character: 'W' */
{0x0F, 0x70, 0x18, 0x38, 0x70, 0x0F},

/* Code: 0x58, ASCII Character: 'X' */
{0x41, 0x62, 0x1C, 0x1C, 0x62, 0x41},

/* Code: 0x59, ASCII Character: 'Y' */
{0x01, 0x02, 0x04, 0x78, 0x04, 0x02},

/* Code: 0x5A, ASCII Character: 'Z' */
{0x00, 0x41, 0x71, 0x49, 0x47, 0x01},

/* Code: 0x5B, ASCII Character: '[' */
{0x00, 0x00, 0xFF, 0x81, 0x81, 0x00},

/* Code: 0x5C, ASCII Character: '\' */
{0x00, 0x00, 0x03, 0x1C, 0x60, 0x80},

/* Code: 0x5D, ASCII Character: ']' */
{0x00, 0x00, 0x00, 0xFF, 0x00, 0x00},

/* Code: 0x5E, ASCII Character: '^' */
{0x00, 0x06, 0x01, 0x06, 0x00, 0x00},

/* Code: 0x5F, ASCII Character: '_' */
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

/* Code: 0x60, ASCII Character: '`' */
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

/* Code: 0x61, ASCII Character: 'a' */
{0x00, 0x30, 0x4A, 0x4A, 0x6A, 0x7C},

/* Code: 0x62, ASCII Character: 'b' */
{0x00, 0x7F, 0x46, 0x42, 0x42, 0x3C},

/* Code: 0x63, ASCII Character: 'c' */
{0x00, 0x3C, 0x46, 0x42, 0x42, 0x00},

/* Code: 0x64, ASCII Character: 'd' */
{0x00, 0x3C, 0x42, 0x42, 0x22, 0x7F},

/* Code: 0x65, ASCII Character: 'e' */
{0x00, 0x3C, 0x6A, 0x4A, 0x4A, 0x4C},

/* Code: 0x66, ASCII Character: 'f' */
{0x04, 0x04, 0x7F, 0x04, 0x04, 0x00},

/* Code: 0x67, ASCII Character: 'g' */
{0xFC, 0x92, 0x92, 0x92, 0x4E, 0x02},

/* Code: 0x68, ASCII Character: 'h' */
{0x00, 0x7F, 0x02, 0x02, 0x7E, 0x00},

/* Code: 0x69, ASCII Character: 'i' */
{0x00, 0x42, 0x42, 0x7E, 0x40, 0x00},

/* Code: 0x6A, ASCII Character: 'j' */
{0x00, 0x02, 0x02, 0x02, 0xFE, 0x00},

/* Code: 0x6B, ASCII Character: 'k' */
{0x00, 0x7F, 0x18, 0x24, 0x42, 0x00},

/* Code: 0x6C, ASCII Character: 'l' */
{0x00, 0x40, 0x40, 0x7F, 0x40, 0x00},

/* Code: 0x6D, ASCII Character: 'm' */
{0x00, 0x7E, 0x02, 0x7E, 0x02, 0x7E},

/* Code: 0x6E, ASCII Character: 'n' */
{0x00, 0x7E, 0x02, 0x02, 0x7E, 0x00},

/* Code: 0x6F, ASCII Character: 'o' */
{0x00, 0x3C, 0x42, 0x42, 0x42, 0x3C},

/* Code: 0x70, ASCII Character: 'p' */
{0x00, 0xFE, 0x46, 0x42, 0x42, 0x3C},

/* Code: 0x71, ASCII Character: 'q' */
{0x00, 0x3C, 0x42, 0x42, 0x22, 0xFE},

/* Code: 0x72, ASCII Character: 'r' */
{0x00, 0x7E, 0x02, 0x02, 0x06, 0x00},

/* Code: 0x73, ASCII Character: 's' */
{0x00, 0x4C, 0x4A, 0x52, 0x32, 0x00},

/* Code: 0x74, ASCII Character: 't' */
{0x02, 0x02, 0x7F, 0x42, 0x42, 0x00},

/* Code: 0x75, ASCII Character: 'u' */
{0x00, 0x7E, 0x40, 0x40, 0x7E, 0x00},

/* Code: 0x76, ASCII Character: 'v' */
{0x00, 0x0E, 0x30, 0x40, 0x30, 0x0E},

/* Code: 0x77, ASCII Character: 'w' */
{0x00, 0x3E, 0x60, 0x1C, 0x30, 0x7C},

/* Code: 0x78, ASCII Character: 'x' */
{0x42, 0x66, 0x18, 0x18, 0x66, 0x42},

/* Code: 0x79, ASCII Character: 'y' */
{0x00, 0x0E, 0xB0, 0x60, 0x18, 0x06},

/* Code: 0x7A, ASCII Character: 'z' */
{0x00, 0x42, 0x72, 0x4E, 0x42, 0x00},

/* Code: 0x7B, ASCII Character: '{' */
{0x10, 0x10, 0xFF, 0x81, 0x00, 0x00},

/* Code: 0x7C, ASCII Character: '|' */
{0x00, 0x00, 0x00, 0xFF, 0x00, 0x00},

/* Code: 0x7D, ASCII Character: '}' */
{0x00, 0x00, 0x81, 0xFF, 0x10, 0x10},

/* Code: 0x7E, ASCII Character: '~' */
{0x18, 0x08, 0x18, 0x10, 0x18, 0x00},

{/* Code: 0x7F, ASCII Character: ' ' */
0xFF, 0xF9, 0x4D, 0xB2, 0x1E, 0x00 }
};



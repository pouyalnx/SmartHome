#include "Driver_SPI.h"
#include "lcd.h"
#include "lpc17xx.h"
#include "cmsis_os.h"

#define lcd_delay(x) osDelay(x)


unsigned short lcd_ram[16384];



void lcd_update(void){
	lcd_sf();
	lcd_wr_pixels_565(lcd_ram,16384);
	lcd_ef();
}

extern ARM_DRIVER_SPI Driver_SPI0;
ARM_DRIVER_SPI *lcd_bus;


void lcd_cs_control(int stat){
if(stat)
	lcd_bus->Control(ARM_SPI_CONTROL_SS,1);
else
	lcd_bus->Control(ARM_SPI_CONTROL_SS,0);
}

void lcd_rs_control(int cmd_0_data_1){
	if(cmd_0_data_1)
		LPC_GPIO4->FIOSET=1<<28;
	else
		LPC_GPIO4->FIOCLR=1<<28;
}

void lcd_reset(void){
	lcd_delay(1);
	LPC_GPIO4->FIOCLR=1<<29;
	lcd_delay(1);
 	LPC_GPIO4->FIOSET=1<<29;	
  lcd_delay(1);
}



int lcd_write(unsigned char in){ //low speed just used in cmd mode
	int time_out=0;
	lcd_bus->Send(&in,1);
	while(lcd_bus->GetStatus().busy){
		time_out++;
		if(time_out>1000000) return 0;
	}
	return 1;
}


#define WAIT_TRESHOLD       100

int lcd_writes(unsigned char *in,int cnt){
	int time_out=0;
	lcd_bus->Send(in,cnt);
	
	while(lcd_bus->GetStatus().busy){
		time_out++;
		if(time_out>8000000) return 0;
		if(cnt>1000) lcd_delay(1);
	}
	return 1;	
}






int lcd_command(unsigned char command,unsigned char *arg,int arg_cnt){
	int gpv0=0;
	lcd_cs_control(1);
	
	lcd_rs_control(0);
	gpv0+=lcd_write(command);
	

	if(arg_cnt>0){
	lcd_rs_control(1);
	gpv0+=lcd_writes(arg,arg_cnt);
	}
	else{
  gpv0++;
	}

	lcd_cs_control(0);
  if(gpv0!=2) return 0;
	return 1;
}


int lcd_select_page(unsigned char xs,unsigned char xe,unsigned char ys,unsigned char ye){
	int gpv0;
	unsigned char data[4];
  gpv0=0;
	data[0]=0;
	data[1]=xs;
	data[2]=0;
	data[3]=xe;
	gpv0+=lcd_command(0x2a,data,4);
  data[0]=0;
	data[1]=ys;
	data[2]=0;
	data[3]=ye;
	gpv0+=lcd_command(0x2b,data,4);
	if(gpv0!=2) return 0;
  return 1;	
}


void lcd_wr_pixel_565(unsigned short pixel){
	lcd_writes((unsigned char *)&pixel,2);
}

void lcd_wr_pixels_565(unsigned short *pixel,int pixel_cnt){
	lcd_writes((unsigned char *)pixel,2*pixel_cnt);	
}

void lcd_sf_address(unsigned char xs,unsigned char xe,unsigned char ys,unsigned char ye){
	lcd_select_page(xs,xe,ys,ye);
	lcd_cs_control(1);
	lcd_rs_control(0);
	lcd_write(0x2c);
	lcd_rs_control(1);
}




void lcd_sf(void){
	lcd_cs_control(1);
	lcd_rs_control(0);
	lcd_write(0x2c);
	lcd_rs_control(1);	
}






void lcd_ef(void){
	lcd_rs_control(0);
	lcd_write(0);
	lcd_cs_control(0);	
}




void lcd_init(void){
unsigned char data[16];
lcd_bus=&Driver_SPI0;
lcd_bus->Initialize(NULL);
lcd_bus->PowerControl(ARM_POWER_FULL);	
lcd_bus->Control(ARM_SPI_MODE_MASTER|ARM_SPI_DATA_BITS(8)|ARM_SPI_SS_MASTER_SW,25000000);	

LPC_GPIO4->FIODIR|=1<<28|1<<29; //p1.28 rs p1.29 reset

	
lcd_reset();
	
lcd_command(1,0,0);
lcd_delay(1);
lcd_command(0x11,0,0);
lcd_delay(1);

data[0]=1;
data[1]=0x2c;
data[2]=0x2d;
lcd_command(0xb1,data,3);


data[0]=1;
data[1]=0x2c;
data[2]=0x2d;
lcd_command(0xb2,data,3);

data[0]=1;
data[1]=0x2c;
data[2]=0x2d;
data[3]=1;
data[4]=0x2c;
data[5]=0x2d;
lcd_command(0xb3,data,6);

data[0]=0x7;
lcd_command(0xb4,data,1);

data[0]=0xa2;
data[1]=0x2;
data[2]=0x84;
lcd_command(0xc0,data,3);

data[0]=0xc5;
lcd_command(0xc1,data,1);

data[0]=0xa;
data[1]=0;
lcd_command(0xc2,data,2);

data[0]=0x8a;
data[1]=0x2a;
lcd_command(0xc3,data,2);

data[0]=0x8a;
data[1]=0xee;
lcd_command(0xc4,data,2);

data[0]=0xe;
lcd_command(0xc5,data,1);


lcd_command(0x20,data,0);

data[0]=0x8|0x60;

lcd_command(0x36,data,1);


data[0]=5;
lcd_command(0x3a,data,1);


lcd_command(0,data,0);

data[0]=0;
data[1]=0;
data[2]=0;
data[3]=127;
lcd_command(0x2a,data,4);


data[0]=0;
data[1]=0;
data[2]=0;
data[3]=127;
lcd_command(0x2b,data,4);



lcd_command(0,data,0);

data[0]=0x2;
data[1]=0x1c;
data[2]=0x7;
data[3]=0x12;
data[4]=0x37;
data[5]=0x32;
data[6]=0x29;
data[7]=0x2d;
data[8]=0x29;
data[9]=0x25;
data[10]=0x2b;
data[11]=0x39;
data[12]=0x0;
data[13]=0x1;
data[14]=0x3;
data[15]=0x10;
lcd_command(0xe0,data,16);
lcd_command(0,data,0);

data[0]=0x3;
data[1]=0x1d;
data[2]=0x7;
data[3]=0x6;
data[4]=0x2e;
data[5]=0x2c;
data[6]=0x29;
data[7]=0x2d;
data[8]=0x2e;
data[9]=0x2e;
data[10]=0x37;
data[11]=0x3f;
data[12]=0x0;
data[13]=0x0;
data[14]=0x2;
data[15]=0x10;
lcd_command(0xe1,data,16);
lcd_command(0,data,0);

lcd_command(0x29,data,0);
lcd_delay(1);
lcd_command(0x13,data,0);
lcd_delay(1);
lcd_command(0,data,0);	
	
}



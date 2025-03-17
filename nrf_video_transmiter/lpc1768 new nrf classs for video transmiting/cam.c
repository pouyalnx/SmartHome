#include "lpc17xx.h"
#include "Driver_i2c.h"
#include "cmsis_os.h"
#include "cam.h"
#include "lpc17xx.h"


extern ARM_DRIVER_I2C Driver_I2C0;
ARM_DRIVER_I2C *cam_i2c;

#define cam_wait(x) osDelay(x)




void cam_reset(void){
	cam_wait(10);
	LPC_GPIO2->FIOCLR=2048;
	cam_wait(10);
	LPC_GPIO2->FIOSET=2048;	
	cam_wait(10);	
}


void cam_power(int power){
	if(power){
		LPC_GPIO2->FIOCLR=4096;
	}
	else{
		LPC_GPIO2->FIOSET=4096;		
	}
}


int cam_cmd(unsigned char reg,unsigned char data){
unsigned char data_out[2];
data_out[0]=reg;
data_out[1]=data;	
cam_i2c->MasterTransmit(0x21,data_out,2,false);
//while(cam_i2c->GetDataCount()!=2){
//	if(cam_i2c->GetDataCount()==-1) return 1;
//}
while(cam_i2c->GetStatus().busy);
if(cam_i2c->GetDataCount()==2)	
return 0;
return 1;
}


int cam_init(void){
int gpv0;	
	
cam_i2c=&Driver_I2C0;	
cam_i2c->Initialize(NULL);	
cam_i2c->PowerControl(ARM_POWER_FULL);
cam_i2c->Control(ARM_I2C_BUS_SPEED,ARM_I2C_BUS_SPEED_STANDARD);

LPC_GPIO2->FIODIR&=~(0X7FF);//data in		
LPC_GPIO2->FIODIR|=2048|4096;   //reset powerdown

	
cam_power(1);

	
LPC_SC->CLKOUTCFG=0;
LPC_SC->CLKOUTCFG=(SystemCoreClock/12000000-1)<<4|1<<8;	
	
	
LPC_PINCON->PINSEL3&=~(3<<22);	
LPC_PINCON->PINSEL3|=1<<22;  //1.27 for clock
cam_reset();	
	

	
gpv0=cam_cmd(0x12, 0x80); /* reset to default values */
gpv0+=cam_cmd(0x11, 0x80);
gpv0+=cam_cmd(0x3b, 0x0A);
gpv0+=cam_cmd(0x3a, 0x04);
gpv0+=cam_cmd(0x3a, 0x04);
gpv0+=cam_cmd(0x12, 0x04); /* output format: rgb */

	
gpv0+=cam_cmd(0x8c, 0x00); /* disable RGB444 */
gpv0+=cam_cmd(0x40, 0xD0); /* set RGB565 */
		
gpv0+=cam_cmd(0x12,0x14);
gpv0+=cam_cmd(0x12,0x14);
gpv0+=cam_cmd(0x12,0x14);
gpv0+=cam_cmd(0x12,0x1c);
	
	
gpv0+=cam_cmd(0x17, 0x16);
gpv0+=cam_cmd(0x18, 0x04);
gpv0+=cam_cmd(0x32, 0x24);
gpv0+=cam_cmd(0x19, 0x02);
gpv0+=cam_cmd(0x1a, 0x7a);
gpv0+=cam_cmd(0x03, 0x0a);
gpv0+=cam_cmd(0x15, 0x02);
gpv0+=cam_cmd(0x0c, 0x04);
gpv0+=cam_cmd(0x3e, 0x1a); // divide by 4
//  gpv0+=cam_cmd(0x3e, 0x1b); // divide by 8
gpv0+=cam_cmd(0x1e, 0x27);
gpv0+=cam_cmd(0x72, 0x22); // downsample by 4
//   gpv0+=cam_cmd(0x72, 0x33); // downsample by 8	
gpv0+=cam_cmd(0x73,0xf2); // divide by 4
	
gpv0+=cam_cmd(0x4f, 0x80);
gpv0+=cam_cmd(0x50, 0x80);
gpv0+=cam_cmd(0x51, 0x00);
gpv0+=cam_cmd(0x52, 0x22);
gpv0+=cam_cmd(0x53, 0x5e);
gpv0+=cam_cmd(0x54, 0x80);
		
		
		
		
gpv0+=cam_cmd(0x56, 0x40);
gpv0+=cam_cmd(0x58, 0x9e);

gpv0+=cam_cmd(0x59, 0x88);
gpv0+=cam_cmd(0x5a, 0x88);
gpv0+=cam_cmd(0x5b, 0x44);
gpv0+=cam_cmd(0x5c, 0x67);
gpv0+=cam_cmd(0x5d, 0x49);
gpv0+=cam_cmd(0x5e, 0x0e);
gpv0+=cam_cmd(0x69, 0x00);
		
gpv0+=cam_cmd(0x6a, 0x40);
gpv0+=cam_cmd(0x6b, 0x01);

gpv0+=cam_cmd(0x6c, 0x0);
gpv0+=cam_cmd(0x6d, 0x0);
gpv0+=cam_cmd(0x6e, 0x0);
gpv0+=cam_cmd(0x6f, 0x0);
	
gpv0+=cam_cmd(0xb0, 0x84);

//gpv0+=cam_cmd(0x70, 0x00);
//gpv0+=cam_cmd(0x71, 0x80);



return gpv0;	
}


#define cam_data_line LPC_GPIO2->FIOPIN

#define cam_v_line    (LPC_GPIO2->FIOPIN&1024)
#define cam_h_line    (LPC_GPIO2->FIOPIN&512)
#define cam_p_line    (LPC_GPIO2->FIOPIN&256)

int cam_image(unsigned char *img){
	int gpv0;
  int gpv1;
	gpv0=0;
  gpv1=0;	
	while(cam_v_line);
	
	while(cam_v_line==0);
	while(cam_v_line){
		
		while(cam_h_line==0){
			if(cam_v_line==0){
				return gpv1;
			}
		}
		
		while(cam_h_line){
			
			while(cam_p_line==0);
			*(img)=cam_data_line;
			while(cam_p_line);
     	    
	    gpv0++;		


		  while(cam_p_line==0);
			*(img+1)=cam_data_line;
			while(cam_p_line);
      
			if(gpv0<129){
				img+=2;
			  gpv1++;
			}

		}
		gpv0=0;
		
	 }
	return gpv1;

}
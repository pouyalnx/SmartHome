#include "nrf.h"
#include <stdio.h>
#include "lpc177x_8x.h"
#include "driver_spi.h"
#include "gpio_lpc17xx.h"
#include "cmsis_os.h"


extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI *nrf;



void nrf_write(unsigned char reg,unsigned char *data,int n){
	nrf->Control(ARM_SPI_CONTROL_SS,1);
	nrf->Send(&reg,1);
	while(nrf->GetStatus().busy);
	if(n){
	nrf->Send(data,n);
	while(nrf->GetStatus().busy);	
	}
	nrf->Control(ARM_SPI_CONTROL_SS,0);
}


void nrf_read(unsigned char reg,unsigned char *data,int n){
	nrf->Control(ARM_SPI_CONTROL_SS,1);
	nrf->Send(&reg,1);
	while(nrf->GetStatus().busy);
	nrf->Receive(data,n);
	while(nrf->GetStatus().busy);	
	nrf->Control(ARM_SPI_CONTROL_SS,0);	
}


int nrf_set_addr(unsigned char *addr){
   nrf_write(NRF_W_REGISTER|NRF_CMD_RX_ADDR_P0,addr,5);
	 nrf_write(NRF_W_REGISTER|NRF_CMD_TX_ADDR,addr,5);	
  	return 1;
}




void nrf_init(void){
	unsigned char arg[5];
	nrf=&Driver_SPI1;
	nrf->Initialize(NULL);
	nrf->PowerControl(ARM_POWER_FULL);
  nrf->Control(ARM_SPI_MODE_MASTER|
	             ARM_SPI_DATA_BITS(8)|
	             ARM_SPI_SS_MASTER_SW,8000000);

  arg[0]=3;
  nrf_write(NRF_W_REGISTER|NRF_CMD_EN_AA,arg,1);

	arg[0]=3;
	nrf_write(NRF_W_REGISTER|NRF_CMD_EN_RXADDR,arg,1);
	
	arg[0]=3;
	nrf_write(NRF_W_REGISTER|NRF_CMD_SETUP_AW,arg,1);
		
	arg[0]=0x2f;
	nrf_write(NRF_W_REGISTER|NRF_CMD_SETUP_RETR,arg,1);	

	arg[0]=0x2;
	nrf_write(NRF_W_REGISTER|NRF_CMD_RF_CH,arg,1);	

	arg[0]=0xf;
	nrf_write(NRF_W_REGISTER|NRF_CMD_RF_SETUP,arg,1);	

	arg[0]=4;
	nrf_write(NRF_W_REGISTER|NRF_CMD_FEATURE,arg,1);
	
	arg[0]=3;
	nrf_write(NRF_W_REGISTER|NRF_CMD_DYNPL,arg,1);		


  arg[0]=0X3a;
	arg[1]=0X3a;
	arg[2]=0X3a;
	arg[3]=0X3a;
	arg[4]=0X3a;
  nrf_set_addr(arg);

  arg[0]=0xe;
	nrf_write(NRF_W_REGISTER|NRF_CMD_CONFIG,arg,1);		

	GPIO_SetDir(0,1,GPIO_DIR_OUTPUT);
  LPC_GPIOINT->IO0IntEnF=1;
}






int nrf_read_stat(void){
	unsigned char stat;
	nrf_read(NRF_R_REGISTER|NRF_CMD_STATUS,&stat,1);
	return stat;
}


int nrf_write_stat(unsigned char stat){
	nrf_write(NRF_W_REGISTER|NRF_CMD_STATUS,&stat,1);
	return stat;
}

void nrf_enable_rf(int mode){
	if(mode) GPIO_PinWrite(0,1,1);
	else     GPIO_PinWrite(0,1,0);
}


void nrf_set_mode(int mode){
	unsigned char arg;
	if(mode) arg=0xf;
	else     arg=0xe;
  nrf_write(NRF_W_REGISTER|NRF_CMD_CONFIG,&arg,1);	
}


int nrf_send_data(char *data,int len){
int timeout=0;	
nrf_set_mode(0);
nrf_write(NRF_W_TX_PAYLOAD,(unsigned char *)data,len);
nrf_write_stat(0x70);
LPC_GPIOINT->IO0IntClr=1;
nrf_enable_rf(1);

while((LPC_GPIOINT->IO0IntStatF & 1)==0){
osDelay(1);	
timeout++;
if(timeout>101) break;	
}

nrf_enable_rf(0);

if((nrf_read_stat() & 0x10) || timeout>100) return 0;
return 1;
}






int nrf_read_data(char *data,int len){
int timeout=0;	
nrf_set_mode(1);
nrf_write_stat(0x70);
LPC_GPIOINT->IO0IntClr=1;
nrf_enable_rf(1);

while((LPC_GPIOINT->IO0IntStatF & 1)==0){
osDelay(1);	
timeout++;
if(timeout>60001) break;	
}

nrf_enable_rf(0);
if(timeout>60000) return 0;
nrf_read(NRF_R_RX_PAYLOAD,(unsigned char *)data,len);
return 1;
}



void nrf_read_data_enable_os(void){
nrf_set_mode(1);
nrf_write_stat(0x70);
LPC_GPIOINT->IO0IntClr=1;
nrf_enable_rf(1);	
}


int nrf_read_data_os(char *data){
	if(LPC_GPIOINT->IO0IntStatF & 1){
    nrf_enable_rf(0);
		nrf_read(NRF_R_RX_PAYLOAD,(unsigned char *)data,32);		
		return 1;
	}
	return 0;
}

void nrf_read_data_disable_os(void){
nrf_enable_rf(0);
}










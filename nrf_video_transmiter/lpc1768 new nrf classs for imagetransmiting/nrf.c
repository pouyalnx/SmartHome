#include "driver_spi.h"
#include "lpc17xx.h"
#include "nrf.h"
#include <string.h>



extern ARM_DRIVER_SPI Driver_SPI0;
ARM_DRIVER_SPI *nrf_bus;	
nrf_profile_type nrf_prof;






void nrf_write_command(unsigned char commnad,unsigned char *data,int cnt){
	nrf_bus->Control(ARM_SPI_CONTROL_SS,1);
  nrf_bus->Send(&commnad,1);
  while(nrf_bus->GetStatus().busy);
	if(cnt){
  nrf_bus->Send(data,cnt);		
  while(nrf_bus->GetStatus().busy);
	}
	nrf_bus->Control(ARM_SPI_CONTROL_SS,0);	
}

void nrf_read_command(unsigned char commnad,unsigned char *data,int cnt){
	nrf_bus->Control(ARM_SPI_CONTROL_SS,1);
  nrf_bus->Send(&commnad,1);
  while(nrf_bus->GetStatus().busy);
	if(cnt){
  nrf_bus->Receive(data,cnt);		
  while(nrf_bus->GetStatus().busy);
	}
	nrf_bus->Control(ARM_SPI_CONTROL_SS,0);	
}


void nrf_set_address(unsigned char *address,unsigned char pipe){
	switch(pipe){
		case 0:
			nrf_write_command(0x20|0xa,address,5);
		  memcpy(nrf_prof.rx_p0_address,address,5);
		break;
		case 1:
			nrf_write_command(0x20|0xb,address,5);
		  memcpy(nrf_prof.rx_p1_address,address,5);			
		break;
		case 100:
			nrf_write_command(0x20|0x10,address,5);
		  memcpy(nrf_prof.tx_address,address,5);			
		break;
	}
	
}

void nrf_set_rf_mode(int mode){
	if(mode){
		//if((nrf_prof.config_reg&1)==0){
			//nrf_prof.config_reg|=1;
		  nrf_prof.config_reg=0xf;
			nrf_write_command(0x20,&nrf_prof.config_reg,1);
		//}
	}
	else{
	//	if(nrf_prof.config_reg&1){
		  //nrf_prof.config_reg&=0xe;
			nrf_prof.config_reg=0xe;
			nrf_write_command(0x20,&nrf_prof.config_reg,1);
		//}		
	}
}




void nrf_enable_rf(int stat){
	if(stat){
    LPC_GPIO0->FIOSET=2;		
		nrf_prof.rf_stat=1;
	}
	else{
    LPC_GPIO0->FIOCLR=2;		
		nrf_prof.rf_stat=0;		
	}
}


void nrf_set_channel(unsigned char in){
	nrf_write_command(NRF_W_REGISTER|NRF_CMD_RF_CH,&in,1);	
}



int nrf_read_stat(void){
	unsigned char stat;
	nrf_read_command(0x7,&stat,1);
	return stat;
}

void nrf_write_stat(unsigned char in){
	nrf_write_command(0x27,&in,1);
}



int nrf_write_data(unsigned char *data,int cnt){
	nrf_write_command(0xa0,data,cnt);
}

int nrf_read_data(unsigned char *data,int cnt){
	nrf_read_command(0x61,data,cnt);
}



int nrf_irq_stat(void){
	return LPC_GPIOINT->IO0IntStatF&1;
}

void nrf_irq_clr_stat(void){
	nrf_write_stat(0x70);
	LPC_GPIOINT->IO0IntClr=1;
}







void nrf_init(void){
	unsigned char arg[5];
	nrf_bus=&Driver_SPI0;
	nrf_bus->Initialize(NULL);
	nrf_bus->PowerControl(ARM_POWER_FULL);
	nrf_bus->Control(ARM_SPI_DATA_BITS(8)|ARM_SPI_SS_MASTER_SW|ARM_SPI_MODE_MASTER,16000000);
  nrf_prof.config_reg=8|4|2;	
	nrf_prof.rx_channel=2;
  nrf_prof.rf_stat=0;  
	
	arg[0]=0xa;
	arg[1]=0xb;
	arg[2]=0xa;
	arg[3]=0xc;
	arg[4]=0xd;
	nrf_set_address(arg,0);			
	nrf_set_address(arg,1);			
	nrf_set_address(arg,100);		
	
	LPC_GPIO0->FIODIR&=~(1);
	LPC_GPIO0->FIODIR|=2;
//	LPC_PINCON->PINSEL0&=~0XF;
 // LPC_PINCON->PINMODE0&=~0XF;
  LPC_GPIO0->FIOCLR=2;
  LPC_GPIOINT->IO0IntClr=1;
	LPC_GPIOINT->IO0IntEnF=1;
 
   arg[0]=1;
  nrf_write_command(NRF_W_REGISTER|NRF_CMD_EN_AA,arg,1);

	arg[0]=1;
	nrf_write_command(NRF_W_REGISTER|NRF_CMD_EN_RXADDR,arg,1);
	
	arg[0]=3;
	nrf_write_command(NRF_W_REGISTER|NRF_CMD_SETUP_AW,arg,1);
		
	arg[0]=0x2f;
	nrf_write_command(NRF_W_REGISTER|NRF_CMD_SETUP_RETR,arg,1);	

	arg[0]=0x2;
	nrf_write_command(NRF_W_REGISTER|NRF_CMD_RF_CH,arg,1);	

	arg[0]=0xf;
	nrf_write_command(NRF_W_REGISTER|NRF_CMD_RF_SETUP,arg,1);	

	arg[0]=4;
	nrf_write_command(NRF_W_REGISTER|NRF_CMD_FEATURE,arg,1);
	
	arg[0]=1;
	nrf_write_command(NRF_W_REGISTER|NRF_CMD_DYNPL,arg,1);		

  arg[0]=0xe;
	nrf_write_command(0x20,&nrf_prof.config_reg,1); 

}



































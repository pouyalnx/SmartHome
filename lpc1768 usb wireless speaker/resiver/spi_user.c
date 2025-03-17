#include "spi_user.h"
#include "driver_spi.h"
#include "gpio_lpc17xx.h"
#include "lpc17xx.h"
#include "cmsis_os.h"

/*********************************************************************//**
 * @brief            
 * @param[in]    
 * @return         
 **********************************************************************/



extern ARM_DRIVER_SPI Driver_SPI0;
ARM_DRIVER_SPI *nrf;

volatile int NRF_irq_flag;
/*********************************************************************//**
 * @brief            
 * @param[in]    
 * @return         
 **********************************************************************/
void nrf_send_call_back(uint32_t events){
	
}

/*********************************************************************//**
 * @brief            
 * @param[in]    
 * @return         
 **********************************************************************/

int nrf_write(unsigned char reg,unsigned char *data,int n){
	nrf->Control(ARM_SPI_CONTROL_SS,1);
	nrf->Send(&reg,1);
	while(nrf->GetStatus().busy);
	if(n){
	nrf->Send(data,n);
	while(nrf->GetStatus().busy);	
	}
	nrf->Control(ARM_SPI_CONTROL_SS,0);
	return 1;
}

/*********************************************************************//**
 * @brief            
 * @param[in]    
 * @return         
 **********************************************************************/

int nrf_read(unsigned char reg,unsigned char *data,int n){
	nrf->Control(ARM_SPI_CONTROL_SS,1);
	nrf->Send(&reg,1);
	while(nrf->GetStatus().busy);
	nrf->Receive(data,n);
	while(nrf->GetStatus().busy);	
	nrf->Control(ARM_SPI_CONTROL_SS,0);	
	return 1;
}

/*********************************************************************//**
 * @brief            
 * @param[in]    
 * @return         
 **********************************************************************/

int nrf_read_stat(void){
	unsigned char stat;
	nrf_read(NRF_R_REGISTER|NRF_CMD_STATUS,&stat,1);
	return stat;
}

/*********************************************************************//**
 * @brief            
 * @param[in]    
 * @return         
 **********************************************************************/

int nrf_write_stat(unsigned char stat){
	nrf_write(NRF_W_REGISTER|NRF_CMD_STATUS,&stat,1);
	return stat;
}

/*********************************************************************//**
 * @brief            
 * @param[in]    
 * @return         
 **********************************************************************/

int nrf_set_addr(unsigned char *addr){
  nrf_write(NRF_W_REGISTER|NRF_CMD_RX_ADDR_P0,addr,5);
	 nrf_write(NRF_W_REGISTER|NRF_CMD_TX_ADDR,addr,5);	
  	return 1;
}


/*********************************************************************//**
 * @brief            
 * @param[in]    
 * @return         
 **********************************************************************/
#define NRF_MODE_PTX 0
#define NRF_MODE_PRX 1

void nrf_set_mode(int mode){
	unsigned char arg;
	if(mode) arg=0xf;
	else     arg=0xe;
  nrf_write(NRF_W_REGISTER|NRF_CMD_CONFIG,&arg,1);	
}

void nrf_enable_rf(int mode){
	if(mode) GPIO_PinWrite(0,1,1);
	else     GPIO_PinWrite(0,1,0);
}

void nrf_enable_irq(int irq_req){
	LPC_GPIOINT->IO0IntClr=1;
	LPC_GPIOINT->IO0IntEnF=1;
	NVIC_EnableIRQ(EINT3_IRQn);
}

int nrf_irq_stat(void){
	return LPC_GPIOINT->IO0IntStatF & 1;
}

void EINT3_IRQHandler(void){
	NRF_irq_flag=1;
	LPC_GPIOINT->IO0IntClr=1;	
	nrf_enable_rf(0);
}

/*********************************************************************//**
 * @brief            
 * @param[in]    
 * @return         
 **********************************************************************/

void NRF_init(void){
	unsigned char arg[5];
	nrf=&Driver_SPI0;
	nrf->Initialize(nrf_send_call_back);
	nrf->PowerControl(ARM_POWER_FULL);
  nrf->Control(ARM_SPI_MODE_MASTER|
	             ARM_SPI_DATA_BITS(8)|
	             ARM_SPI_SS_MASTER_SW,8000000);
/*
	tx rx  (hexnumbers)
e f
0 0
1 1
3 3
0 0
f f
//register rf_setup
f f
70 70
/////////////////////
//resgister:rx and tx addr:who carry
//////////////////
//rx size register
20 20

//1d register
0 0
	*/
	arg[0]=0;
  nrf_write(NRF_W_REGISTER|NRF_CMD_EN_AA,arg,1);

	arg[0]=1;
	nrf_write(NRF_W_REGISTER|NRF_CMD_EN_RXADDR,arg,1);
	
	arg[0]=3;
	nrf_write(NRF_W_REGISTER|NRF_CMD_SETUP_AW,arg,1);
		
	arg[0]=0;
	nrf_write(NRF_W_REGISTER|NRF_CMD_SETUP_RETR,arg,1);	

	arg[0]=0x1f;
	nrf_write(NRF_W_REGISTER|NRF_CMD_RF_CH,arg,1);	

	arg[0]=0xf;
	nrf_write(NRF_W_REGISTER|NRF_CMD_RF_SETUP,arg,1);	

	arg[0]=0;
	nrf_write(NRF_W_REGISTER|NRF_CMD_FEATURE,arg,1);
	
	arg[0]=0;
	nrf_write(NRF_W_REGISTER|NRF_CMD_DYNPL,arg,1);		

	arg[0]=32;
	nrf_write(NRF_W_REGISTER|0x11,arg,1);		


  arg[0]=0XF1;
	arg[1]=0XE6;
	arg[2]=0X6E;
	arg[3]=0X1F;
	arg[4]=0XAA;
  nrf_set_addr(arg);

  arg[0]=0xe;
	nrf_write(NRF_W_REGISTER|NRF_CMD_CONFIG,arg,1);	
	
	GPIO_SetDir(0,1,GPIO_DIR_OUTPUT);

 nrf_enable_irq(0);
}



/*********************************************************************//**
 * @brief        new group api for transmite better than old api    
 * @param[in]    and able to use rtx function for delay
 * @return         effiecent for real timee applications
 **********************************************************************/

void nrf_read_data(unsigned char *data,int size){
	nrf_read(NRF_R_RX_PAYLOAD,data,size);
  nrf_write_stat(0x70);
	nrf_enable_rf(1);  
}

void nrf_write_data(unsigned char *data,int size){
	nrf_write(NRF_W_TX_PAYLOAD,data,size);
  nrf_write_stat(0x70);
	nrf_enable_rf(1);
	NRF_irq_flag=0;
}


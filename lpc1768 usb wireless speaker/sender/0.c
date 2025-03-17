#include "spi_user.h"
#include "lpc17xx.h"
#include "board_dac.h"
#include "gpdma_lpc17xx.h"



short adc_data[16];
int update;
int counter;

void TIMER0_IRQHandler(void){
  int sample;
	if(update){
		update=0;
		counter=0;
	}
  if(counter<16){
		sample=adc_data[counter++];
		DAC_SetValue(sample);
		DAC_SetValue(sample);
	}
	LPC_TIM0->IR=1;
}



int main(){
	NRF_init();
	DAC_Initialize();
	GPDMA_Initialize();
	LPC_TIM0->TCR=2;
	LPC_TIM0->MR0=100000/32-1;
  LPC_TIM0->MCR=3;
	LPC_TIM0->PR=0;
	LPC_TIM0->TCR=1;
  NVIC_EnableIRQ(TIMER0_IRQn);
  nrf_read_data((unsigned char *)adc_data,32);
	
	counter=0;
	update=0;
	while(1){
  if(NRF_irq_flag && counter>15){
  NRF_irq_flag=0;
  update=1;		
  nrf_read_data((unsigned char *)adc_data,32);		
	}
	}		
}

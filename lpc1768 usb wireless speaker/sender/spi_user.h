#ifndef _spi_user_temp_nrf
#define _spi_user_temp_nrf






enum nrf_command{
	NRF_R_REGISTER       =0,
	NRF_W_REGISTER       =0X20,
	NRF_R_RX_PAYLOAD     =0X61,
	NRF_W_TX_PAYLOAD     =0XA0,
	NRF_FLUSH_TX         =0XE1,
	NRF_FLUSH_RX         =0XE2,
	NRF_REUSE_TX_PL      =0XE3,
	NRF_ACTIVE           =0X50,
	NRF_R_RX_PL_WID      =0X60,
	NRF_W_ACK_PAYLOAD    =0XA8,
	NRF_W_TX_PAYLOAD_NO  =0XB0,
	NRF_NOP              =0XFF,
};



#define NRF_ADDRESS_SIZE  5 

#define NRF_CMD_CONFIG      0X00
#define NRF_CMD_EN_AA       0X01
#define NRF_CMD_EN_RXADDR   0X02
#define NRF_CMD_SETUP_AW    0X03
#define NRF_CMD_SETUP_RETR  0X04
#define NRF_CMD_RF_CH       0X05
#define NRF_CMD_RF_SETUP    0X06
#define NRF_CMD_STATUS      0X07
#define NRF_CMD_OBSERVE_TX  0X08
#define NRF_CMD_CD          0X09
#define NRF_CMD_TX_ADDR     0X10
#define NRF_CMD_RX_ADDR_P0  0X0A
#define NRF_CMD_RX_ADDR_P1  0X0B
#define NRF_CMD_RX_ADDR_P2  0X0C
#define NRF_CMD_RX_ADDR_P3  0X0D
#define NRF_CMD_RX_ADDR_P4  0X0E
#define NRF_CMD_RX_ADDR_P5  0X0f

#define NRF_CMD_DYNPL       0X1C   
#define NRF_CMD_FEATURE     0X1D

extern volatile int NRF_irq_flag;

extern void NRF_init(void);
extern int  nrf_read_stat(void);
extern int  nrf_write_stat(unsigned char stat);	
extern int  nrf_write(unsigned char reg,unsigned char *data,int n);
extern int  nrf_read(unsigned char reg,unsigned char *data,int n);
extern void nrf_read_data(unsigned char *data,int size);
extern void nrf_write_data(unsigned char *data,int size);


#endif


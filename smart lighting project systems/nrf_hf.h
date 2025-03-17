#ifndef __nrf_hf_
#define __nrf_hf_
#include "common_struct.h"




extern int nrf_destt_hf(Device *dev);
extern int nrf_deset_hf(Device *dev,char *data,int len);
extern int nrf_deget_hf(char *in,int in_len,char *out,int *out_len,Device *dev);
extern void nrf_hf_init(void);  //hf:hardware function



#endif


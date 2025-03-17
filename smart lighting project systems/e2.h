#ifndef _e2_press
#define _e2_press










extern void e2_init(unsigned int core_clk);
extern int e2_write(unsigned int *data,int len,int page);
extern int e2_read(unsigned int *data,int len,int page);



#endif

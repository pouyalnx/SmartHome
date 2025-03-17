#ifndef dec_0_h
#define dec_0_h

typedef struct{
	unsigned char   address[5];
	unsigned char   update_req;
  char            name[8];
  unsigned short  signal_power;	
	unsigned        char_stat;
}signal_t;








extern int input_read(void);










#endif


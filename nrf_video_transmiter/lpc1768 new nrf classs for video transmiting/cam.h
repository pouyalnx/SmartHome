#ifndef __cam_lib_001
#define __cam_lib_001







extern void cam_reset(void);
extern void cam_power(int power);
extern int cam_cmd(unsigned char reg,unsigned char data);
extern int cam_init(void);
extern int cam_image(unsigned char *img);


#endif



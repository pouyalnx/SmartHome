#ifndef _lcd_st7735_0
#define _lcd_st7735_0


extern unsigned short lcd_ram[16384];

extern void lcd_cs_control(int stat);
extern void lcd_rs_control(int cmd_0_data_1);
extern void lcd_reset(void);
extern int lcd_write(unsigned char in);
extern int lcd_writes(unsigned char *in,int cnt);
extern int lcd_command(unsigned char command,unsigned char *arg,int arg_cnt);
extern void lcd_wr_pixel_565(unsigned short pixel);
extern void lcd_wr_pixels_565(unsigned short *pixel,int pixel_cnt);
extern void lcd_sf_address(unsigned char xs,unsigned char xe,unsigned char ys,unsigned char ye);
extern void lcd_sf(void); 
extern void lcd_ef(void);
extern int lcd_select_page(unsigned char xs,unsigned char xe,unsigned char ys,unsigned char ye);
extern void lcd_init(void);
extern void lcd_update(void);












#endif

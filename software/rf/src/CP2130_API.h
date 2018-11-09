#ifndef CP2130_API_H
#define CP2130_API_H

//SPI_world_cmd
#define H2D_req 0x40
#define D2H_REQ 0xC0

//SPI读写指令
#define SPI_READ_CMD 0x00
#define SPI_WRITE_CMD 0x01
#define SPI_WRITEREAD_CMD 0x02
#define SPI_READ_RTR_CMD 0x04
#define SPI_RESET_CMD 0X10

#define GET_GPIO_value 0x20
#define SET_GPIO_value 0X21
#define GET_GPIO 0x22
#define SET_GPIO 0X23
#define GET_CHIPSELECT 0x24
#define SET_CHIPSELECT 0x25
#define GET_SPI_WORLD_CMD 0x30
#define SET_SPI_WORLD_CMD 0x31

//GPIO模式配置表
#define GPIO_INPUT 0X00
#define GPIO_OPEN_DRAIN 0X01
#define GPIO_PUSH_PULL 0X02

#define GPIO_LOGIC_LOW 0x00
#define GPIO_LOGIC_HIGH 0X01

//SPI片选配置表
#define DIS_CHIP 0X00
#define EN_CHIP 0X01
#define EN_CHIP_ONLY 0X02

//SPI单通道配置表
//时钟相位
#define LEADING_EDGE 0X00
#define TRAILING_EDGE 0X20
//时钟极性
#define IDLE_LOW 0X00
#define IDLE_HIGH 0X10

//SPI片选输出模式
#define CS_OPEN_DRAIN 0x00
#define CS_PUSH_PULL 0x08

//SPI频率参数
#define F12MHZ 0X00
#define F6MHZ 0X01
#define F3MHZ 0x02
#define F1_5MHZ 0X03
#define F750KHZ 0X04
#define F375kHZ 0X05
#define F187_5kHZ 0X06
#define F93_8kHZ 0X07

#include <stdint.h>

int init_CP2130();//初始化CP2130
int clear_cp2130Handle();//清空CP2130
int CP2130_set_channel(int cs,char control);//设置CP2130通道参数
int CP2130_get_channel();//获取通道参数
int CP2130_set_chipSelect(int cs,char control);//设置片选
int CP2130_get_chipSelect();//获取片选
int CP2130_set_GPIO_Mode_And_Level(int gpio,unsigned char mode,unsigned char level);//设置GPIO
int CP2130_get_GPIO_Mode_And_Level(int gpio);//获取GPIO参数
int CP2130_set_GPIO_value(int gpio,int value);
int CP2130_get_GPIO_value(int gpio);
int CP2130_set_pin_conf();


int CP2130_SPI_write(const unsigned char *value,uint32_t size);
int CP2130_SPI_read(unsigned char *value,uint32_t size);
int CP2130_SPI_writeRead(unsigned char *value,uint32_t size);










#endif

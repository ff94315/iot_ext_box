#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "debug.h"
#include "nrf24l01.h"
#include "CP2130_API.h"

#define uchar unsigned char
#define uint  unsigned int

const uchar TX_ADDRESS[TX_ADR_WIDTH]={0xFF,0xFF,0xFF,0xFF,0xFF}; //发送地址
const uchar RX_ADDRESS[RX_ADR_WIDTH]={0xFF,0xFF,0xFF,0xFF,0xFF}; //发送地址

#define NRF_CE   9
#define NRF_CSN  0
#define NRF_IRQ  10




uchar rece_buf[32];

/*********************************************/
/* 函数功能：给24L01的寄存器写值（一个字节） */
/* 入口参数：reg   要写的寄存器地址          */
/*           value 给寄存器写的值            */
/* 出口参数：status 状态值                   */
/*********************************************/
uchar NRF24L01_Write_Reg(uchar reg,uchar value)
{
	uchar buf[2] = {0};
	
	buf[0] = reg;
	buf[1] = value;
	CP2130_set_chipSelect(NRF_CSN, EN_CHIP);//CSN=0;

	CP2130_SPI_writeRead(buf,sizeof(buf));

	CP2130_set_chipSelect(NRF_CSN, DIS_CHIP);
	
	return buf[0];
}
/*************************************************/
/* 函数功能：读24L01的寄存器值 （一个字节）      */
/* 入口参数：reg  要读的寄存器地址               */
/* 出口参数：value 读出寄存器的值                */
/*************************************************/
uchar NRF24L01_Read_Reg(uchar reg)
{
	uchar buf[2] = {0XFF};
	buf[0] = reg;
	CP2130_set_chipSelect(NRF_CSN, EN_CHIP);//CSN=0;

	CP2130_SPI_writeRead(buf,sizeof(buf));

	CP2130_set_chipSelect(NRF_CSN, DIS_CHIP);
	
	return buf[1];

}
/*********************************************/
/* 函数功能：读24L01的寄存器值（多个字节）   */
/* 入口参数：reg   寄存器地址                */
/*           *pBuf 读出寄存器值的存放数组    */
/*           len   数组字节长度              */
/* 出口参数：status 状态值                   */
/*********************************************/
uchar NRF24L01_Read_Buf(uchar reg,uchar *pBuf,uchar len)
{

	uchar buf[len + 1] = {0xFF};
	buf[0] = reg;
	
	CP2130_set_chipSelect(NRF_CSN, EN_CHIP);        //CSN=0      
	CP2130_SPI_writeRead(buf,sizeof(buf));
	CP2130_set_chipSelect(NRF_CSN, DIS_CHIP);       //CSN=1

	memcpy(pBuf,buf + 1,len);
  	return buf[0];        			//返回读到的状态值
}
/**********************************************/
/* 函数功能：给24L01的寄存器写值（多个字节）  */
/* 入口参数：reg  要写的寄存器地址            */
/*           *pBuf 值的存放数组               */
/*           len   数组字节长度               */
/**********************************************/
uchar NRF24L01_Write_Buf(uchar reg, uchar *pBuf, uchar len)
{
	uchar buf[len + 1] = {0xFF};
	buf[0] = reg;

	memcpy(buf + 1,pBuf,len);

	CP2130_set_chipSelect(NRF_CSN, EN_CHIP);
	CP2130_SPI_writeRead(buf, sizeof(buf));
	CP2130_set_chipSelect(NRF_CSN, DIS_CHIP);
  	return buf[0];          		//返回读到的状态值
}							  					   

/*********************************************/
/* 函数功能：24L01接收数据                   */
/* 入口参数：rxbuf 接收数据数组              */
/* 返回值： 0   成功收到数据                 */
/*          1   没有收到数据                 */
/*********************************************/
uchar NRF24L01_RxPacket(uchar *rxbuf)
{
	uchar state;
	 
	state=NRF24L01_Read_Reg(STATUS);  			//读取状态寄存器的值    	 
	NRF24L01_Write_Reg(WRITE_REG+STATUS,state); //清除TX_DS或MAX_RT中断标志
	if(state&RX_OK)								//接收到数据
	{
		CP2130_set_GPIO_value(NRF_CE,0);
		NRF24L01_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
		NRF24L01_Write_Reg(FLUSH_RX,0xff);					//清除RX FIFO寄存器
		CP2130_set_GPIO_value(NRF_CE,1);
		usleep(150);
		return 0; 
	}	   
	return 1;//没收到任何数据
}
/**********************************************/
/* 函数功能：设置24L01为发送模式              */
/* 入口参数：txbuf  发送数据数组              */
/* 返回值； 0x10    达到最大重发次数，发送失败*/
/*          0x20    成功发送完成              */
/*          0xff    发送失败                  */
/**********************************************/
uchar NRF24L01_TxPacket(uchar *txbuf)
{
	uchar state;
   
	CP2130_set_GPIO_value(NRF_CE,0);						//CE拉低，使能24L01配置
  	NRF24L01_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);	//写数据到TX BUF  32个字节
 	CP2130_set_GPIO_value(NRF_CE,1);						//CE置高，使能发送
	while(CP2130_get_GPIO_value(NRF_IRQ)==1){
		debug(LOG_INFO," gpio ce[%d] gpio irq[%d]",CP2130_get_GPIO_value(NRF_CE),CP2130_get_GPIO_value(NRF_IRQ));
	}				//等待发送完成
	state=NRF24L01_Read_Reg(STATUS);  						//读取状态寄存器的值	   
	NRF24L01_Write_Reg(WRITE_REG+STATUS,state); 			//清除TX_DS或MAX_RT中断标志
	if(state&MAX_TX)										//达到最大重发次数
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xff);					//清除TX FIFO寄存器 
		return MAX_TX; 
	}
	if(state&TX_OK)											//发送完成
	{
		return TX_OK;
	}
	return 0xff;											//发送失败
}

/********************************************/
/* 函数功能：检测24L01是否存在              */
/* 返回值；  0  存在                        */
/*           1  不存在                      */
/********************************************/ 	  
uchar NRF24L01_Check(void)
{
	uchar check_in_buf[5]={0x11,0x22,0x33,0x44,0x55};
	uchar check_out_buf[5]={0x00};

	NRF24L01_Write_Buf(WRITE_REG+TX_ADDR, check_in_buf, 5);
	NRF24L01_Read_Buf(READ_REG+TX_ADDR, check_out_buf, 5);
	
	if(memcmp(check_in_buf,check_out_buf,5) == 0)
		return 0;
	else
		return 1;
}			


void NRF24L01_RT_Init(void)
{	
	CP2130_set_GPIO_value(NRF_CE,0);		  
  	NRF24L01_Write_Reg(WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度
	NRF24L01_Write_Reg(FLUSH_RX,0xff);									//清除RX FIFO寄存器    
  	NRF24L01_Write_Buf(WRITE_REG+TX_ADDR,(uchar*)TX_ADDRESS,TX_ADR_WIDTH);//写TX节点地址 
  	NRF24L01_Write_Buf(WRITE_REG+RX_ADDR_P0,(uchar*)RX_ADDRESS,RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK	  
  	NRF24L01_Write_Reg(WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答    
  	NRF24L01_Write_Reg(WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址  
  	NRF24L01_Write_Reg(WRITE_REG+SETUP_RETR,0x1a);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
  	NRF24L01_Write_Reg(WRITE_REG+RF_CH,0);        //设置RF通道为2.400GHz  频率=2.4+0GHz
  	NRF24L01_Write_Reg(WRITE_REG+RF_SETUP,0x0F);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  	NRF24L01_Write_Reg(WRITE_REG+CONFIG,0x0f);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
	CP2130_set_GPIO_value(NRF_CE,1);			  //CE置高，使能发送
}

void SEND_BUF(uchar *buf)
{
	
	CP2130_set_GPIO_value(NRF_CE,0);
	NRF24L01_Write_Reg(WRITE_REG+CONFIG,0x0e);
	CP2130_set_GPIO_value(NRF_CE,1);
	usleep(15);
	NRF24L01_TxPacket(buf);
	CP2130_set_GPIO_value(NRF_CE,0);
	NRF24L01_Write_Reg(WRITE_REG+CONFIG, 0x0f);
	CP2130_set_GPIO_value(NRF_CE,1);
}
void NRF24L01_start()
{
	//首先设置GPIO以及SPI
	CP2130_set_channel(0, F12MHZ + CS_PUSH_PULL);
	CP2130_set_channel(1, F12MHZ + CS_PUSH_PULL);
	CP2130_set_channel(2, F12MHZ + CS_PUSH_PULL);
	CP2130_set_channel(3, F12MHZ + CS_PUSH_PULL);

	CP2130_set_chipSelect(0, DIS_CHIP);
	CP2130_set_chipSelect(1, DIS_CHIP);
	CP2130_set_chipSelect(2, DIS_CHIP);
	CP2130_set_chipSelect(3, DIS_CHIP);

	CP2130_set_GPIO_Mode_And_Level(NRF_CE, GPIO_PUSH_PULL, GPIO_LOGIC_HIGH);
	CP2130_set_GPIO_Mode_And_Level(NRF_IRQ, GPIO_INPUT, GPIO_LOGIC_HIGH);
	
	while(NRF24L01_Check()); // 等待检测到NRF24L01，程序才会向下执行
	debug(LOG_INFO,"Find NRF24L01!");
	NRF24L01_RT_Init();

	debug(LOG_INFO,"start_APP");
	while(1)
	{	
		
		if(CP2130_get_GPIO_value(NRF_IRQ) == 0)	 	// 如果无线模块接收到数据
		{	
			debug(LOG_INFO,"recv");
			if(NRF24L01_RxPacket(rece_buf) == 0)
			{			   
				if(	rece_buf[1]=='1')		   	//第1位以后是收到的命令数据，rece_buf[0]是数据位数长度
					debug(LOG_INFO,"RECV 1");
				if(	rece_buf[1]=='2')	 		//第1位以后是收到的命令数据，rece_buf[0]是数据位数长度
					debug(LOG_INFO,"RECV 2");

				debug(LOG_INFO,"RECV DATA[%s]",rece_buf+1);

					
			}
		}
		
		if(0)								//按下S1发送ASCII码 “123”
		{
			rece_buf[1]='1';
			rece_buf[2]='2';
			rece_buf[3]='3';
			rece_buf[0]=3;	  					//一共要发送3个字节，rece_buf[0]必须是3！！！！！！
			SEND_BUF(rece_buf);
			//while(!S1);	
		}
		if(0)								//按下S1发送ASCII码 “ABCD”
		{
			rece_buf[1]='A';
			rece_buf[2]='B';
			rece_buf[3]='C';
			rece_buf[4]='D';
			rece_buf[0]=4;	   					//一共要发送4个字节，rece_buf[0]必须是4！！！！！！
			SEND_BUF(rece_buf);
			//while(!S2);	
		}
		if(1)			  					//发送 “上海宝嵌电子”
		{
			strcpy((char *)rece_buf + 1,"MY TEST PROGRAM\n");
			rece_buf[0]=strlen((char *)rece_buf + 1) + 1;
			
			SEND_BUF(rece_buf);
			
		}
		sleep(10);
	}
}





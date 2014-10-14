#include "common.h"



const u8* portnum="30000";		 

//WIFI STA模式,设置要去连接的路由器无线参数,请根据你自己的路由器设置,自行修改.
const u8* wifista_ssid="05zwp";			//路由器SSID号
const u8* wifista_encryption="wpawpa2_aes";	//wpa/wpa2 aes加密方式
const u8* wifista_password="301415926"; 	//连接密码
const u8* remoteIp ="192.168.1.30";


int atk_rm04_init(void)
{
#if AT_MODE_CONFIG==0
	RCC->APB2ENR|=1<<2;    	//PA时钟使能	   
	GPIOA->CRL&=0XFFF0FFFF;	 
	GPIOA->CRL|=3<<4*4;		//PA4推挽输出 
	ES_CTRL=1;	//PA4输出高
#endif
 	USART2_Init(36,115200);	//初始化串口2 	
	return 0;
} 

//ATK-RM04发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* atk_rm04_check_cmd(u8 *str)
{
	
	char *strx=0;
	if(USART2_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART2_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//向ATK-RM04发送命令
//cmd:发送的命令字符串(不需要添加回车了)
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 atk_rm04_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART2_RX_STA=0;
	u2_printf("%s\r",cmd);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART2_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(atk_rm04_check_cmd(ack))break;//得到有效数据 
				USART2_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 
//ATK-RM04退出透传模式
//返回值:0,退出成功;
//       1,退出失败
u8 atk_rm04_quit_trans(void)
{
#if AT_MODE_CONFIG==1	//使用串口特殊序列退出透传,进入AT模式
	while(DMA1_Channel7->CNDTR!=0);	//等待通道7传输完成   
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART2->SR&0X40)==0);	//等待发送空
	USART2->DR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART2->SR&0X40)==0);	//等待发送空
	USART2->DR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART2->SR&0X40)==0);	//等待发送空
	USART2->DR='+';      
	delay_ms(500);					//等待500ms
	while((USART2->SR&0X40)==0);	//等待发送空
	USART2->DR=0X1B;      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART2->SR&0X40)==0);	//等待发送空
	USART2->DR=0X1B;      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART2->SR&0X40)==0);	//等待发送空
	USART2->DR=0X1B;      
	delay_ms(15);					//大于串口组帧时间(10ms) 
	return atk_rm04_send_cmd("at","at",20);//退出透传判断.
#else					//使用ES/RST引脚控制,退出透传,进入AT模式
	ES_CTRL=0;			//拉低ES/RST
	delay_ms(120);		//等待120ms
	ES_CTRL=1;			//ES/RST恢复
	return 0;
#endif
}
//获取ATK-RM04模块的连接状态
//返回值:0,未连接;1,连接成功.
u8 atk_rm04_consta_check(void)
{
	u8 *p;
	u8 res;
	if(atk_rm04_quit_trans())return 0;			//退出透传 
	atk_rm04_send_cmd("at+S2N_Stat=?","?",50);	//发送at+S2N_Stat=?指令,查询连接状态
	p=atk_rm04_check_cmd("\r\n"); 
	res=*(p+2);									//得到连接状态	
	atk_rm04_send_cmd("at+out_trans=0","ok",50);//发送at+out_trans=0,进入透传模式
	return res;
}

//获取wan ip地址
//ipbuf:ip地址输出缓存区
void atk_rm04_get_wanip(u8* ipbuf)
{
	u8 *p,*p1;
	if(atk_rm04_send_cmd("at+net_wanip=?","?",50))//获取WAN IP地址失败
	{
		ipbuf[0]=0;
		return;
	}		
	p=atk_rm04_check_cmd("\r\n");
	p1=(u8*)strstr((const char*)p,",");
	*p1=0;
	sprintf((char*)ipbuf,"%s",p+2);
}
//获取WIFI STA连接状态(检查WIFI STA是否连上路由器)
//返回值:0,连接失败;1,连接成功;
u8 atk_rm04_get_wifista_state(void)
{
	u8 *p; 
	atk_rm04_send_cmd("at+wifi_ConState=?","?",20);	//获取WIFI STA连接状态
	p=atk_rm04_check_cmd("\r\n"); 
	
	printf(" p = %s  \n\n",p);
	return strstr((const char*)p,"Connected")?1:0;
}

//ATK-RM04模块测试主函数
void atk_rm04_test(void)
{
	
	
	
	atk_rm04_quit_trans();//退出透传 
	
	printf(" searching wifi model....... \n");
	while(atk_rm04_send_cmd("at","\r\n",20))//检查WIFI模块是否在线
	{
		
		delay_ms(800);
		atk_rm04_quit_trans();//退出透传 
	} 	
	
	delay_ms(10);
	printf("  finded wifi model try to set wifi model..... \n");
	atk_rm04_wifista_test();//WIFI STA测试
	
}






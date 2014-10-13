#include "common.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//ATK-RM04 WIFI模块 串口无线（COM-WIFI STA）驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/4/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 


//ATK-RM04 WIFI STA测试
//用于测试TCP/UDP连接
//返回值:0,正常
//    其他,错误代码
u8 atk_rm04_wifista_test(void)
{
	u8 netpro=0;	//网络模式
	u8 key;
	u8 timex=0; 
	u8 ipbuf[16]; 	//IP缓存
	u8 p[100]; 	//申请32字节内存
	u16 t=999;		//加速第一次获取链接状态
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;	//连接状态
	

	printf(" wifi sta  start  \n");
	atk_rm04_send_cmd("at+netmode=2","ok",500);		//设置WIFI STA(wifi网卡)模式
	atk_rm04_send_cmd("at+dhcpd=0","ok",500);		//DHCP服务器关闭(仅AP模式有效) 
	atk_rm04_send_cmd("at+dhcpc=1","ok",500);		//DHCP客户端使能(net_ip无效)
	//设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!! 
	sprintf((char*)p,"at+wifi_conf=%s,%s,%s",wifista_ssid,wifista_encryption,wifista_password);//设置无线参数:ssid,加密方式,密码
	atk_rm04_send_cmd(p,"ok",500);					//配置无线参数
	printf(" wifi model config %s " , p);
	
		
  atk_rm04_send_cmd("at+remotepro=tcp","ok",500);			//TCP协议
	sprintf((char*)p,"at+remoteport=%s",portnum);
	atk_rm04_send_cmd(p,"ok",500);					//设置 端口号. 
	
	printf(" wifi model port = %s " , p);
	
	
	
		sprintf((char*)p,"at+remoteip=%s",ipbuf);
		atk_rm04_send_cmd(p,"ok",500);					//设置远端(连接)IP地址. 
		atk_rm04_send_cmd("at+mode=client","ok",500);	//设置为客户端		
				 
	
	
          printf(" wifi model is configing ...  \n");

	if(atk_rm04_send_cmd("at+net_commit=1","\r\n",4000))//提交网络配置,最长可能需要等待40s
 	{ 
	
	  printf(" wifi model is configed error   \n");
		delay_ms(800);        
		res=1; 
	}else
	{	
		atk_rm04_send_cmd("at+reconn=1","ok",500);	//重启串口转换服务
			
		delay_ms(600);
	
		atk_rm04_quit_trans();						//退出透传
		while(atk_rm04_get_wifista_state()==0)		//等待ATK-RM04连接上路由器 
		{ 	
		
			delay_ms(800);  
         printf("router conneted error \n");			
			delay_ms(800); 
		}
		
		
		USART2_RX_STA=0;
		while(1)
		{
			
				sprintf((char*)p,"ATK-RM04 测试%02d\r\n",t++);//测试数据
				printf(" send msg : %s ",p);
				u2_printf("%s",p);		//发送该数据到ATK-RM04模块
			
			delay_ms(10);
			if(USART2_RX_STA&0X8000)		//接收到一次数据了
			{ 
				rlen=USART2_RX_STA&0X7FFF;	//得到本次接收到的数据长度
				USART2_RX_BUF[rlen]=0;		//添加结束符 
				printf("%s",USART2_RX_BUF);	//发送到串口   
				sprintf((char*)p,"收到%d字节,内容如下",rlen);//接收到的字节数
				
				printf(" recive data %s \n",p);
				printf(" %s ",USART2_RX_BUF);
				
				USART2_RX_STA=0;
				             
			}  
			
// 			if(t==1000)//连续10秒钟没有收到任何数据,检查连接是不是还存在.
// 			{
// 				constate=atk_rm04_consta_check()-'0';//得到连接状态
// 				if(constate)Show_Str(30+30,80,200,12,"连接成功",12,0);  //连接状态
// 				else Show_Str(30+30,80,200,12,"连接失败",12,0); 	 
// 				t=0;
// 			}
			if((t%20)==0)LED0=!LED0;
			
		}
	} 
	
	atk_rm04_quit_trans();	//退出透传
	return res;
} 





























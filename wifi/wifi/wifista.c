#include "common.h"

//ATK-RM04 WIFI STA测试
//用于测试TCP/UDP连接
//返回值:0,正常
//    其他,错误代码
u8 atk_rm04_wifista_test(void)
{
	
	u8 p[100]; 	//申请32字节内存
	u16 t=999;		//加速第一次获取链接状态
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;	//连接状态
	
	atk_rm04_send_cmd("at+netmode=2","ok",500);		//设置WIFI STA(wifi网卡)模式
	atk_rm04_send_cmd("at+dhcpd=0","ok",500);		//DHCP服务器关闭(仅AP模式有效) 
	atk_rm04_send_cmd("at+dhcpc=1","ok",500);		//DHCP客户端使能(net_ip无效)
	//设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!! 
	sprintf((char*)p,"at+wifi_conf=%s,%s,%s",wifista_ssid,wifista_encryption,wifista_password);//设置无线参数:ssid,加密方式,密码
	atk_rm04_send_cmd(p,"ok",500);					//配置无线参数
	printf(" wifi model router: %s \n" , p);
	
		
  atk_rm04_send_cmd("at+remotepro=tcp","ok",500);			//TCP协议
	sprintf((char*)p,"at+remoteport=%s",portnum);
	atk_rm04_send_cmd(p,"ok",500);					//设置 端口号. 
	
	printf(" wifi model remoteport = %d \n " , portnum);
	
	
	
		sprintf((char*)p,"at+remoteip=%s",remoteIp);
		atk_rm04_send_cmd(p,"ok",500);					//设置远端(连接)IP地址. 
		atk_rm04_send_cmd("at+mode=client","ok",500);	//设置为客户端		
		 printf(" wifi model remoteip = %s \n",remoteIp);		 
	
	   delay_ms(10);
     printf(" wifi model config is committing.....  \n");

	if(atk_rm04_send_cmd("at+net_commit=1","\r\n",4000))//提交网络配置,最长可能需要等待40s
 	{ 
	  printf(" wifi model is configed error trying again.   \n");
		delay_ms(800);        
		res=1; 
	}else
	{	
		printf(" wifi model is configed succsed restart connection...  \n");
		atk_rm04_send_cmd("at+reconn=1","ok",500);	//重启串口转换服务		
		delay_ms(600);
		atk_rm04_quit_trans();						//退出透传
		
		
		while(atk_rm04_get_wifista_state()==0)		//等待ATK-RM04连接上路由器 
		{ 	
			 
      printf("wifi model connected error,try again ...\n");			
			delay_ms(800); 
		}
		
		
		USART2_RX_STA=0;
		while(1)
		{
			
			
// 			  if(t%100==0)	
// 				{
// 					// sprintf((char*)p,"ATK-RM04 测试%02d\r\n",t);//测试数据
// 					 p[0]= (char)0xff;
//  				   printf(" send msg : %s ",p);
//  				   u2_printf("%s",p);		//发送该数据到ATK-RM04模块
//         }
			 
			
			
			delay_ms(10);
			
			
			//检查是否接受数据
			if(USART2_RX_STA&0X8000)		//接收到一次数据了
			{ 
				rlen=USART2_RX_STA&0X7FFF;	//得到本次接收到的数据长度
				USART2_RX_BUF[rlen]=0;		//添加结束符 
				sprintf((char*)p,"收到%d字节,内容如下",rlen);//接收到的字节数	
				//printf(" recive data %s \n",p);
				printf(" %s + t= %d\n",USART2_RX_BUF,t);
				//printf(" delay 4000\n");
				LED0=!LED0;
				USART2_RX_STA=0;	
			}  
			
			
			
			//检查是否与服务器连接
			if(t%1000==0)//连续10秒钟没有收到任何数据,检查连接是不是还存在.
			{
				constate=atk_rm04_consta_check()-'0';//得到连接状态
				if(constate)
					printf(" server is online   \n");  //连接状态
				else
				  printf(" server is't online  \n"); 
				t=0; 
				USART2_RX_STA=0;	
			}
			
			
			
			t++;
			
		}
	} 
	
	atk_rm04_quit_trans();	//退出透传
	return res;
} 





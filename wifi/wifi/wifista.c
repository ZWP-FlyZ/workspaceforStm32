#include "common.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//ATK-RM04 WIFIģ�� �������ߣ�COM-WIFI STA����������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/4/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 


//ATK-RM04 WIFI STA����
//���ڲ���TCP/UDP����
//����ֵ:0,����
//    ����,�������
u8 atk_rm04_wifista_test(void)
{
	u8 netpro=0;	//����ģʽ
	u8 key;
	u8 timex=0; 
	u8 ipbuf[16]; 	//IP����
	u8 p[100]; 	//����32�ֽ��ڴ�
	u16 t=999;		//���ٵ�һ�λ�ȡ����״̬
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;	//����״̬
	

	printf(" wifi sta  start  \n");
	atk_rm04_send_cmd("at+netmode=2","ok",500);		//����WIFI STA(wifi����)ģʽ
	atk_rm04_send_cmd("at+dhcpd=0","ok",500);		//DHCP�������ر�(��APģʽ��Ч) 
	atk_rm04_send_cmd("at+dhcpc=1","ok",500);		//DHCP�ͻ���ʹ��(net_ip��Ч)
	//�������ӵ���WIFI��������/���ܷ�ʽ/����,�⼸��������Ҫ�������Լ���·�������ý����޸�!! 
	sprintf((char*)p,"at+wifi_conf=%s,%s,%s",wifista_ssid,wifista_encryption,wifista_password);//�������߲���:ssid,���ܷ�ʽ,����
	atk_rm04_send_cmd(p,"ok",500);					//�������߲���
	printf(" wifi model config %s " , p);
	
		
  atk_rm04_send_cmd("at+remotepro=tcp","ok",500);			//TCPЭ��
	sprintf((char*)p,"at+remoteport=%s",portnum);
	atk_rm04_send_cmd(p,"ok",500);					//���� �˿ں�. 
	
	printf(" wifi model port = %s " , p);
	
	
	
		sprintf((char*)p,"at+remoteip=%s",ipbuf);
		atk_rm04_send_cmd(p,"ok",500);					//����Զ��(����)IP��ַ. 
		atk_rm04_send_cmd("at+mode=client","ok",500);	//����Ϊ�ͻ���		
				 
	
	
          printf(" wifi model is configing ...  \n");

	if(atk_rm04_send_cmd("at+net_commit=1","\r\n",4000))//�ύ��������,�������Ҫ�ȴ�40s
 	{ 
	
	  printf(" wifi model is configed error   \n");
		delay_ms(800);        
		res=1; 
	}else
	{	
		atk_rm04_send_cmd("at+reconn=1","ok",500);	//��������ת������
			
		delay_ms(600);
	
		atk_rm04_quit_trans();						//�˳�͸��
		while(atk_rm04_get_wifista_state()==0)		//�ȴ�ATK-RM04������·���� 
		{ 	
		
			delay_ms(800);  
         printf("router conneted error \n");			
			delay_ms(800); 
		}
		
		
		USART2_RX_STA=0;
		while(1)
		{
			
				sprintf((char*)p,"ATK-RM04 ����%02d\r\n",t++);//��������
				printf(" send msg : %s ",p);
				u2_printf("%s",p);		//���͸����ݵ�ATK-RM04ģ��
			
			delay_ms(10);
			if(USART2_RX_STA&0X8000)		//���յ�һ��������
			{ 
				rlen=USART2_RX_STA&0X7FFF;	//�õ����ν��յ������ݳ���
				USART2_RX_BUF[rlen]=0;		//��ӽ����� 
				printf("%s",USART2_RX_BUF);	//���͵�����   
				sprintf((char*)p,"�յ�%d�ֽ�,��������",rlen);//���յ����ֽ���
				
				printf(" recive data %s \n",p);
				printf(" %s ",USART2_RX_BUF);
				
				USART2_RX_STA=0;
				             
			}  
			
// 			if(t==1000)//����10����û���յ��κ�����,��������ǲ��ǻ�����.
// 			{
// 				constate=atk_rm04_consta_check()-'0';//�õ�����״̬
// 				if(constate)Show_Str(30+30,80,200,12,"���ӳɹ�",12,0);  //����״̬
// 				else Show_Str(30+30,80,200,12,"����ʧ��",12,0); 	 
// 				t=0;
// 			}
			if((t%20)==0)LED0=!LED0;
			
		}
	} 
	
	atk_rm04_quit_trans();	//�˳�͸��
	return res;
} 





























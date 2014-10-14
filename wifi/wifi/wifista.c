#include "common.h"

//ATK-RM04 WIFI STA����
//���ڲ���TCP/UDP����
//����ֵ:0,����
//    ����,�������
u8 atk_rm04_wifista_test(void)
{
	
	u8 p[100]; 	//����32�ֽ��ڴ�
	u16 t=999;		//���ٵ�һ�λ�ȡ����״̬
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;	//����״̬
	
	atk_rm04_send_cmd("at+netmode=2","ok",500);		//����WIFI STA(wifi����)ģʽ
	atk_rm04_send_cmd("at+dhcpd=0","ok",500);		//DHCP�������ر�(��APģʽ��Ч) 
	atk_rm04_send_cmd("at+dhcpc=1","ok",500);		//DHCP�ͻ���ʹ��(net_ip��Ч)
	//�������ӵ���WIFI��������/���ܷ�ʽ/����,�⼸��������Ҫ�������Լ���·�������ý����޸�!! 
	sprintf((char*)p,"at+wifi_conf=%s,%s,%s",wifista_ssid,wifista_encryption,wifista_password);//�������߲���:ssid,���ܷ�ʽ,����
	atk_rm04_send_cmd(p,"ok",500);					//�������߲���
	printf(" wifi model router: %s \n" , p);
	
		
  atk_rm04_send_cmd("at+remotepro=tcp","ok",500);			//TCPЭ��
	sprintf((char*)p,"at+remoteport=%s",portnum);
	atk_rm04_send_cmd(p,"ok",500);					//���� �˿ں�. 
	
	printf(" wifi model remoteport = %d \n " , portnum);
	
	
	
		sprintf((char*)p,"at+remoteip=%s",remoteIp);
		atk_rm04_send_cmd(p,"ok",500);					//����Զ��(����)IP��ַ. 
		atk_rm04_send_cmd("at+mode=client","ok",500);	//����Ϊ�ͻ���		
		 printf(" wifi model remoteip = %s \n",remoteIp);		 
	
	   delay_ms(10);
     printf(" wifi model config is committing.....  \n");

	if(atk_rm04_send_cmd("at+net_commit=1","\r\n",4000))//�ύ��������,�������Ҫ�ȴ�40s
 	{ 
	  printf(" wifi model is configed error trying again.   \n");
		delay_ms(800);        
		res=1; 
	}else
	{	
		printf(" wifi model is configed succsed restart connection...  \n");
		atk_rm04_send_cmd("at+reconn=1","ok",500);	//��������ת������		
		delay_ms(600);
		atk_rm04_quit_trans();						//�˳�͸��
		
		
		while(atk_rm04_get_wifista_state()==0)		//�ȴ�ATK-RM04������·���� 
		{ 	
			 
      printf("wifi model connected error,try again ...\n");			
			delay_ms(800); 
		}
		
		
		USART2_RX_STA=0;
		while(1)
		{
			
			
// 			  if(t%100==0)	
// 				{
// 					// sprintf((char*)p,"ATK-RM04 ����%02d\r\n",t);//��������
// 					 p[0]= (char)0xff;
//  				   printf(" send msg : %s ",p);
//  				   u2_printf("%s",p);		//���͸����ݵ�ATK-RM04ģ��
//         }
			 
			
			
			delay_ms(10);
			
			
			//����Ƿ��������
			if(USART2_RX_STA&0X8000)		//���յ�һ��������
			{ 
				rlen=USART2_RX_STA&0X7FFF;	//�õ����ν��յ������ݳ���
				USART2_RX_BUF[rlen]=0;		//��ӽ����� 
				sprintf((char*)p,"�յ�%d�ֽ�,��������",rlen);//���յ����ֽ���	
				//printf(" recive data %s \n",p);
				printf(" %s + t= %d\n",USART2_RX_BUF,t);
				//printf(" delay 4000\n");
				LED0=!LED0;
				USART2_RX_STA=0;	
			}  
			
			
			
			//����Ƿ������������
			if(t%1000==0)//����10����û���յ��κ�����,��������ǲ��ǻ�����.
			{
				constate=atk_rm04_consta_check()-'0';//�õ�����״̬
				if(constate)
					printf(" server is online   \n");  //����״̬
				else
				  printf(" server is't online  \n"); 
				t=0; 
				USART2_RX_STA=0;	
			}
			
			
			
			t++;
			
		}
	} 
	
	atk_rm04_quit_trans();	//�˳�͸��
	return res;
} 





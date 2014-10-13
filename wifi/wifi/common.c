#include "common.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//ATK-RM04 WIFIģ�� ������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/4/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 


/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//�û�������

//���Ӷ˿ں�:8086,�������޸�Ϊ�����˿�.
const u8* portnum="8086";		 

//WIFI STAģʽ,����Ҫȥ���ӵ�·�������߲���,��������Լ���·��������,�����޸�.
const u8* wifista_ssid="05zwp";			//·����SSID��
const u8* wifista_encryption="wpawpa2_aes";	//wpa/wpa2 aes���ܷ�ʽ
const u8* wifista_password="301415926"; 	//��������
const u8* remoteIp ="192.168.1.30";


void atk_rm04_init(void)
{
#if AT_MODE_CONFIG==0
	RCC->APB2ENR|=1<<2;    	//PAʱ��ʹ��	   
	GPIOA->CRL&=0XFFF0FFFF;	 
	GPIOA->CRL|=3<<4*4;		//PA4������� 
	ES_CTRL=1;	//PA4�����
#endif
 	USART2_Init(36,115200);	//��ʼ������2 	
	
	printf("wifi model init ok ");
	
} 

//ATK-RM04���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//    ����,�ڴ�Ӧ������λ��(str��λ��)
u8* atk_rm04_check_cmd(u8 *str)
{
	
	char *strx=0;
	if(USART2_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strstr((const char*)USART2_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//��ATK-RM04��������
//cmd:���͵������ַ���(����Ҫ��ӻس���)
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 atk_rm04_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART2_RX_STA=0;
	u2_printf("%s\r",cmd);	//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
			if(USART2_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(atk_rm04_check_cmd(ack))break;//�õ���Ч���� 
				USART2_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 
//ATK-RM04�˳�͸��ģʽ
//����ֵ:0,�˳��ɹ�;
//       1,�˳�ʧ��
u8 atk_rm04_quit_trans(void)
{
#if AT_MODE_CONFIG==1	//ʹ�ô������������˳�͸��,����ATģʽ
	while(DMA1_Channel7->CNDTR!=0);	//�ȴ�ͨ��7�������   
	delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART2->SR&0X40)==0);	//�ȴ����Ϳ�
	USART2->DR='+';      
	delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART2->SR&0X40)==0);	//�ȴ����Ϳ�
	USART2->DR='+';      
	delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART2->SR&0X40)==0);	//�ȴ����Ϳ�
	USART2->DR='+';      
	delay_ms(500);					//�ȴ�500ms
	while((USART2->SR&0X40)==0);	//�ȴ����Ϳ�
	USART2->DR=0X1B;      
	delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART2->SR&0X40)==0);	//�ȴ����Ϳ�
	USART2->DR=0X1B;      
	delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART2->SR&0X40)==0);	//�ȴ����Ϳ�
	USART2->DR=0X1B;      
	delay_ms(15);					//���ڴ�����֡ʱ��(10ms) 
	return atk_rm04_send_cmd("at","at",20);//�˳�͸���ж�.
#else					//ʹ��ES/RST���ſ���,�˳�͸��,����ATģʽ
	ES_CTRL=0;			//����ES/RST
	delay_ms(120);		//�ȴ�120ms
	ES_CTRL=1;			//ES/RST�ָ�
	return 0;
#endif
}
//��ȡATK-RM04ģ�������״̬
//����ֵ:0,δ����;1,���ӳɹ�.
u8 atk_rm04_consta_check(void)
{
	u8 *p;
	u8 res;
	if(atk_rm04_quit_trans())return 0;			//�˳�͸�� 
	atk_rm04_send_cmd("at+S2N_Stat=?","?",50);	//����at+S2N_Stat=?ָ��,��ѯ����״̬
	p=atk_rm04_check_cmd("\r\n"); 
	res=*(p+2);									//�õ�����״̬	
	atk_rm04_send_cmd("at+out_trans=0","ok",50);//����at+out_trans=0,����͸��ģʽ
	return res;
}

//��ȡwan ip��ַ
//ipbuf:ip��ַ���������
void atk_rm04_get_wanip(u8* ipbuf)
{
	u8 *p,*p1;
	if(atk_rm04_send_cmd("at+net_wanip=?","?",50))//��ȡWAN IP��ַʧ��
	{
		ipbuf[0]=0;
		return;
	}		
	p=atk_rm04_check_cmd("\r\n");
	p1=(u8*)strstr((const char*)p,",");
	*p1=0;
	sprintf((char*)ipbuf,"%s",p+2);
}
//��ȡWIFI STA����״̬(���WIFI STA�Ƿ�����·����)
//����ֵ:0,����ʧ��;1,���ӳɹ�;
u8 atk_rm04_get_wifista_state(void)
{
	u8 *p; 
	atk_rm04_send_cmd("at+wifi_ConState=?","?",20);	//��ȡWIFI STA����״̬
	p=atk_rm04_check_cmd("\r\n"); 
	
	printf(" p = %s  \n\n",p);
	return strstr((const char*)p,"Connected")?1:0;
}

//ATK-RM04ģ�����������
void atk_rm04_test(void)
{
	u8 key;
	u8 timex;
	
	atk_rm04_quit_trans();//�˳�͸�� 
	while(atk_rm04_send_cmd("at","\r\n",20))//���WIFIģ���Ƿ�����
	{
		printf("cant find  wifi model \n");
		delay_ms(800);
		atk_rm04_quit_trans();//�˳�͸�� 
	} 	
	
	printf("  find  wifi model begin sta \n");
	atk_rm04_wifista_test();//WIFI STA����
	
}




















































#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "LED.h"
#include "common.h"

int main (void)
{
			u8 t = 0;
			Stm32_Clock_Init(9);
			delay_init(72);
			uart_init(72,9600);
      led_init();
	
	   atk_rm04_init();		//��ʼ��RM04ģ��
	   atk_rm04_test();
	
	
// 	    while(1)
// 			{
// 				printf("t : %d\n",t);
// 				delay_ms(500);
// 				t++;
// 				LED0=!LED0;
// 			}
	
}


#include "LED.h"
#include "sys.h"

void led_init(void)
{
   RCC->APB2ENR|=1<<2;
	 GPIOA->CRH&=0xfffffff0;
	 GPIOA->CRH|=0x00000003;
	 GPIOA->ODR=1<<8;
}

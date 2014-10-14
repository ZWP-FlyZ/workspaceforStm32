#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "common.h"
#include "init.h"

int main (void)
{
	    int state =-1;
	
			Stm32_Clock_Init(9);
			delay_init(72);
			uart_init(72,9600);
		 
	    state = InitAllDevice();
      if(!state)  printf(" user's devices init ok!\r\n");	
	    
			state = -1;
			state = atk_rm04_init();		//初始化RM04模块
			if(!state)   printf(" wifi model init ok \n");
	
	    atk_rm04_test();//进入wifip配置
	
	    
}
